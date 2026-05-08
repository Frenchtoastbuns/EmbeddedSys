#include "game.h"
#include "generated_maps.h"
#include "game_story.h"
#include "Map.h"
#include "Player.h"

/*
 * Game rules and state machine.
 *
 * Active flow:
 *   Overworld -> Speaker task -> Display boss -> SYSTEM RESTORED.
 *
 * This file does not draw pixels or read ADC pins directly. It only reads the
 * current GameInput_t and updates GameState_t for render.c to display.
 */

static GameState_t game_state;

static void show_level_complete_dialogue(void);
static uint8_t game_get_collision_tile(uint8_t tile_x, uint8_t tile_y);
static void start_story_dialogue(StorySequenceId_t sequence);

#define OVERWORLD_TILE_EMPTY 0u
#define OVERWORLD_TILE_WALL 1u
#define OVERWORLD_TILE_SPEAKER 2u
#define OVERWORLD_TILE_DISPLAY 3u
#define REMOVABLE_TYPE_LINT 1u
#define DIALOGUE_NONE 0xFFu

static const char system_restored_title[] = "SYSTEM RESTORED";
static const char system_restored_message[] = "All hardware online";

/* -------------------------------------------------------------------------
 * Map lookup helpers
 * ------------------------------------------------------------------------- */

static uint8_t game_map_is_wall_tile(int tile_x, int tile_y)
{
    if (tile_x < 0 || tile_y < 0 ||
        tile_x >= game_get_active_map_width() ||
        tile_y >= game_get_active_map_height()) {
        return 1u;
    }
    return (uint8_t)(game_get_collision_tile((uint8_t)tile_x,
                                             (uint8_t)tile_y) != 0u);
}

static GeneratedMapId_t current_generated_map_id(void)
{
    if (game_state.area_mode == AREA_MODE_OVERWORLD) {
        return GENERATED_MAP_OVERWORLD;
    }
    if (game_state.current_level == LEVEL_SPEAKER) {
        return GENERATED_MAP_SPEAKER;
    }
    if (game_state.current_level == LEVEL_DISPLAY_BOSS) {
        return GENERATED_MAP_DISPLAY;
    }
    return GENERATED_MAP_SPEAKER;
}

static const GeneratedMapData_t* get_active_map_data(void)
{
    return generated_map_get(current_generated_map_id());
}

uint8_t game_get_active_map_width(void)
{
    return get_active_map_data()->width;
}

uint8_t game_get_active_map_height(void)
{
    return get_active_map_data()->height;
}

int16_t game_get_active_world_width(void)
{
    return (int16_t)(game_get_active_map_width() * GAME_TILE_SIZE);
}

int16_t game_get_active_world_height(void)
{
    return (int16_t)(game_get_active_map_height() * GAME_TILE_SIZE);
}

static uint8_t game_get_collision_tile(uint8_t tile_x, uint8_t tile_y)
{
    const GeneratedMapData_t* map = get_active_map_data();

    if (tile_x >= game_get_active_map_width() ||
        tile_y >= game_get_active_map_height()) {
        return 1u;
    }

    return generated_map_tile_at(map, map->collision, tile_x, tile_y);
}

uint8_t game_get_tile(uint8_t tile_x, uint8_t tile_y)
{
    const GeneratedMapData_t* map = get_active_map_data();

    if (tile_x >= game_get_active_map_width() ||
        tile_y >= game_get_active_map_height()) {
        return 1u;
    }

    return generated_map_tile_at(map, map->ground, tile_x, tile_y);
}

uint8_t game_get_object_tile(uint8_t tile_x, uint8_t tile_y)
{
    const GeneratedMapData_t* map = get_active_map_data();

    if (tile_x >= game_get_active_map_width() ||
        tile_y >= game_get_active_map_height()) {
        return 0u;
    }

    return generated_map_tile_at(map, map->objects, tile_x, tile_y);
}

