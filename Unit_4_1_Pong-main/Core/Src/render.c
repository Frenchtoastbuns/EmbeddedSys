#include "render.h"

#include "assets_tiles.h"
#include "assets_sprites.h"
#include "game.h"
#include "LCD.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

/*
 * LCD drawing only.
 *
 * This renderer reads GameState_t and draws the active Tiled map through a
 * camera. It does not change objectives, movement, dialogue, or transitions.
 */
#define RENDER_SCREEN_WIDTH 320u
#define RENDER_SCREEN_HEIGHT 240u
#define TOPDOWN_HUD_HEIGHT  20u
#define TOPDOWN_SCALE       2u
#define TOPDOWN_VIEW_WORLD_WIDTH  (RENDER_SCREEN_WIDTH / TOPDOWN_SCALE)
#define TOPDOWN_VIEW_WORLD_HEIGHT ((RENDER_SCREEN_HEIGHT - TOPDOWN_HUD_HEIGHT) / TOPDOWN_SCALE)
#define TOPDOWN_TILE_PIXELS (GAME_TILE_SIZE * TOPDOWN_SCALE)
#define TOPDOWN_WALL_FRONT_HEIGHT 8u
#define DISPLAY_DIAL_INACTIVE_TILESET TILESET_OVERWORLD
#define DISPLAY_DIAL_INACTIVE_COMPACT_TILE TILE_BOSS_DIAL_INACTIVE
#define CONTEXT_PROMPT_Y (RENDER_SCREEN_HEIGHT - 22u)
#define CONTEXT_PROMPT_TEXT_Y (RENDER_SCREEN_HEIGHT - 16u)
#define DIALOGUE_BOX_Y (RENDER_SCREEN_HEIGHT - 98u)
#define DIALOGUE_TITLE_Y (DIALOGUE_BOX_Y + 8u)
#define DIALOGUE_TEXT_Y (DIALOGUE_BOX_Y + 28u)
#define DIALOGUE_ACTION_Y (DIALOGUE_BOX_Y + 70u)
#define TILE_WALL_NORMAL 1u
#define TILE_WALL_AUDIO 3u
#define TILE_WALL_DISPLAY 4u
#define TILE_WALL_BOSS_CORE 5u
#define TILE_WALL_CORRUPTED 6u
#define TILE_DECOR_AUDIO_GRILLE 9u
#define TILE_DECOR_AUDIO_WAVE 10u
#define TILE_DECOR_DISPLAY_PIXEL 11u
#define TILE_DECOR_DISPLAY_GLITCH 12u
#define TILE_RESTORED_FLOOR 251u
#define TILE_REPAIR_PAD_ON 252u
#define TILE_ACTIVE_AUDIO_PATH 253u

#define TD_BLACK   0u
#define TD_WHITE   1u
#define TD_RED     2u
#define TD_GREEN   3u
#define TD_BLUE    4u
#define TD_ORANGE  5u
#define TD_YELLOW  6u
#define TD_PINK    7u
#define TD_PURPLE  8u
#define TD_NAVY    9u
#define TD_GOLD    10u
#define TD_VIOLET  11u
#define TD_BROWN   12u
#define TD_GREY    13u
#define TD_CYAN    14u
#define TD_MAGENTA 15u

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

typedef struct {
    int16_t x;
    int16_t y;
} TopdownCamera_t;

static uint16_t clamp_screen_x(int16_t value)
{
    if (value < 0) {
        return 0u;
    }

    if (value > (int16_t)(RENDER_SCREEN_WIDTH - 1u)) {
        return (uint16_t)(RENDER_SCREEN_WIDTH - 1u);
    }

    return (uint16_t)value;
}

static uint16_t clamp_screen_y(int16_t value)
{
    if (value < 0) {
        return 0u;
    }

    if (value > (int16_t)(RENDER_SCREEN_HEIGHT - 1u)) {
        return (uint16_t)(RENDER_SCREEN_HEIGHT - 1u);
    }

    return (uint16_t)value;
}

static int32_t abs_i32(int32_t value)
{
    return (value < 0) ? -value : value;
}

static void draw_display_corruption(const GameState_t* game)
{
    if (game->display_corruption_ticks == 0u) {
        return;
    }

    /*
     * Boss display corruption is intentionally drawn as black pixel blackout
     * patches. It covers more of the play area during the boss fight, but the
     * blocks stay separated so the screen still reads as a controlled attack.
     */
    LCD_Draw_Rect(12u, 26u, 58u, 22u, TD_BLACK, 1u);
    LCD_Draw_Rect(92u, 38u, 54u, 34u, TD_BLACK, 1u);
    LCD_Draw_Rect(166u, 62u, 42u, 52u, TD_BLACK, 1u);
    LCD_Draw_Rect(34u, 124u, 88u, 28u, TD_BLACK, 1u);
    LCD_Draw_Rect(132u, 158u, 76u, 24u, TD_BLACK, 1u);
    LCD_Draw_Rect(56u, 198u, 118u, 18u, TD_BLACK, 1u);
}


static TopdownCamera_t get_topdown_camera(const GameState_t* game)
{
    TopdownCamera_t camera;
    int16_t player_center_x = (int16_t)(game->player.x + (game->player.size / 2));
    int16_t player_center_y = (int16_t)(game->player.y + (game->player.size / 2));
    int16_t max_x = (int16_t)(game_get_active_world_width() - TOPDOWN_VIEW_WORLD_WIDTH);
    int16_t max_y = (int16_t)(game_get_active_world_height() - TOPDOWN_VIEW_WORLD_HEIGHT);

    camera.x = (int16_t)(player_center_x - (TOPDOWN_VIEW_WORLD_WIDTH / 2));
    camera.y = (int16_t)(player_center_y - (TOPDOWN_VIEW_WORLD_HEIGHT / 2));

    if (max_x < 0) {
        max_x = 0;
    }
    if (max_y < 0) {
        max_y = 0;
    }

    if (camera.x < 0) {
        camera.x = 0;
    }
    if (camera.y < 0) {
        camera.y = 0;
    }
    if (camera.x > max_x) {
        camera.x = max_x;
    }
    if (camera.y > max_y) {
        camera.y = max_y;
    }

    return camera;
}

static void draw_clipped_rect(int16_t x,
                              int16_t y,
                              int16_t width,
                              int16_t height,
                              uint8_t colour,
                              uint8_t fill)
{
    if (x < 0) {
        width = (int16_t)(width + x);
        x = 0;
    }

    if (y < (int16_t)TOPDOWN_HUD_HEIGHT) {
        height = (int16_t)(height - ((int16_t)TOPDOWN_HUD_HEIGHT - y));
        y = (int16_t)TOPDOWN_HUD_HEIGHT;
    }

    if ((x + width) > (int16_t)RENDER_SCREEN_WIDTH) {
        width = (int16_t)((int16_t)RENDER_SCREEN_WIDTH - x);
    }

    if ((y + height) > (int16_t)RENDER_SCREEN_HEIGHT) {
        height = (int16_t)((int16_t)RENDER_SCREEN_HEIGHT - y);
    }

    if (width <= 0 || height <= 0) {
        return;
    }

    LCD_Draw_Rect((uint16_t)x, (uint16_t)y, (uint16_t)width, (uint16_t)height, colour, fill);
}

