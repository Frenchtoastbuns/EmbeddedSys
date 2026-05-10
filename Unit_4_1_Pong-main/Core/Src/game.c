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

static void level_done_talk(void);
static uint8_t wall_at(uint8_t tile_x, uint8_t tile_y);
static void talk(StorySequenceId_t sequence);

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

/* checks if this map tile should block the player */
static uint8_t wall_tile(int tile_x, int tile_y)
{
    if (tile_x < 0 || tile_y < 0 ||
        tile_x >= map_w() ||
        tile_y >= map_h()) {
        return 1u;
    }
    return (uint8_t)(wall_at((uint8_t)tile_x,
                                             (uint8_t)tile_y) != 0u);
}

/* picks which generated map the game is using right now */
static GeneratedMapId_t which_map(void)
{
    if (game_state.area == AREA_MODE_OVERWORLD) {
        return GENERATED_MAP_OVERWORLD;
    }
    if (game_state.level == LEVEL_SPEAKER) {
        return GENERATED_MAP_SPEAKER;
    }
    if (game_state.level == LEVEL_DISPLAY_BOSS) {
        return GENERATED_MAP_DISPLAY;
    }
    return GENERATED_MAP_SPEAKER;
}

/* gets the active map data */
static const GeneratedMapData_t* map_now(void)
{
    return generated_map_get(which_map());
}

/* gives render.c the current map width */
uint8_t map_w(void)
{
    return map_now()->width;
}

/* gives render.c the current map height */
uint8_t map_h(void)
{
    return map_now()->height;
}

/* gets the map width in pixels */
int16_t world_w(void)
{
    return (int16_t)(map_w() * GAME_TILE_SIZE);
}

/* gets the map height in pixels */
int16_t world_h(void)
{
    return (int16_t)(map_h() * GAME_TILE_SIZE);
}

/* reads the collision layer at one tile */
static uint8_t wall_at(uint8_t tile_x, uint8_t tile_y)
{
    const GeneratedMapData_t* map = map_now();

    if (tile_x >= map_w() ||
        tile_y >= map_h()) {
        return 1u;
    }

    return generated_map_tile_at(map, map->collision, tile_x, tile_y);
}

/* reads the ground tile for drawing */
uint8_t ground_at(uint8_t tile_x, uint8_t tile_y)
{
    const GeneratedMapData_t* map = map_now();

    if (tile_x >= map_w() ||
        tile_y >= map_h()) {
        return 1u;
    }

    return generated_map_tile_at(map, map->ground, tile_x, tile_y);
}

/* reads the object layer tile for drawing */
uint8_t thing_at(uint8_t tile_x, uint8_t tile_y)
{
    const GeneratedMapData_t* map = map_now();

    if (tile_x >= map_w() ||
        tile_y >= map_h()) {
        return 0u;
    }

    return generated_map_tile_at(map, map->objects, tile_x, tile_y);
}

/* checks if an overworld tile is a module door */
uint8_t door_at(uint8_t tile_x, uint8_t tile_y)
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

/* returns the short name for the current level */
const char* get_level_name(CampaignLevel_t level)
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

/* small absolute value helper for signed 16 bit numbers */
static int16_t abs16(int16_t value)
{
    return (value < 0) ? (int16_t)-value : value;
}

/* converts a tile position into a player pixel position */
static int16_t tile_pixel(uint8_t tile)
{
    return (int16_t)((tile * GAME_TILE_SIZE) + ((GAME_TILE_SIZE - GAME_PLAYER_SIZE) / 2));
}

/* converts a tile position for bigger sprites like the boss */
static int16_t tile_to_pixel_for(uint8_t tile, uint8_t size)
{
    return (int16_t)((tile * GAME_TILE_SIZE) + ((GAME_TILE_SIZE - size) / 2));
}

