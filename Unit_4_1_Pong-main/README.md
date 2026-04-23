# STM32 Embedded Game Framework

A small reusable game framework for STM32 projects using the existing CubeMX/HAL setup, joystick input, and ST7789 LCD rendering support.

The Pong-specific ball, paddle, collision, lives, and scoring logic has been removed. The project now starts from a generic 2D prototype: a joystick-controlled rectangle in a bounded world, plus an action button state that can be extended into game-specific behavior.

## File Structure

```text
Core/
  Inc/
    main.h          CubeMX-generated board definitions
    game.h          Game state model and update API
    input.h         Joystick/button input state API
    render.h        LCD rendering API
  Src/
    main.c          HAL initialization and fixed timestep loop
    game.c          Starter game state and integer movement logic
    input.c         Joystick and button polling
    render.c        LCD setup and frame drawing

Joystick/           Reusable joystick driver
ST7789V2_Driver_STM32L4/
                    Reusable LCD driver
Drivers/            STM32 HAL/CMSIS drivers
cmake/              CubeMX CMake integration
```

Removed from the active framework:

```text
Ball/
Paddle/
PongEngine/
Core/Inc/raycast.h
Core/Src/raycast.c
```

## Main Loop

`Core/Src/main.c` owns hardware initialization and frame timing. The game loop intentionally stays small:

```c
while (1)
{
    uint32_t now = HAL_GetTick();
    if ((uint32_t)(now - last_tick) < GAME_FRAME_TIME_MS) {
        continue;
    }
    last_tick = now;

    read_input();
    update_game();
    render_frame();
}
```

## Module Responsibilities

`input.c` polls the joystick ADC and action buttons, debounces button presses, and stores the latest `GameInput_t`. Other modules can read it through `input_get_state()`.

`game.c` owns the static `GameState_t`, updates movement using integer math, clamps the player rectangle to the world bounds, and exposes a read-only pointer through `game_get_state()`.

`render.c` initializes the LCD driver, clears the frame buffer, draws a simple grid/world/player view, prints lightweight debug state, and refreshes the LCD.

No dynamic memory is used. The starter logic avoids floating point and keeps state in fixed-size static structs so it is straightforward to extend with tile maps, actors, collisions, menus, or scenes.