static void draw_world_rect_scaled(const TopdownCamera_t* camera,
                                   int16_t world_x,
                                   int16_t world_y,
                                   uint8_t width,
                                   uint8_t height,
                                   uint8_t colour,
                                   uint8_t fill)
{
    int16_t screen_x = (int16_t)((world_x - camera->x) * TOPDOWN_SCALE);
    int16_t screen_y = (int16_t)(TOPDOWN_HUD_HEIGHT +
                                 ((world_y - camera->y) * TOPDOWN_SCALE));
    int16_t screen_w = (int16_t)(width * TOPDOWN_SCALE);
    int16_t screen_h = (int16_t)(height * TOPDOWN_SCALE);

    draw_clipped_rect(screen_x, screen_y, screen_w, screen_h, colour, fill);
}

static uint16_t world_to_topdown_screen_x(const TopdownCamera_t* camera, int16_t world_x)
{
    return clamp_screen_x((int16_t)((world_x - camera->x) * TOPDOWN_SCALE));
}

static uint16_t world_to_topdown_screen_y(const TopdownCamera_t* camera, int16_t world_y)
{
    return clamp_screen_y((int16_t)(TOPDOWN_HUD_HEIGHT +
                                    ((world_y - camera->y) * TOPDOWN_SCALE)));
}

static TilesetTheme_t topdown_tileset_theme(const GameState_t* game)
{
    if (game->area_mode == AREA_MODE_OVERWORLD) {
        return TILESET_OVERWORLD;
    }

    if (game->current_level == LEVEL_SPEAKER) {
        return TILESET_SPEAKER;
    }

    if (game->current_level == LEVEL_DISPLAY_BOSS) {
        return TILESET_DISPLAY;
    }

    return TILESET_SPEAKER;
}

static LogicalTileId_t topdown_wall_top_logical_tile(uint8_t tile)
{
    if (tile == TILE_WALL_BOSS_CORE) {
        return TILE_BOSS_CORE;
    }

    if (tile == TILE_WALL_CORRUPTED) {
        return TILE_CORRUPTION;
    }

    return TILE_WALL_TOP;
}

static LogicalTileId_t topdown_wall_front_logical_tile(uint8_t tile)
{
    if (tile == TILE_WALL_BOSS_CORE) {
        return TILE_BOSS_CORE;
    }

    if (tile == TILE_WALL_CORRUPTED) {
        return TILE_CORRUPTION;
    }

    return TILE_WALL_FRONT;
}

static const uint16_t asset_lcd_palette[16] = {
    RGB565_ASSET_BLACK, RGB565_ASSET_WHITE, RGB565_ASSET_RED, RGB565_ASSET_GREEN,
    RGB565_ASSET_BLUE, RGB565_ASSET_ORANGE, RGB565_ASSET_GOLD, RGB565_ASSET_PINK,
    RGB565_ASSET_PURPLE, RGB565_ASSET_NAVY, RGB565_ASSET_LIGHT_BLUE, RGB565_ASSET_DARK_VIOLET,
    RGB565_ASSET_DARK_PLUM, RGB565_ASSET_STEEL, RGB565_ASSET_CYAN, RGB565_ASSET_MAGENTA
};

static const uint8_t asset_scale_20_to_16[20] = {
    0u, 0u, 1u, 2u, 3u, 4u, 4u, 5u, 6u, 7u,
    8u, 8u, 9u, 10u, 11u, 12u, 12u, 13u, 14u, 15u
};

static void rgb565_to_rgb888(uint16_t colour, uint8_t* r, uint8_t* g, uint8_t* b)
{
    uint16_t rgb = (uint16_t)((colour >> 8) | (colour << 8));
    uint8_t r5 = (uint8_t)((rgb >> 11) & 0x1Fu);
    uint8_t g6 = (uint8_t)((rgb >> 5) & 0x3Fu);
    uint8_t b5 = (uint8_t)(rgb & 0x1Fu);

    *r = (uint8_t)((r5 << 3) | (r5 >> 2));
    *g = (uint8_t)((g6 << 2) | (g6 >> 4));
    *b = (uint8_t)((b5 << 3) | (b5 >> 2));
}

static void rgb565_standard_to_rgb888(uint16_t colour, uint8_t* r, uint8_t* g, uint8_t* b)
{
    uint8_t r5 = (uint8_t)((colour >> 11) & 0x1Fu);
    uint8_t g6 = (uint8_t)((colour >> 5) & 0x3Fu);
    uint8_t b5 = (uint8_t)(colour & 0x1Fu);

    *r = (uint8_t)((r5 << 3) | (r5 >> 2));
    *g = (uint8_t)((g6 << 2) | (g6 >> 4));
    *b = (uint8_t)((b5 << 3) | (b5 >> 2));
}

static uint8_t palette_index_from_rgb565(uint16_t colour)
{
    /*
     * The LCD framebuffer stores 4-bit palette indices, not raw RGB565.
     * Converted asset tiles still live in flash as RGB565; this maps each
     * source pixel to the nearest entry in PALETTE_CUSTOM, whose colours were
     * sampled from the imported 16x16 tileset. It is still only 16 colours,
     * but it preserves the map's blue/purple/steel palette far better than the
     * old generic high-contrast mapper.
     */
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t best_index = 0u;
    uint32_t best_distance = 0xFFFFFFFFu;

    switch (colour) {
    case RGB565_ASSET_BLACK:       return TD_BLACK;
    case RGB565_ASSET_WHITE:       return TD_WHITE;
    case RGB565_ASSET_RED:         return TD_RED;
    case RGB565_ASSET_GREEN:       return TD_GREEN;
    case RGB565_ASSET_BLUE:        return TD_BLUE;
    case RGB565_ASSET_ORANGE:      return TD_ORANGE;
    case RGB565_ASSET_GOLD:        return TD_YELLOW;
    case RGB565_ASSET_PINK:        return TD_PINK;
    case RGB565_ASSET_PURPLE:      return TD_PURPLE;
    case RGB565_ASSET_NAVY:        return TD_NAVY;
    case RGB565_ASSET_LIGHT_BLUE:  return TD_GOLD;
    case RGB565_ASSET_DARK_VIOLET: return TD_VIOLET;
    case RGB565_ASSET_DARK_PLUM:   return TD_BROWN;
    case RGB565_ASSET_STEEL:       return TD_GREY;
    case RGB565_ASSET_CYAN:        return TD_CYAN;
    case RGB565_ASSET_MAGENTA:     return TD_MAGENTA;
    default:
        break;
    }

    rgb565_to_rgb888(colour, &r, &g, &b);

    for (uint8_t i = 0u; i < 16u; i++) {
        uint8_t pr;
        uint8_t pg;
        uint8_t pb;
        int16_t dr;
        int16_t dg;
        int16_t db;
        uint32_t distance;

        rgb565_to_rgb888(asset_lcd_palette[i], &pr, &pg, &pb);
        dr = (int16_t)r - (int16_t)pr;
        dg = (int16_t)g - (int16_t)pg;
        db = (int16_t)b - (int16_t)pb;
        distance = (uint32_t)((dr * dr) + (dg * dg) + (db * db));

        if (distance < best_distance) {
            best_distance = distance;
            best_index = i;
        }
    }

    return best_index;
}