/* moves the player to the spawn point for the current map */
static void put_player_here(void)
{
    GeneratedEntity_t entity;
    const GeneratedMapData_t* map = map_now();
    uint8_t spawn_x = 1u;
    uint8_t spawn_y = 1u;
    if (generated_map_find_entity(map,
                                  GENERATED_ENTITY_PLAYER_SPAWN,
                                  0u,
                                  &entity) != 0u) {
        spawn_x = entity.tile_x;
        spawn_y = entity.tile_y;
    }
    game_state.player.x = tile_pixel(spawn_x);
    game_state.player.y = tile_pixel(spawn_y);
    game_state.player.size = GAME_PLAYER_SIZE;
    game_state.player.angle_degrees = 0u;
}

/* gets the middle pixel of a tile */
static int16_t tile_middle(uint8_t tile)
{
    return (int16_t)((tile * GAME_TILE_SIZE) + (GAME_TILE_SIZE / 2));
}

/* gets the centre of a small sprite or object */
static int16_t mid(int16_t position, uint8_t size)
{
    return (int16_t)(position + (size / 2));
}

/* finds the tile under the player centre */
static uint8_t player_spot(uint8_t* tile_x, uint8_t* tile_y)
{
    int16_t player_x = mid(game_state.player.x, game_state.player.size);
    int16_t player_y = mid(game_state.player.y, game_state.player.size);

    if (player_x < 0 || player_y < 0) {
        return 0u;
    }

    *tile_x = (uint8_t)(player_x / GAME_TILE_SIZE);
    *tile_y = (uint8_t)(player_y / GAME_TILE_SIZE);

    if (*tile_x >= map_w() ||
        *tile_y >= map_h()) {
        return 0u;
    }

    return 1u;
}

/* -------------------------------------------------------------------------
 * Speaker task state
 * ------------------------------------------------------------------------- */

/* resets the speaker lint list before loading a level */
static void clear_lint_list(void)
{
    game_state.lint_total = 0u;
    game_state.lint_done = 0u;

    for (uint8_t i = 0u; i < MAX_REMOVABLE_OBJECTS; i++) {
        game_state.lint[i].tile_x = 0u;
        game_state.lint[i].tile_y = 0u;
        game_state.lint[i].active = 0u;
        game_state.lint[i].type = 0u;
    }
}

/* -------------------------------------------------------------------------
 * Movement, collision and shooting
 * ------------------------------------------------------------------------- */

/* checks collision using a pixel position */
static uint8_t wall_pixel(int16_t world_x, int16_t world_y)
{
    int16_t tile_x;
    int16_t tile_y;

    if (world_x < 0 || world_y < 0) {
        return 1u;
    }
    tile_x = (int16_t)(world_x / GAME_TILE_SIZE);
    tile_y = (int16_t)(world_y / GAME_TILE_SIZE);

    if (tile_x >= (int16_t)map_w() ||
        tile_y >= (int16_t)map_h()) {
        return 1u;
    }

    return (uint8_t)(wall_at((uint8_t)tile_x, (uint8_t)tile_y) != 0u);
}

/* checks all corners before the player moves */
static uint8_t ok_to_move(int16_t x, int16_t y)
{
    int16_t right = (int16_t)(x + game_state.player.size - 1);
    int16_t bottom = (int16_t)(y + game_state.player.size - 1);

    if (x < 0 || y < 0) {
        return 0u;
    }

    if (right >= world_w() || bottom >= world_h()) {
        return 0u;
    }
    return (uint8_t)(wall_pixel(x, y) == 0u &&
                     wall_pixel(right, y) == 0u &&
                     wall_pixel(x, bottom) == 0u &&
                     wall_pixel(right, bottom) == 0u);
}

/* moves the player one step using joystick input */
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

    if (ok_to_move(next_x, game_state.player.y) != 0u) {
        game_state.player.x = next_x;
    }

    if (ok_to_move(game_state.player.x, next_y) != 0u) {
        game_state.player.y = next_y;
    }
}
/* loads the lint objects for the speaker level */
static void make_speaker_level(void)
{
    GeneratedEntity_t entity;
    const GeneratedMapData_t* map = map_now();

    clear_lint_list();
    game_state.speaker_task_done = 0u;

    for (uint8_t i = 0u; i < MAX_REMOVABLE_OBJECTS; i++) {
        if (generated_map_find_entity(map,
                                      GENERATED_ENTITY_SPEAKER_LINT,
                                      i,
                                      &entity) == 0u) {
            break;
        }
        game_state.lint[i].tile_x = entity.tile_x;
        game_state.lint[i].tile_y = entity.tile_y;
        game_state.lint[i].active = 1u;
        game_state.lint[i].type = REMOVABLE_TYPE_LINT;
        game_state.lint_total++;
    }
}

