#ifndef INPUT_H
#define INPUT_H

/* Public input state used by game.c. */

#include <stdint.h>
#include "Player.h"

typedef struct {
    int8_t move_x;          /* -1 left, 0 idle, +1 right */
    int8_t move_y;          /* -1 up, 0 idle, +1 down */
    Direction joy1_direction;
    Direction joy2_direction;
    uint8_t action_down;    /* 1 while the action button is held */
    uint8_t action_pressed; /* 1 for one frame on a new press */
} GameInput_t;

void start_input(void);
void read_buttons(void);
const GameInput_t* get_input(void);

#endif /* INPUT_H */
