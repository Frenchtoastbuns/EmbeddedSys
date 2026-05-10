#ifndef MAP_H
#define MAP_H

/* Current-map wall query used by the player movement code. */

#include <stdint.h>

void set_wall_checker(uint8_t (*check)(int tile_x, int tile_y));
int is_wall(int tile_x, int tile_y);

#endif /* MAP_H */