static uint8_t palette_index_from_sprite_rgb565(uint16_t colour)
{
    /*
     * Sprite converter emits standard RGB565:
     * r5 = r >> 3, g6 = g >> 2, b5 = b >> 3,
     * rgb565 = (r5 << 11) | (g6 << 5) | b5.
     * Tiles currently use byte-swapped RGB565, so sprites need their own
     * non-swapped palette matcher to avoid red/blue/purple colour drift.
     */
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t best_index = 0u;
    uint32_t best_distance = 0xFFFFFFFFu;

    rgb565_standard_to_rgb888(colour, &r, &g, &b);

    for (uint8_t i = 0u; i < 16u; i++) {
        uint8_t pr;
        uint8_t pg;
        uint8_t pb;
        int16_t dr;
        int16_t dg;
        int16_t db;
        uint32_t distance;

        rgb565_to_rgb888(asset_lcd_palette[i], &pr, &pg, &pb);
        dr = (int16_t)r - (int16_t)pr;
        dg = (int16_t)g - (int16_t)pg;
        db = (int16_t)b - (int16_t)pb;
        distance = (uint32_t)((dr * dr) + (dg * dg) + (db * db));

        if (distance < best_distance) {
            best_distance = distance;
            best_index = i;
        }
    }

    return best_index;
}

static uint8_t draw_asset_tile_scaled(const TopdownCamera_t* camera,
                                      int16_t world_x,
                                      int16_t world_y,
                                      uint8_t world_width,
                                      uint8_t world_height,
                                      const TileAsset_t* asset)
{
    int16_t screen_x;
    int16_t screen_y;
    uint8_t screen_w = (uint8_t)(world_width * TOPDOWN_SCALE);
    uint8_t screen_h = (uint8_t)(world_height * TOPDOWN_SCALE);

    if (asset == 0 || asset->pixels == 0 ||
        asset->width != ASSETS_TILE_SIZE || asset->height != ASSETS_TILE_SIZE ||
        screen_w == 0u || screen_h == 0u) {
        return 0u;
    }

    screen_x = (int16_t)((world_x - camera->x) * TOPDOWN_SCALE);
    screen_y = (int16_t)(TOPDOWN_HUD_HEIGHT + ((world_y - camera->y) * TOPDOWN_SCALE));

    /*
     * Source art is converted on the PC and stored in Flash as RGB565. The
     * active camera keeps gameplay/collision in the small world grid, so each
     * source tile is sampled
     * into the requested on-screen tile/face footprint. No framebuffer,
     * scaling buffer, PNG decoder, or dynamic allocation is used.
     */
    for (uint8_t dy = 0u; dy < screen_h; dy++) {
        int16_t y = (int16_t)(screen_y + dy);
        uint8_t src_y;

        if (y < (int16_t)TOPDOWN_HUD_HEIGHT || y >= (int16_t)RENDER_SCREEN_HEIGHT) {
            continue;
        }

        if (screen_h == TOPDOWN_TILE_PIXELS) {
            src_y = asset_scale_20_to_16[dy];
        } else {
            src_y = (uint8_t)(((uint16_t)dy * ASSETS_TILE_SIZE) / screen_h);
        }

        for (uint8_t dx = 0u; dx < screen_w; dx++) {
            int16_t x = (int16_t)(screen_x + dx);
            uint8_t src_x;
            uint16_t colour;

            if (x < 0 || x >= (int16_t)RENDER_SCREEN_WIDTH) {
                continue;
            }

            if (screen_w == TOPDOWN_TILE_PIXELS) {
                src_x = asset_scale_20_to_16[dx];
            } else {
                src_x = (uint8_t)(((uint16_t)dx * ASSETS_TILE_SIZE) / screen_w);
            }
            colour = asset->pixels[((uint16_t)src_y * ASSETS_TILE_SIZE) + src_x];
            if (colour == ASSETS_TILE_TRANSPARENT_RGB565) {
                continue;
            }
            LCD_Set_Pixel((uint16_t)x, (uint16_t)y, palette_index_from_rgb565(colour));
        }
    }

    return 1u;
}

static uint8_t draw_topdown_asset_tile_if_available(const GameState_t* game,
                                                    const TopdownCamera_t* camera,
                                                    LogicalTileId_t logical_tile,
                                                    int16_t world_x,
                                                    int16_t world_y)
{
    const TileAsset_t* asset = assets_tiles_resolve(topdown_tileset_theme(game), logical_tile);

    return draw_asset_tile_scaled(camera, world_x, world_y, GAME_TILE_SIZE, GAME_TILE_SIZE, asset);
}

static uint8_t draw_topdown_asset_tile_id_if_available(const GameState_t* game,
                                                       const TopdownCamera_t* camera,
                                                       uint8_t tile_id,
                                                       int16_t world_x,
                                                       int16_t world_y)
{
    const TileAsset_t* asset = assets_tiles_resolve_id(topdown_tileset_theme(game), tile_id);

    return draw_asset_tile_scaled(camera, world_x, world_y, GAME_TILE_SIZE, GAME_TILE_SIZE, asset);
}

static uint8_t draw_topdown_asset_region_if_available(const GameState_t* game,
                                                      const TopdownCamera_t* camera,
                                                      LogicalTileId_t logical_tile,
                                                      int16_t world_x,
                                                      int16_t world_y,
                                                      uint8_t world_width,
                                                      uint8_t world_height)
{
    const TileAsset_t* asset = assets_tiles_resolve(topdown_tileset_theme(game), logical_tile);

    return draw_asset_tile_scaled(camera, world_x, world_y, world_width, world_height, asset);
}

static uint8_t draw_sprite_frame_screen(int16_t screen_x,
                                        int16_t screen_y,
                                        const SpriteFrame_t* frame,
                                        uint8_t flip_x)
{
    if (frame == 0 || frame->pixels == 0 ||
        frame->width == 0u || frame->height == 0u) {
        return 0u;
    }

    if (screen_x >= (int16_t)RENDER_SCREEN_WIDTH ||
        screen_y >= (int16_t)RENDER_SCREEN_HEIGHT ||
        (int16_t)(screen_x + frame->width) <= 0 ||
        (int16_t)(screen_y + frame->height) <= (int16_t)TOPDOWN_HUD_HEIGHT) {
        return 0u;
    }

    for (uint8_t src_y = 0u; src_y < frame->height; src_y++) {
        int16_t y = (int16_t)(screen_y + src_y);

        if (y < (int16_t)TOPDOWN_HUD_HEIGHT || y >= (int16_t)RENDER_SCREEN_HEIGHT) {
            continue;
        }

        for (uint8_t src_x = 0u; src_x < frame->width; src_x++) {
            int16_t x = (int16_t)(screen_x + src_x);
            uint16_t colour;
            uint8_t sample_x = (flip_x != 0u) ?
                (uint8_t)(frame->width - 1u - src_x) : src_x;

            if (x < 0 || x >= (int16_t)RENDER_SCREEN_WIDTH) {
                continue;
            }

            colour = frame->pixels[((uint16_t)src_y * frame->width) + sample_x];
            if (colour == SPRITE_TRANSPARENT_RGB565) {
                continue;
            }

            LCD_Set_Pixel((uint16_t)x, (uint16_t)y, palette_index_from_sprite_rgb565(colour));
        }
    }

    return 1u;
}

