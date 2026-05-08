#!/usr/bin/env python3
"""Convert a PNG sprite sheet into STM32-friendly RGB565 C data.

Usage:
    py tools/convert_sprite_sheet_to_rgb565.py "character sprites/walk_front-Sheet.png" \
        player_walk_front 22 32

The script is PC-side only. The STM32 firmware must not decode PNG/GIF files,
load filesystems, or allocate sprite buffers at runtime. Generated arrays should
be pasted/imported into Core/Src/assets_sprites.c as static const data.
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

TRANSPARENT_RGB565 = 0x0001


def rgb888_to_rgb565(r: int, g: int, b: int) -> int:
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)


def sanitize_symbol(text: str) -> str:
    out = []
    for char in text.strip().lower():
        out.append(char if char.isalnum() else "_")
    symbol = "".join(out).strip("_")
    while "__" in symbol:
        symbol = symbol.replace("__", "_")
    if not symbol:
        symbol = "sprite"
    if symbol[0].isdigit():
        symbol = f"sprite_{symbol}"
    return symbol


def main() -> int:
    parser = argparse.ArgumentParser(description="Convert equal-frame PNG sprite sheet to RGB565 C arrays.")
    parser.add_argument("png", type=Path, help="Input PNG sprite sheet")
    parser.add_argument("symbol", help="C symbol prefix")
    parser.add_argument("frame_width", type=int, help="Frame width in source pixels")
    parser.add_argument("frame_height", type=int, help="Frame height in source pixels")
    parser.add_argument("--alpha-threshold", type=int, default=128, help="Alpha below this becomes transparent")
    args = parser.parse_args()

    try:
        from PIL import Image
    except ImportError:
        print("error: Pillow is required. Install on PC with: py -m pip install pillow", file=sys.stderr)
        return 1

    if args.frame_width <= 0 or args.frame_height <= 0:
        print("error: frame dimensions must be positive", file=sys.stderr)
        return 1

    image = Image.open(args.png).convert("RGBA")
    if image.width % args.frame_width != 0 or image.height % args.frame_height != 0:
        print(
            f"warning: image {image.width}x{image.height} is not divisible by "
            f"{args.frame_width}x{args.frame_height}; partial frames will be ignored",
            file=sys.stderr,
        )

    columns = image.width // args.frame_width
    rows = image.height // args.frame_height
    frame_count = columns * rows
    symbol = sanitize_symbol(args.symbol)

    print("/* Generated from", args.png.as_posix(), "*/")
    print("#include <stdint.h>")
    print()
    print(f"#define {symbol.upper()}_FRAME_WIDTH {args.frame_width}u")
    print(f"#define {symbol.upper()}_FRAME_HEIGHT {args.frame_height}u")
    print(f"#define {symbol.upper()}_FRAME_COUNT {frame_count}u")
    print()
    print(f"static const uint16_t {symbol}_pixels[{frame_count}u][{args.frame_width * args.frame_height}u] = {{")

    for frame in range(frame_count):
        fx = (frame % columns) * args.frame_width
        fy = (frame // columns) * args.frame_height
        print(f"    /* frame {frame} */")
        print("    {")
        for y in range(args.frame_height):
            values = []
            for x in range(args.frame_width):
                r, g, b, a = image.getpixel((fx + x, fy + y))
                if a < args.alpha_threshold:
                    value = TRANSPARENT_RGB565
                else:
                    value = rgb888_to_rgb565(r, g, b)
                    if value == TRANSPARENT_RGB565:
                        value = 0x0000
                values.append(f"0x{value:04X}u")
            print("        " + ", ".join(values) + ",")
        print("    },")

    print("};")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
