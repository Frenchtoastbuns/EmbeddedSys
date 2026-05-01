#include "render.h"

#include "game.h"
#include "LCD.h"
#include "main.h"
#include <stdio.h>

#define RENDER_MAP_X        0u
#define RENDER_MAP_Y        0u
#define RENDER_TEXT_X       4u
#define RENDER_TEXT_Y       170u
#define RENDER_FAULT_SIZE   6u

static ST7789V2_cfg_t lcd_cfg = {
    .setup_done = 0,
    .spi = SPI2,
    .RST = {.port = GPIOB, .pin = GPIO_PIN_2},
    .BL = {.port = GPIOB, .pin = GPIO_PIN_1},
    .DC = {.port = GPIOB, .pin = GPIO_PIN_11},
    .CS = {.port = GPIOB, .pin = GPIO_PIN_12},
    .MOSI = {.port = GPIOB, .pin = GPIO_PIN_15},
    .SCLK = {.port = GPIOB, .pin = GPIO_PIN_13},
    .dma = {.instance = DMA1, .channel = DMA1_Channel5}
};

static char render_text_line[32];

static uint16_t world_to_screen_x(int16_t x)
{
    return (uint16_t)(RENDER_MAP_X + x);
}

static uint16_t world_to_screen_y(int16_t y)
{
    return (uint16_t)(RENDER_MAP_Y + y);
}

static uint16_t clamp_screen_coord(int16_t value)
{
    if (value < 0) {
        return 0u;
    }

    if (value > 239) {
        return 239u;
    }

    return (uint16_t)value;
}

static void draw_tile_map(void)
{
    uint8_t tile_x;
    uint8_t tile_y;

    for (tile_y = 0u; tile_y < GAME_MAP_HEIGHT; tile_y++) {
        for (tile_x = 0u; tile_x < GAME_MAP_WIDTH; tile_x++) {
            uint16_t x = (uint16_t)(RENDER_MAP_X + (tile_x * GAME_TILE_SIZE));
            uint16_t y = (uint16_t)(RENDER_MAP_Y + (tile_y * GAME_TILE_SIZE));

            if (game_get_tile(tile_x, tile_y) != 0u) {
                LCD_Draw_Rect(x,
                              y,
                              GAME_TILE_SIZE,
                              GAME_TILE_SIZE,
                              9u,
                              1u);
            } else {
                LCD_Draw_Rect(x,
                              y,
                              GAME_TILE_SIZE,
                              GAME_TILE_SIZE,
                              13u,
                              0u);
            }
        }
    }
}

static void draw_player(const GameState_t* game)
{
    LCD_Draw_Rect(world_to_screen_x(game->player.x),
                  world_to_screen_y(game->player.y),
                  game->player.size,
                  game->player.size,
                  14u,
                  1u);
}

static void draw_player_direction(const GameState_t* game)
{
    GameVector2i_t forward = get_player_forward_vector_int();
    int16_t center_x = (int16_t)(world_to_screen_x(game->player.x) + (game->player.size / 2));
    int16_t center_y = (int16_t)(world_to_screen_y(game->player.y) + (game->player.size / 2));
    int16_t end_x = (int16_t)(center_x + (forward.x * 2));
    int16_t end_y = (int16_t)(center_y + (forward.y * 2));

    LCD_Draw_Line(clamp_screen_coord(center_x),
                  clamp_screen_coord(center_y),
                  clamp_screen_coord(end_x),
                  clamp_screen_coord(end_y),
                  6u);
}

static void draw_shot_flash(const GameState_t* game)
{
    GameVector2i_t forward;
    int16_t center_x;
    int16_t center_y;
    int16_t end_x;
    int16_t end_y;

    if (game->shot_flash == 0u) {
        return;
    }

    forward = get_player_forward_vector_int();
    center_x = (int16_t)(world_to_screen_x(game->player.x) + (game->player.size / 2));
    center_y = (int16_t)(world_to_screen_y(game->player.y) + (game->player.size / 2));
    end_x = (int16_t)(center_x + (forward.x * (SHOT_RANGE / 8)));
    end_y = (int16_t)(center_y + (forward.y * (SHOT_RANGE / 8)));

    LCD_Draw_Line(clamp_screen_coord(center_x),
                  clamp_screen_coord(center_y),
                  clamp_screen_coord(end_x),
                  clamp_screen_coord(end_y),
                  10u);
}