static uint8_t draw_sprite_animation_screen(SpriteAnimationId_t animation_id,
                                            uint8_t frame_tick,
                                            int16_t screen_x,
                                            int16_t screen_y,
                                            uint8_t flip_x,
                                            uint8_t force_idle_frame)
{
#if USE_SPRITE_ASSETS
    const SpriteAnimation_t* animation = assets_sprites_get(animation_id);
    const SpriteFrame_t* frame;
    uint8_t frame_index;
    uint8_t ticks_per_frame;

    if (animation == 0 || animation->frames == 0 || animation->frame_count == 0u) {
        return 0u;
    }

    ticks_per_frame = (animation->ticks_per_frame == 0u) ? 1u : animation->ticks_per_frame;
    frame_index = (force_idle_frame != 0u) ?
        0u : (uint8_t)((frame_tick / ticks_per_frame) % animation->frame_count);
    frame = &animation->frames[frame_index];

    return draw_sprite_frame_screen(screen_x, screen_y, frame, flip_x);
#else
    (void)animation_id;
    (void)frame_tick;
    (void)screen_x;
    (void)screen_y;
    return 0u;
#endif
}

static SpriteAnimationId_t player_sprite_animation_id(const GameState_t* game)
{
    if (game->player.angle_degrees == 270u) {
        return SPRITE_ANIM_PLAYER_WALK_BACK;
    }

    if (game->player.angle_degrees == 90u) {
        return SPRITE_ANIM_PLAYER_WALK_FRONT;
    }

    return SPRITE_ANIM_PLAYER_WALK_RIGHT;
}

static uint8_t draw_topdown_player_sprite(const GameState_t* game,
                                          const TopdownCamera_t* camera)
{
    const SpriteAnimation_t* animation;
    const SpriteFrame_t* first_frame;
    SpriteAnimationId_t animation_id = player_sprite_animation_id(game);
    uint8_t moving = (uint8_t)(game->last_input.move_x != 0 || game->last_input.move_y != 0);
    uint8_t flip_x = (uint8_t)(game->player.angle_degrees == 180u);
    int16_t anchor_x;
    int16_t anchor_y;
    int16_t screen_x;
    int16_t screen_y;

#if USE_SPRITE_ASSETS
    animation = assets_sprites_get(animation_id);
    if (animation == 0 || animation->frames == 0 || animation->frame_count == 0u) {
        return 0u;
    }

    first_frame = &animation->frames[0];
    anchor_x = (int16_t)(game->player.x + (game->player.size / 2));
    anchor_y = (int16_t)(game->player.y + game->player.size + 2);
    screen_x = (int16_t)world_to_topdown_screen_x(camera, anchor_x);
    screen_y = (int16_t)world_to_topdown_screen_y(camera, anchor_y);
    screen_x = (int16_t)(screen_x - (first_frame->width / 2));
    screen_y = (int16_t)(screen_y - first_frame->height);

    return draw_sprite_animation_screen(animation_id,
                                        (uint8_t)game->frame_count,
                                        screen_x,
                                        screen_y,
                                        flip_x,
                                        (uint8_t)(moving == 0u));
#else
    (void)game;
    (void)camera;
    return 0u;
#endif
}

static uint8_t draw_topdown_boss_sprite(const GameState_t* game,
                                        const TopdownCamera_t* camera)
{
    const SpriteAnimation_t* animation;
    const SpriteFrame_t* first_frame;
    int16_t anchor_x;
    int16_t anchor_y;
    int16_t screen_x;
    int16_t screen_y;

#if USE_SPRITE_ASSETS
    animation = assets_sprites_get(SPRITE_ANIM_BOSS_IDLE);
    if (animation == 0 || animation->frames == 0 || animation->frame_count == 0u) {
        return 0u;
    }

    first_frame = &animation->frames[0];
    anchor_x = (int16_t)(game->boss.x + (BOSS_SIZE / 2));
    anchor_y = (int16_t)(game->boss.y + BOSS_SIZE + 8);
    screen_x = (int16_t)world_to_topdown_screen_x(camera, anchor_x);
    screen_y = (int16_t)world_to_topdown_screen_y(camera, anchor_y);
    screen_x = (int16_t)(screen_x - (first_frame->width / 2));
    screen_y = (int16_t)(screen_y - first_frame->height);

    return draw_sprite_animation_screen(SPRITE_ANIM_BOSS_IDLE,
                                        (uint8_t)game->frame_count,
                                        screen_x,
                                        screen_y,
                                        0u,
                                        0u);
#else
    (void)game;
    (void)camera;
    return 0u;
#endif
}

static uint8_t topdown_wall_accent_colour(uint8_t tile);

static uint8_t topdown_floor_colour(const GameState_t* game)
{
    if (game->area_mode == AREA_MODE_OVERWORLD) {
        return TD_GREEN;
    }

    if (game->current_level == LEVEL_SPEAKER) {
        return TD_NAVY;
    }

    if (game->current_level == LEVEL_DISPLAY_BOSS) {
        return TD_PURPLE;
    }

    return TD_NAVY;
}

static uint8_t topdown_overworld_colour_from_id(uint8_t tile_id, uint8_t layer)
{
    static const uint8_t ground_palette[6] = {
        TD_GREEN, TD_CYAN, TD_BLUE, TD_GREY, TD_GOLD, TD_NAVY
    };
    static const uint8_t object_palette[6] = {
        TD_BROWN, TD_GREY, TD_ORANGE, TD_BLUE, TD_PURPLE, TD_CYAN
    };

    if (tile_id == 0u) {
        return (layer == 0u) ? TD_GREEN : TD_GREY;
    }

    if (layer == 0u) {
        return ground_palette[tile_id % 6u];
    }

    return object_palette[tile_id % 6u];
}

static void draw_topdown_overworld_tile(const GameState_t* game,
                                        const TopdownCamera_t* camera,
                                        uint8_t tile_x,
                                        uint8_t tile_y,
                                        int16_t world_x,
                                        int16_t world_y)
{
    uint8_t ground = game_get_tile(tile_x, tile_y);
    uint8_t object = game_get_object_tile(tile_x, tile_y);
    uint8_t ground_colour = topdown_overworld_colour_from_id(ground, 0u);

    if (draw_topdown_asset_tile_id_if_available(game, camera, ground, world_x, world_y) == 0u) {
        draw_world_rect_scaled(camera, world_x, world_y, GAME_TILE_SIZE, GAME_TILE_SIZE, ground_colour, 1u);

        if (((tile_x + tile_y + ground) & 3u) == 0u) {
            draw_world_rect_scaled(camera,
                                   (int16_t)(world_x + 1),
                                   (int16_t)(world_y + 1),
                                   2u,
                                   1u,
                                   TD_WHITE,
                                   1u);
        }
    }

    if (object != 0u) {
        if (draw_topdown_asset_tile_id_if_available(game, camera, object, world_x, world_y) == 0u) {
            uint8_t object_colour = topdown_overworld_colour_from_id(object, 1u);

            draw_world_rect_scaled(camera, (int16_t)(world_x + 1), (int16_t)(world_y + 6), 8u, 2u, TD_BLACK, 1u);
            draw_world_rect_scaled(camera, (int16_t)(world_x + 2), (int16_t)(world_y + 2), 6u, 6u, object_colour, 1u);
            draw_world_rect_scaled(camera, (int16_t)(world_x + 2), (int16_t)(world_y + 2), 6u, 6u, TD_WHITE, 0u);
        }
    }

}

