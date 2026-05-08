# STM32 Circuit Rescue Game

A 2D top-down embedded game built in C for the STM32 Nucleo-L476RG. The player is sent inside a broken circuit board, repairs the speaker module, disables cache nodes in the display controller, and defeats a final hardware bug.

> Firmware project folder: `Unit_4_1_Pong-main/`

This project started from a simple joystick-controlled LCD prototype and grew into a small game demo with Tiled maps, sprite rendering, dialogue, collision, objectives, and a boss fight.

## Demo Summary

- Platform: STM32 Nucleo-L476RG
- Language: C with STM32 HAL / CubeMX setup
- Display: 2.8 inch SPI LCD, ILI9341-style 240x320 screen
- Input: one analog joystick plus action button
- Style: Pokemon/Zelda-inspired top-down movement
- Maps: hand-authored in Tiled and converted offline into static C arrays
- Assets: selected tiles and sprites converted offline into RGB565 C data
- Runtime constraints: no dynamic allocation, no runtime JSON parsing, no image decoding, no filesystem loading

## Gameplay

The final demo flow is intentionally short and focused:

1. Intro dialogue sets the scene in Lab 1.60.
2. The player enters a motherboard-style overworld.
3. The Speaker module opens first.
4. The player cleans all lint/dropping objects from the Speaker level.
5. The Display/Boss module unlocks.
6. The player disables six cache nodes.
7. The boss becomes vulnerable.
8. The player shoots the boss and reaches the `SYSTEM RESTORED` ending.

Older experimental systems such as raycasting, platforming, the LED bar level, PWM audio, and external LED bar hardware were removed from the active game path to keep the final demo stable.

## Hardware Used

| Part | Purpose |
| --- | --- |
| STM32 Nucleo-L476RG | Main microcontroller board |
| SPI LCD | Main game display |
| Analog joystick | Player movement |
| Action button | Dialogue, interaction, shooting |
| PC-side Tiled editor | Level design workflow |

The final game does not require the old Grove LED bar, DFPlayer, buzzer, PWM outputs, or second joystick.

## Code Structure

```text
Unit_4_1_Pong-main/
  Core/
    Inc/
      game.h              Main game state and gameplay constants
      input.h             Public input state
      render.h            LCD render entry points
      Player.h            Direction and movement-facing types
      Map.h               Wall-check adapter used by movement
      game_story.h        Dialogue sequence definitions
      generated_maps.h    Static Tiled map metadata
      assets_tiles.h      RGB565 tile asset metadata
      assets_sprites.h    RGB565 sprite asset metadata
      hardware.h          Safe optional hardware hooks

    Src/
      main.c              CubeMX init and simple fixed-timestep loop
      input.c             Joystick ADC reading and button debounce
      game.c              Gameplay rules, transitions, objectives, dialogue
      render.c            Camera, map drawing, sprites, dialogue boxes
      game_story.c        Static dialogue strings
      generated_maps.c    Tiled maps converted to const arrays
      assets_tiles.c      Converted RGB565 tile data
      assets_sprites.c    Converted RGB565 sprite data

  tools/
    convert_tiled_json_to_c.py
    convert_tile_to_rgb565.py
    convert_sprite_sheet_to_rgb565.py

  maps/
    Tiled map source files
```

## Main Loop

The runtime loop is deliberately simple:

```c
while (1)
{
    if (game_frame_due(&last_tick) != 0u) {
        read_input();
        update_game();
        hardware_update(game_get_state());
        render_frame();
    }
}
```

`main.c` handles board setup and timing. `input.c`, `game.c`, `hardware.c`, and `render.c` each own one clear step of the frame.

## Map System

Maps are designed in Tiled and exported as `.tmj` files. A PC-side converter turns them into firmware-safe data:

- `Ground` layer: floor/background visuals
- `Objects` layer: visible doors, walls, props, machines
- `Collision` layer: invisible blocked tiles
- `Entities` layer: invisible triggers and gameplay markers

The STM32 does not parse Tiled files at runtime. It only reads static `const` arrays generated ahead of time.

## Gameplay Systems

### Movement and Collision

The player moves in four directions using joystick input. Collision uses the active map's `Collision` layer, not the visual tile layers. X and Y movement are checked separately so the player can slide along walls.

### Dialogue

Dialogue is stored as static strings in `game_story.c`. The game tracks the current sequence and line index, then advances with the action button. No heap allocation is used.

### Speaker Objective

The Speaker level has removable lint/dropping objects. These are gameplay entities, not permanent map tiles. Each object is hidden once cleaned, and the module completes when all are removed.

### Display Boss Objective

The Display level has six cache nodes/dials. Each dial starts active and turns inactive after interaction. The boss is invulnerable until all six dials are disabled.

### Shooting

Shooting is intentionally simple for the demo. If the boss is in front of the player and within range, the shot counts. If the boss is still invulnerable, dialogue reminds the player to clear all cache nodes.

## Embedded Design Choices

- Static arrays instead of dynamic memory
- Integer movement and collision
- Offline asset/map conversion
- No runtime PNG/JPG/TMJ/JSON parsing
- No framebuffer allocation beyond the LCD driver's existing buffer
- Short fixed-size arrays for dials and removable objects
- Hardware experiments disabled behind safe no-op hooks

## Build

From the firmware folder:

```powershell
cd Unit_4_1_Pong-main
cmake --build build\Debug
```

Typical current build size:

```text
RAM:   ~42.6 KB / 96 KB
FLASH: ~308 KB / 1 MB
```

## Flash

```powershell
& "C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe" -c port=SWD -w "C:\Users\aaron\Downloads\Unit_4_1_Pong-main\Unit_4_1_Pong-main\build\Debug\Unit_4_1_GameEngine.elf" -v -rst
```

## What I Learned

- Reading analog joystick input reliably needs calibration, deadzones, and filtering.
- LCD rendering performance has to be considered when tuning movement speed.
- Runtime parsing and dynamic allocation are a poor fit for a small embedded demo.
- Separating input, game rules, rendering, and hardware hooks makes the project easier to debug.
- Tiled maps are useful, but they need an offline conversion workflow for STM32 firmware.

## Current Status

The current build is focused on a stable two-level demo:

- Speaker repair objective
- Display boss objective
- Dialogue-driven story
- Camera-following top-down maps
- Sprite/tile rendering
- Final `SYSTEM RESTORED` screen

The final code intentionally prioritizes a working embedded demo over extra experimental systems.