static void draw_debug_pulse_effect(const GameState_t* game)
{
    int16_t center_x;
    int16_t center_y;
    uint16_t left;
    uint16_t top;
    uint16_t right;
    uint16_t bottom;

    if (game->debug_pulse_active_ticks == 0u) {
        return;
    }

    center_x = (int16_t)(world_to_screen_x(game->player.x) + (game->player.size / 2));
    center_y = (int16_t)(world_to_screen_y(game->player.y) + (game->player.size / 2));
    left = clamp_screen_coord((int16_t)(center_x - DEBUG_PULSE_RADIUS));
    top = clamp_screen_coord((int16_t)(center_y - DEBUG_PULSE_RADIUS));
    right = clamp_screen_coord((int16_t)(center_x + DEBUG_PULSE_RADIUS));
    bottom = clamp_screen_coord((int16_t)(center_y + DEBUG_PULSE_RADIUS));

    if (right > left && bottom > top) {
        LCD_Draw_Rect(left,
                      top,
                      (uint16_t)(right - left),
                      (uint16_t)(bottom - top),
                      14u,
                      0u);
    }
}

static void draw_enemy(const Enemy_t* enemy)
{
    if (enemy->active == 0u) {
        return;
    }

    LCD_Draw_Rect(world_to_screen_x(enemy->x),
                  world_to_screen_y(enemy->y),
                  ENEMY_SIZE,
                  ENEMY_SIZE,
                  5u,
                  1u);
}

static void draw_enemies(const GameState_t* game)
{
    for (uint8_t i = 0u; i < game->enemy_count; i++) {
        draw_enemy(&game->enemies[i]);
    }
}

static void draw_boss(const GameState_t* game)
{
    uint8_t colour;

    if (game->boss.active == 0u) {
        return;
    }

    colour = (game->boss.vulnerable != 0u) ? 10u : 8u;

    LCD_Draw_Rect(world_to_screen_x(game->boss.x),
                  world_to_screen_y(game->boss.y),
                  BOSS_SIZE,
                  BOSS_SIZE,
                  colour,
                  1u);
    LCD_Draw_Rect(world_to_screen_x(game->boss.x),
                  world_to_screen_y(game->boss.y),
                  BOSS_SIZE,
                  BOSS_SIZE,
                  15u,
                  0u);
}

static void draw_display_corruption(const GameState_t* game)
{
    if (game->display_corruption_ticks == 0u) {
        return;
    }

    LCD_Draw_Rect(18u, 24u, 34u, 8u, 15u, 1u);
    LCD_Draw_Rect(92u, 42u, 22u, 18u, 8u, 1u);
    LCD_Draw_Rect(48u, 124u, 64u, 6u, 10u, 1u);
    LCD_Draw_Rect(126u, 86u, 20u, 10u, 15u, 0u);
}

static void draw_fault_node(const FaultNode_t* node)
{
    uint16_t x = (uint16_t)(RENDER_MAP_X +
                            (node->tile_x * GAME_TILE_SIZE) +
                            ((GAME_TILE_SIZE - RENDER_FAULT_SIZE) / 2u));
    uint16_t y = (uint16_t)(RENDER_MAP_Y +
                            (node->tile_y * GAME_TILE_SIZE) +
                            ((GAME_TILE_SIZE - RENDER_FAULT_SIZE) / 2u));
    uint8_t colour = (node->fixed != 0u) ? 3u : 2u;

    if (node->active == 0u) {
        return;
    }

    LCD_Draw_Rect(x,
                  y,
                  RENDER_FAULT_SIZE,
                  RENDER_FAULT_SIZE,
                  colour,
                  1u);
}

static void draw_fault_nodes(const GameState_t* game)
{
    for (uint8_t i = 0u; i < game->fault_node_count; i++) {
        draw_fault_node(&game->fault_nodes[i]);
    }
}

