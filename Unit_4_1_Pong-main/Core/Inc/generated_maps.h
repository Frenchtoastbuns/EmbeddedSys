#ifndef GENERATED_MAPS_H
#define GENERATED_MAPS_H

/*
 * Static map data produced by the PC-side Tiled converter.
 * The STM32 does not parse JSON/TMJ at runtime; it only reads these arrays.
 */

#include <stdint.h>
#include "game.h"

#define GENERATED_MAP_MAX_W 32u
#define GENERATED_MAP_MAX_H 32u

typedef enum {
    GENERATED_MAP_OVERWORLD = 0,
    GENERATED_MAP_SPEAKER,
    GENERATED_MAP_DISPLAY
} GeneratedMapId_t;

typedef enum {
    GENERATED_ENTITY_NONE = 0,
    GENERATED_ENTITY_PLAYER_SPAWN,
    GENERATED_ENTITY_MODULE_ENTRANCE,
    GENERATED_ENTITY_MODULE_EXIT,
    GENERATED_ENTITY_SPEAKER_LINT,
    GENERATED_ENTITY_FAULT_NODE,
    GENERATED_ENTITY_BOSS_DIAL,
    GENERATED_ENTITY_BOSS_SPAWN
} GeneratedEntityType_t;

typedef struct {
    uint8_t type;
    uint8_t tile_x;
    uint8_t tile_y;
    CampaignLevel_t level;
} GeneratedEntity_t;

typedef struct {
    uint8_t width;
    uint8_t height;
    const uint8_t* ground;
    const uint8_t* objects;
    const uint8_t* collision;
    const GeneratedEntity_t* entities;
    uint8_t entity_count;
} GeneratedMapData_t;

const GeneratedMapData_t* generated_map_get(GeneratedMapId_t id);
uint8_t generated_map_tile_at(const GeneratedMapData_t* map, const uint8_t* layer, uint8_t tile_x, uint8_t tile_y);
uint8_t generated_map_find_entity(const GeneratedMapData_t* map, uint8_t entity_type, uint8_t occurrence, GeneratedEntity_t* out_entity);

#endif /* GENERATED_MAPS_H */