/* -------------------------------------------------------------------------
 * Display boss setup
 * ------------------------------------------------------------------------- */

/* loads the six boss dials for the display level */
static void make_dials(void)
{
    const GeneratedMapData_t* map = map_now();
    GeneratedEntity_t entity;

    static const uint8_t fallback_dials[MAX_BOSS_DIALS][2] = {
        {11u, 21u},
        {15u, 21u},
        {19u, 21u},
        {9u, 2u},
        {13u, 2u},
        {17u, 2u}
    };

    game_state.dial_total = 0u;
    game_state.dial_done = 0u;

    for (uint8_t i = 0u; i < MAX_BOSS_DIALS; i++) {
        game_state.dials[i].tile_x = 0u;
        game_state.dials[i].tile_y = 0u;
        game_state.dials[i].active = 0u;
        game_state.dials[i].disabled = 0u;
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
        game_state.dials[i].tile_x = tile_x;
        game_state.dials[i].tile_y = tile_y;
        game_state.dials[i].active = 1u;
        game_state.dials[i].disabled = 0u;
        game_state.dial_total++;
    }
}

/* switches to the final restored screen */
static void show_win(void)
{
    game_state.screen = GAME_STATE_LEVEL_COMPLETE;
    game_state.level = LEVEL_COMPLETE;
    game_state.talk_title = system_restored_title;
    game_state.talk_text = system_restored_message;
}

/* starts the ending dialogue after the boss is beaten */
static void end_talk(void)
{
    game_state.boss.active = 0u;
    game_state.boss.vulnerable = 0u;
    talk(STORY_ENDING);
}

/* -------------------------------------------------------------------------
 * Dialogue
 * ------------------------------------------------------------------------- */

/* copies the current dialogue line into the game state */
static void set_talk(void)
{
    if (game_state.talk_id >= STORY_SEQUENCE_COUNT) {
        return;
    }
    game_state.talk_title =
        story_defs[game_state.talk_id].lines[game_state.talk_line].speaker;
    game_state.talk_text =
        story_defs[game_state.talk_id].lines[game_state.talk_line].text;
}

/* starts a dialogue sequence from the first line */
static void talk(StorySequenceId_t sequence)
{
    game_state.screen = GAME_STATE_DIALOGUE;
    game_state.talk_id = (uint8_t)sequence;
    game_state.talk_line = 0u;
    set_talk();
}

/* starts dialogue only if it has not already played */
static void talk_once(StorySequenceId_t sequence, StoryFlag_t flag)
{
    if ((game_state.story_done & flag) != 0u) {
        return;
    }
    game_state.story_done |= flag;
    talk(sequence);
}

/* -------------------------------------------------------------------------
 * Area transitions and level setup
 * ------------------------------------------------------------------------- */

/* places the boss in the display map */
static void make_boss(CampaignLevel_t level)
{
    GeneratedEntity_t entity;
    uint8_t boss_tile_x = 20u;
    uint8_t boss_tile_y = 18u;

    if (level == LEVEL_DISPLAY_BOSS &&
        generated_map_find_entity(map_now(),
                                  GENERATED_ENTITY_BOSS_SPAWN,
                                  0u,
                                  &entity) != 0u) {
        boss_tile_x = entity.tile_x;
        boss_tile_y = entity.tile_y;
    }

    game_state.boss.x = tile_to_pixel_for(boss_tile_x, BOSS_SIZE);
    game_state.boss.y = tile_to_pixel_for(boss_tile_y, BOSS_SIZE);
    game_state.boss.active = 0u;
    game_state.boss.vulnerable = 0u;
    game_state.glitch_time = 0u;
    if (level == LEVEL_DISPLAY_BOSS) {
        game_state.boss.active = 1u;
        game_state.boss.vulnerable = 0u;
    }
}