uint8_t game_get_overworld_entrance_tile(uint8_t tile_x, uint8_t tile_y)
{
    const GeneratedMapData_t* map = generated_map_get(GENERATED_MAP_OVERWORLD);

    for (uint8_t i = 0u; i < map->entity_count; i++) {
        if (map->entities[i].type == GENERATED_ENTITY_MODULE_ENTRANCE &&
            tile_x == map->entities[i].tile_x &&
            tile_y == map->entities[i].tile_y) {
            if (map->entities[i].level == LEVEL_SPEAKER) {
                return OVERWORLD_TILE_SPEAKER;
            }

            if (map->entities[i].level == LEVEL_DISPLAY_BOSS) {
                return OVERWORLD_TILE_DISPLAY;
            }
        }
    }

    return OVERWORLD_TILE_EMPTY;
}

const char* game_get_level_name(CampaignLevel_t level)
{
    switch (level) {
    case LEVEL_SPEAKER:
        return "Speaker";

    case LEVEL_DISPLAY_BOSS:
        return "Display";

    default:
        return system_restored_title;
    }
}

/* -------------------------------------------------------------------------
 * Small math / position helpers
 * ------------------------------------------------------------------------- */

static int16_t abs_i16(int16_t value)
{
    return (value < 0) ? (int16_t)-value : value;
}

static int16_t tile_center_to_world(uint8_t tile)
{
    return (int16_t)((tile * GAME_TILE_SIZE) + ((GAME_TILE_SIZE - GAME_PLAYER_SIZE) / 2));
}

static int16_t tile_center_to_world_for_size(uint8_t tile, uint8_t size)
{
    return (int16_t)((tile * GAME_TILE_SIZE) + ((GAME_TILE_SIZE - size) / 2));
}

static void reset_player_to_spawn(void)
{
    GeneratedEntity_t entity;
    const GeneratedMapData_t* map = get_active_map_data();
    uint8_t spawn_x = 1u;
    uint8_t spawn_y = 1u;
    if (generated_map_find_entity(map,
                                  GENERATED_ENTITY_PLAYER_SPAWN,
                                  0u,
                                  &entity) != 0u) {
        spawn_x = entity.tile_x;
        spawn_y = entity.tile_y;
    }
    game_state.player.x = tile_center_to_world(spawn_x);
    game_state.player.y = tile_center_to_world(spawn_y);
    game_state.player.size = GAME_PLAYER_SIZE;
    game_state.player.angle_degrees = 0u;
}

static int16_t tile_to_center(uint8_t tile)
{
    return (int16_t)((tile * GAME_TILE_SIZE) + (GAME_TILE_SIZE / 2));
}

static int16_t entity_center(int16_t position, uint8_t size)
{
    return (int16_t)(position + (size / 2));
}

static uint8_t player_center_tile(uint8_t* tile_x, uint8_t* tile_y)
{
    int16_t player_center_x = entity_center(game_state.player.x, game_state.player.size);
    int16_t player_center_y = entity_center(game_state.player.y, game_state.player.size);

    if (player_center_x < 0 || player_center_y < 0) {
        return 0u;
    }

    *tile_x = (uint8_t)(player_center_x / GAME_TILE_SIZE);
    *tile_y = (uint8_t)(player_center_y / GAME_TILE_SIZE);

    if (*tile_x >= game_get_active_map_width() ||
        *tile_y >= game_get_active_map_height()) {
        return 0u;
    }

    return 1u;
}

/* -------------------------------------------------------------------------
 * Speaker task state
 * ------------------------------------------------------------------------- */

static void clear_removable_objects(void)
{
    game_state.speaker_lint_count = 0u;
    game_state.speaker_lint_removed = 0u;

    for (uint8_t i = 0u; i < MAX_REMOVABLE_OBJECTS; i++) {
        game_state.removable_objects[i].tile_x = 0u;
        game_state.removable_objects[i].tile_y = 0u;
        game_state.removable_objects[i].active = 0u;
        game_state.removable_objects[i].type = 0u;
    }
}

/* -------------------------------------------------------------------------
 * Movement, collision and shooting
 * ------------------------------------------------------------------------- */

