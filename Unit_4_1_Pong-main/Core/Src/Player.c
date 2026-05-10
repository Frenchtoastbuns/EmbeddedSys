#include "Player.h"
#include "Map.h"

/*
 * Player movement driver.
 *
 * The rest of the game stores player position in screen/world pixels, but this
 * movement helper uses fixed-point tile coordinates. That keeps movement smooth
 * without using float maths on the STM32.
 */

/*
 * Simple top-down movement in 1/256 tile units per frame.
 * 32 fixed-point units is just over one LCD/world pixel per frame because
 * game.c converts this back into pixel coordinates each update.
 */
#define MOVE_SPEED_FP 32

/* old movement helper kept for the player driver file */
void move_player_driver(Player_t* player, Joystick_t* joy1, Joystick_t* joy2)
{
    int16_t new_x = player->x_fp;
    int16_t new_y = player->y_fp;
    (void)joy2;

    if (joy1->direction == N) {
        new_y -= MOVE_SPEED_FP;
        player->angle_deg = 270u;
    } else if (joy1->direction == S) {
        new_y += MOVE_SPEED_FP;
        player->angle_deg = 90u;
    } else if (joy1->direction == E) {
        new_x += MOVE_SPEED_FP;
        player->angle_deg = 0u;
    } else if (joy1->direction == W) {
        new_x -= MOVE_SPEED_FP;
        player->angle_deg = 180u;
    }

    /* Check X and Y separately so the player can slide along walls. */
    if (!is_wall((int)(new_x / PLAYER_FP_SCALE),
                     (int)(player->y_fp / PLAYER_FP_SCALE))) {
        player->x_fp = new_x;
    }
    if (!is_wall((int)(player->x_fp / PLAYER_FP_SCALE),
                     (int)(new_y / PLAYER_FP_SCALE))) {
        player->y_fp = new_y;
    }
}