static void draw_topdown_generated_object_tile(const GameState_t* game,
                                               const TopdownCamera_t* camera,
                                               uint8_t tile,
                                               int16_t world_x,
                                               int16_t world_y)
{
    uint8_t colour;

    if (tile == 0u) {
        return;
    }

    if (draw_topdown_asset_tile_id_if_available(game, camera, tile, world_x, world_y) != 0u) {
        return;
    }

    if (game->current_level == LEVEL_SPEAKER) {
        colour = (uint8_t)((tile & 1u) ? TD_VIOLET : TD_BLUE);
    } else if (game->current_level == LEVEL_DISPLAY_BOSS) {
        colour = (uint8_t)((tile & 1u) ? TD_MAGENTA : TD_GREY);
    } else {
        colour = topdown_overworld_colour_from_id(tile, 1u);
    }

    draw_world_rect_scaled(camera, (int16_t)(world_x + 1), (int16_t)(world_y + 7), 8u, 2u, TD_BLACK, 1u);
    draw_world_rect_scaled(camera, (int16_t)(world_x + 2), (int16_t)(world_y + 2), 6u, 6u, colour, 1u);
    draw_world_rect_scaled(camera, (int16_t)(world_x + 2), (int16_t)(world_y + 2), 6u, 6u, TD_WHITE, 0u);

    if ((tile & 2u) != 0u) {
        draw_world_rect_scaled(camera, (int16_t)(world_x + 4), (int16_t)(world_y + 3), 2u, 4u, TD_BLACK, 1u);
    }
}

static uint8_t topdown_wall_colour(uint8_t tile)
{
    if (tile == TILE_WALL_AUDIO) {
        return TD_BLUE;
    }

    if (tile == TILE_WALL_DISPLAY) {
        return TD_GREY;
    }

    if (tile == TILE_WALL_BOSS_CORE) {
        return TD_RED;
    }

    if (tile == TILE_WALL_CORRUPTED) {
        return TD_MAGENTA;
    }

    return TD_BROWN;
}

static uint8_t topdown_wall_accent_colour(uint8_t tile)
{
    if (tile == TILE_WALL_AUDIO) {
        return TD_VIOLET;
    }

    if (tile == TILE_WALL_DISPLAY) {
        return TD_WHITE;
    }

    if (tile == TILE_WALL_BOSS_CORE || tile == TILE_WALL_CORRUPTED) {
        return TD_PINK;
    }

    return TD_GREY;
}

static uint8_t topdown_wall_front_colour(uint8_t tile)
{
    if (tile == TILE_WALL_AUDIO) {
        return TD_NAVY;
    }

    if (tile == TILE_WALL_DISPLAY) {
        return TD_BLUE;
    }

    if (tile == TILE_WALL_BOSS_CORE) {
        return TD_BROWN;
    }

    if (tile == TILE_WALL_CORRUPTED) {
        return TD_PURPLE;
    }

    return TD_BLACK;
}

static uint8_t topdown_tile_is_decor(uint8_t tile)
{
    return (uint8_t)(tile == TILE_RESTORED_FLOOR ||
                     tile == TILE_REPAIR_PAD_ON ||
                     tile == TILE_ACTIVE_AUDIO_PATH);
}

static uint8_t topdown_tile_draws_as_raised(const GameState_t* game, uint8_t tile)
{
    (void)tile;

    if (game->area_mode == AREA_MODE_OVERWORLD) {
        return 0u;
    }

    return 0u;
}

static void draw_topdown_decor_tile(const GameState_t* game,
                                    const TopdownCamera_t* camera,
                                    uint8_t tile,
                                    int16_t world_x,
                                    int16_t world_y)
{
    uint8_t anim = (uint8_t)((game->frame_count >> 4) & 1u);

    /*
     * Tiny procedural props/floor details. These are visual-only walkable
     * tiles, so they add life without changing collision or gameplay rules.
     */
    if (tile == TILE_REPAIR_PAD_ON) {
        draw_world_rect_scaled(camera, (int16_t)(world_x + 2), (int16_t)(world_y + 2), 6u, 6u, TD_CYAN, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 3), (int16_t)(world_y + 3), 4u, 4u, TD_WHITE, 0u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 4), (int16_t)(world_y + 4), 2u, 2u, TD_WHITE, 1u);
        return;
    }

    if (tile == TILE_DECOR_AUDIO_GRILLE) {
        draw_world_rect_scaled(camera, (int16_t)(world_x + 2), (int16_t)(world_y + 2), 6u, 1u, TD_CYAN, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 2), (int16_t)(world_y + 5), 6u, 1u, TD_CYAN, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 2), (int16_t)(world_y + 8), 6u, 1u, TD_CYAN, 1u);
        return;
    }

    if (tile == TILE_DECOR_AUDIO_WAVE) {
        uint8_t height = (anim != 0u) ? 6u : 3u;
        draw_world_rect_scaled(camera, (int16_t)(world_x + 2), (int16_t)(world_y + 7 - height), 1u, height, TD_VIOLET, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 4), (int16_t)(world_y + 4), 1u, 4u, TD_GOLD, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 6), (int16_t)(world_y + 7 - height), 1u, height, TD_VIOLET, 1u);
        return;
    }

    if (tile == TILE_ACTIVE_AUDIO_PATH) {
        draw_world_rect_scaled(camera, (int16_t)(world_x + 1), (int16_t)(world_y + 4), 2u, 2u, TD_CYAN, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 4), (int16_t)(world_y + 2), 2u, 6u, TD_CYAN, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 7), (int16_t)(world_y + 4), 2u, 2u, TD_CYAN, 1u);
        return;
    }

    if (tile == TILE_DECOR_DISPLAY_PIXEL) {
        draw_world_rect_scaled(camera, (int16_t)(world_x + 2), (int16_t)(world_y + 2), 2u, 2u, TD_CYAN, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 6), (int16_t)(world_y + 2), 2u, 2u, TD_BLUE, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 2), (int16_t)(world_y + 6), 2u, 2u, TD_BLUE, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 6), (int16_t)(world_y + 6), 2u, 2u, TD_CYAN, 1u);
        return;
    }

    if (tile == TILE_DECOR_DISPLAY_GLITCH) {
        uint8_t colour = (anim != 0u) ? TD_MAGENTA : TD_RED;
        draw_world_rect_scaled(camera, (int16_t)(world_x + 1), (int16_t)(world_y + 2), 7u, 2u, colour, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 4), (int16_t)(world_y + 6), 5u, 2u, TD_PINK, 1u);
        return;
    }

    if (tile == TILE_RESTORED_FLOOR) {
        draw_world_rect_scaled(camera, (int16_t)(world_x + 2), (int16_t)(world_y + 2), 2u, 2u, TD_CYAN, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 6), (int16_t)(world_y + 2), 2u, 2u, TD_WHITE, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 2), (int16_t)(world_y + 6), 2u, 2u, TD_WHITE, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 6), (int16_t)(world_y + 6), 2u, 2u, TD_CYAN, 1u);
    }
}