static uint8_t game_is_wall_at_world(int16_t world_x, int16_t world_y)
{
    int16_t tile_x;
    int16_t tile_y;

    if (world_x < 0 || world_y < 0) {
        return 1u;
    }
    tile_x = (int16_t)(world_x / GAME_TILE_SIZE);
    tile_y = (int16_t)(world_y / GAME_TILE_SIZE);

    if (tile_x >= (int16_t)game_get_active_map_width() ||
        tile_y >= (int16_t)game_get_active_map_height()) {
        return 1u;
    }

    return (uint8_t)(game_get_collision_tile((uint8_t)tile_x, (uint8_t)tile_y) != 0u);
}

static uint8_t game_player_can_move_to(int16_t x, int16_t y)
{
    int16_t right = (int16_t)(x + game_state.player.size - 1);
    int16_t bottom = (int16_t)(y + game_state.player.size - 1);

    if (x < 0 || y < 0) {
        return 0u;
    }

    if (right >= game_get_active_world_width() || bottom >= game_get_active_world_height()) {
        return 0u;
    }
    return (uint8_t)(game_is_wall_at_world(x, y) == 0u &&
                     game_is_wall_at_world(right, y) == 0u &&
                     game_is_wall_at_world(x, bottom) == 0u &&
                     game_is_wall_at_world(right, bottom) == 0u);
}

static void move_player(const GameInput_t* input)
{
    int16_t next_x = game_state.player.x;
    int16_t next_y = game_state.player.y;

    /*
     * Direct pixel movement is easier to tune for the final demo than the old
     * fixed-point driver path. X and Y are still checked separately so the
     * player can slide along walls.
     */
    if (input->joy1_direction == N) {
        next_y = (int16_t)(next_y - GAME_PLAYER_SPEED);
        game_state.player.angle_degrees = 270u;
    } else if (input->joy1_direction == S) {
        next_y = (int16_t)(next_y + GAME_PLAYER_SPEED);
        game_state.player.angle_degrees = 90u;
    } else if (input->joy1_direction == E) {
        next_x = (int16_t)(next_x + GAME_PLAYER_SPEED);
        game_state.player.angle_degrees = 0u;
    } else if (input->joy1_direction == W) {
        next_x = (int16_t)(next_x - GAME_PLAYER_SPEED);
        game_state.player.angle_degrees = 180u;
    }

    if (game_player_can_move_to(next_x, game_state.player.y) != 0u) {
        game_state.player.x = next_x;
    }

    if (game_player_can_move_to(game_state.player.x, next_y) != 0u) {
        game_state.player.y = next_y;
    }
}
static void init_speaker_task_objective(void)
{
    GeneratedEntity_t entity;
    const GeneratedMapData_t* map = get_active_map_data();

    clear_removable_objects();
    game_state.speaker_task_done = 0u;

    for (uint8_t i = 0u; i < MAX_REMOVABLE_OBJECTS; i++) {
        if (generated_map_find_entity(map,
                                      GENERATED_ENTITY_SPEAKER_LINT,
                                      i,
                                      &entity) == 0u) {
            break;
        }
        game_state.removable_objects[i].tile_x = entity.tile_x;
        game_state.removable_objects[i].tile_y = entity.tile_y;
        game_state.removable_objects[i].active = 1u;
        game_state.removable_objects[i].type = REMOVABLE_TYPE_LINT;
        game_state.speaker_lint_count++;
    }
}

/* -------------------------------------------------------------------------
 * Display boss setup
 * ------------------------------------------------------------------------- */

static void init_boss_dials(void)
{
    const GeneratedMapData_t* map = get_active_map_data();
    GeneratedEntity_t entity;

    static const uint8_t fallback_dials[MAX_BOSS_DIALS][2] = {
        {11u, 21u},
        {15u, 21u},
        {19u, 21u},
        {9u, 2u},
        {13u, 2u},
        {17u, 2u}
    };

    game_state.boss_dial_count = 0u;
    game_state.boss_dials_disabled = 0u;

    for (uint8_t i = 0u; i < MAX_BOSS_DIALS; i++) {
        game_state.boss_dials[i].tile_x = 0u;
        game_state.boss_dials[i].tile_y = 0u;
        game_state.boss_dials[i].active = 0u;
        game_state.boss_dials[i].disabled = 0u;
    }

    for (uint8_t i = 0u; i < DISPLAY_DIAL_COUNT; i++) {
        uint8_t tile_x = fallback_dials[i][0];
        uint8_t tile_y = fallback_dials[i][1];

        if (generated_map_find_entity(map,
                                      GENERATED_ENTITY_BOSS_DIAL,
                                      i,
                                      &entity) != 0u) {
            tile_x = entity.tile_x;
            tile_y = entity.tile_y;
        }
        game_state.boss_dials[i].tile_x = tile_x;
        game_state.boss_dials[i].tile_y = tile_y;
        game_state.boss_dials[i].active = 1u;
        game_state.boss_dials[i].disabled = 0u;
        game_state.boss_dial_count++;
    }
}

