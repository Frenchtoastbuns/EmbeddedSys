#include "game.h"

static const uint8_t map[GAME_MAP_HEIGHT][GAME_MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,1,0,1,0,0,0,0,1},
    {1,0,1,1,0,0,0,0,1,0,0,0,1,1,0,1},
    {1,0,0,0,0,1,1,0,0,0,1,0,0,0,0,1},
    {1,0,0,1,0,0,0,0,1,0,0,0,1,0,0,1},
    {1,0,0,1,0,1,0,0,0,0,1,0,1,0,0,1},
    {1,0,0,0,0,1,0,1,1,0,1,0,0,0,0,1},
    {1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1},
    {1,0,0,0,0,1,0,1,0,1,0,0,0,0,0,1},
    {1,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1},
    {1,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1},
    {1,0,0,0,0,1,1,0,1,0,0,0,0,1,0,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

typedef struct {
    uint8_t tile_x;
    uint8_t tile_y;
} SpawnTile_t;

static const SpawnTile_t enemy_spawns[ENEMY_MAX_COUNT] = {
    {14, 14}, {14, 1}, {1, 14}, {12, 10}, {7, 14},
    {14, 7}, {6, 1}, {1, 8}, {10, 13}, {13, 3}
};

static const SpawnTile_t fault_node_spawns[FAULT_NODE_MAX_COUNT] = {
    {3, 1}, {11, 4}, {2, 10}, {13, 14}
};

static int16_t tile_to_centered_world(uint8_t tile, uint8_t entity_size)
{
    return (int16_t)((tile * GAME_TILE_SIZE) + ((GAME_TILE_SIZE - entity_size) / 2));
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

static int16_t entity_center(int16_t position, uint8_t size)
{
    return (int16_t)(position + (size / 2));
}

uint8_t game_get_tile(uint8_t tile_x, uint8_t tile_y)
{
    if (tile_x >= GAME_MAP_WIDTH || tile_y >= GAME_MAP_HEIGHT) {
        return 1u;
    }

    return map[tile_y][tile_x];
}

uint8_t game_is_wall_at_world(int16_t world_x, int16_t world_y)
{
    if (world_x < 0 || world_y < 0) {
        return 1u;
    }

    int16_t tile_x = (int16_t)(world_x / GAME_TILE_SIZE);
    int16_t tile_y = (int16_t)(world_y / GAME_TILE_SIZE);
    if (tile_x >= (int16_t)GAME_MAP_WIDTH || tile_y >= (int16_t)GAME_MAP_HEIGHT) {
        return 1u;
    }

    return game_get_tile((uint8_t)tile_x, (uint8_t)tile_y);
}

uint8_t game_player_can_stand_at(int16_t x, int16_t y)
{
    int16_t right = (int16_t)(x + PLAYER_SIZE - 1);
    int16_t bottom = (int16_t)(y + PLAYER_SIZE - 1);

    return (uint8_t)(!game_is_wall_at_world(x, y) &&
                     !game_is_wall_at_world(right, y) &&
                     !game_is_wall_at_world(x, bottom) &&
                     !game_is_wall_at_world(right, bottom));
}

uint8_t game_enemy_can_stand_at(int16_t x, int16_t y)
{
    int16_t right = (int16_t)(x + ENEMY_SIZE - 1);
    int16_t bottom = (int16_t)(y + ENEMY_SIZE - 1);

    return (uint8_t)(!game_is_wall_at_world(x, y) &&
                     !game_is_wall_at_world(right, y) &&
                     !game_is_wall_at_world(x, bottom) &&
                     !game_is_wall_at_world(right, bottom));
}

static void move_player(Player_t* player, const GameInput_t* input)
{
    if (input->move_x != 0 || input->move_y != 0) {
        player->facing_x = input->move_x;
        player->facing_y = input->move_y;
    }

    int16_t next_x = (int16_t)(player->x + (input->move_x * PLAYER_SPEED));
    int16_t next_y = (int16_t)(player->y + (input->move_y * PLAYER_SPEED));

    if (game_player_can_stand_at(next_x, player->y)) {
        player->x = next_x;
    }

    if (game_player_can_stand_at(player->x, next_y)) {
        player->y = next_y;
    }
}

uint8_t game_player_is_near_fault_node(const Player_t* player, const FaultNode_t* node)
{
    int16_t player_center_x = entity_center(player->x, PLAYER_SIZE);
    int16_t player_center_y = entity_center(player->y, PLAYER_SIZE);
    int16_t node_center_x = entity_center(node->x, FAULT_NODE_SIZE);
    int16_t node_center_y = entity_center(node->y, FAULT_NODE_SIZE);

    return (uint8_t)(abs_i16((int16_t)(node_center_x - player_center_x)) <= FAULT_REPAIR_RANGE &&
                     abs_i16((int16_t)(node_center_y - player_center_y)) <= FAULT_REPAIR_RANGE);
}

uint8_t game_enemy_is_hit_by_shot(const Player_t* player, const Enemy_t* enemy)
{
    if (enemy->active == 0u) {
        return 0u;
    }

    int16_t player_center_x = entity_center(player->x, PLAYER_SIZE);
    int16_t player_center_y = entity_center(player->y, PLAYER_SIZE);
    int16_t enemy_center_x = entity_center(enemy->x, ENEMY_SIZE);
    int16_t enemy_center_y = entity_center(enemy->y, ENEMY_SIZE);
    int16_t dx = (int16_t)(enemy_center_x - player_center_x);
    int16_t dy = (int16_t)(enemy_center_y - player_center_y);

    int16_t forward = (int16_t)((dx * player->facing_x) + (dy * player->facing_y));
    if (forward <= 0 || forward > SHOT_RANGE) {
        return 0u;
    }

    /*
     * Hitscan lane test. Cardinal shots use perpendicular distance.
     * Diagonal shots compare the two axis distances for a cheap cone.
     */
    if (player->facing_x == 0) {
        return (uint8_t)(abs_i16(dx) <= SHOT_HALF_WIDTH);
    }
    if (player->facing_y == 0) {
        return (uint8_t)(abs_i16(dy) <= SHOT_HALF_WIDTH);
    }

    return (uint8_t)(abs_i16((int16_t)(abs_i16(dx) - abs_i16(dy))) <= SHOT_HALF_WIDTH);
}

static void handle_shooting(GameState_t* game, const GameInput_t* input, uint8_t allow_shot)
{
    Player_t* player = &game->player;
    uint8_t best_enemy = ENEMY_MAX_COUNT;
    int16_t best_forward = SHOT_RANGE;

    if (player->shot_cooldown > 0u) {
        player->shot_cooldown--;
    }
    if (player->shot_flash > 0u) {
        player->shot_flash--;
    }

    if (allow_shot == 0u || input->action_pressed == 0u || player->shot_cooldown > 0u) {
        return;
    }

    player->shot_cooldown = SHOT_COOLDOWN_FRAMES;
    player->shot_flash = SHOT_FLASH_FRAMES;
    game->shots_fired++;

    for (uint8_t i = 0u; i < game->enemy_count && i < ENEMY_MAX_COUNT; i++) {
        if (game_enemy_is_hit_by_shot(player, &game->enemies[i]) != 0u) {
            int16_t player_center_x = entity_center(player->x, PLAYER_SIZE);
            int16_t player_center_y = entity_center(player->y, PLAYER_SIZE);
            int16_t enemy_center_x = entity_center(game->enemies[i].x, ENEMY_SIZE);
            int16_t enemy_center_y = entity_center(game->enemies[i].y, ENEMY_SIZE);
            int16_t dx = (int16_t)(enemy_center_x - player_center_x);
            int16_t dy = (int16_t)(enemy_center_y - player_center_y);
            int16_t forward = (int16_t)((dx * player->facing_x) + (dy * player->facing_y));

            if (forward <= best_forward) {
                best_forward = forward;
                best_enemy = i;
            }
        }
    }

    if (best_enemy < ENEMY_MAX_COUNT) {
        game->enemies[best_enemy].active = 0u;
        game->enemies_hit++;
    }
}

static void init_fault_nodes(GameState_t* game)
{
    game->fault_node_count = FAULT_NODE_MAX_COUNT;
    game->fixed_fault_count = 0u;
    game->active_repair_node = FAULT_NODE_NONE;

    for (uint8_t i = 0u; i < FAULT_NODE_MAX_COUNT; i++) {
        game->fault_nodes[i].x = tile_to_centered_world(fault_node_spawns[i].tile_x, FAULT_NODE_SIZE);
        game->fault_nodes[i].y = tile_to_centered_world(fault_node_spawns[i].tile_y, FAULT_NODE_SIZE);
        game->fault_nodes[i].fixed = 0u;
        game->fault_nodes[i].repair_progress = 0u;
    }
}

static uint8_t handle_fault_repairs(GameState_t* game, const GameInput_t* input)
{
    game->active_repair_node = FAULT_NODE_NONE;

    if (input->action_down == 0u) {
        return 0u;
    }

    for (uint8_t i = 0u; i < game->fault_node_count && i < FAULT_NODE_MAX_COUNT; i++) {
        FaultNode_t* node = &game->fault_nodes[i];
        if (node->fixed != 0u || game_player_is_near_fault_node(&game->player, node) == 0u) {
            continue;
        }

        game->active_repair_node = i;
        if (node->repair_progress < FAULT_REPAIR_COMPLETE) {
            node->repair_progress++;
        }

        if (node->repair_progress >= FAULT_REPAIR_COMPLETE) {
            node->repair_progress = FAULT_REPAIR_COMPLETE;
            node->fixed = 1u;
            game->fixed_fault_count++;
            game->active_repair_node = FAULT_NODE_NONE;
        }

        return 1u;
    }

    return 0u;
}

static void init_enemies(GameState_t* game)
{
    game->enemy_count = ENEMY_MAX_COUNT;

    for (uint8_t i = 0u; i < ENEMY_MAX_COUNT; i++) {
        game->enemies[i].x = tile_to_centered_world(enemy_spawns[i].tile_x, ENEMY_SIZE);
        game->enemies[i].y = tile_to_centered_world(enemy_spawns[i].tile_y, ENEMY_SIZE);
        game->enemies[i].active = 1u;
    }
}

static uint8_t try_move_enemy_axis(Enemy_t* enemy, int8_t step_x, int8_t step_y)
{
    int16_t next_x = (int16_t)(enemy->x + (step_x * ENEMY_SPEED));
    int16_t next_y = (int16_t)(enemy->y + (step_y * ENEMY_SPEED));

    if (game_enemy_can_stand_at(next_x, next_y)) {
        enemy->x = next_x;
        enemy->y = next_y;
        return 1u;
    }

    return 0u;
}

static void move_enemy_toward_player(Enemy_t* enemy, const Player_t* player)
{
    int16_t dx = (int16_t)(player->x - enemy->x);
    int16_t dy = (int16_t)(player->y - enemy->y);
    int8_t step_x = sign_i16(dx);
    int8_t step_y = sign_i16(dy);

    /*
     * Simple distance approximation:
     * move along the axis with the larger absolute gap, then try the other
     * axis only when the preferred step is blocked.
     */
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

static void update_enemies(GameState_t* game)
{
    for (uint8_t i = 0u; i < game->enemy_count && i < ENEMY_MAX_COUNT; i++) {
        if (game->enemies[i].active != 0u) {
            move_enemy_toward_player(&game->enemies[i], &game->player);
        }
    }
}

void game_init(GameState_t* game)
{
    game->frame_count = 0u;
    game->last_input.move_x = 0;
    game->last_input.move_y = 0;
    game->last_input.action_down = 0u;
    game->last_input.action_pressed = 0u;
    game->player.x = tile_to_centered_world(2u, PLAYER_SIZE);
    game->player.y = tile_to_centered_world(2u, PLAYER_SIZE);
    game->player.facing_x = 1;
    game->player.facing_y = 0;
    game->player.shot_cooldown = 0u;
    game->player.shot_flash = 0u;
    game->enemies_hit = 0u;
    game->shots_fired = 0u;
    init_enemies(game);
    init_fault_nodes(game);
}

void update_game(GameState_t* game, const GameInput_t* input)
{
    game->frame_count++;
    game->last_input = *input;
    move_player(&game->player, input);
    uint8_t repairing = handle_fault_repairs(game, input);
    handle_shooting(game, input, (uint8_t)!repairing);
    update_enemies(game);
}
