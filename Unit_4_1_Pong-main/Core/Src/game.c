#include "game.h"

static GameState_t game_state;

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

static int16_t tile_center_to_world(uint8_t tile)
{
    return (int16_t)((tile * GAME_TILE_SIZE) + ((GAME_TILE_SIZE - GAME_PLAYER_SIZE) / 2));
}

uint8_t game_get_tile(uint8_t tile_x, uint8_t tile_y)
{
    if (tile_x >= GAME_MAP_WIDTH || tile_y >= GAME_MAP_HEIGHT) {
        return 1u;
    }

    return game_map[tile_y][tile_x];
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

void game_init(void)
{
    game_state.frame_count = 0u;
    game_state.player.x = tile_center_to_world(1u);
    game_state.player.y = tile_center_to_world(1u);
    game_state.player.size = GAME_PLAYER_SIZE;
    game_state.last_input.move_x = 0;
    game_state.last_input.move_y = 0;
    game_state.last_input.action_down = 0u;
    game_state.last_input.action_pressed = 0u;
}

void update_game(void)
{
    const GameInput_t* input = input_get_state();

    game_state.frame_count++;
    game_state.last_input = *input;
    move_player(input);
}

const GameState_t* game_get_state(void)
{
    return &game_state;
}