static void enter_level_complete_state(void)
{
    game_state.run_state = GAME_STATE_LEVEL_COMPLETE;
    game_state.current_level = LEVEL_COMPLETE;
    game_state.dialogue_title = system_restored_title;
    game_state.dialogue_message = system_restored_message;
}

static void enter_ending_dialogue(void)
{
    game_state.boss.active = 0u;
    game_state.boss.vulnerable = 0u;
    start_story_dialogue(STORY_ENDING);
}

/* -------------------------------------------------------------------------
 * Dialogue
 * ------------------------------------------------------------------------- */

static void set_dialogue_line(void)
{
    if (game_state.dialogue_sequence >= STORY_SEQUENCE_COUNT) {
        return;
    }
    game_state.dialogue_title =
        story_defs[game_state.dialogue_sequence].lines[game_state.dialogue_index].speaker;
    game_state.dialogue_message =
        story_defs[game_state.dialogue_sequence].lines[game_state.dialogue_index].text;
}

static void start_story_dialogue(StorySequenceId_t sequence)
{
    game_state.run_state = GAME_STATE_DIALOGUE;
    game_state.dialogue_sequence = (uint8_t)sequence;
    game_state.dialogue_index = 0u;
    set_dialogue_line();
}

static void start_story_dialogue_once(StorySequenceId_t sequence, StoryFlag_t flag)
{
    if ((game_state.story_flags & flag) != 0u) {
        return;
    }
    game_state.story_flags |= flag;
    start_story_dialogue(sequence);
}

/* -------------------------------------------------------------------------
 * Area transitions and level setup
 * ------------------------------------------------------------------------- */

static void init_boss_for_level(CampaignLevel_t level)
{
    GeneratedEntity_t entity;
    uint8_t boss_tile_x = 20u;
    uint8_t boss_tile_y = 18u;

    if (level == LEVEL_DISPLAY_BOSS &&
        generated_map_find_entity(get_active_map_data(),
                                  GENERATED_ENTITY_BOSS_SPAWN,
                                  0u,
                                  &entity) != 0u) {
        boss_tile_x = entity.tile_x;
        boss_tile_y = entity.tile_y;
    }

    game_state.boss.x = tile_center_to_world_for_size(boss_tile_x, BOSS_SIZE);
    game_state.boss.y = tile_center_to_world_for_size(boss_tile_y, BOSS_SIZE);
    game_state.boss.active = 0u;
    game_state.boss.vulnerable = 0u;
    game_state.display_corruption_ticks = 0u;
    if (level == LEVEL_DISPLAY_BOSS) {
        game_state.boss.active = 1u;
        game_state.boss.vulnerable = 0u;
    }
}

static void start_level(CampaignLevel_t level)
{
    game_state.area_mode = AREA_MODE_COMPONENT_INTERIOR;
    game_state.current_level = level;
    game_state.run_state = GAME_STATE_PLAYING;
    game_state.dialogue_title = game_get_level_name(level);
    game_state.dialogue_message = "";
    clear_removable_objects();
    reset_player_to_spawn();

    if (level == LEVEL_SPEAKER) {
        init_speaker_task_objective();
    } else if (level == LEVEL_DISPLAY_BOSS) {
        init_boss_dials();
    }

    init_boss_for_level(level);
    game_state.speaker_task_done = 0u;

    if (level == LEVEL_SPEAKER) {
        start_story_dialogue_once(STORY_SPEAKER_ENTRY, STORY_FLAG_SPEAKER_ENTRY_DONE);
    } else if (level == LEVEL_DISPLAY_BOSS) {
        start_story_dialogue_once(STORY_DISPLAY_ENTRY, STORY_FLAG_DISPLAY_ENTRY_DONE);
    }
}

