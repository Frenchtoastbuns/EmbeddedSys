#ifndef ASSETS_TILES_H
#define ASSETS_TILES_H

/*
 * Flash-resident 16x16 RGB565 tile assets.
 * Headers only expose metadata and lookup functions; pixel arrays stay in .c.
 */

#include <stdint.h>

#define ASSETS_TILE_SIZE   16u
#define ASSETS_TILE_PIXELS (ASSETS_TILE_SIZE * ASSETS_TILE_SIZE)
#define ASSETS_OVERWORLD_TILE_COUNT 156u
#define ASSETS_SPEAKER_TILE_COUNT 145u
#define ASSETS_DISPLAY_TILE_COUNT 137u
#define ASSETS_TILE_TRANSPARENT_RGB565 0x0001u

typedef enum {
    TILESET_OVERWORLD = 0,
    TILESET_SPEAKER,
    TILESET_DISPLAY
} TilesetTheme_t;

typedef enum {
    TILE_EMPTY = 0,
    TILE_FLOOR,
    TILE_WALL,
    TILE_WALL_TOP,
    TILE_WALL_FRONT,
    TILE_DOOR,
    TILE_PROP,
    TILE_FAULT,
    TILE_CORRUPTION,
    TILE_BOSS_CORE
} LogicalTileId_t;

typedef struct {
    const uint16_t* pixels;
    uint8_t width;
    uint8_t height;
} TileAsset_t;

const TileAsset_t* assets_tiles_resolve(TilesetTheme_t theme, LogicalTileId_t logical_tile);
const TileAsset_t* assets_tiles_resolve_id(TilesetTheme_t theme, uint8_t tile_id);
uint8_t assets_tiles_count(TilesetTheme_t theme);

#endif /* ASSETS_TILES_H */
