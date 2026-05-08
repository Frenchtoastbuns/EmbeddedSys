#ifndef GAME_H
#define GAME_H

/*
 * Main game state shared between game.c and render.c.
 * The actual rules stay in game.c; render.c only reads this state.
 */

#include <stdint.h>
#include "input.h"

/* Tile/world sizing used by maps, collision, movement and rendering. */
#define GAME_MAP_WIDTH     32u
#define GAME_MAP_HEIGHT    32u
#define GAME_TILE_SIZE     10
#define GAME_PLAYER_SIZE   8
#define GAME_PLAYER_SPEED  4

/* Red display dial and the green/off version used after interaction. */
#define TILE_BOSS_DIAL_ACTIVE    55u
#define TILE_BOSS_DIAL_INACTIVE  89u

#define FAULT_REPAIR_DISTANCE  14
#define CAMPAIGN_LEVEL_COUNT 2u
#define MAX_REMOVABLE_OBJECTS 5u

#define SHOT_RANGE         70

#define BOSS_SIZE          10u
#define MAX_BOSS_DIALS     6u
#define DISPLAY_DIAL_COUNT 6u

#define DISPLAY_CORRUPTION_TICKS 8u
#define DISPLAY_CORRUPTION_INTERVAL_FRAMES 32u

typedef enum {
    GAME_STATE_PLAYING = 0,
    GAME_STATE_DIALOGUE,
    GAME_STATE_LEVEL_COMPLETE
} GameRunState_t;

typedef enum {
    /*
     * Speaker/Display keep their original numeric IDs because generated Tiled
     * entity data stores the level value directly.
     */
    LEVEL_SPEAKER = 1,
    LEVEL_DISPLAY_BOSS = 2,
    LEVEL_COMPLETE = 3
} CampaignLevel_t;

typedef enum {
    AREA_MODE_OVERWORLD = 0,
    AREA_MODE_COMPONENT_INTERIOR
} AreaMode_t;

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t size;
    uint16_t angle_degrees;
} GamePlayer_t;

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t active;
    uint8_t vulnerable;
} Boss_t;

typedef struct {
    uint8_t tile_x;
    uint8_t tile_y;
    uint8_t active;
    uint8_t disabled;
} BossDial_t;

typedef struct {
    uint8_t tile_x;
    uint8_t tile_y;
    uint8_t active;
    uint8_t type;
} RemovableObject_t;

typedef struct {
    uint32_t frame_count;
    GameRunState_t run_state;
    AreaMode_t area_mode;
    CampaignLevel_t current_level;

    GamePlayer_t player;
    int16_t overworld_return_x;
    int16_t overworld_return_y;

    uint8_t speaker_restored;
    uint8_t display_restored;
    uint8_t display_corruption_ticks;

    uint8_t speaker_task_done;
    uint8_t speaker_lint_count;
    uint8_t speaker_lint_removed;

    uint8_t boss_dial_count;
    uint8_t boss_dials_disabled;

    uint8_t dialogue_sequence;
    uint8_t dialogue_index;
    uint16_t story_flags;

    GameInput_t last_input;
    Boss_t boss;
    RemovableObject_t removable_objects[MAX_REMOVABLE_OBJECTS];
    BossDial_t boss_dials[MAX_BOSS_DIALS];

    const char* dialogue_title;
    const char* dialogue_message;
} GameState_t;

void game_init(void);
void update_game(void);
const GameState_t* game_get_state(void);
const char* game_get_level_name(CampaignLevel_t level);
uint8_t game_get_active_map_width(void);
uint8_t game_get_active_map_height(void);
int16_t game_get_active_world_width(void);
int16_t game_get_active_world_height(void);
uint8_t game_get_tile(uint8_t tile_x, uint8_t tile_y);
uint8_t game_get_object_tile(uint8_t tile_x, uint8_t tile_y);
uint8_t game_get_overworld_entrance_tile(uint8_t tile_x, uint8_t tile_y);

#endif /* GAME_H */
