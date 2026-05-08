#!/usr/bin/env python3
"""Convert a Tiled JSON map export into STM32-friendly static C arrays.

Usage:
    python tools/convert_tiled_json_to_c.py maps/overworld.json overworld
    py tools\\convert_tiled_json_to_c.py overworld.tmj overworld > generated_maps.c

Workflow:
    1. Design the map in Tiled.
    2. Export as JSON / .tmj.
    3. Run this PC-side converter.
    4. Paste/import the generated static const arrays into game map data.

The STM32 firmware must not parse JSON/TMX, load files, or decode images at
runtime. The firmware should only consume static const C arrays.
"""

from __future__ import annotations

import argparse
import json
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path
from typing import Any, TextIO


TILED_GID_MASK = 0x0FFFFFFF
UINT8_MAX = 255
COMPACT_TILESET_COLUMNS = 16

LAYER_NAMES = {
    "ground": "Ground",
    "objects": "Objects",
    "collision": "Collision",
}

ENTITY_NAMES = (
    "player_spawn",
    "module_entrance",
    "module_exit",
    "return_to_overworld",
    "exit_overworld",
    "removable_lint",
    "speaker_lint",
    "fault_node",
    "enemy_spawn",
    "boss_dial",
    "boss_spawn",
)


def sanitize_symbol(text: str) -> str:
    symbol = re.sub(r"[^0-9A-Za-z_]", "_", text.strip().lower())
    symbol = re.sub(r"_+", "_", symbol).strip("_")
    if not symbol:
        symbol = "map"
    if symbol[0].isdigit():
        symbol = f"map_{symbol}"
    return symbol


def load_tiled_json(path: Path) -> dict[str, Any]:
    with path.open("r", encoding="utf-8") as handle:
        map_data = json.load(handle)

    if not isinstance(map_data, dict) or map_data.get("type") != "map":
        raise ValueError("input file does not look like a Tiled JSON map")
    if map_data.get("infinite", False):
        raise ValueError("infinite Tiled maps are not supported for static STM32 arrays")

    return map_data


def load_optional_pillow():
    try:
        from PIL import Image
    except ImportError as exc:
        raise RuntimeError(
            "Pillow is required for --compact-tileset. Install it on the PC with: py -m pip install pillow"
        ) from exc

    return Image


def find_layer(map_data: dict[str, Any], name: str, layer_type: str) -> dict[str, Any] | None:
    for layer in map_data.get("layers", []):
        if layer.get("type") == layer_type and str(layer.get("name", "")).lower() == name.lower():
            return layer
    return None


def strip_tiled_flags(gid: int) -> int:
    return gid & TILED_GID_MASK


def collect_used_gids(*layers: list[list[int]]) -> list[int]:
    return sorted({tile for layer in layers for row in layer for tile in row if tile != 0})


def read_tile_layer(layer: dict[str, Any] | None, width: int, height: int) -> list[list[int]]:
    if layer is None:
        return [[0 for _ in range(width)] for _ in range(height)]

    data = layer.get("data")
    if not isinstance(data, list):
        raise ValueError(
            f"layer '{layer.get('name')}' must use uncompressed JSON array data; "
            "base64/compressed layer data is not supported"
        )

    if len(data) != width * height:
        raise ValueError(f"layer '{layer.get('name')}' has {len(data)} cells, expected {width * height}")

    return [
        [strip_tiled_flags(int(data[(y * width) + x])) for x in range(width)]
        for y in range(height)
    ]


def compact_used_ids(*layers: list[list[int]]) -> dict[int, int]:
    used_ids = collect_used_gids(*layers)
    if len(used_ids) > UINT8_MAX:
        raise ValueError(f"map uses {len(used_ids)} unique non-empty tile IDs; compact uint8_t only supports 255")
    return {gid: index for index, gid in enumerate(used_ids, start=1)}


def remap_layer(layer: list[list[int]], mapping: dict[int, int]) -> list[list[int]]:
    return [[mapping.get(tile, 0) for tile in row] for row in layer]


