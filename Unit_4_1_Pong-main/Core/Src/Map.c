#include "Map.h"
#include "game.h"

/*
 * Thin wall-check adapter for Player.c.
 *
 * Player.c should not know about Tiled maps or generated map data, so game.c
 * gives this file a callback for the current active map.
 */

static uint8_t (*map_wall_check)(int tile_x, int tile_y);

void set_wall_checker(uint8_t (*check)(int tile_x, int tile_y))
{
    map_wall_check = check;
}

int is_wall(int tile_x, int tile_y)
{
    if (map_wall_check == 0) {
        return 1;
    }

    return map_wall_check(tile_x, tile_y) ? 1 : 0;
}
