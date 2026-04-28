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
    snprintf(render_text_line, sizeof(render_text_line), "Frame: %lu",
             (unsigned long)game->frame_count);
    LCD_printString(render_text_line, RENDER_TEXT_X, RENDER_TEXT_Y, 1u, 1u);

    snprintf(render_text_line, sizeof(render_text_line), "Player: %d,%d",
             (int)game->player.x,
             (int)game->player.y);
    LCD_printString(render_text_line, RENDER_TEXT_X, RENDER_TEXT_Y + 16u, 1u, 1u);

    snprintf(render_text_line, sizeof(render_text_line), "Faults: %u/%u",
             (unsigned int)game->fixed_fault_count,
             (unsigned int)game->fault_node_count);
    LCD_printString(render_text_line, RENDER_TEXT_X, RENDER_TEXT_Y + 32u, 1u, 1u);

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

    LCD_printString(render_text_line, RENDER_TEXT_X, RENDER_TEXT_Y + 48u, 1u, 1u);
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
    draw_tile_map();
    draw_fault_nodes(game);
    draw_player(game);
    draw_debug_text(game);
    LCD_Refresh(&lcd_cfg);
}
