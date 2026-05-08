# Tiled To STM32 Map Workflow

Tiled is a PC-side design tool. The STM32 firmware should only use static
`const` C arrays. There is no JSON/TMX parser, filesystem loader, or image
decoder in the firmware.

## Basic Workflow

1. Design a finite orthogonal map in Tiled.
2. Create tile layers named:
   - `Ground`
   - `Objects`
   - `Collision`
3. Optional: create an object layer named `Entities`.
4. Export as JSON / `.tmj`.
5. Run the converter on your PC:

```powershell
py tools\convert_tiled_json_to_c.py maps\overworld.json overworld > generated_maps.c
```

For the current repo-level `overworld.tmj`, this also works:

```powershell
py tools\convert_tiled_json_to_c.py overworld.tmj overworld > generated_maps.c
```

If Tiled global tile IDs are larger than `255`, the script warns because the
default output arrays are `uint8_t`. For a lossless compact table, use:

```powershell
py tools\convert_tiled_json_to_c.py overworld.tmj overworld --compact > generated_maps.c
```

To also make a smaller PC-side PNG containing only the tiles used by the map,
use `--compact-tileset`:

```powershell
py tools\convert_tiled_json_to_c.py maps\overworld.tmj overworld --ground-layer "Tile Layer 1" --objects-layer objects --compact-tileset assets\overworld_trimmed.png > generated_maps.c
```

`--compact-tileset` automatically enables compact ID output. The generated C
arrays and the trimmed PNG use the same order:

```text
old gid 12 -> new id 1
old gid 45 -> new id 2
old gid 91 -> new id 3
```

That means tile `1` in the generated C arrays corresponds to the first tile in
the trimmed PNG, tile `2` to the second tile, and so on. The STM32 still does
not load the PNG at runtime; the trimmed PNG is a convenient PC-side source for
converting only selected tiles into C tile assets later.

If the Tiled `.tsx` files are not available but you have the big tileset PNG,
pass it directly:

```powershell
py tools\convert_tiled_json_to_c.py maps\overworld.tmj overworld --ground-layer "Tile Layer 1" --objects-layer objects --compact-tileset assets\overworld_trimmed.png --source-tileset-png path\to\big_tileset.png --source-firstgid 1 > generated_maps.c
```

Use the `firstgid` from Tiled for that source image. If your map uses multiple
tilesets, either keep the `.tsx` files available so the converter can resolve
them, or make/export one curated tileset and remap the Tiled map to that one
tileset.

## Generated Output Format

The generated C looks like:

```c
#include <stdint.h>

#define OVERWORLD_MAP_W 32u
#define OVERWORLD_MAP_H 21u
#define OVERWORLD_TILE_W 32u
#define OVERWORLD_TILE_H 32u

static const uint8_t overworld_ground[OVERWORLD_MAP_H][OVERWORLD_MAP_W] = {
    { 0u, 1u, 1u, 2u },
};

static const uint8_t overworld_objects[OVERWORLD_MAP_H][OVERWORLD_MAP_W] = {
    { 0u, 0u, 5u, 0u },
};

static const uint8_t overworld_collision[OVERWORLD_MAP_H][OVERWORLD_MAP_W] = {
    { 1u, 1u, 0u, 0u },
};
```

`0` means empty. Non-zero IDs are Tiled GIDs when they fit in `uint8_t`.
With `--compact` or `--compact-tileset`, non-zero IDs are compact IDs and the
generated comment table shows which compact ID maps back to each original Tiled
GID.

## Entities Layer

If an object layer named `Entities` exists, the converter prints comments for
objects that look like:

- `player_spawn`
- `module_entrance`
- `module_exit`, `return_to_overworld`, or `exit_overworld`
- `removable_lint` or `speaker_lint`
- `fault_node`
- `enemy_spawn`
- `boss_dial`
- `boss_spawn`

Those comments include tile coordinates, world coordinates, object name/type,
GID, and properties. This is intentionally descriptive for now; gameplay logic
is not changed by the converter.

Runtime convention:

- `Ground` is visible floor art.
- `Objects` is visible wall, door, prop, and machine art.
- `Collision` is invisible movement blocking.
- `Entities` is invisible gameplay data only. Do not paint visible art here.

For the current firmware, module doors should have their visible tile art on
`Objects`, with an invisible `module_entrance` or `module_exit` rectangle on
`Entities`. Speaker lint should be placed as five `removable_lint` or
`speaker_lint` objects on `Entities`; the game draws and removes those from
state, so the static map arrays remain const.
