#ifndef ASSETS_SPRITES_H
#define ASSETS_SPRITES_H

/*
 * Flash-resident player/boss sprite animations.
 * PNGs are converted offline; firmware only sees RGB565 arrays.
 */

#include <stdint.h>

#ifndef USE_SPRITE_ASSETS
#define USE_SPRITE_ASSETS 1u
#endif

#define SPRITE_TRANSPARENT_RGB565 0x0001u

typedef enum {
    SPRITE_ANIM_PLAYER_WALK_FRONT = 0,
    SPRITE_ANIM_PLAYER_WALK_BACK,
    SPRITE_ANIM_PLAYER_WALK_RIGHT,
    SPRITE_ANIM_BOSS_IDLE,
    SPRITE_ANIM_COUNT
} SpriteAnimationId_t;

typedef struct {
    const uint16_t* pixels;
    uint8_t width;
    uint8_t height;
} SpriteFrame_t;

typedef struct {
    const SpriteFrame_t* frames;
    uint8_t frame_count;
    uint8_t ticks_per_frame;
} SpriteAnimation_t;

const SpriteAnimation_t* assets_sprites_get(SpriteAnimationId_t animation);
uint32_t assets_sprites_flash_bytes(void);

#endif /* ASSETS_SPRITES_H */