static void draw_topdown_floor_tile(const GameState_t* game,
                                    const TopdownCamera_t* camera,
                                    uint8_t tile_x,
                                    uint8_t tile_y)
{
    uint8_t tile = game_get_tile(tile_x, tile_y);
    int16_t world_x = (int16_t)(tile_x * GAME_TILE_SIZE);
    int16_t world_y = (int16_t)(tile_y * GAME_TILE_SIZE);
    uint8_t decor = topdown_tile_is_decor(tile);
    uint8_t base = topdown_floor_colour(game);
    uint8_t object_tile = game_get_object_tile(tile_x, tile_y);
    uint8_t drew_asset = 0u;

    if (game->area_mode == AREA_MODE_OVERWORLD) {
        draw_topdown_overworld_tile(game, camera, tile_x, tile_y, world_x, world_y);
        return;
    }

    if (decor == 0u) {
        drew_asset = draw_topdown_asset_tile_id_if_available(game, camera, tile, world_x, world_y);
    }

    if (drew_asset != 0u) {
        draw_topdown_generated_object_tile(game, camera, object_tile, world_x, world_y);
        return;
    }

    if (decor != 0u || drew_asset == 0u) {
        draw_world_rect_scaled(camera, world_x, world_y, GAME_TILE_SIZE, GAME_TILE_SIZE, base, 1u);
    }

    if (decor != 0u) {
        draw_topdown_decor_tile(game, camera, tile, world_x, world_y);
        draw_topdown_generated_object_tile(game, camera, object_tile, world_x, world_y);
        return;
    }

    if (game->area_mode == AREA_MODE_OVERWORLD && tile > TILE_WALL_NORMAL) {
        draw_world_rect_scaled(camera, world_x, world_y, GAME_TILE_SIZE, GAME_TILE_SIZE, TD_WHITE, 0u);
        draw_world_rect_scaled(camera,
                               (int16_t)(world_x + 2),
                               (int16_t)(world_y + 2),
                               6u,
                               6u,
                               topdown_wall_accent_colour(tile),
                               1u);
        return;
    }

    if (((tile_x + tile_y) & 1u) == 0u) {
        draw_world_rect_scaled(camera,
                               (int16_t)(world_x + 1),
                               (int16_t)(world_y + 1),
                               1u,
                               1u,
                               TD_GREY,
                               1u);
    }

    draw_topdown_generated_object_tile(game, camera, object_tile, world_x, world_y);
}

static void draw_topdown_wall_top(const GameState_t* game,
                                  const TopdownCamera_t* camera,
                                  uint8_t tile,
                                  int16_t world_x,
                                  int16_t world_y)
{
    uint8_t top_colour = topdown_wall_colour(tile);
    uint8_t accent_colour = topdown_wall_accent_colour(tile);

    if (draw_topdown_asset_tile_if_available(game,
                                             camera,
                                             topdown_wall_top_logical_tile(tile),
                                             world_x,
                                             world_y) != 0u) {
        return;
    }

    draw_world_rect_scaled(camera, world_x, world_y, GAME_TILE_SIZE, GAME_TILE_SIZE, top_colour, 1u);
    draw_world_rect_scaled(camera, world_x, world_y, GAME_TILE_SIZE, 1u, TD_WHITE, 1u);
    draw_world_rect_scaled(camera, world_x, world_y, 1u, GAME_TILE_SIZE, TD_WHITE, 1u);
    draw_world_rect_scaled(camera,
                           (int16_t)(world_x + GAME_TILE_SIZE - 1),
                           world_y,
                           1u,
                           GAME_TILE_SIZE,
                           TD_BLACK,
                           1u);

    draw_world_rect_scaled(camera,
                           (int16_t)(world_x + 2),
                           (int16_t)(world_y + 2),
                           6u,
                           1u,
                           accent_colour,
                           1u);
}

static void draw_topdown_wall_front(const GameState_t* game,
                                    const TopdownCamera_t* camera,
                                    uint8_t tile,
                                    int16_t world_x,
                                    int16_t world_y)
{
    uint8_t front_colour = topdown_wall_front_colour(tile);
    uint8_t front_world_height = (uint8_t)(TOPDOWN_WALL_FRONT_HEIGHT / TOPDOWN_SCALE);

    if (draw_topdown_asset_region_if_available(game,
                                               camera,
                                               topdown_wall_front_logical_tile(tile),
                                               world_x,
                                               (int16_t)(world_y + GAME_TILE_SIZE),
                                               GAME_TILE_SIZE,
                                               front_world_height) != 0u) {
        return;
    }

    draw_world_rect_scaled(camera,
                           world_x,
                           (int16_t)(world_y + GAME_TILE_SIZE),
                           GAME_TILE_SIZE,
                           front_world_height,
                           front_colour,
                           1u);
    draw_world_rect_scaled(camera,
                           world_x,
                           (int16_t)(world_y + GAME_TILE_SIZE),
                           GAME_TILE_SIZE,
                           1u,
                           TD_BLACK,
                           1u);
}

static void draw_topdown_floors_and_wall_tops(const GameState_t* game, const TopdownCamera_t* camera)
{
    uint8_t start_x = (uint8_t)(camera->x / GAME_TILE_SIZE);
    uint8_t start_y = (uint8_t)(camera->y / GAME_TILE_SIZE);
    uint8_t end_x = (uint8_t)((camera->x + TOPDOWN_VIEW_WORLD_WIDTH) / GAME_TILE_SIZE + 1);
    uint8_t end_y = (uint8_t)((camera->y + TOPDOWN_VIEW_WORLD_HEIGHT) / GAME_TILE_SIZE + 1);
    uint8_t map_width = game_get_active_map_width();
    uint8_t map_height = game_get_active_map_height();

    if (end_x > map_width) {
        end_x = map_width;
    }
    if (end_y > map_height) {
        end_y = map_height;
    }

    for (uint8_t tile_y = start_y; tile_y < end_y; tile_y++) {
        for (uint8_t tile_x = start_x; tile_x < end_x; tile_x++) {
            uint8_t tile = game_get_tile(tile_x, tile_y);
            int16_t world_x = (int16_t)(tile_x * GAME_TILE_SIZE);
            int16_t world_y = (int16_t)(tile_y * GAME_TILE_SIZE);

            draw_topdown_floor_tile(game, camera, tile_x, tile_y);

            if (game->area_mode != AREA_MODE_OVERWORLD &&
                topdown_tile_draws_as_raised(game, tile) != 0u) {
                draw_topdown_wall_top(game, camera, tile, world_x, world_y);
            }
        }
    }
}

