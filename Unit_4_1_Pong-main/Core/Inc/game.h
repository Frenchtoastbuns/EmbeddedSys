#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include "input.h"

#define GAME_MAP_WIDTH     16u
#define GAME_MAP_HEIGHT    16u
#define GAME_TILE_SIZE     10
#define GAME_WORLD_WIDTH   (GAME_MAP_WIDTH * GAME_TILE_SIZE)
#define GAME_WORLD_HEIGHT  (GAME_MAP_HEIGHT * GAME_TILE_SIZE)
#define GAME_PLAYER_SIZE   8
#define GAME_PLAYER_SPEED  2

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t size;
} Player_t;

typedef struct {
    uint32_t frame_count;
    Player_t player;
    GameInput_t last_input;
} GameState_t;

void game_init(void);
void update_game(void);
const GameState_t* game_get_state(void);
uint8_t game_get_tile(uint8_t tile_x, uint8_t tile_y);
uint8_t game_is_wall_at_world(int16_t world_x, int16_t world_y);
uint8_t game_player_can_move_to(int16_t x, int16_t y);

#endif /* GAME_H */
