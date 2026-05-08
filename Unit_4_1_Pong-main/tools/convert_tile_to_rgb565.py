#!/usr/bin/env python3
"""Convert one selected 32x32 PNG tile into a C RGB565 array.

This is a PC-side helper only. The STM32 firmware does not decode PNG/JPG
files and does not load image files at runtime.

Example:
    python tools/convert_tile_to_rgb565.py floor.png tile_floor

The output uses the byte-swapped RGB565 convention used by LCD.h so the values
can be pasted into Core/Src/assets_tiles.c.
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path


TILE_SIZE = 32


def rgb888_to_rgb565(r: int, g: int, b: int) -> int:
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)


def byte_swap_u16(value: int) -> int:
    return ((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8)


def main() -> int:
    parser = argparse.ArgumentParser(description="Convert one 32x32 PNG tile to a C RGB565 array.")
    parser.add_argument("png", type=Path, help="Input 32x32 PNG file")
    parser.add_argument("symbol", help="C symbol name, e.g. tile_wall_top")
    parser.add_argument(
        "--no-byte-swap",
        action="store_true",
        help="Emit standard RGB565 instead of the LCD.h byte-swapped format",
    )
    args = parser.parse_args()

    try:
        from PIL import Image
    except ImportError:
        print("Pillow is required on the PC side: py -m pip install pillow", file=sys.stderr)
        return 2

    image = Image.open(args.png).convert("RGB")
    if image.size != (TILE_SIZE, TILE_SIZE):
        print(f"warning: expected 32x32, got {image.size[0]}x{image.size[1]}", file=sys.stderr)
        return 1

    print(f"static const uint16_t {args.symbol}[ASSETS_TILE_PIXELS] = {{")
    for y in range(TILE_SIZE):
        row_values: list[str] = []
        for x in range(TILE_SIZE):
            r, g, b = image.getpixel((x, y))
            value = rgb888_to_rgb565(r, g, b)
            if not args.no_byte_swap:
                value = byte_swap_u16(value)
            row_values.append(f"0x{value:04X}")
        print("    " + ", ".join(row_values) + ("," if y < TILE_SIZE - 1 else ""))
    print("};")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
