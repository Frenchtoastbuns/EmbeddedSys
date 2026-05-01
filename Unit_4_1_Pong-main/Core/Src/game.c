#include "game.h"

static GameState_t game_state;

typedef struct {
    uint8_t tile_x;
    uint8_t tile_y;
} FaultSpawn_t;

typedef struct {
    uint8_t tile_x;
    uint8_t tile_y;
} EnemySpawn_t;

typedef struct {
    const char* name;
    const char* complete_title;
    const char* complete_message;
    FaultSpawn_t fault_spawns[MAX_FAULT_NODES];
    EnemySpawn_t enemy_spawns[MAX_ENEMIES];
} CampaignLevelDef_t;

static const uint8_t game_map[GAME_MAP_HEIGHT][GAME_MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

static const CampaignLevelDef_t campaign_levels[CAMPAIGN_LEVEL_COUNT] = {
    {
        "LED Bar",
        "LED BAR FAULT REPAIRED",
        "DEBUG PULSE UNLOCKED",
        {{3u, 1u}, {11u, 4u}, {14u, 14u}},
        {{6u, 1u}, {1u, 8u}, {10u, 13u}}
    },
    {
        "Speaker",
        "SPEAKER OUTPUT RESTORED",
        "DISPLAY CONTROLLER CORRUPTED",
        {{2u, 10u}, {9u, 8u}, {13u, 3u}},
        {{14u, 1u}, {4u, 5u}, {12u, 10u}}
    },
    {
        "Display",
        "BOSS POWER SOURCE REPAIRED",
        "FINAL BUG VULNERABLE",
        {{6u, 1u}, {4u, 13u}, {12u, 10u}},
        {{1u, 14u}, {10u, 3u}, {14u, 7u}}
    }
};

static const char system_restored_title[] = "SYSTEM RESTORED";
static const char system_restored_message[] = "All hardware online";

static int16_t clamp_i16(int16_t value, int16_t min_value, int16_t max_value)
{
    if (value < min_value) {
        return min_value;
    }

    if (value > max_value) {
        return max_value;
    }

    return value;
}

static int16_t abs_i16(int16_t value)
{
    return (value < 0) ? (int16_t)-value : value;
}

static int8_t sign_i16(int16_t value)
{
    if (value > 0) {
        return 1;
    }

    if (value < 0) {
        return -1;
    }

    return 0;
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
    game_state.player.x = tile_center_to_world(1u);
    game_state.player.y = tile_center_to_world(1u);
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

static void add_debug_pulse_charge(uint8_t amount)
{
    uint16_t next_charge = (uint16_t)(game_state.debug_pulse_charge + amount);

    if (next_charge > DEBUG_PULSE_MAX_CHARGE) {
        next_charge = DEBUG_PULSE_MAX_CHARGE;
    }

    game_state.debug_pulse_charge = (uint8_t)next_charge;
    game_state.debug_pulse_ready =
        (game_state.debug_pulse_charge >= DEBUG_PULSE_MAX_CHARGE) ? 1u : 0u;
}

uint8_t game_get_tile(uint8_t tile_x, uint8_t tile_y)
{
    if (tile_x >= GAME_MAP_WIDTH || tile_y >= GAME_MAP_HEIGHT) {
        return 1u;
    }

    return game_map[tile_y][tile_x];
}

const char* game_get_level_name(CampaignLevel_t level)
{
    if (level >= LEVEL_COMPLETE) {
        return system_restored_title;
    }

    return campaign_levels[level].name;
}

uint16_t wrap_angle(int16_t angle_degrees)
{
    while (angle_degrees < 0) {
        angle_degrees = (int16_t)(angle_degrees + 360);
    }

    while (angle_degrees >= 360) {
        angle_degrees = (int16_t)(angle_degrees - 360);
    }

    return (uint16_t)angle_degrees;
}

uint16_t get_player_angle(void)
{
    return game_state.player.angle_degrees;
}

GameVector2i_t get_player_forward_vector_int(void)
{
    GameVector2i_t forward = {0, 0};
    uint16_t angle = get_player_angle();

    if (angle < 23u || angle >= 338u) {
        forward.x = 8;
        forward.y = 0;
    } else if (angle < 68u) {
        forward.x = 6;
        forward.y = 6;
    } else if (angle < 113u) {
        forward.x = 0;
        forward.y = 8;
    } else if (angle < 158u) {
        forward.x = -6;
        forward.y = 6;
    } else if (angle < 203u) {
        forward.x = -8;
        forward.y = 0;
    } else if (angle < 248u) {
        forward.x = -6;
        forward.y = -6;
    } else if (angle < 293u) {
        forward.x = 0;
        forward.y = -8;
    } else {
        forward.x = 6;
        forward.y = -6;
    }

    return forward;
}

uint8_t game_is_wall_at_world(int16_t world_x, int16_t world_y)
{
    int16_t tile_x;
    int16_t tile_y;

    if (world_x < 0 || world_y < 0) {
        return 1u;
    }

    tile_x = (int16_t)(world_x / GAME_TILE_SIZE);
    tile_y = (int16_t)(world_y / GAME_TILE_SIZE);

    if (tile_x >= (int16_t)GAME_MAP_WIDTH || tile_y >= (int16_t)GAME_MAP_HEIGHT) {
        return 1u;
    }

    return game_get_tile((uint8_t)tile_x, (uint8_t)tile_y);
}

uint8_t game_player_can_move_to(int16_t x, int16_t y)
{
    int16_t right = (int16_t)(x + game_state.player.size - 1);
    int16_t bottom = (int16_t)(y + game_state.player.size - 1);

    if (x < 0 || y < 0) {
        return 0u;
    }

    if (right >= GAME_WORLD_WIDTH || bottom >= GAME_WORLD_HEIGHT) {
        return 0u;
    }

    return (uint8_t)(game_is_wall_at_world(x, y) == 0u &&
                     game_is_wall_at_world(right, y) == 0u &&
                     game_is_wall_at_world(x, bottom) == 0u &&
                     game_is_wall_at_world(right, bottom) == 0u);
}

static uint8_t enemy_can_move_to(int16_t x, int16_t y)
{
    int16_t right = (int16_t)(x + ENEMY_SIZE - 1);
    int16_t bottom = (int16_t)(y + ENEMY_SIZE - 1);

    if (x < 0 || y < 0) {
        return 0u;
    }

    if (right >= GAME_WORLD_WIDTH || bottom >= GAME_WORLD_HEIGHT) {
        return 0u;
    }

    return (uint8_t)(game_is_wall_at_world(x, y) == 0u &&
                     game_is_wall_at_world(right, y) == 0u &&
                     game_is_wall_at_world(x, bottom) == 0u &&
                     game_is_wall_at_world(right, bottom) == 0u);
}

uint8_t game_player_is_near_fault_node(const Player_t* player, const FaultNode_t* node)
{
    int16_t player_center_x = (int16_t)(player->x + (player->size / 2));
    int16_t player_center_y = (int16_t)(player->y + (player->size / 2));
    int16_t node_center_x = tile_to_center(node->tile_x);
    int16_t node_center_y = tile_to_center(node->tile_y);

    return (uint8_t)(abs_i16((int16_t)(node_center_x - player_center_x)) <= FAULT_REPAIR_DISTANCE &&
                     abs_i16((int16_t)(node_center_y - player_center_y)) <= FAULT_REPAIR_DISTANCE);
}

static void move_player(const GameInput_t* input)
{
    int16_t next_x = (int16_t)(game_state.player.x +
                               (input->move_x * GAME_PLAYER_SPEED));
    int16_t next_y = (int16_t)(game_state.player.y +
                               (input->move_y * GAME_PLAYER_SPEED));

    next_x = clamp_i16(next_x, 0, GAME_WORLD_WIDTH - game_state.player.size);
    next_y = clamp_i16(next_y, 0, GAME_WORLD_HEIGHT - game_state.player.size);

    if (game_player_can_move_to(next_x, game_state.player.y) != 0u) {
        game_state.player.x = next_x;
    }

    if (game_player_can_move_to(game_state.player.x, next_y) != 0u) {
        game_state.player.y = next_y;
    }
}

static void turn_player(const GameInput_t* input)
{
    int16_t angle = (int16_t)game_state.player.angle_degrees;

    if (input->move_x < 0) {
        angle = (int16_t)(angle - GAME_TURN_SPEED_DEGREES);
    } else if (input->move_x > 0) {
        angle = (int16_t)(angle + GAME_TURN_SPEED_DEGREES);
    }

    game_state.player.angle_degrees = wrap_angle(angle);
}

static void update_fault_counts(void)
{
    uint8_t active_count = 0u;
    uint8_t fixed_count = 0u;

    for (uint8_t i = 0u; i < game_state.fault_node_count; i++) {
        if (game_state.fault_nodes[i].active != 0u &&
            game_state.fault_nodes[i].fixed == 0u) {
            active_count++;
        }

        if (game_state.fault_nodes[i].fixed != 0u) {
            fixed_count++;
        }
    }

    game_state.active_fault_count = active_count;
    game_state.fixed_fault_count = fixed_count;
    game_state.all_faults_fixed = (active_count == 0u) ? 1u : 0u;
}

static void init_fault_nodes(CampaignLevel_t level)
{
    game_state.fault_node_count = MAX_FAULT_NODES;
    game_state.repairing_fault_index = FAULT_NODE_NONE;

    for (uint8_t i = 0u; i < MAX_FAULT_NODES; i++) {
        game_state.fault_nodes[i].tile_x = campaign_levels[level].fault_spawns[i].tile_x;
        game_state.fault_nodes[i].tile_y = campaign_levels[level].fault_spawns[i].tile_y;
        game_state.fault_nodes[i].active = 1u;
        game_state.fault_nodes[i].fixed = 0u;
        game_state.fault_nodes[i].repair_progress = 0u;
    }

    update_fault_counts();
}

static void init_enemies(CampaignLevel_t level)
{
    game_state.enemy_count = MAX_ENEMIES;

    for (uint8_t i = 0u; i < MAX_ENEMIES; i++) {
        int16_t x = tile_center_to_world_for_size(campaign_levels[level].enemy_spawns[i].tile_x,
                                                  ENEMY_SIZE);
        int16_t y = tile_center_to_world_for_size(campaign_levels[level].enemy_spawns[i].tile_y,
                                                  ENEMY_SIZE);

        game_state.enemies[i].x = x;
        game_state.enemies[i].y = y;
        game_state.enemies[i].active = enemy_can_move_to(x, y);
        game_state.enemies[i].health = ENEMY_HEALTH;
    }
}

static void enter_level_complete_state(void)
{
    game_state.run_state = GAME_STATE_LEVEL_COMPLETE;
    game_state.current_level = LEVEL_COMPLETE;
    game_state.dialogue_title = system_restored_title;
    game_state.dialogue_message = system_restored_message;
    game_state.repairing_fault_index = FAULT_NODE_NONE;
}

static void enter_campaign_dialogue(const char* title, const char* message)
{
    game_state.run_state = GAME_STATE_DIALOGUE;
    game_state.dialogue_title = title;
    game_state.dialogue_message = message;
    game_state.repairing_fault_index = FAULT_NODE_NONE;
}

static void sync_boss_flags(void)
{
    game_state.boss_active = game_state.boss.active;
    game_state.boss_vulnerable = game_state.boss.vulnerable;
    game_state.boss_health = game_state.boss.health;
}

static void init_boss_for_level(CampaignLevel_t level)
{
    game_state.boss.x = tile_center_to_world_for_size(13u, BOSS_SIZE);
    game_state.boss.y = tile_center_to_world_for_size(14u, BOSS_SIZE);
    game_state.boss.active = 0u;
    game_state.boss.vulnerable = 0u;
    game_state.boss.health = 0u;
    game_state.display_corruption_ticks = 0u;

    if (level == LEVEL_DISPLAY_BOSS &&
        enemy_can_move_to(game_state.boss.x, game_state.boss.y) != 0u) {
        game_state.boss.active = 1u;
        game_state.boss.vulnerable = 0u;
        game_state.boss.health = BOSS_HEALTH;
    }

    sync_boss_flags();
}

static void start_level(CampaignLevel_t level)
{
    game_state.current_level = level;
    game_state.run_state = GAME_STATE_PLAYING;
    game_state.dialogue_title = game_get_level_name(level);
    game_state.dialogue_message = "";

    reset_player_to_spawn();
    init_fault_nodes(level);
    init_enemies(level);
    init_boss_for_level(level);
    game_state.damage_cooldown = 0u;
    game_state.shot_cooldown = 0u;
    game_state.shot_flash = 0u;
    game_state.debug_pulse_active_ticks = 0u;
}

static void reset_campaign_progress(void)
{
    game_state.led_bar_restored = 0u;
    game_state.debug_pulse_unlocked = 0u;
    game_state.speaker_restored = 0u;
    game_state.audio_unlocked = 0u;
    game_state.display_restored = 0u;
    game_state.debug_pulse_charge = 0u;
    game_state.debug_pulse_ready = 0u;
    game_state.debug_pulse_active_ticks = 0u;
    game_state.display_corruption_ticks = 0u;
}

static void enter_current_level_completed_dialogue(void)
{
    const CampaignLevelDef_t* level = &campaign_levels[game_state.current_level];

    if (game_state.current_level == LEVEL_LED_BAR) {
        game_state.led_bar_restored = 1u;
        game_state.debug_pulse_unlocked = 1u;
    } else if (game_state.current_level == LEVEL_SPEAKER) {
        game_state.speaker_restored = 1u;
        game_state.audio_unlocked = 1u;
    } else if (game_state.current_level == LEVEL_DISPLAY_BOSS) {
        game_state.display_restored = 1u;
        game_state.boss.vulnerable = 1u;
        sync_boss_flags();
    }

    enter_campaign_dialogue(level->complete_title, level->complete_message);
}

static uint8_t find_near_repairable_fault(void)
{
    for (uint8_t i = 0u; i < game_state.fault_node_count; i++) {
        FaultNode_t* node = &game_state.fault_nodes[i];

        if (node->active == 0u || node->fixed != 0u) {
            continue;
        }

        if (game_player_is_near_fault_node(&game_state.player, node) != 0u) {
            return i;
        }
    }

    return FAULT_NODE_NONE;
}

static uint8_t update_fault_repairs(const GameInput_t* input)
{
    uint8_t node_index = find_near_repairable_fault();

    game_state.repairing_fault_index = FAULT_NODE_NONE;

    if (node_index == FAULT_NODE_NONE) {
        return 0u;
    }

    if (input->action_down == 0u) {
        return 1u;
    }

    FaultNode_t* node = &game_state.fault_nodes[node_index];
    game_state.repairing_fault_index = node_index;

    if (node->repair_progress < FAULT_REPAIR_THRESHOLD) {
        node->repair_progress++;
    }

    if (node->repair_progress >= FAULT_REPAIR_THRESHOLD) {
        node->repair_progress = FAULT_REPAIR_THRESHOLD;
        node->fixed = 1u;
        game_state.repairing_fault_index = FAULT_NODE_NONE;
        add_debug_pulse_charge(DEBUG_PULSE_REPAIR_CHARGE);
        update_fault_counts();
    }

    return 1u;
}

static uint8_t enemy_touches_player(const Enemy_t* enemy)
{
    int16_t player_center_x = entity_center(game_state.player.x, game_state.player.size);
    int16_t player_center_y = entity_center(game_state.player.y, game_state.player.size);
    int16_t enemy_center_x = entity_center(enemy->x, ENEMY_SIZE);
    int16_t enemy_center_y = entity_center(enemy->y, ENEMY_SIZE);

    return (uint8_t)(abs_i16((int16_t)(enemy_center_x - player_center_x)) <= ENEMY_CONTACT_DISTANCE &&
                     abs_i16((int16_t)(enemy_center_y - player_center_y)) <= ENEMY_CONTACT_DISTANCE);
}

static void apply_enemy_contact_damage(void)
{
    if (game_state.damage_cooldown > 0u) {
        game_state.damage_cooldown--;
        return;
    }

    for (uint8_t i = 0u; i < game_state.enemy_count; i++) {
        if (game_state.enemies[i].active == 0u) {
            continue;
        }

        if (enemy_touches_player(&game_state.enemies[i]) != 0u) {
            if (game_state.system_stability > 0u) {
                game_state.system_stability--;
            }

            game_state.damage_cooldown = PLAYER_DAMAGE_COOLDOWN_FRAMES;
            return;
        }
    }
}

static uint8_t try_move_enemy_axis(Enemy_t* enemy, int8_t step_x, int8_t step_y)
{
    int16_t next_x = (int16_t)(enemy->x + step_x);
    int16_t next_y = (int16_t)(enemy->y + step_y);

    if (enemy_can_move_to(next_x, next_y) != 0u) {
        enemy->x = next_x;
        enemy->y = next_y;
        return 1u;
    }

    return 0u;
}

static void move_enemy_toward_player(Enemy_t* enemy)
{
    int16_t enemy_center_x = entity_center(enemy->x, ENEMY_SIZE);
    int16_t enemy_center_y = entity_center(enemy->y, ENEMY_SIZE);
    int16_t player_center_x = entity_center(game_state.player.x, game_state.player.size);
    int16_t player_center_y = entity_center(game_state.player.y, game_state.player.size);
    int16_t dx = (int16_t)(player_center_x - enemy_center_x);
    int16_t dy = (int16_t)(player_center_y - enemy_center_y);
    int8_t step_x = sign_i16(dx);
    int8_t step_y = sign_i16(dy);

    if (abs_i16(dx) >= abs_i16(dy)) {
        if (try_move_enemy_axis(enemy, step_x, 0) == 0u && step_y != 0) {
            (void)try_move_enemy_axis(enemy, 0, step_y);
        }
    } else {
        if (try_move_enemy_axis(enemy, 0, step_y) == 0u && step_x != 0) {
            (void)try_move_enemy_axis(enemy, step_x, 0);
        }
    }
}

static void update_enemies(void)
{
    if ((game_state.frame_count % ENEMY_MOVE_INTERVAL_FRAMES) != 0u) {
        return;
    }

    for (uint8_t i = 0u; i < game_state.enemy_count; i++) {
        if (game_state.enemies[i].active != 0u) {
            move_enemy_toward_player(&game_state.enemies[i]);
        }
    }
}

static uint8_t target_is_in_shot(int16_t target_x,
                                 int16_t target_y,
                                 uint8_t target_size,
                                 int16_t* forward_distance)
{
    GameVector2i_t forward = get_player_forward_vector_int();
    int16_t player_center_x = entity_center(game_state.player.x, game_state.player.size);
    int16_t player_center_y = entity_center(game_state.player.y, game_state.player.size);
    int16_t target_center_x = entity_center(target_x, target_size);
    int16_t target_center_y = entity_center(target_y, target_size);
    int16_t dx = (int16_t)(target_center_x - player_center_x);
    int16_t dy = (int16_t)(target_center_y - player_center_y);
    int16_t dot = (int16_t)((dx * forward.x) + (dy * forward.y));
    int16_t cross = (int16_t)((dx * forward.y) - (dy * forward.x));

    if (dot <= 0 || dot > (SHOT_RANGE * 8)) {
        return 0u;
    }

    if (abs_i16(cross) > (SHOT_HALF_WIDTH * 8)) {
        return 0u;
    }

    *forward_distance = dot;
    return 1u;
}

static uint8_t enemy_is_in_shot(const Enemy_t* enemy, int16_t* forward_distance)
{
    return target_is_in_shot(enemy->x, enemy->y, ENEMY_SIZE, forward_distance);
}

static uint8_t boss_is_in_shot(const Boss_t* boss, int16_t* forward_distance)
{
    return target_is_in_shot(boss->x, boss->y, BOSS_SIZE, forward_distance);
}

static void fire_shot(void)
{
    uint8_t target_index = MAX_ENEMIES;
    int16_t best_distance = (int16_t)(SHOT_RANGE * 8);

    game_state.shot_cooldown = SHOT_COOLDOWN_FRAMES;
    game_state.shot_flash = SHOT_FLASH_FRAMES;

    for (uint8_t i = 0u; i < game_state.enemy_count; i++) {
        int16_t forward_distance;

        if (game_state.enemies[i].active == 0u) {
            continue;
        }

        if (enemy_is_in_shot(&game_state.enemies[i], &forward_distance) == 0u) {
            continue;
        }

        if (forward_distance <= best_distance) {
            best_distance = forward_distance;
            target_index = i;
        }
    }

    if (target_index < MAX_ENEMIES) {
        Enemy_t* enemy = &game_state.enemies[target_index];

        if (enemy->health > 0u) {
            enemy->health--;
        }

        if (enemy->health == 0u) {
            enemy->active = 0u;
            add_debug_pulse_charge(DEBUG_PULSE_ENEMY_CHARGE);
        }

        return;
    }

    if (game_state.boss.active != 0u && game_state.boss.vulnerable != 0u) {
        int16_t boss_distance;

        if (boss_is_in_shot(&game_state.boss, &boss_distance) != 0u) {
            if (game_state.boss.health > 0u) {
                game_state.boss.health--;
            }

            if (game_state.boss.health == 0u) {
                game_state.boss.active = 0u;
                game_state.boss.vulnerable = 0u;
                sync_boss_flags();
                enter_level_complete_state();
                return;
            }

            sync_boss_flags();
        }
    }
}

static uint8_t enemy_is_in_debug_pulse_radius(const Enemy_t* enemy)
{
    int16_t player_center_x = entity_center(game_state.player.x, game_state.player.size);
    int16_t player_center_y = entity_center(game_state.player.y, game_state.player.size);
    int16_t enemy_center_x = entity_center(enemy->x, ENEMY_SIZE);
    int16_t enemy_center_y = entity_center(enemy->y, ENEMY_SIZE);

    return (uint8_t)(abs_i16((int16_t)(enemy_center_x - player_center_x)) <= DEBUG_PULSE_RADIUS &&
                     abs_i16((int16_t)(enemy_center_y - player_center_y)) <= DEBUG_PULSE_RADIUS);
}

static void activate_debug_pulse(void)
{
    for (uint8_t i = 0u; i < game_state.enemy_count; i++) {
        if (game_state.enemies[i].active == 0u) {
            continue;
        }

        if (enemy_is_in_debug_pulse_radius(&game_state.enemies[i]) != 0u) {
            game_state.enemies[i].active = 0u;
            game_state.enemies[i].health = 0u;
        }
    }

    game_state.debug_pulse_charge = 0u;
    game_state.debug_pulse_ready = 0u;
    game_state.debug_pulse_active_ticks = DEBUG_PULSE_ACTIVE_TICKS;
}

static void update_debug_pulse_ticks(void)
{
    if (game_state.debug_pulse_active_ticks > 0u) {
        game_state.debug_pulse_active_ticks--;
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

static uint8_t update_debug_pulse(const GameInput_t* input, uint8_t repair_priority_active)
{
    if (repair_priority_active != 0u) {
        return 0u;
    }

    if (game_state.debug_pulse_unlocked == 0u) {
        return 0u;
    }

    if (input->action_pressed != 0u && game_state.debug_pulse_ready != 0u) {
        activate_debug_pulse();
        return 1u;
    }

    return 0u;
}

static void update_shooting(const GameInput_t* input, uint8_t action_consumed)
{
    if (game_state.shot_cooldown > 0u) {
        game_state.shot_cooldown--;
    }

    if (game_state.shot_flash > 0u) {
        game_state.shot_flash--;
    }

    if (action_consumed != 0u) {
        return;
    }

    if (input->action_pressed != 0u && game_state.shot_cooldown == 0u) {
        fire_shot();
    }
}

static uint8_t current_level_repair_goal_pending(void)
{
    if (game_state.current_level == LEVEL_LED_BAR) {
        return (uint8_t)(game_state.led_bar_restored == 0u);
    }

    if (game_state.current_level == LEVEL_SPEAKER) {
        return (uint8_t)(game_state.speaker_restored == 0u);
    }

    if (game_state.current_level == LEVEL_DISPLAY_BOSS) {
        return (uint8_t)(game_state.display_restored == 0u);
    }

    return 0u;
}

static void advance_dialogue(void)
{
    if (game_state.current_level == LEVEL_LED_BAR) {
        start_level(LEVEL_SPEAKER);
    } else if (game_state.current_level == LEVEL_SPEAKER) {
        start_level(LEVEL_DISPLAY_BOSS);
    } else if (game_state.current_level == LEVEL_DISPLAY_BOSS) {
        game_state.run_state = GAME_STATE_PLAYING;
        game_state.dialogue_title = game_get_level_name(game_state.current_level);
        game_state.dialogue_message = "";
    } else {
        enter_level_complete_state();
    }
}

void game_init(void)
{
    game_state.frame_count = 0u;
    game_state.system_stability = PLAYER_MAX_STABILITY;
    game_state.damage_cooldown = 0u;
    game_state.shot_cooldown = 0u;
    game_state.shot_flash = 0u;
    game_state.debug_pulse_charge = 0u;
    game_state.debug_pulse_ready = 0u;
    game_state.debug_pulse_unlocked = 0u;
    game_state.debug_pulse_active_ticks = 0u;
    game_state.last_input.move_x = 0;
    game_state.last_input.move_y = 0;
    game_state.last_input.action_down = 0u;
    game_state.last_input.action_pressed = 0u;
    reset_campaign_progress();
    start_level(LEVEL_LED_BAR);
}

void update_game(void)
{
    const GameInput_t* input = input_get_state();

    game_state.frame_count++;
    game_state.last_input = *input;

    if (game_state.run_state == GAME_STATE_PLAYING) {
        uint8_t repair_priority_active;
        uint8_t action_consumed;

        update_debug_pulse_ticks();
        update_display_corruption();
        turn_player(input);
        move_player(input);
        repair_priority_active = update_fault_repairs(input);
        action_consumed = update_debug_pulse(input, repair_priority_active);
        update_shooting(input, (uint8_t)(repair_priority_active || action_consumed));
        if (game_state.run_state != GAME_STATE_PLAYING) {
            return;
        }
        update_enemies();
        apply_enemy_contact_damage();

        if (game_state.all_faults_fixed != 0u &&
            current_level_repair_goal_pending() != 0u) {
            enter_current_level_completed_dialogue();
        }
    } else if (game_state.run_state == GAME_STATE_DIALOGUE) {
        if (input->action_pressed != 0u) {
            advance_dialogue();
        }
    } else {
        if (input->action_pressed != 0u) {
            reset_campaign_progress();
            game_state.system_stability = PLAYER_MAX_STABILITY;
            start_level(LEVEL_LED_BAR);
        }
    }
}

const GameState_t* game_get_state(void)
{
    return &game_state;
}
