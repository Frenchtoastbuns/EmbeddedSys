#ifndef PLAYER_H
#define PLAYER_H

/*
 * Small movement types used by input.c, Player.c and game.c.
 * Joystick_t is kept tiny now: only the final direction matters to movement.
 */

#include <stdint.h>

#define PLAYER_FP_SCALE 256     /* sub-pixel scale factor */

typedef enum {
    CENTRE = 0,
    N,
    NE,
    E,
    SE,
    S,
    SW,
    W,
    NW
} Direction;

typedef struct {
    Direction direction;
} Joystick_t;

typedef struct {
    int16_t  x_fp;          /* position in 1/256 tile units */
    int16_t  y_fp;
    uint16_t angle_deg;     /* 0-359 degrees */
    uint8_t  health;
} Player_t;

void Player_Update(Player_t* player, Joystick_t* joy1, Joystick_t* joy2);

#endif /* PLAYER_H */
