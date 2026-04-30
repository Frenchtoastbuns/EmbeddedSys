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
#define MAX_FAULT_NODES    3u
#define FAULT_NODE_NONE    0xFFu
#define FAULT_REPAIR_DISTANCE  14
#define FAULT_REPAIR_THRESHOLD 90u
#define SUBSYSTEM_COUNT    4u

typedef enum {
    GAME_STATE_PLAYING = 0,
    GAME_STATE_DIALOGUE,
    GAME_STATE_LEVEL_COMPLETE
} GameRunState_t;

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t size;
} Player_t;

typedef struct {
    uint8_t tile_x;
    uint8_t tile_y;
    uint8_t active;
    uint8_t fixed;
    uint16_t repair_progress;
} FaultNode_t;

typedef struct {
    uint32_t frame_count;
    GameRunState_t run_state;
    uint8_t current_subsystem;
    Player_t player;
    GameInput_t last_input;
    FaultNode_t fault_nodes[MAX_FAULT_NODES];
    uint8_t fault_node_count;
    uint8_t active_fault_count;
    uint8_t fixed_fault_count;
    uint8_t all_faults_fixed;
    uint8_t repairing_fault_index;
    const char* dialogue_title;
    const char* dialogue_message;
} GameState_t;

void game_init(void);
void update_game(void);
const GameState_t* game_get_state(void);
const char* game_get_subsystem_name(uint8_t subsystem_index);
uint8_t game_get_tile(uint8_t tile_x, uint8_t tile_y);
uint8_t game_is_wall_at_world(int16_t world_x, int16_t world_y);
uint8_t game_player_can_move_to(int16_t x, int16_t y);
uint8_t game_player_is_near_fault_node(const Player_t* player, const FaultNode_t* node);

#endif /* GAME_H */