static void enter_overworld(void)
{
    game_state.area_mode = AREA_MODE_OVERWORLD;
    game_state.run_state = GAME_STATE_PLAYING;
    game_state.dialogue_title = "Motherboard";
    game_state.dialogue_message = "";
    game_state.boss.active = 0u;
    game_state.boss.vulnerable = 0u;
    game_state.boss_dial_count = 0u;
    game_state.boss_dials_disabled = 0u;
    clear_removable_objects();
    game_state.display_corruption_ticks = 0u;
    game_state.player.x = game_state.overworld_return_x;
    game_state.player.y = game_state.overworld_return_y;
    game_state.player.size = GAME_PLAYER_SIZE;
}

static void reset_campaign_progress(void)
{
    GeneratedEntity_t entity;
    const GeneratedMapData_t* overworld = generated_map_get(GENERATED_MAP_OVERWORLD);

    game_state.speaker_restored = 0u;
    game_state.display_restored = 0u;
    game_state.display_corruption_ticks = 0u;
    game_state.speaker_task_done = 0u;
    clear_removable_objects();
    game_state.boss_dial_count = 0u;
    game_state.boss_dials_disabled = 0u;

    game_state.overworld_return_x = tile_center_to_world(2u);
    game_state.overworld_return_y = tile_center_to_world(3u);

    if (generated_map_find_entity(overworld,
                                  GENERATED_ENTITY_PLAYER_SPAWN,
                                  0u,
                                  &entity) != 0u) {
        game_state.overworld_return_x = tile_center_to_world(entity.tile_x);
        game_state.overworld_return_y = tile_center_to_world(entity.tile_y);
    }
}

/* -------------------------------------------------------------------------
 * Objectives
 * ------------------------------------------------------------------------- */

static void show_level_complete_dialogue(void)
{
    if (game_state.current_level == LEVEL_SPEAKER) {
        game_state.speaker_restored = 1u;
        game_state.speaker_task_done = 1u;
        start_story_dialogue(STORY_SPEAKER_COMPLETE);
        return;
    }

    if (game_state.current_level == LEVEL_DISPLAY_BOSS) {
        game_state.display_restored = 1u;
        game_state.boss.vulnerable = 1u;
        game_state.story_flags |= STORY_FLAG_BOSS_VULN_DONE;
        start_story_dialogue(STORY_BOSS_VULNERABLE);
        return;
    }
}

static uint8_t player_is_near_removable_object(const RemovableObject_t* object)
{
    int16_t player_center_x = entity_center(game_state.player.x, game_state.player.size);
    int16_t player_center_y = entity_center(game_state.player.y, game_state.player.size);
    int16_t object_center_x = tile_to_center(object->tile_x);
    int16_t object_center_y = tile_to_center(object->tile_y);

    return (uint8_t)(abs_i16((int16_t)(player_center_x - object_center_x)) <= FAULT_REPAIR_DISTANCE &&
                     abs_i16((int16_t)(player_center_y - object_center_y)) <= FAULT_REPAIR_DISTANCE);
}

static uint8_t update_removable_objects(const GameInput_t* input)
{
    if (game_state.current_level != LEVEL_SPEAKER ||
        game_state.area_mode != AREA_MODE_COMPONENT_INTERIOR) {
        return 0u;
    }
    for (uint8_t i = 0u; i < game_state.speaker_lint_count; i++) {
        RemovableObject_t* object = &game_state.removable_objects[i];

        if (object->active == 0u || object->type != REMOVABLE_TYPE_LINT) {
            continue;
        }
        if (player_is_near_removable_object(object) == 0u) {
            continue;
        }
        if (input->action_pressed == 0u) {
            return 1u;
        }

        object->active = 0u;

        if (game_state.speaker_lint_removed < game_state.speaker_lint_count) {
            game_state.speaker_lint_removed++;
        }

        if (game_state.speaker_lint_removed >= game_state.speaker_lint_count &&
            game_state.speaker_restored == 0u) {
            game_state.speaker_task_done = 1u;
            show_level_complete_dialogue();
        }
        return 1u;
    }
    return 0u;
}

