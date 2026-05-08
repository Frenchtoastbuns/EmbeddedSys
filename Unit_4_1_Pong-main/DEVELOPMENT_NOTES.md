# Development Notes

This project started as a simple LCD/joystick test and grew into the board
repair game. I kept the final loop simple:

```c
read_input();
update_game();
hardware_update(game_get_state());
render_frame();
```

Things that went wrong while building it:

- The joystick was too twitchy at first, so I added a deadzone and a few
  confirmed reads before changing direction.
- The map colours were wrong more than once while converting tiles from Tiled.
- The hardware bar experiment was unreliable during testing, so I removed it
  from the final game path and kept the demo focused on LCD and joystick gameplay.
- Movement changed a few times. The final version uses joystick X to turn and
  joystick Y to move forward/back, while still checking wall collision.
- Some earlier experiments, like raycasting and side-view movement, were removed
  from the final gameplay path because they made the demo too complicated.

The final goal was not to make a full engine. It was to make a short embedded
demo that shows joystick control, Tiled maps, LCD drawing, small objectives,
dialogue, and hardware feedback on the STM32.
