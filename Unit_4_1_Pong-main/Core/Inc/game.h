#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include "input.h"

#define GAME_MAP_WIDTH      16u
#define GAME_MAP_HEIGHT     16u
#define GAME_TILE_SIZE      16
#define PLAYER_SIZE         8
#define PLAYER_SPEED        2
#define ENEMY_MAX_COUNT     10u
#define ENEMY_SIZE          8
#define ENEMY_SPEED         1
#define SHOT_RANGE          80
#define SHOT_HALF_WIDTH     8
#define SHOT_COOLDOWN_FRAMES 20u
#define SHOT_FLASH_FRAMES    4u
#define FAULT_NODE_MAX_COUNT 4u
#define FAULT_NODE_SIZE      8
#define FAULT_REPAIR_RANGE   14
#define FAULT_REPAIR_COMPLETE 90u
#define FAULT_NODE_NONE      0xFFu

typedef struct {
    int16_t x;
    int16_t y;
    int8_t facing_x;
    int8_t facing_y;
    uint8_t shot_cooldown;
    uint8_t shot_flash;
} Player_t;

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t active;
} Enemy_t;

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t fixed;
    uint8_t repair_progress;
} FaultNode_t;

typedef struct {
    uint32_t frame_count;
    GameInput_t last_input;
    Player_t player;
    Enemy_t enemies[ENEMY_MAX_COUNT];
    uint8_t enemy_count;
    uint16_t enemies_hit;
    uint16_t shots_fired;
    FaultNode_t fault_nodes[FAULT_NODE_MAX_COUNT];
    uint8_t fault_node_count;
    uint8_t fixed_fault_count;
    uint8_t active_repair_node;
} GameState_t;

void game_init(GameState_t* game);
void update_game(GameState_t* game, const GameInput_t* input);
uint8_t game_get_tile(uint8_t tile_x, uint8_t tile_y);
uint8_t game_is_wall_at_world(int16_t world_x, int16_t world_y);
uint8_t game_player_can_stand_at(int16_t x, int16_t y);
uint8_t game_enemy_can_stand_at(int16_t x, int16_t y);
uint8_t game_enemy_is_hit_by_shot(const Player_t* player, const Enemy_t* enemy);
uint8_t game_player_is_near_fault_node(const Player_t* player, const FaultNode_t* node);

#endif /* GAME_H */
