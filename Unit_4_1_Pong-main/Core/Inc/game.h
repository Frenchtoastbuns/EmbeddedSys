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
#define GAME_TURN_SPEED_DEGREES 6
#define PLAYER_MAX_STABILITY  5u
#define PLAYER_DAMAGE_COOLDOWN_FRAMES 45u
#define MAX_FAULT_NODES    3u
#define FAULT_NODE_NONE    0xFFu
#define FAULT_REPAIR_DISTANCE  14
#define FAULT_REPAIR_THRESHOLD 90u
#define CAMPAIGN_LEVEL_COUNT 3u
#define MAX_ENEMIES        3u
#define ENEMY_SIZE         8u
#define ENEMY_MOVE_INTERVAL_FRAMES 6u
#define ENEMY_CONTACT_DISTANCE 8
#define ENEMY_HEALTH       1u
#define SHOT_RANGE         70
#define SHOT_HALF_WIDTH    8
#define SHOT_COOLDOWN_FRAMES 12u
#define SHOT_FLASH_FRAMES  4u
#define DEBUG_PULSE_MAX_CHARGE 100u
#define DEBUG_PULSE_REPAIR_CHARGE 35u
#define DEBUG_PULSE_ENEMY_CHARGE 15u
#define DEBUG_PULSE_RADIUS 32
#define DEBUG_PULSE_ACTIVE_TICKS 8u
#define BOSS_SIZE          10u
#define BOSS_HEALTH        5u
#define DISPLAY_CORRUPTION_TICKS 8u
#define DISPLAY_CORRUPTION_INTERVAL_FRAMES 32u

typedef enum {
    GAME_STATE_PLAYING = 0,
    GAME_STATE_DIALOGUE,
    GAME_STATE_LEVEL_COMPLETE
} GameRunState_t;

typedef enum {
    LEVEL_LED_BAR = 0,
    LEVEL_SPEAKER,
    LEVEL_DISPLAY_BOSS,
    LEVEL_COMPLETE
} CampaignLevel_t;

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t size;
    uint16_t angle_degrees;
} Player_t;

typedef struct {
    int8_t x;
    int8_t y;
} GameVector2i_t;

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t active;
    uint8_t health;
} Enemy_t;

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t active;
    uint8_t vulnerable;
    uint8_t health;
} Boss_t;

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
    CampaignLevel_t current_level;
    Player_t player;
    uint8_t system_stability;
    uint8_t damage_cooldown;
    uint8_t shot_cooldown;
    uint8_t shot_flash;
    uint8_t debug_pulse_charge;
    uint8_t debug_pulse_ready;
    uint8_t debug_pulse_unlocked;
    uint8_t debug_pulse_active_ticks;
    uint8_t led_bar_restored;
    uint8_t speaker_restored;
    uint8_t audio_unlocked;
    uint8_t display_restored;
    uint8_t boss_active;
    uint8_t boss_vulnerable;
    uint8_t boss_health;
    uint8_t display_corruption_ticks;
    GameInput_t last_input;
    FaultNode_t fault_nodes[MAX_FAULT_NODES];
    uint8_t fault_node_count;
    uint8_t active_fault_count;
    uint8_t fixed_fault_count;
    uint8_t all_faults_fixed;
    uint8_t repairing_fault_index;
    Enemy_t enemies[MAX_ENEMIES];
    uint8_t enemy_count;
    Boss_t boss;
    const char* dialogue_title;
    const char* dialogue_message;
} GameState_t;

void game_init(void);
void update_game(void);
const GameState_t* game_get_state(void);
const char* game_get_level_name(CampaignLevel_t level);
uint16_t wrap_angle(int16_t angle_degrees);
uint16_t get_player_angle(void);
GameVector2i_t get_player_forward_vector_int(void);
uint8_t game_get_tile(uint8_t tile_x, uint8_t tile_y);
uint8_t game_is_wall_at_world(int16_t world_x, int16_t world_y);
uint8_t game_player_can_move_to(int16_t x, int16_t y);
uint8_t game_player_is_near_fault_node(const Player_t* player, const FaultNode_t* node);

#endif /* GAME_H */
