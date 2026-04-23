#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include "input.h"

#define GAME_WORLD_WIDTH    160
#define GAME_WORLD_HEIGHT   160
#define GAME_PLAYER_SIZE    10
#define GAME_PLAYER_SPEED   2
#define GAME_ACTION_FRAMES  8u

typedef struct {
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
} GameRect_t;

typedef struct {
    uint32_t frame_count;
    GameRect_t player;
    GameInput_t last_input;
    uint8_t action_active;
    uint8_t action_pulse;
} GameState_t;

void game_init(void);
void update_game(void);
const GameState_t* game_get_state(void);

#endif /* GAME_H */
