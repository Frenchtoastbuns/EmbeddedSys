#ifndef MAP_H
#define MAP_H

/* Current-map wall query used by the player movement code. */

#include <stdint.h>

void Map_Set_Wall_Check(uint8_t (*check)(int tile_x, int tile_y));
int Map_Is_Wall(int tile_x, int tile_y);

#endif /* MAP_H */