static uint8_t player_is_near_boss_dial(const BossDial_t* dial)
{
    int16_t player_center_x = entity_center(game_state.player.x, game_state.player.size);
    int16_t player_center_y = entity_center(game_state.player.y, game_state.player.size);
    int16_t dial_center_x = tile_to_center(dial->tile_x);
    int16_t dial_center_y = tile_to_center(dial->tile_y);

    return (uint8_t)(abs_i16((int16_t)(player_center_x - dial_center_x)) <= FAULT_REPAIR_DISTANCE &&
                     abs_i16((int16_t)(player_center_y - dial_center_y)) <= FAULT_REPAIR_DISTANCE);
}

static uint8_t update_boss_dials(const GameInput_t* input)
{
    if (game_state.current_level != LEVEL_DISPLAY_BOSS ||
        game_state.area_mode != AREA_MODE_COMPONENT_INTERIOR) {
        return 0u;
    }
    for (uint8_t i = 0u; i < game_state.boss_dial_count; i++) {
        BossDial_t* dial = &game_state.boss_dials[i];

        if (dial->active == 0u || dial->disabled != 0u) {
            continue;
        }
        if (player_is_near_boss_dial(dial) == 0u) {
            continue;
        }
        if (input->action_pressed == 0u) {
            return 1u;
        }

        dial->disabled = 1u;
        game_state.boss_dials_disabled++;

        if (game_state.boss_dials_disabled >= game_state.boss_dial_count &&
            game_state.display_restored == 0u) {
            show_level_complete_dialogue();
        }
        return 1u;
    }
    return 0u;
}

static uint8_t boss_is_in_shot(void)
{
    int16_t player_center_x = entity_center(game_state.player.x, game_state.player.size);
    int16_t player_center_y = entity_center(game_state.player.y, game_state.player.size);
    int16_t boss_center_x = entity_center(game_state.boss.x, BOSS_SIZE);
    int16_t boss_center_y = entity_center(game_state.boss.y, BOSS_SIZE);
    int16_t dx = (int16_t)(boss_center_x - player_center_x);
    int16_t dy = (int16_t)(boss_center_y - player_center_y);

    /*
     * Simple final-demo shot check:
     * the boss has to be roughly in the direction the player is facing and
     * within range. No cone maths or projectile simulation.
     */
    switch (game_state.player.angle_degrees) {
    case 0u:
        return (uint8_t)(dx > 0 && dx <= SHOT_RANGE && abs_i16(dy) <= BOSS_SIZE);

    case 90u:
        return (uint8_t)(dy > 0 && dy <= SHOT_RANGE && abs_i16(dx) <= BOSS_SIZE);

    case 180u:
        return (uint8_t)(dx < 0 && abs_i16(dx) <= SHOT_RANGE && abs_i16(dy) <= BOSS_SIZE);

    case 270u:
        return (uint8_t)(dy < 0 && abs_i16(dy) <= SHOT_RANGE && abs_i16(dx) <= BOSS_SIZE);

    default:
        return 0u;
    }
}

static void fire_shot(void)
{
    if (game_state.boss.active != 0u && game_state.boss.vulnerable == 0u) {
        if (boss_is_in_shot() != 0u) {
            start_story_dialogue_once(STORY_BOSS_INVULNERABLE, STORY_FLAG_BOSS_INVULN_DONE);
        }
        return;
    }

    if (game_state.boss.active != 0u && game_state.boss.vulnerable != 0u) {
        if (boss_is_in_shot() != 0u) {
            enter_ending_dialogue();
        }
    }
}

static void update_display_corruption(void)
{
    if (game_state.current_level == LEVEL_DISPLAY_BOSS &&
        game_state.boss.active != 0u &&
        game_state.boss.vulnerable == 0u) {
        if (game_state.display_corruption_ticks > 0u) {
            game_state.display_corruption_ticks--;
        } else if ((game_state.frame_count % DISPLAY_CORRUPTION_INTERVAL_FRAMES) == 0u) {
            game_state.display_corruption_ticks = DISPLAY_CORRUPTION_TICKS;
        }

    } else {
        game_state.display_corruption_ticks = 0u;
    }
}