/* enters one of the module levels */
static void go_level(CampaignLevel_t level)
{
    game_state.area = AREA_MODE_COMPONENT_INTERIOR;
    game_state.level = level;
    game_state.screen = GAME_STATE_PLAYING;
    game_state.talk_title = get_level_name(level);
    game_state.talk_text = "";
    clear_lint_list();
    put_player_here();

    if (level == LEVEL_SPEAKER) {
        make_speaker_level();
    } else if (level == LEVEL_DISPLAY_BOSS) {
        make_dials();
    }

    make_boss(level);
    game_state.speaker_task_done = 0u;

    if (level == LEVEL_SPEAKER) {
        talk_once(STORY_SPEAKER_ENTRY, STORY_FLAG_SPEAKER_ENTRY_DONE);
    } else if (level == LEVEL_DISPLAY_BOSS) {
        talk_once(STORY_DISPLAY_ENTRY, STORY_FLAG_DISPLAY_ENTRY_DONE);
    }
}

/* returns the player back to the overworld board */
static void go_board(void)
{
    game_state.area = AREA_MODE_OVERWORLD;
    game_state.screen = GAME_STATE_PLAYING;
    game_state.talk_title = "Motherboard";
    game_state.talk_text = "";
    game_state.boss.active = 0u;
    game_state.boss.vulnerable = 0u;
    game_state.dial_total = 0u;
    game_state.dial_done = 0u;
    clear_lint_list();
    game_state.glitch_time = 0u;
    game_state.player.x = game_state.return_x;
    game_state.player.y = game_state.return_y;
    game_state.player.size = GAME_PLAYER_SIZE;
}

/* resets the main progress flags for a new run */
static void reset_stuff(void)
{
    GeneratedEntity_t entity;
    const GeneratedMapData_t* overworld = generated_map_get(GENERATED_MAP_OVERWORLD);

    game_state.speaker_done = 0u;
    game_state.display_done = 0u;
    game_state.glitch_time = 0u;
    game_state.speaker_task_done = 0u;
    clear_lint_list();
    game_state.dial_total = 0u;
    game_state.dial_done = 0u;

    game_state.return_x = tile_pixel(2u);
    game_state.return_y = tile_pixel(3u);

    if (generated_map_find_entity(overworld,
                                  GENERATED_ENTITY_PLAYER_SPAWN,
                                  0u,
                                  &entity) != 0u) {
        game_state.return_x = tile_pixel(entity.tile_x);
        game_state.return_y = tile_pixel(entity.tile_y);
    }
}

/* -------------------------------------------------------------------------
 * Objectives
 * ------------------------------------------------------------------------- */

/* marks a level done and starts its completion dialogue */
static void level_done_talk(void)
{
    if (game_state.level == LEVEL_SPEAKER) {
        game_state.speaker_done = 1u;
        game_state.speaker_task_done = 1u;
        talk(STORY_SPEAKER_COMPLETE);
        return;
    }

    if (game_state.level == LEVEL_DISPLAY_BOSS) {
        game_state.display_done = 1u;
        game_state.boss.vulnerable = 1u;
        game_state.story_done |= STORY_FLAG_BOSS_VULN_DONE;
        talk(STORY_BOSS_VULNERABLE);
        return;
    }
}

/* checks if the player is close enough to clean lint */
static uint8_t by_lint(const Lint_t* object)
{
    int16_t player_x = mid(game_state.player.x, game_state.player.size);
    int16_t player_y = mid(game_state.player.y, game_state.player.size);
    int16_t lint_x = tile_middle(object->tile_x);
    int16_t lint_y = tile_middle(object->tile_y);

    return (uint8_t)(abs16((int16_t)(player_x - lint_x)) <= FAULT_REPAIR_DISTANCE &&
                     abs16((int16_t)(player_y - lint_y)) <= FAULT_REPAIR_DISTANCE);
}