def parse_tsx_image(tsx_path: Path) -> tuple[Path, int, int]:
    tree = ET.parse(tsx_path)
    root = tree.getroot()
    image = root.find("image")
    if image is None or image.get("source") is None:
        raise ValueError(f"tileset '{tsx_path}' does not contain an image source")

    tilewidth = int(root.get("tilewidth", "0"))
    tileheight = int(root.get("tileheight", "0"))
    if tilewidth <= 0 or tileheight <= 0:
        raise ValueError(f"tileset '{tsx_path}' must define positive tilewidth/tileheight")

    return (tsx_path.parent / image.get("source", "")).resolve(), tilewidth, tileheight


def resolve_tilesets(map_data: dict[str, Any], map_path: Path, tilewidth: int, tileheight: int) -> list[dict[str, Any]]:
    resolved: list[dict[str, Any]] = []

    for tileset in map_data.get("tilesets", []):
        firstgid = int(tileset.get("firstgid", 1))
        source_path: Path | None = None
        source_tilewidth = tilewidth
        source_tileheight = tileheight

        if "source" in tileset:
            tsx_path = (map_path.parent / str(tileset["source"])).resolve()
            if not tsx_path.exists():
                raise FileNotFoundError(
                    f"tileset source '{tsx_path}' was not found. "
                    "Keep the .tsx files next to the Tiled export, or pass "
                    "--source-tileset-png path\\to\\big_tileset.png with the matching --source-firstgid."
                )
            source_path, source_tilewidth, source_tileheight = parse_tsx_image(tsx_path)
        elif "image" in tileset:
            source_path = (map_path.parent / str(tileset["image"])).resolve()
            source_tilewidth = int(tileset.get("tilewidth", tilewidth))
            source_tileheight = int(tileset.get("tileheight", tileheight))

        if source_path is None:
            raise ValueError(f"tileset starting at firstgid {firstgid} has no source/image")

        resolved.append(
            {
                "firstgid": firstgid,
                "image": source_path,
                "tilewidth": source_tilewidth,
                "tileheight": source_tileheight,
            }
        )

    if not resolved:
        raise ValueError("map does not define any tilesets, so a compact PNG cannot be generated")

    return sorted(resolved, key=lambda item: int(item["firstgid"]))


def tileset_for_gid(tilesets: list[dict[str, Any]], gid: int) -> dict[str, Any]:
    selected = tilesets[0]
    for tileset in tilesets:
        if gid >= int(tileset["firstgid"]):
            selected = tileset
        else:
            break
    return selected