static void update_shooting(const GameInput_t* input, uint8_t action_consumed)
{
    if (action_consumed != 0u) {
        return;
    }

    if (input->action_pressed != 0u) {
        fire_shot();
    }
}

/* -------------------------------------------------------------------------
 * Overworld entrance / module exit checks
 * ------------------------------------------------------------------------- */

static uint8_t entrance_tile_to_level(uint8_t tile_value, CampaignLevel_t* level)
{
    if (tile_value == OVERWORLD_TILE_SPEAKER &&
        game_state.speaker_restored == 0u) {
        *level = LEVEL_SPEAKER;
        return 1u;
    }

    if (tile_value == OVERWORLD_TILE_DISPLAY &&
        game_state.speaker_restored != 0u &&
        game_state.current_level != LEVEL_COMPLETE) {
        *level = LEVEL_DISPLAY_BOSS;
        return 1u;
    }
    return 0u;
}

static uint8_t find_current_overworld_entrance(CampaignLevel_t* level)
{
    int16_t player_center_x = entity_center(game_state.player.x, game_state.player.size);
    int16_t player_center_y = entity_center(game_state.player.y, game_state.player.size);
    uint8_t tile_x;
    uint8_t tile_y;

    if (player_center_x < 0 || player_center_y < 0) {
        return 0u;
    }

    tile_x = (uint8_t)(player_center_x / GAME_TILE_SIZE);
    tile_y = (uint8_t)(player_center_y / GAME_TILE_SIZE);

    if (tile_x >= game_get_active_map_width() || tile_y >= game_get_active_map_height()) {
        return 0u;
    }

    return entrance_tile_to_level(game_get_overworld_entrance_tile(tile_x, tile_y), level);
}

static void set_return_spawn(void)
{
    static const int8_t return_offsets[4][2] = {
        {0, 1},
        {1, 0},
        {0, -1},
        {-1, 0}
    };

    game_state.overworld_return_x = game_state.player.x;
    game_state.overworld_return_y = game_state.player.y;

    for (uint8_t i = 0u; i < 4u; i++) {
        int16_t candidate_x = (int16_t)(game_state.player.x +
                                        (return_offsets[i][0] * GAME_TILE_SIZE));
        int16_t candidate_y = (int16_t)(game_state.player.y +
                                        (return_offsets[i][1] * GAME_TILE_SIZE));
        if (game_player_can_move_to(candidate_x, candidate_y) != 0u) {
            game_state.overworld_return_x = candidate_x;
            game_state.overworld_return_y = candidate_y;
            return;
        }
    }
}

static uint8_t module_exit_hit(void)
{
    const GeneratedMapData_t* map = get_active_map_data();
    uint8_t tile_x;
    uint8_t tile_y;

    if (game_state.area_mode != AREA_MODE_COMPONENT_INTERIOR) {
        return 0u;
    }

    if (player_center_tile(&tile_x, &tile_y) == 0u) {
        return 0u;
    }

    for (uint8_t i = 0u; i < map->entity_count; i++) {
        const GeneratedEntity_t* entity = &map->entities[i];

        if (entity->type != GENERATED_ENTITY_MODULE_EXIT &&
            entity->type != GENERATED_ENTITY_MODULE_ENTRANCE) {
            continue;
        }

        if (abs_i16((int16_t)(tile_x - entity->tile_x)) <= 1 &&
            abs_i16((int16_t)(tile_y - entity->tile_y)) <= 1) {
            return 1u;
        }
    }
    if (game_state.current_level == LEVEL_SPEAKER) {
        return (uint8_t)(tile_x <= 1u);
    }

    if (game_state.current_level == LEVEL_DISPLAY_BOSS) {
        return (uint8_t)(tile_x >= 23u && tile_y >= 27u);
    }
    return 0u;
}