/* handles pressing action near lint pieces */
static uint8_t do_lint(const GameInput_t* input)
{
    if (game_state.level != LEVEL_SPEAKER ||
        game_state.area != AREA_MODE_COMPONENT_INTERIOR) {
        return 0u;
    }
    for (uint8_t i = 0u; i < game_state.lint_total; i++) {
        Lint_t* object = &game_state.lint[i];

        if (object->active == 0u || object->type != REMOVABLE_TYPE_LINT) {
            continue;
        }
        if (by_lint(object) == 0u) {
            continue;
        }
        if (input->action_pressed == 0u) {
            return 1u;
        }

        object->active = 0u;

        if (game_state.lint_done < game_state.lint_total) {
            game_state.lint_done++;
        }

        if (game_state.lint_done >= game_state.lint_total &&
            game_state.speaker_done == 0u) {
            game_state.speaker_task_done = 1u;
            level_done_talk();
        }
        return 1u;
    }
    return 0u;
}

/* checks if the player is close enough to a boss dial */
static uint8_t by_dial(const Dial_t* dial)
{
    int16_t player_x = mid(game_state.player.x, game_state.player.size);
    int16_t player_y = mid(game_state.player.y, game_state.player.size);
    int16_t dial_x = tile_middle(dial->tile_x);
    int16_t dial_y = tile_middle(dial->tile_y);

    return (uint8_t)(abs16((int16_t)(player_x - dial_x)) <= FAULT_REPAIR_DISTANCE &&
                     abs16((int16_t)(player_y - dial_y)) <= FAULT_REPAIR_DISTANCE);
}

/* handles turning red boss dials green */
static uint8_t do_dials(const GameInput_t* input)
{
    if (game_state.level != LEVEL_DISPLAY_BOSS ||
        game_state.area != AREA_MODE_COMPONENT_INTERIOR) {
        return 0u;
    }
    for (uint8_t i = 0u; i < game_state.dial_total; i++) {
        Dial_t* dial = &game_state.dials[i];

        if (dial->active == 0u || dial->disabled != 0u) {
            continue;
        }
        if (by_dial(dial) == 0u) {
            continue;
        }
        if (input->action_pressed == 0u) {
            return 1u;
        }

        dial->disabled = 1u;
        game_state.dial_done++;

        if (game_state.dial_done >= game_state.dial_total &&
            game_state.display_done == 0u) {
            level_done_talk();
        }
        return 1u;
    }
    return 0u;
}

/* checks if the boss is in front of the player shot */
static uint8_t hit_boss(void)
{
    int16_t player_x = mid(game_state.player.x, game_state.player.size);
    int16_t player_y = mid(game_state.player.y, game_state.player.size);
    int16_t boss_x = mid(game_state.boss.x, BOSS_SIZE);
    int16_t boss_y = mid(game_state.boss.y, BOSS_SIZE);
    int16_t dx = (int16_t)(boss_x - player_x);
    int16_t dy = (int16_t)(boss_y - player_y);

    /*
     * Simple final-demo shot check:
     * the boss has to be roughly in the direction the player is facing and
     * within range. No cone maths or projectile simulation.
     */
    switch (game_state.player.angle_degrees) {
    case 0u:
        return (uint8_t)(dx > 0 && dx <= SHOT_RANGE && abs16(dy) <= BOSS_SIZE);

    case 90u:
        return (uint8_t)(dy > 0 && dy <= SHOT_RANGE && abs16(dx) <= BOSS_SIZE);

    case 180u:
        return (uint8_t)(dx < 0 && abs16(dx) <= SHOT_RANGE && abs16(dy) <= BOSS_SIZE);

    case 270u:
        return (uint8_t)(dy < 0 && abs16(dy) <= SHOT_RANGE && abs16(dx) <= BOSS_SIZE);

    default:
        return 0u;
    }
}

/* handles the action button when it is used to shoot */
static void shoot_now(void)
{
    if (game_state.boss.active != 0u && game_state.boss.vulnerable == 0u) {
        if (hit_boss() != 0u) {
            talk_once(STORY_BOSS_INVULNERABLE, STORY_FLAG_BOSS_INVULN_DONE);
        }
        return;
    }

    if (game_state.boss.active != 0u && game_state.boss.vulnerable != 0u) {
        if (hit_boss() != 0u) {
            end_talk();
        }
    }
}