static void draw_topdown_wall_fronts_for_row(const GameState_t* game,
                                             const TopdownCamera_t* camera,
                                             uint8_t tile_y)
{
    uint8_t start_x = (uint8_t)(camera->x / GAME_TILE_SIZE);
    uint8_t end_x = (uint8_t)((camera->x + TOPDOWN_VIEW_WORLD_WIDTH) / GAME_TILE_SIZE + 1);
    uint8_t map_width = game_get_active_map_width();
    uint8_t map_height = game_get_active_map_height();

    if (tile_y >= map_height || game->area_mode == AREA_MODE_OVERWORLD) {
        return;
    }

    if (end_x > map_width) {
        end_x = map_width;
    }

    for (uint8_t tile_x = start_x; tile_x < end_x; tile_x++) {
        uint8_t tile = game_get_tile(tile_x, tile_y);

        if (topdown_tile_draws_as_raised(game, tile) != 0u) {
            draw_topdown_wall_front(game,
                                    camera,
                                    tile,
                                    (int16_t)(tile_x * GAME_TILE_SIZE),
                                    (int16_t)(tile_y * GAME_TILE_SIZE));
        }
    }
}

static void draw_topdown_player(const GameState_t* game, const TopdownCamera_t* camera)
{
    int16_t body_x = (int16_t)(game->player.x - 1);
    int16_t body_y = (int16_t)(game->player.y - 4);
    int16_t arrow_x = (int16_t)(game->player.x + 3);
    int16_t arrow_y = (int16_t)(game->player.y + 3);

    /*
     * If the sprite build is off, draw a simple block player instead.
     */
    if (draw_topdown_player_sprite(game, camera) != 0u) {
        return;
    }

    draw_world_rect_scaled(camera,
                           (int16_t)(game->player.x - 1),
                           (int16_t)(game->player.y + 7),
                           10u,
                           2u,
                           TD_BLACK,
                           1u);
    draw_world_rect_scaled(camera,
                           body_x,
                           body_y,
                           10u,
                           12u,
                           TD_CYAN,
                           1u);
    draw_world_rect_scaled(camera,
                           body_x,
                           body_y,
                           10u,
                           12u,
                           TD_WHITE,
                           0u);
    draw_world_rect_scaled(camera,
                           (int16_t)(body_x + 2),
                           (int16_t)(body_y + 3),
                           6u,
                           2u,
                           TD_NAVY,
                           1u);

    if (game->player.angle_degrees == 270u) {
        draw_world_rect_scaled(camera, arrow_x, (int16_t)(game->player.y - 7), 2u, 4u, TD_GOLD, 1u);
        draw_world_rect_scaled(camera, (int16_t)(arrow_x - 1), (int16_t)(game->player.y - 5), 4u, 1u, TD_GOLD, 1u);
    } else if (game->player.angle_degrees == 90u) {
        draw_world_rect_scaled(camera, arrow_x, (int16_t)(game->player.y + 11), 2u, 4u, TD_GOLD, 1u);
        draw_world_rect_scaled(camera, (int16_t)(arrow_x - 1), (int16_t)(game->player.y + 13), 4u, 1u, TD_GOLD, 1u);
    } else if (game->player.angle_degrees == 180u) {
        draw_world_rect_scaled(camera, (int16_t)(game->player.x - 7), arrow_y, 4u, 2u, TD_GOLD, 1u);
        draw_world_rect_scaled(camera, (int16_t)(game->player.x - 5), (int16_t)(arrow_y - 1), 1u, 4u, TD_GOLD, 1u);
    } else {
        draw_world_rect_scaled(camera, (int16_t)(game->player.x + 11), arrow_y, 4u, 2u, TD_GOLD, 1u);
        draw_world_rect_scaled(camera, (int16_t)(game->player.x + 13), (int16_t)(arrow_y - 1), 1u, 4u, TD_GOLD, 1u);
    }
}

static void draw_topdown_removable_objects_for_row(const GameState_t* game,
                                                   const TopdownCamera_t* camera,
                                                   uint8_t row)
{
    if (game->current_level != LEVEL_SPEAKER) {
        return;
    }

    for (uint8_t i = 0u; i < game->speaker_lint_count; i++) {
        const RemovableObject_t* object = &game->removable_objects[i];
        int16_t world_x;
        int16_t world_y;

        if (object->active == 0u || object->tile_y != row) {
            continue;
        }

        world_x = (int16_t)((object->tile_x * GAME_TILE_SIZE) + 2);
        world_y = (int16_t)((object->tile_y * GAME_TILE_SIZE) + 3);

        draw_world_rect_scaled(camera, world_x, world_y, 6u, 5u, TD_GREY, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 1), (int16_t)(world_y - 1), 5u, 3u, TD_WHITE, 0u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 2), (int16_t)(world_y + 2), 2u, 1u, TD_BLACK, 1u);
    }
}

static void draw_topdown_boss_dials_for_row(const GameState_t* game,
                                            const TopdownCamera_t* camera,
                                            uint8_t row)
{
    if (game->current_level != LEVEL_DISPLAY_BOSS) {
        return;
    }

    for (uint8_t i = 0u; i < game->boss_dial_count; i++) {
        const BossDial_t* dial = &game->boss_dials[i];
        int16_t world_x;
        int16_t world_y;
        uint8_t colour;

        if (dial->active == 0u || dial->tile_y != row) {
            continue;
        }

        world_x = (int16_t)(dial->tile_x * GAME_TILE_SIZE);
        world_y = (int16_t)(dial->tile_y * GAME_TILE_SIZE);
        colour = (dial->disabled != 0u) ? TD_GREEN : TD_RED;

        if (dial->disabled != 0u) {
            const TileAsset_t* inactive_asset =
                assets_tiles_resolve_id(DISPLAY_DIAL_INACTIVE_TILESET,
                                        DISPLAY_DIAL_INACTIVE_COMPACT_TILE);

            /* Draw the green dial tile without editing the const map. */
            if (draw_asset_tile_scaled(camera,
                                       world_x,
                                       world_y,
                                       GAME_TILE_SIZE,
                                       GAME_TILE_SIZE,
                                       inactive_asset) == 0u) {
                draw_world_rect_scaled(camera, world_x, world_y, GAME_TILE_SIZE, GAME_TILE_SIZE, TD_GREEN, 1u);
            }
        }

        draw_world_rect_scaled(camera, (int16_t)(world_x + 1), (int16_t)(world_y - 1), 8u, 9u, colour, 1u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 1), (int16_t)(world_y - 1), 8u, 9u, TD_WHITE, 0u);
        draw_world_rect_scaled(camera, (int16_t)(world_x + 4), (int16_t)(world_y + 2), 2u, 3u, TD_BLACK, 1u);
    }
}

static void draw_topdown_boss_for_row(const GameState_t* game,
                                      const TopdownCamera_t* camera,
                                      uint8_t row)
{
    uint8_t colour;
    uint8_t boss_row = (uint8_t)((game->boss.y + BOSS_SIZE - 1u) / GAME_TILE_SIZE);

    if (game->current_level != LEVEL_DISPLAY_BOSS || game->boss.active == 0u) {
        return;
    }

    if (boss_row != row) {
        return;
    }

    if (draw_topdown_boss_sprite(game, camera) != 0u) {
        return;
    }

    colour = (game->boss.vulnerable != 0u) ? TD_RED : TD_MAGENTA;
    draw_world_rect_scaled(camera,
                           (int16_t)(game->boss.x - 5),
                           (int16_t)(game->boss.y + 8),
                           20u,
                           3u,
                           TD_BLACK,
                           1u);
    draw_world_rect_scaled(camera,
                           (int16_t)(game->boss.x - 5),
                           (int16_t)(game->boss.y - 10),
                           20u,
                           22u,
                           colour,
                           1u);
    draw_world_rect_scaled(camera,
                           (int16_t)(game->boss.x - 5),
                           (int16_t)(game->boss.y - 10),
                           20u,
                           22u,
                           TD_WHITE,
                           0u);
    draw_world_rect_scaled(camera,
                           (int16_t)(game->boss.x),
                           (int16_t)(game->boss.y - 3),
                           10u,
                           4u,
                           TD_BLACK,
                           1u);
}

