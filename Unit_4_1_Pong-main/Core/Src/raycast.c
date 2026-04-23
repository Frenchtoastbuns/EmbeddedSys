#include "raycast.h"

#include "LCD.h"
#include <stdint.h>

#define LCD_WIDTH             240
#define LCD_HEIGHT            240
#define RAY_COUNT              60
#define RAY_WIDTH               4
#define FOV_DEG                60
#define HALF_FOV_DEG          (FOV_DEG / 2)
#define FP_SHIFT                8
#define TRIG_SCALE            256
#define TRIG_STEP_DEG           5
#define TRIG_ENTRIES           72
#define RAY_STEP_WORLD          2
#define MAX_RAY_DISTANCE      192
#define MIN_WALL_HEIGHT         6
#define WALL_SCALE           (GAME_TILE_SIZE * 160)
#define WALL_COLOUR            13

/* Sine lookup table for 0..355 degrees in 5 degree steps, scaled by 256. */
static const int16_t sin_lut[TRIG_ENTRIES] = {
       0,   22,   44,   66,   88,  108,  128,  147,  165,  181,  196,  209,
     222,  232,  241,  247,  252,  255,  256,  255,  252,  247,  241,  232,
     222,  209,  196,  181,  165,  147,  128,  108,   88,   66,   44,   22,
       0,  -22,  -44,  -66,  -88, -108, -128, -147, -165, -181, -196, -209,
    -222, -232, -241, -247, -252, -255, -256, -255, -252, -247, -241, -232,
    -222, -209, -196, -181, -165, -147, -128, -108,  -88,  -66,  -44,  -22
};

static int16_t wrap_angle(int16_t angle)
{
    while (angle < 0) {
        angle = (int16_t)(angle + 360);
    }
    while (angle >= 360) {
        angle = (int16_t)(angle - 360);
    }
    return angle;
}

static uint8_t angle_to_index(int16_t angle)
{
    angle = wrap_angle(angle);
    return (uint8_t)(((angle + (TRIG_STEP_DEG / 2)) / TRIG_STEP_DEG) % TRIG_ENTRIES);
}

static int16_t sin_fixed(int16_t angle)
{
    return sin_lut[angle_to_index(angle)];
}

static int16_t cos_fixed(int16_t angle)
{
    return sin_lut[angle_to_index((int16_t)(angle + 90))];
}

static uint8_t is_wall_cell(int16_t world_x, int16_t world_y)
{
    if (world_x < 0 || world_y < 0) {
        return 1;
    }

    uint8_t tile_x = (uint8_t)(world_x / GAME_TILE_SIZE);
    uint8_t tile_y = (uint8_t)(world_y / GAME_TILE_SIZE);
    return game_get_tile(tile_x, tile_y);
}

static uint16_t cast_ray_distance(const GameState* game, int16_t angle)
{
    /* Position is stored as world pixels; rays use 8-bit fixed-point pixels. */
    int32_t ray_x = ((int32_t)game->player.x) << FP_SHIFT;
    int32_t ray_y = ((int32_t)game->player.y) << FP_SHIFT;
    int32_t step_x = (int32_t)cos_fixed(angle) * RAY_STEP_WORLD;
    int32_t step_y = (int32_t)sin_fixed(angle) * RAY_STEP_WORLD;
    uint16_t distance = 0;

    while (distance < MAX_RAY_DISTANCE) {
        ray_x += step_x;
        ray_y += step_y;
        distance = (uint16_t)(distance + RAY_STEP_WORLD);

        int16_t world_x = (int16_t)(ray_x >> FP_SHIFT);
        int16_t world_y = (int16_t)(ray_y >> FP_SHIFT);
        if (is_wall_cell(world_x, world_y)) {
            return distance;
        }
    }

    return MAX_RAY_DISTANCE;
}

static uint16_t corrected_distance(uint16_t distance, int16_t ray_offset)
{
    int16_t correction = cos_fixed(ray_offset);
    int32_t corrected = ((int32_t)distance * correction) / TRIG_SCALE;

    if (corrected < 1) {
        corrected = 1;
    }
    return (uint16_t)corrected;
}

static uint16_t wall_height_from_distance(uint16_t distance)
{
    uint16_t height = (uint16_t)(WALL_SCALE / distance);

    if (height < MIN_WALL_HEIGHT) {
        height = MIN_WALL_HEIGHT;
    }
    if (height > LCD_HEIGHT) {
        height = LCD_HEIGHT;
    }

    return height;
}

void raycast_render(const GameState* game)
{
    for (uint8_t ray = 0; ray < RAY_COUNT; ray++) {
        /* One degree per ray gives 60 columns across a 60 degree FOV. */
        int16_t ray_offset = (int16_t)(ray - HALF_FOV_DEG);
        int16_t ray_angle = wrap_angle((int16_t)(game->player.angle + ray_offset));
        uint16_t distance = cast_ray_distance(game, ray_angle);
        uint16_t corrected = corrected_distance(distance, ray_offset);
        uint16_t wall_height = wall_height_from_distance(corrected);

        uint16_t x = (uint16_t)(ray * RAY_WIDTH);
        int16_t y_start = (int16_t)((LCD_HEIGHT - wall_height) / 2);
        uint16_t draw_height = wall_height;

        if (y_start < 0) {
            draw_height = (uint16_t)(draw_height + y_start);
            y_start = 0;
        }

        LCD_Draw_Rect(x, (uint16_t)y_start, RAY_WIDTH, draw_height, WALL_COLOUR, 1);
    }
}