/* runs the simple display glitch timer */
static void do_glitch(void)
{
    if (game_state.level == LEVEL_DISPLAY_BOSS &&
        game_state.boss.active != 0u &&
        game_state.boss.vulnerable == 0u) {
        if (game_state.glitch_time > 0u) {
            game_state.glitch_time--;
        } else if ((game_state.frame % DISPLAY_CORRUPTION_INTERVAL_FRAMES) == 0u) {
            game_state.glitch_time = DISPLAY_CORRUPTION_TICKS;
        }

    } else {
        game_state.glitch_time = 0u;
    }
}

/* decides if action should fire a shot this frame */
static void do_shoot(const GameInput_t* input, uint8_t action_consumed)
{
    if (action_consumed != 0u) {
        return;
    }

    if (input->action_pressed != 0u) {
        shoot_now();
    }
}

/* -------------------------------------------------------------------------
 * Overworld entrance / module exit checks
 * ------------------------------------------------------------------------- */

/* converts a door tile into a campaign level */
static uint8_t door_level(uint8_t tile_value, CampaignLevel_t* level)
{
    if (tile_value == OVERWORLD_TILE_SPEAKER &&
        game_state.speaker_done == 0u) {
        *level = LEVEL_SPEAKER;
        return 1u;
    }

    if (tile_value == OVERWORLD_TILE_DISPLAY &&
        game_state.speaker_done != 0u &&
        game_state.level != LEVEL_COMPLETE) {
        *level = LEVEL_DISPLAY_BOSS;
        return 1u;
    }
    return 0u;
}

/* checks the overworld door under the player */
static uint8_t door_check(CampaignLevel_t* level)
{
    int16_t player_x = mid(game_state.player.x, game_state.player.size);
    int16_t player_y = mid(game_state.player.y, game_state.player.size);
    uint8_t tile_x;
    uint8_t tile_y;

    if (player_x < 0 || player_y < 0) {
        return 0u;
    }

    tile_x = (uint8_t)(player_x / GAME_TILE_SIZE);
    tile_y = (uint8_t)(player_y / GAME_TILE_SIZE);

    if (tile_x >= map_w() || tile_y >= map_h()) {
        return 0u;
    }

    return door_level(door_at(tile_x, tile_y), level);
}

/* saves where the player should return on the board */
static void save_spot(void)
{
    static const int8_t return_offsets[4][2] = {
        {0, 1},
        {1, 0},
        {0, -1},
        {-1, 0}
    };

    game_state.return_x = game_state.player.x;
    game_state.return_y = game_state.player.y;

    for (uint8_t i = 0u; i < 4u; i++) {
        int16_t try_x = (int16_t)(game_state.player.x +
                                        (return_offsets[i][0] * GAME_TILE_SIZE));
        int16_t try_y = (int16_t)(game_state.player.y +
                                        (return_offsets[i][1] * GAME_TILE_SIZE));
        if (ok_to_move(try_x, try_y) != 0u) {
            game_state.return_x = try_x;
            game_state.return_y = try_y;
            return;
        }
    }
}

/* checks if the player is standing on a module exit */
static uint8_t exit_hit(void)
{
    const GeneratedMapData_t* map = map_now();
    uint8_t tile_x;
    uint8_t tile_y;

    if (game_state.area != AREA_MODE_COMPONENT_INTERIOR) {
        return 0u;
    }

    if (player_spot(&tile_x, &tile_y) == 0u) {
        return 0u;
    }

    for (uint8_t i = 0u; i < map->entity_count; i++) {
        const GeneratedEntity_t* entity = &map->entities[i];

        if (entity->type != GENERATED_ENTITY_MODULE_EXIT &&
            entity->type != GENERATED_ENTITY_MODULE_ENTRANCE) {
            continue;
        }

        if (abs16((int16_t)(tile_x - entity->tile_x)) <= 1 &&
            abs16((int16_t)(tile_y - entity->tile_y)) <= 1) {
            return 1u;
        }
    }
    if (game_state.level == LEVEL_SPEAKER) {
        return (uint8_t)(tile_x <= 1u);
    }

    if (game_state.level == LEVEL_DISPLAY_BOSS) {
        return (uint8_t)(tile_x >= 23u && tile_y >= 27u);
    }
    return 0u;
}

