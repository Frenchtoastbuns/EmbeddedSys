#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

typedef struct {
    int8_t move_x;          /* -1 left, 0 idle, +1 right */
    int8_t move_y;          /* -1 up, 0 idle, +1 down */
    uint8_t action_down;    /* 1 while the action button is held */
    uint8_t action_pressed; /* 1 for one frame on a new press */
} GameInput_t;

void input_init(void);
void read_input(GameInput_t* input);

#endif /* INPUT_H */
