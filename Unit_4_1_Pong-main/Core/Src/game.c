#include "game.h"

static GameState_t game_state;

typedef struct {
    uint8_t tile_x;
    uint8_t tile_y;
} FaultSpawn_t;

typedef struct {
    const char* name;
    const char* repaired_title;
    const char* repaired_message;
    FaultSpawn_t fault_spawns[MAX_FAULT_NODES];
} SubsystemDef_t;

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

static const SubsystemDef_t subsystems[SUBSYSTEM_COUNT] = {
    {
        "MMU",
        "MMU fault repaired",
        "Address translation stable",
        {{3u, 1u}, {11u, 4u}, {14u, 14u}}
    },
    {
        "Scheduler",
        "Scheduler repaired",
        "Scheduler timing restored",
        {{2u, 10u}, {9u, 8u}, {13u, 3u}}
    },
    {
        "Cache",
        "Cache fault cleared",
        "Cache coherency stable",
        {{6u, 1u}, {4u, 13u}, {12u, 10u}}
    },
    {
        "I/O Controller",
        "I/O controller online",
        "Device bus responding",
        {{1u, 8u}, {10u, 13u}, {14u, 7u}}
    }
};

static const char system_restored_title[] = "System restored";
static const char system_restored_message[] = "All subsystems online";

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

static int16_t tile_center_to_world(uint8_t tile)
{
    return (int16_t)((tile * GAME_TILE_SIZE) + ((GAME_TILE_SIZE - GAME_PLAYER_SIZE) / 2));
}

static void reset_player_to_spawn(void)
{
    game_state.player.x = tile_center_to_world(1u);
    game_state.player.y = tile_center_to_world(1u);
    game_state.player.size = GAME_PLAYER_SIZE;
}

static int16_t tile_to_center(uint8_t tile)
{
    return (int16_t)((tile * GAME_TILE_SIZE) + (GAME_TILE_SIZE / 2));
}

uint8_t game_get_tile(uint8_t tile_x, uint8_t tile_y)
{
    if (tile_x >= GAME_MAP_WIDTH || tile_y >= GAME_MAP_HEIGHT) {
        return 1u;
    }

    return game_map[tile_y][tile_x];
}

const char* game_get_subsystem_name(uint8_t subsystem_index)
{
    if (subsystem_index >= SUBSYSTEM_COUNT) {
        return system_restored_title;
    }

    return subsystems[subsystem_index].name;
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

static void init_fault_nodes(uint8_t subsystem_index)
{
    game_state.fault_node_count = MAX_FAULT_NODES;
    game_state.repairing_fault_index = FAULT_NODE_NONE;

    for (uint8_t i = 0u; i < MAX_FAULT_NODES; i++) {
        game_state.fault_nodes[i].tile_x = subsystems[subsystem_index].fault_spawns[i].tile_x;
        game_state.fault_nodes[i].tile_y = subsystems[subsystem_index].fault_spawns[i].tile_y;
        game_state.fault_nodes[i].active = 1u;
        game_state.fault_nodes[i].fixed = 0u;
        game_state.fault_nodes[i].repair_progress = 0u;
    }

    update_fault_counts();
}

static void enter_dialogue_state(void)
{
    const SubsystemDef_t* subsystem = &subsystems[game_state.current_subsystem];

    game_state.run_state = GAME_STATE_DIALOGUE;
    game_state.dialogue_title = subsystem->repaired_title;
    game_state.dialogue_message = subsystem->repaired_message;
    game_state.repairing_fault_index = FAULT_NODE_NONE;
}

static void enter_level_complete_state(void)
{
    game_state.run_state = GAME_STATE_LEVEL_COMPLETE;
    game_state.dialogue_title = system_restored_title;
    game_state.dialogue_message = system_restored_message;
    game_state.repairing_fault_index = FAULT_NODE_NONE;
}

static void start_subsystem(uint8_t subsystem_index)
{
    game_state.current_subsystem = subsystem_index;
    game_state.run_state = GAME_STATE_PLAYING;
    game_state.dialogue_title = subsystems[subsystem_index].name;
    game_state.dialogue_message = "";

    reset_player_to_spawn();
    init_fault_nodes(subsystem_index);
}

static void update_fault_repairs(const GameInput_t* input)
{
    game_state.repairing_fault_index = FAULT_NODE_NONE;

    if (input->action_down == 0u) {
        return;
    }

    for (uint8_t i = 0u; i < game_state.fault_node_count; i++) {
        FaultNode_t* node = &game_state.fault_nodes[i];

        if (node->active == 0u || node->fixed != 0u) {
            continue;
        }

        if (game_player_is_near_fault_node(&game_state.player, node) == 0u) {
            continue;
        }

        game_state.repairing_fault_index = i;

        if (node->repair_progress < FAULT_REPAIR_THRESHOLD) {
            node->repair_progress++;
        }

        if (node->repair_progress >= FAULT_REPAIR_THRESHOLD) {
            node->repair_progress = FAULT_REPAIR_THRESHOLD;
            node->fixed = 1u;
            game_state.repairing_fault_index = FAULT_NODE_NONE;
            update_fault_counts();
        }

        return;
    }
}

void game_init(void)
{
    game_state.frame_count = 0u;
    game_state.last_input.move_x = 0;
    game_state.last_input.move_y = 0;
    game_state.last_input.action_down = 0u;
    game_state.last_input.action_pressed = 0u;
    start_subsystem(0u);
}

void update_game(void)
{
    const GameInput_t* input = input_get_state();

    game_state.frame_count++;
    game_state.last_input = *input;

    if (game_state.run_state == GAME_STATE_PLAYING) {
        move_player(input);
        update_fault_repairs(input);

        if (game_state.all_faults_fixed != 0u) {
            enter_dialogue_state();
        }
    } else if (game_state.run_state == GAME_STATE_DIALOGUE) {
        if (input->action_pressed != 0u) {
            uint8_t next_subsystem = (uint8_t)(game_state.current_subsystem + 1u);

            if (next_subsystem >= SUBSYSTEM_COUNT) {
                enter_level_complete_state();
            } else {
                start_subsystem(next_subsystem);
            }
        }
    } else {
        if (input->action_pressed != 0u) {
            start_subsystem(0u);
        }
    }
}

const GameState_t* game_get_state(void)
{
    return &game_state;
}