static void draw_debug_text(const GameState_t* game)
{
    snprintf(render_text_line, sizeof(render_text_line), "Level: %s",
             game_get_level_name(game->current_level));
    LCD_printString(render_text_line, RENDER_TEXT_X, RENDER_TEXT_Y, 1u, 1u);

    snprintf(render_text_line, sizeof(render_text_line), "Faults: %u/%u",
             (unsigned int)game->fixed_fault_count,
             (unsigned int)game->fault_node_count);
    LCD_printString(render_text_line, RENDER_TEXT_X, RENDER_TEXT_Y + 16u, 1u, 1u);

    if (game->repairing_fault_index != FAULT_NODE_NONE) {
        const FaultNode_t* node = &game->fault_nodes[game->repairing_fault_index];

        snprintf(render_text_line, sizeof(render_text_line), "Repairing: %u/%u",
                 (unsigned int)node->repair_progress,
                 (unsigned int)FAULT_REPAIR_THRESHOLD);
    } else if (game->all_faults_fixed != 0u) {
        snprintf(render_text_line, sizeof(render_text_line), "All faults fixed");
    } else {
        snprintf(render_text_line, sizeof(render_text_line), "Active: %u",
                 (unsigned int)game->active_fault_count);
    }

    LCD_printString(render_text_line, RENDER_TEXT_X, RENDER_TEXT_Y + 32u, 1u, 1u);

    if (game->debug_pulse_unlocked == 0u) {
        snprintf(render_text_line, sizeof(render_text_line), "Stab:%u/%u PULSE LOCKED",
                 (unsigned int)game->system_stability,
                 (unsigned int)PLAYER_MAX_STABILITY);
    } else if (game->debug_pulse_ready != 0u) {
        snprintf(render_text_line, sizeof(render_text_line), "Stab:%u/%u PULSE READY",
                 (unsigned int)game->system_stability,
                 (unsigned int)PLAYER_MAX_STABILITY);
    } else {
        snprintf(render_text_line, sizeof(render_text_line), "Stab:%u/%u Pulse:%u%%",
                 (unsigned int)game->system_stability,
                 (unsigned int)PLAYER_MAX_STABILITY,
                 (unsigned int)game->debug_pulse_charge);
    }
    LCD_printString(render_text_line, RENDER_TEXT_X, RENDER_TEXT_Y + 48u, 1u, 1u);
}

static void draw_playing_screen(const GameState_t* game)
{
    draw_tile_map();
    draw_fault_nodes(game);
    draw_enemies(game);
    draw_boss(game);
    draw_display_corruption(game);
    draw_debug_pulse_effect(game);
    draw_shot_flash(game);
    draw_player(game);
    draw_player_direction(game);
    draw_debug_text(game);
}

static void draw_dialogue_screen(const GameState_t* game)
{
    LCD_Draw_Rect(10u, 70u, 220u, 88u, 13u, 0u);
    LCD_printString("DIAGNOSTIC", 88u, 84u, 10u, 1u);
    LCD_printString(game->dialogue_title, 16u, 102u, 1u, 1u);
    LCD_printString(game->dialogue_message, 16u, 118u, 1u, 1u);
    LCD_printString("Press action", 82u, 140u, 14u, 1u);
}

static void draw_complete_screen(const GameState_t* game)
{
    LCD_Draw_Rect(10u, 72u, 220u, 84u, 3u, 0u);
    LCD_printString(game->dialogue_title, 62u, 92u, 3u, 1u);
    LCD_printString(game->dialogue_message, 20u, 112u, 1u, 1u);
    LCD_printString("Press action", 82u, 136u, 14u, 1u);
}

void render_init(void)
{
    LCD_init(&lcd_cfg);
    LCD_Fill_Buffer(0u);
    LCD_Refresh(&lcd_cfg);
}

void render_frame(void)
{
    const GameState_t* game = game_get_state();

    LCD_Fill_Buffer(0u);

    if (game->run_state == GAME_STATE_PLAYING) {
        draw_playing_screen(game);
    } else if (game->run_state == GAME_STATE_DIALOGUE) {
        draw_dialogue_screen(game);
    } else {
        draw_complete_screen(game);
    }

    LCD_Refresh(&lcd_cfg);
}