static void draw_topdown_player_for_row(const GameState_t* game,
                                        const TopdownCamera_t* camera,
                                        uint8_t row)
{
    uint8_t player_row = (uint8_t)((game->player.y + game->player.size - 1u) / GAME_TILE_SIZE);

    if (player_row == row) {
        draw_topdown_player(game, camera);
    }
}

static void draw_topdown_context_prompt(const GameState_t* game)
{
    int16_t player_center_x = (int16_t)(game->player.x + (game->player.size / 2));
    int16_t player_center_y = (int16_t)(game->player.y + (game->player.size / 2));

    if (game->current_level == LEVEL_SPEAKER) {
        for (uint8_t i = 0u; i < game->speaker_lint_count; i++) {
            const RemovableObject_t* object = &game->removable_objects[i];
            int16_t object_center_x = (int16_t)((object->tile_x * GAME_TILE_SIZE) + (GAME_TILE_SIZE / 2));
            int16_t object_center_y = (int16_t)((object->tile_y * GAME_TILE_SIZE) + (GAME_TILE_SIZE / 2));

            if (object->active == 0u) {
                continue;
            }

            if (abs_i32((int32_t)player_center_x - object_center_x) <= FAULT_REPAIR_DISTANCE &&
                abs_i32((int32_t)player_center_y - object_center_y) <= FAULT_REPAIR_DISTANCE) {
                LCD_Draw_Rect(0u, CONTEXT_PROMPT_Y, RENDER_SCREEN_WIDTH, 22u, TD_BLACK, 1u);
                LCD_printString("CLEAN: LINT", 12u, CONTEXT_PROMPT_TEXT_Y, TD_YELLOW, 1u);
                return;
            }
        }
    }

    if (game->current_level == LEVEL_DISPLAY_BOSS) {
        for (uint8_t i = 0u; i < game->boss_dial_count; i++) {
            const BossDial_t* dial = &game->boss_dials[i];
            int16_t dial_center_x = (int16_t)((dial->tile_x * GAME_TILE_SIZE) + (GAME_TILE_SIZE / 2));
            int16_t dial_center_y = (int16_t)((dial->tile_y * GAME_TILE_SIZE) + (GAME_TILE_SIZE / 2));

            if (dial->active == 0u || dial->disabled != 0u) {
                continue;
            }

            if (abs_i32((int32_t)player_center_x - dial_center_x) <= FAULT_REPAIR_DISTANCE &&
                abs_i32((int32_t)player_center_y - dial_center_y) <= FAULT_REPAIR_DISTANCE) {
                LCD_Draw_Rect(0u, CONTEXT_PROMPT_Y, RENDER_SCREEN_WIDTH, 22u, TD_BLACK, 1u);
                LCD_printString("DISABLE DIAL", 12u, CONTEXT_PROMPT_TEXT_Y, TD_YELLOW, 1u);
                return;
            }
        }
    }

}

static void draw_topdown_camera_screen(const GameState_t* game)
{
    TopdownCamera_t camera = get_topdown_camera(game);
    uint8_t start_row = (uint8_t)(camera.y / GAME_TILE_SIZE);
    uint8_t end_row = (uint8_t)((camera.y + TOPDOWN_VIEW_WORLD_HEIGHT) / GAME_TILE_SIZE + 2);
    uint8_t map_height = game_get_active_map_height();

    if (end_row > map_height) {
        end_row = map_height;
    }

    /* Draw map rows top-to-bottom so lower objects appear in front. */
    draw_topdown_floors_and_wall_tops(game, &camera);
    for (uint8_t row = start_row; row < end_row; row++) {
        draw_topdown_wall_fronts_for_row(game, &camera, row);
        draw_topdown_removable_objects_for_row(game, &camera, row);
        draw_topdown_boss_dials_for_row(game, &camera, row);
        draw_topdown_boss_for_row(game, &camera, row);
        draw_topdown_player_for_row(game, &camera, row);
    }

    draw_display_corruption(game);
    draw_topdown_context_prompt(game);
}

static void draw_playing_screen(const GameState_t* game)
{
    draw_topdown_camera_screen(game);
}

static void draw_dialogue_screen(const GameState_t* game)
{
    const char* text = game->dialogue_message;
    uint8_t offset = 0u;
    uint8_t line = 0u;

    LCD_Draw_Rect(8u, DIALOGUE_BOX_Y, 224u, 88u, TD_BLACK, 1u);
    LCD_Draw_Rect(8u, DIALOGUE_BOX_Y, 224u, 88u, TD_WHITE, 0u);
    LCD_printString(game->dialogue_title, 16u, DIALOGUE_TITLE_Y, TD_CYAN, 1u);

    while (text[offset] != '\0' && line < 3u) {
        uint8_t count = 0u;
        uint8_t last_space = 0u;

        while (text[offset + count] != '\0' && count < 28u) {
            if (text[offset + count] == ' ') {
                last_space = count;
            }
            count++;
        }

        if (text[offset + count] != '\0' && last_space > 0u) {
            count = last_space;
        }

        memcpy(render_text_line, &text[offset], count);
        render_text_line[count] = '\0';
        LCD_printString(render_text_line, 16u, (uint16_t)(DIALOGUE_TEXT_Y + (line * 14u)), TD_WHITE, 1u);

        offset = (uint8_t)(offset + count);
        while (text[offset] == ' ') {
            offset++;
        }
        line++;
    }

    LCD_printString("A", 218u, DIALOGUE_ACTION_Y, TD_GOLD, 1u);
}

static void draw_complete_screen(const GameState_t* game)
{
    LCD_Draw_Rect(10u, 112u, 220u, 84u, 3u, 0u);
    LCD_printString(game->dialogue_title, 62u, 132u, 3u, 1u);
    LCD_printString(game->dialogue_message, 20u, 152u, 1u, 1u);
    LCD_printString("Press action", 82u, 176u, 14u, 1u);
}

void render_init(void)
{
    LCD_init(&lcd_cfg);
    LCD_Set_Palette(PALETTE_CUSTOM);
    LCD_Fill_Buffer(0u);
    LCD_Refresh(&lcd_cfg);
}

void render_frame(void)
{
    const GameState_t* game = game_get_state();

    LCD_Fill_Buffer(0u);

    if (game->run_state == GAME_STATE_PLAYING) {
        draw_playing_screen(game);
        if (game->dialogue_sequence != 0xFFu) {
            draw_dialogue_screen(game);
        }
    } else if (game->run_state == GAME_STATE_DIALOGUE) {
        draw_dialogue_screen(game);
    } else {
        draw_complete_screen(game);
    }

    LCD_Refresh(&lcd_cfg);
}