/* updates overworld movement and door entry */
static void do_board(const GameInput_t* input)
{
    CampaignLevel_t level;

    move_player(input);

    if (door_check(&level) != 0u) {
        save_spot();
        go_level(level);
    }
}
/* moves dialogue to the next line or closes it */
static void talk_next(void)
{
    if (game_state.talk_id < STORY_SEQUENCE_COUNT) {
        game_state.talk_line++;

        if (game_state.talk_line < story_defs[game_state.talk_id].length) {
            set_talk();
            return;
        }

        /*
         * The opening is chained in three short parts so the player sees the
         * lab setup before being dropped into the circuit.
         */
        if (game_state.talk_id == STORY_LAB_INTRO) {
            talk(STORY_INTRO);
            return;
        }

        if (game_state.talk_id == STORY_INTRO) {
            game_state.story_done |= STORY_FLAG_INTRO_DONE;
            talk(STORY_BOARD_ENTRY);
            return;
        }

        if (game_state.talk_id == STORY_BOARD_ENTRY) {
            game_state.story_done |= STORY_FLAG_BOARD_DONE;
            game_state.screen = GAME_STATE_PLAYING;
            game_state.talk_id = DIALOGUE_NONE;
            return;
        }

        if (game_state.talk_id == STORY_ENDING) {
            game_state.talk_id = DIALOGUE_NONE;
            show_win();
            return;
        }

        game_state.screen = GAME_STATE_PLAYING;
        game_state.talk_id = DIALOGUE_NONE;
        game_state.talk_title = get_level_name(game_state.level);
        game_state.talk_text = "";
        return;
    }
}

/* -------------------------------------------------------------------------
 * Public game API
 * ------------------------------------------------------------------------- */

/* sets the game back to the start state */
void start_game(void)
{
    game_state.frame = 0u;
    game_state.talk_id = DIALOGUE_NONE;
    game_state.talk_line = 0u;
    game_state.story_done = 0u;
    game_state.input.move_x = 0;
    game_state.input.move_y = 0;
    game_state.input.action_down = 0u;
    game_state.input.action_pressed = 0u;
    game_state.input.joy1_direction = CENTRE;
    game_state.input.joy2_direction = CENTRE;

    /*
     * Player.c owns movement, but the wall lookup depends on whichever Tiled
     * map is currently active. This callback connects those two pieces.
     */
    set_wall_checker(wall_tile);

    reset_stuff();
    game_state.level = LEVEL_SPEAKER;
    go_board();
    talk(STORY_LAB_INTRO);
}

/* main game update called once per frame */
void play_game(void)
{
    const GameInput_t* input = get_input();

    game_state.frame++;
    game_state.input = *input;

    if (game_state.screen == GAME_STATE_PLAYING) {
        uint8_t action_used = 0u;

        if (game_state.area == AREA_MODE_OVERWORLD) {
            do_board(input);
            return;
        }

        do_glitch();
        move_player(input);

        if (exit_hit() != 0u) {
            go_board();
            return;
        }

        if (do_lint(input) != 0u) {
            action_used = 1u;
        }

        if (do_dials(input) != 0u) {
            action_used = 1u;
        }

        do_shoot(input, action_used);

        if (game_state.screen != GAME_STATE_PLAYING) {
            return;
        }

        if (game_state.level == LEVEL_SPEAKER &&
            game_state.speaker_done == 0u &&
            game_state.lint_total != 0u &&
            game_state.lint_done >= game_state.lint_total) {
            level_done_talk();
        }

    } else if (game_state.screen == GAME_STATE_DIALOGUE) {
        if (input->action_pressed != 0u) {
            talk_next();
        }

    } else {
        if (input->action_pressed != 0u) {
            reset_stuff();
            game_state.talk_id = DIALOGUE_NONE;
            game_state.talk_line = 0u;
            game_state.story_done = 0u;
            game_state.level = LEVEL_SPEAKER;
            go_board();
            talk(STORY_LAB_INTRO);
        }
    }
}

/* lets render and hardware read the game state */
const GameState_t* get_game(void)
{
    return &game_state;
}
