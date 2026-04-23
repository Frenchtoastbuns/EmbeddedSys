#include "render.h"

#include "game.h"
#include "LCD.h"
#include "main.h"
#include <stdio.h>

#define RENDER_WORLD_X       0u
#define RENDER_WORLD_Y       0u
#define RENDER_WORLD_WIDTH   GAME_WORLD_WIDTH
#define RENDER_WORLD_HEIGHT  GAME_WORLD_HEIGHT
#define RENDER_GRID_STEP     16u
#define RENDER_TEXT_X        4u
#define RENDER_TEXT_Y        170u

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
    return (uint16_t)(RENDER_WORLD_X + x);
}

static uint16_t world_to_screen_y(int16_t y)
{
    return (uint16_t)(RENDER_WORLD_Y + y);
}

static void draw_world_bounds(void)
{
    LCD_Draw_Rect(RENDER_WORLD_X,
                  RENDER_WORLD_Y,
                  RENDER_WORLD_WIDTH,
                  RENDER_WORLD_HEIGHT,
                  1u,
                  0u);
}

static void draw_grid(void)
{
    for (uint16_t x = RENDER_GRID_STEP; x < RENDER_WORLD_WIDTH; x += RENDER_GRID_STEP) {
        LCD_Draw_Line((uint16_t)(RENDER_WORLD_X + x),
                      RENDER_WORLD_Y,
                      (uint16_t)(RENDER_WORLD_X + x),
                      (uint16_t)(RENDER_WORLD_Y + RENDER_WORLD_HEIGHT - 1u),
                      13u);
    }

    for (uint16_t y = RENDER_GRID_STEP; y < RENDER_WORLD_HEIGHT; y += RENDER_GRID_STEP) {
        LCD_Draw_Line(RENDER_WORLD_X,
                      (uint16_t)(RENDER_WORLD_Y + y),
                      (uint16_t)(RENDER_WORLD_X + RENDER_WORLD_WIDTH - 1u),
                      (uint16_t)(RENDER_WORLD_Y + y),
                      13u);
    }
}

static void draw_player(const GameState_t* game)
{
    uint8_t colour = (game->action_active != 0u) ? 10u : 14u;

    if (game->action_pulse != 0u) {
        uint16_t pulse_x = world_to_screen_x(game->player.x);
        uint16_t pulse_y = world_to_screen_y(game->player.y);
        uint16_t pulse_w = (uint16_t)game->player.width;
        uint16_t pulse_h = (uint16_t)game->player.height;

        if (pulse_x >= 2u) {
            pulse_x -= 2u;
            pulse_w += 2u;
        }
        if (pulse_y >= 2u) {
            pulse_y -= 2u;
            pulse_h += 2u;
        }
        if ((pulse_x + pulse_w + 2u) < RENDER_WORLD_WIDTH) {
            pulse_w += 2u;
        }
        if ((pulse_y + pulse_h + 2u) < RENDER_WORLD_HEIGHT) {
            pulse_h += 2u;
        }

        LCD_Draw_Rect(pulse_x,
                      pulse_y,
                      pulse_w,
                      pulse_h,
                      6u,
                      0u);
    }

    LCD_Draw_Rect(world_to_screen_x(game->player.x),
                  world_to_screen_y(game->player.y),
                  (uint16_t)game->player.width,
                  (uint16_t)game->player.height,
                  colour,
                  1u);
}

static void draw_debug_text(const GameState_t* game)
{
    snprintf(render_text_line, sizeof(render_text_line), "Frame: %lu",
             (unsigned long)game->frame_count);
    LCD_printString(render_text_line, RENDER_TEXT_X, RENDER_TEXT_Y, 1u, 1u);

    snprintf(render_text_line, sizeof(render_text_line), "Pos: %d,%d",
             (int)game->player.x,
             (int)game->player.y);
    LCD_printString(render_text_line, RENDER_TEXT_X, RENDER_TEXT_Y + 16u, 1u, 1u);

    snprintf(render_text_line, sizeof(render_text_line), "Move: %d,%d",
             (int)game->last_input.move_x,
             (int)game->last_input.move_y);
    LCD_printString(render_text_line, RENDER_TEXT_X, RENDER_TEXT_Y + 32u, 1u, 1u);

    snprintf(render_text_line, sizeof(render_text_line), "Action: %u",
             (unsigned int)game->action_active);
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

    draw_grid();
    draw_world_bounds();
    draw_player(game);
    draw_debug_text(game);

    LCD_Refresh(&lcd_cfg);
}