static void update_overworld(const GameInput_t* input)
{
    CampaignLevel_t level;

    move_player(input);

    if (find_current_overworld_entrance(&level) != 0u) {
        set_return_spawn();
        start_level(level);
    }
}
static void advance_dialogue(void)
{
    if (game_state.dialogue_sequence < STORY_SEQUENCE_COUNT) {
        game_state.dialogue_index++;

        if (game_state.dialogue_index < story_defs[game_state.dialogue_sequence].length) {
            set_dialogue_line();
            return;
        }

        /*
         * The opening is chained in three short parts so the player sees the
         * lab setup before being dropped into the circuit.
         */
        if (game_state.dialogue_sequence == STORY_LAB_INTRO) {
            start_story_dialogue(STORY_INTRO);
            return;
        }

        if (game_state.dialogue_sequence == STORY_INTRO) {
            game_state.story_flags |= STORY_FLAG_INTRO_DONE;
            start_story_dialogue(STORY_BOARD_ENTRY);
            return;
        }

        if (game_state.dialogue_sequence == STORY_BOARD_ENTRY) {
            game_state.story_flags |= STORY_FLAG_BOARD_DONE;
            game_state.run_state = GAME_STATE_PLAYING;
            game_state.dialogue_sequence = DIALOGUE_NONE;
            return;
        }

        if (game_state.dialogue_sequence == STORY_ENDING) {
            game_state.dialogue_sequence = DIALOGUE_NONE;
            enter_level_complete_state();
            return;
        }

        game_state.run_state = GAME_STATE_PLAYING;
        game_state.dialogue_sequence = DIALOGUE_NONE;
        game_state.dialogue_title = game_get_level_name(game_state.current_level);
        game_state.dialogue_message = "";
        return;
    }
}

/* -------------------------------------------------------------------------
 * Public game API
 * ------------------------------------------------------------------------- */

void game_init(void)
{
    game_state.frame_count = 0u;
    game_state.dialogue_sequence = DIALOGUE_NONE;
    game_state.dialogue_index = 0u;
    game_state.story_flags = 0u;
    game_state.last_input.move_x = 0;
    game_state.last_input.move_y = 0;
    game_state.last_input.action_down = 0u;
    game_state.last_input.action_pressed = 0u;
    game_state.last_input.joy1_direction = CENTRE;
    game_state.last_input.joy2_direction = CENTRE;

    /*
     * Player.c owns movement, but the wall lookup depends on whichever Tiled
     * map is currently active. This callback connects those two pieces.
     */
    Map_Set_Wall_Check(game_map_is_wall_tile);

    reset_campaign_progress();
    game_state.current_level = LEVEL_SPEAKER;
    enter_overworld();
    start_story_dialogue(STORY_LAB_INTRO);
}

void update_game(void)
{
    const GameInput_t* input = input_get_state();

    game_state.frame_count++;
    game_state.last_input = *input;

    if (game_state.run_state == GAME_STATE_PLAYING) {
        uint8_t action_used = 0u;

        if (game_state.area_mode == AREA_MODE_OVERWORLD) {
            update_overworld(input);
            return;
        }

        update_display_corruption();
        move_player(input);

        if (module_exit_hit() != 0u) {
            enter_overworld();
            return;
        }

        if (update_removable_objects(input) != 0u) {
            action_used = 1u;
        }

        if (update_boss_dials(input) != 0u) {
            action_used = 1u;
        }

        update_shooting(input, action_used);

        if (game_state.run_state != GAME_STATE_PLAYING) {
            return;
        }

        if (game_state.current_level == LEVEL_SPEAKER &&
            game_state.speaker_restored == 0u &&
            game_state.speaker_lint_count != 0u &&
            game_state.speaker_lint_removed >= game_state.speaker_lint_count) {
            show_level_complete_dialogue();
        }

    } else if (game_state.run_state == GAME_STATE_DIALOGUE) {
        if (input->action_pressed != 0u) {
            advance_dialogue();
        }

    } else {
        if (input->action_pressed != 0u) {
            reset_campaign_progress();
            game_state.dialogue_sequence = DIALOGUE_NONE;
            game_state.dialogue_index = 0u;
            game_state.story_flags = 0u;
            game_state.current_level = LEVEL_SPEAKER;
            enter_overworld();
            start_story_dialogue(STORY_LAB_INTRO);
        }
    }
}

const GameState_t* game_get_state(void)
{
    return &game_state;
}