def generate_compact_tileset_png(
    map_data: dict[str, Any],
    map_path: Path,
    output_png: Path,
    used_gids: list[int],
    tilewidth: int,
    tileheight: int,
    source_tileset_png: Path | None = None,
    source_firstgid: int = 1,
) -> None:
    if not used_gids:
        raise ValueError("no non-empty tiles were used; compact tileset would be empty")

    Image = load_optional_pillow()
    if source_tileset_png is not None:
        tilesets = [
            {
                "firstgid": source_firstgid,
                "image": source_tileset_png.resolve(),
                "tilewidth": tilewidth,
                "tileheight": tileheight,
            }
        ]
    else:
        tilesets = resolve_tilesets(map_data, map_path, tilewidth, tileheight)
    dst_columns = min(len(used_gids), COMPACT_TILESET_COLUMNS)
    dst_rows = (len(used_gids) + dst_columns - 1) // dst_columns
    dst = Image.new("RGBA", (dst_columns * tilewidth, dst_rows * tileheight), (0, 0, 0, 0))
    source_cache: dict[Path, Any] = {}

    output_png.parent.mkdir(parents=True, exist_ok=True)

    for new_index, old_gid in enumerate(used_gids):
        tileset = tileset_for_gid(tilesets, old_gid)
        image_path = Path(tileset["image"])
        source_tilewidth = int(tileset["tilewidth"])
        source_tileheight = int(tileset["tileheight"])

        if not image_path.exists():
            raise FileNotFoundError(
                f"tileset image '{image_path}' for old GID {old_gid} was not found. "
                "Keep the TSX/image files next to the Tiled map or use a map export with resolvable tileset paths."
            )
        if source_tilewidth != tilewidth or source_tileheight != tileheight:
            raise ValueError(
                f"tileset image '{image_path}' uses {source_tilewidth}x{source_tileheight}, "
                f"but the map uses {tilewidth}x{tileheight}; mixed tile sizes are not supported"
            )

        if image_path not in source_cache:
            source_cache[image_path] = Image.open(image_path).convert("RGBA")
        src = source_cache[image_path]
        src_columns = src.width // tilewidth
        local_id = old_gid - int(tileset["firstgid"])

        if local_id < 0:
            raise ValueError(f"old GID {old_gid} is before tileset firstgid {tileset['firstgid']}")

        src_x = (local_id % src_columns) * tilewidth
        src_y = (local_id // src_columns) * tileheight

        if src_x + tilewidth > src.width or src_y + tileheight > src.height:
            raise ValueError(f"old GID {old_gid} points outside tileset image '{image_path}'")

        dst_x = (new_index % dst_columns) * tilewidth
        dst_y = (new_index // dst_columns) * tileheight
        tile = src.crop((src_x, src_y, src_x + tilewidth, src_y + tileheight))
        dst.paste(tile, (dst_x, dst_y))

    dst.save(output_png)


def clamp_layer_to_uint8(layer: list[list[int]], layer_name: str) -> tuple[list[list[int]], list[int]]:
    overflows = sorted({tile for row in layer for tile in row if tile > UINT8_MAX})
    if overflows:
        print(
            f"warning: layer '{layer_name}' contains tile IDs above 255; "
            "those cells are emitted as 255 in uint8_t output. "
            "Use --compact if you want a lossless compact ID table.",
            file=sys.stderr,
        )
        preview = ", ".join(str(value) for value in overflows[:12])
        more = " ..." if len(overflows) > 12 else ""
        print(f"warning: overflowing IDs: {preview}{more}", file=sys.stderr)

    clamped = [[tile if tile <= UINT8_MAX else UINT8_MAX for tile in row] for row in layer]
    return clamped, overflows


def emit_array(out: TextIO, name: str, layer: list[list[int]], h_macro: str, w_macro: str) -> None:
    out.write(f"static const uint8_t {name}[{h_macro}][{w_macro}] = {{\n")
    for row_index, row in enumerate(layer):
        suffix = "," if row_index < len(layer) - 1 else ""
        values = ", ".join(f"{tile:3d}u" for tile in row)
        out.write(f"    {{ {values} }}{suffix}\n")
    out.write("};\n\n")


def object_properties(obj: dict[str, Any]) -> str:
    props = obj.get("properties", [])
    if not isinstance(props, list) or not props:
        return ""
    return ", ".join(f"{prop.get('name')}={prop.get('value')}" for prop in props)


def classify_entity(obj: dict[str, Any]) -> str:
    text = f"{obj.get('name', '')} {obj.get('type', '')} {obj.get('class', '')}".lower()
    props = object_properties(obj).lower()
    text = f"{text} {props}"

    for entity_name in ENTITY_NAMES:
        if entity_name in text or entity_name.replace("_", " ") in text:
            return entity_name

    if "player" in text and "spawn" in text:
        return "player_spawn"
    if "return" in text and "spawn" in text:
        return "player_spawn"
    if "boss" in text and "dial" in text:
        return "boss_dial"
    if "lint" in text:
        return "speaker_lint"
    if (
        "module_exit" in text
        or "return_to_overworld" in text
        or "return to overworld" in text
        or "exit_overworld" in text
        or "exit overworld" in text
        or ("exit" in text and "module" in text)
    ):
        return "module_exit"
    if (
        "entrance" in text
        or "module" in text
        or "door" in text
    ):
        return "module_entrance"
    if "fault" in text or "repair" in text:
        return "fault_node"
    if "enemy" in text or "bug" in text:
        return "enemy_spawn"
    if "boss" in text:
        return "boss_spawn"

    return "unknown"


def emit_entities_comments(out: TextIO, map_data: dict[str, Any], tilewidth: int, tileheight: int) -> None:
    entities_layer = find_layer(map_data, "Entities", "objectgroup")

    out.write("/* Entities object layer:\n")
    if entities_layer is None:
        out.write(" *   No object layer named \"Entities\" was found.\n")
        out.write(" *   Add Tiled objects named/player typed like player_spawn,\n")
        out.write(" *   module_entrance/module_exit/return_to_overworld,\n")
        out.write(" *   fault_node, enemy_spawn, or boss_spawn.\n")
        out.write(" */\n")
        return

    objects = entities_layer.get("objects", [])
    if not objects:
        out.write(" *   Entities layer exists but contains no objects.\n")
        out.write(" */\n")
        return

    for obj in objects:
        name = str(obj.get("name", ""))
        obj_type = str(obj.get("type", ""))
        x = int(round(float(obj.get("x", 0))))
        y = int(round(float(obj.get("y", 0))))
        tile_x = x // tilewidth if tilewidth else 0
        tile_y = y // tileheight if tileheight else 0
        gid = strip_tiled_flags(int(obj.get("gid", 0)))
        kind = classify_entity(obj)
        props = object_properties(obj)
        props_text = f", props: {props}" if props else ""

        out.write(
            f" *   {kind}: name=\"{name}\", type=\"{obj_type}\", "
            f"tile=({tile_x},{tile_y}), world=({x},{y}), gid={gid}{props_text}\n"
        )
    out.write(" */\n")


def emit_compact_table(out: TextIO, mapping: dict[int, int]) -> None:
    out.write("/* Compact tile ID table generated by --compact / --compact-tileset:\n")
    out.write(" *   0 means empty/no tile.\n")
    for gid, compact_id in sorted(mapping.items(), key=lambda item: item[1]):
        out.write(f" *   old gid {gid} -> new id {compact_id}\n")
    out.write(" */\n\n")


def main() -> int:
    parser = argparse.ArgumentParser(description="Convert Tiled JSON layers into STM32 static C arrays.")
    parser.add_argument("map_json", type=Path, help="Input Tiled JSON/.tmj map")
    parser.add_argument("prefix", nargs="?", help="C symbol prefix, e.g. overworld")
    parser.add_argument("-o", "--output", type=Path, help="Optional output .c file; defaults to stdout")
    parser.add_argument("--ground-layer", default=LAYER_NAMES["ground"], help="Ground tile layer name")
    parser.add_argument("--objects-layer", default=LAYER_NAMES["objects"], help="Objects tile layer name")
    parser.add_argument("--collision-layer", default=LAYER_NAMES["collision"], help="Collision tile layer name")
    parser.add_argument(
        "--compact",
        action="store_true",
        help="Losslessly compact used Tiled GIDs into 1..255 instead of preserving raw IDs",
    )
    parser.add_argument(
        "--compact-tileset",
        type=Path,
        help="Optional PC-side PNG output containing only used tiles in compact ID order",
    )
    parser.add_argument(
        "--source-tileset-png",
        type=Path,
        help="Optional source tileset PNG override when TSX/image paths are not available",
    )
    parser.add_argument(
        "--source-firstgid",
        type=int,
        default=1,
        help="Firstgid for --source-tileset-png; default is 1",
    )
    args = parser.parse_args()

    try:
        map_data = load_tiled_json(args.map_json)
        width = int(map_data.get("width", 0))
        height = int(map_data.get("height", 0))
        tilewidth = int(map_data.get("tilewidth", 0))
        tileheight = int(map_data.get("tileheight", 0))
        if width <= 0 or height <= 0 or tilewidth <= 0 or tileheight <= 0:
            raise ValueError("map width/height/tilewidth/tileheight must all be positive")

        prefix = sanitize_symbol(args.prefix or args.map_json.stem)

        ground_layer = find_layer(map_data, args.ground_layer, "tilelayer")
        objects_layer = find_layer(map_data, args.objects_layer, "tilelayer")
        collision_layer = find_layer(map_data, args.collision_layer, "tilelayer")

        ground = read_tile_layer(ground_layer, width, height)
        objects = read_tile_layer(objects_layer, width, height)
        collision = read_tile_layer(collision_layer, width, height)

        compact_mapping: dict[int, int] | None = None
        if args.compact or args.compact_tileset:
            compact_mapping = compact_used_ids(ground, objects, collision)
            if args.compact_tileset:
                used_gids = [gid for gid, _ in sorted(compact_mapping.items(), key=lambda item: item[1])]
                generate_compact_tileset_png(
                    map_data,
                    args.map_json,
                    args.compact_tileset,
                    used_gids,
                    tilewidth,
                    tileheight,
                    args.source_tileset_png,
                    args.source_firstgid,
                )
                print(f"wrote compact tileset PNG: {args.compact_tileset}", file=sys.stderr)
            ground = remap_layer(ground, compact_mapping)
            objects = remap_layer(objects, compact_mapping)
            collision = remap_layer(collision, compact_mapping)
        else:
            ground, _ = clamp_layer_to_uint8(ground, args.ground_layer)
            objects, _ = clamp_layer_to_uint8(objects, args.objects_layer)
            collision, _ = clamp_layer_to_uint8(collision, args.collision_layer)

        out_handle: TextIO
        should_close = False
        if args.output:
            out_handle = args.output.open("w", encoding="utf-8", newline="\n")
            should_close = True
        else:
            out_handle = sys.stdout

        macro_prefix = prefix.upper()
        out_handle.write("/*\n")
        out_handle.write(f" * Generated from Tiled JSON: {args.map_json.name}\n")
        out_handle.write(" * PC-side output only. Do not parse JSON/TMX on the STM32.\n")
        out_handle.write(" * Firmware should consume these static const arrays only.\n")
        out_handle.write(" */\n\n")
        out_handle.write("#include <stdint.h>\n\n")
        out_handle.write(f"#define {macro_prefix}_MAP_W {width}u\n")
        out_handle.write(f"#define {macro_prefix}_MAP_H {height}u\n")
        out_handle.write(f"#define {macro_prefix}_TILE_W {tilewidth}u\n")
        out_handle.write(f"#define {macro_prefix}_TILE_H {tileheight}u\n\n")
        out_handle.write("/* Layer selection:\n")
        out_handle.write(f" *   Ground    -> {args.ground_layer if ground_layer else '<missing; zero-filled>'}\n")
        out_handle.write(f" *   Objects   -> {args.objects_layer if objects_layer else '<missing; zero-filled>'}\n")
        out_handle.write(f" *   Collision -> {args.collision_layer if collision_layer else '<missing; zero-filled>'}\n")
        out_handle.write(" */\n\n")

        if compact_mapping is not None:
            emit_compact_table(out_handle, compact_mapping)
            if args.compact_tileset:
                out_handle.write("/* Compact tileset PNG:\n")
                out_handle.write(f" *   {args.compact_tileset}\n")
                out_handle.write(" * The STM32 firmware still does not load this PNG at runtime.\n")
                out_handle.write(" * Convert selected tiles from this PNG into flash-resident C tile assets.\n")
                out_handle.write(" */\n\n")

        emit_array(out_handle, f"{prefix}_ground", ground, f"{macro_prefix}_MAP_H", f"{macro_prefix}_MAP_W")
        emit_array(out_handle, f"{prefix}_objects", objects, f"{macro_prefix}_MAP_H", f"{macro_prefix}_MAP_W")
        emit_array(out_handle, f"{prefix}_collision", collision, f"{macro_prefix}_MAP_H", f"{macro_prefix}_MAP_W")
        emit_entities_comments(out_handle, map_data, tilewidth, tileheight)

        if should_close:
            out_handle.close()
    except Exception as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
