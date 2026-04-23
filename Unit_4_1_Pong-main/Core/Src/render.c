#include "render.h"

#include "game.h"
#include "LCD.h"
#include "main.h"
#include <stdio.h>

#define RENDER_MAP_X        0u
#define RENDER_MAP_Y        0u
#define RENDER_TEXT_X       4u
#define RENDER_TEXT_Y       170u

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

static void draw_debug_text(const GameState_t* game)
{
    snprintf(render_text_line, sizeof(render_text_line), "Frame: %lu",
             (unsigned long)game->frame_count);
    LCD_printString(render_text_line, RENDER_TEXT_X, RENDER_TEXT_Y, 1u, 1u);

    snprintf(render_text_line, sizeof(render_text_line), "Player: %d,%d",
             (int)game->player.x,
             (int)game->player.y);
    LCD_printString(render_text_line, RENDER_TEXT_X, RENDER_TEXT_Y + 16u, 1u, 1u);

    snprintf(render_text_line, sizeof(render_text_line), "Input: %d,%d",
             (int)game->last_input.move_x,
             (int)game->last_input.move_y);
    LCD_printString(render_text_line, RENDER_TEXT_X, RENDER_TEXT_Y + 32u, 1u, 1u);
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
    draw_player(game);
    draw_debug_text(game);
    LCD_Refresh(&lcd_cfg);
}
