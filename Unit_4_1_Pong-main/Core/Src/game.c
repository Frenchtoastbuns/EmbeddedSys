#include "game.h"

static GameState_t game_state;

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

void game_init(void)
{
    game_state.frame_count = 0u;
    game_state.player.width = GAME_PLAYER_SIZE;
    game_state.player.height = GAME_PLAYER_SIZE;
    game_state.player.x = (GAME_WORLD_WIDTH - GAME_PLAYER_SIZE) / 2;
    game_state.player.y = (GAME_WORLD_HEIGHT - GAME_PLAYER_SIZE) / 2;
    game_state.last_input.move_x = 0;
    game_state.last_input.move_y = 0;
    game_state.last_input.action_down = 0u;
    game_state.last_input.action_pressed = 0u;
    game_state.action_active = 0u;
    game_state.action_pulse = 0u;
}

void update_game(void)
{
    const GameInput_t* input = input_get_state();

    game_state.frame_count++;
    game_state.last_input = *input;

    int16_t next_x = (int16_t)(game_state.player.x +
                               (input->move_x * GAME_PLAYER_SPEED));
    int16_t next_y = (int16_t)(game_state.player.y +
                               (input->move_y * GAME_PLAYER_SPEED));

    game_state.player.x = clamp_i16(next_x,
                                    0,
                                    GAME_WORLD_WIDTH - game_state.player.width);
    game_state.player.y = clamp_i16(next_y,
                                    0,
                                    GAME_WORLD_HEIGHT - game_state.player.height);

    if (input->action_pressed != 0u) {
        game_state.action_active = (uint8_t)!game_state.action_active;
        game_state.action_pulse = GAME_ACTION_FRAMES;
    } else if (game_state.action_pulse > 0u) {
        game_state.action_pulse--;
    }
}

const GameState_t* game_get_state(void)
{
    return &game_state;
}
