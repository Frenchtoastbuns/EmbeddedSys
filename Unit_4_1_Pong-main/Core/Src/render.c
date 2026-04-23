#include "render.h"

#include "LCD.h"
#include "main.h"
#include <stdio.h>

#define RENDER_TILE_SIZE 10u
#define RENDER_MAP_X      0u
#define RENDER_MAP_Y      0u

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

static void draw_map(void)
{
    for (uint8_t y = 0u; y < GAME_MAP_HEIGHT; y++) {
        for (uint8_t x = 0u; x < GAME_MAP_WIDTH; x++) {
            if (game_get_tile(x, y) != 0u) {
                LCD_Draw_Rect((uint16_t)(RENDER_MAP_X + (x * RENDER_TILE_SIZE)),
                              (uint16_t)(RENDER_MAP_Y + (y * RENDER_TILE_SIZE)),
                              RENDER_TILE_SIZE,
                              RENDER_TILE_SIZE,
                              2,
                              1);
            }
        }
    }
}

static void draw_player(const Player_t* player)
{
    uint16_t x = (uint16_t)(RENDER_MAP_X +
                 (((int32_t)player->x * RENDER_TILE_SIZE) / GAME_TILE_SIZE));
    uint16_t y = (uint16_t)(RENDER_MAP_Y +
                 (((int32_t)player->y * RENDER_TILE_SIZE) / GAME_TILE_SIZE));
    uint16_t size = (uint16_t)(((uint32_t)PLAYER_SIZE * RENDER_TILE_SIZE) / GAME_TILE_SIZE);

    if (size == 0u) {
        size = 1u;
    }

    LCD_Draw_Rect(x, y, size, size, 12, 1);

    if (player->shot_flash != 0u) {
        int16_t center_x = (int16_t)(x + (size / 2u));
        int16_t center_y = (int16_t)(y + (size / 2u));
        int16_t end_x = (int16_t)(center_x + (player->facing_x * 32));
        int16_t end_y = (int16_t)(center_y + (player->facing_y * 32));

        if (end_x < 0) {
            end_x = 0;
        }
        if (end_y < 0) {
            end_y = 0;
        }

        LCD_Draw_Line((uint16_t)center_x, (uint16_t)center_y,
                      (uint16_t)end_x, (uint16_t)end_y,
                      14);
    }
}

static void draw_enemy(const Enemy_t* enemy)
{
    uint16_t x = (uint16_t)(RENDER_MAP_X +
                 (((int32_t)enemy->x * RENDER_TILE_SIZE) / GAME_TILE_SIZE));
    uint16_t y = (uint16_t)(RENDER_MAP_Y +
                 (((int32_t)enemy->y * RENDER_TILE_SIZE) / GAME_TILE_SIZE));
    uint16_t size = (uint16_t)(((uint32_t)ENEMY_SIZE * RENDER_TILE_SIZE) / GAME_TILE_SIZE);

    if (size == 0u) {
        size = 1u;
    }

    LCD_Draw_Rect(x, y, size, size, 4, 1);
}

static void draw_fault_node(const FaultNode_t* node)
{
    uint16_t x = (uint16_t)(RENDER_MAP_X +
                 (((int32_t)node->x * RENDER_TILE_SIZE) / GAME_TILE_SIZE));
    uint16_t y = (uint16_t)(RENDER_MAP_Y +
                 (((int32_t)node->y * RENDER_TILE_SIZE) / GAME_TILE_SIZE));
    uint16_t size = (uint16_t)(((uint32_t)FAULT_NODE_SIZE * RENDER_TILE_SIZE) / GAME_TILE_SIZE);
    uint8_t colour = (node->fixed != 0u) ? 10u : 6u;

    if (size == 0u) {
        size = 1u;
    }

    LCD_Draw_Rect(x, y, size, size, colour, 1);
}

static void draw_enemies(const GameState_t* game)
{
    for (uint8_t i = 0u; i < game->enemy_count && i < ENEMY_MAX_COUNT; i++) {
        if (game->enemies[i].active != 0u) {
            draw_enemy(&game->enemies[i]);
        }
    }
}

static void draw_fault_nodes(const GameState_t* game)
{
    for (uint8_t i = 0u; i < game->fault_node_count && i < FAULT_NODE_MAX_COUNT; i++) {
        draw_fault_node(&game->fault_nodes[i]);
    }
}

void render_init(void)
{
    LCD_init(&lcd_cfg);
    LCD_Fill_Buffer(0);
    LCD_Refresh(&lcd_cfg);
}

void render_frame(const GameState_t* game)
{
    LCD_Fill_Buffer(0);

    draw_map();
    draw_fault_nodes(game);
    draw_enemies(game);
    draw_player(&game->player);

    snprintf(render_text_line, sizeof(render_text_line), "Frame: %lu",
             (unsigned long)game->frame_count);
    LCD_printString(render_text_line, 4, 170, 1, 1);

    snprintf(render_text_line, sizeof(render_text_line), "PX:%d PY:%d",
             (int)game->player.x,
             (int)game->player.y);
    LCD_printString(render_text_line, 4, 186, 1, 1);

    snprintf(render_text_line, sizeof(render_text_line), "Fault:%u/%u",
             (unsigned int)game->fixed_fault_count,
             (unsigned int)game->fault_node_count);
    LCD_printString(render_text_line, 4, 202, 1, 1);

    uint8_t repair_progress = 0u;
    if (game->active_repair_node < FAULT_NODE_MAX_COUNT) {
        repair_progress = game->fault_nodes[game->active_repair_node].repair_progress;
    }

    snprintf(render_text_line, sizeof(render_text_line), "Rep:%u Hit:%u",
             (unsigned int)repair_progress,
             (unsigned int)game->enemies_hit);
    LCD_printString(render_text_line, 4, 218, 1, 1);

    LCD_Refresh(&lcd_cfg);
}
