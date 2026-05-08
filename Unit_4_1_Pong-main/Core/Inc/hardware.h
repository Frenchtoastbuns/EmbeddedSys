#ifndef HARDWARE_H
#define HARDWARE_H

/* Safe optional hardware interface called by main.c once per frame. */

#include "game.h"

void hardware_init(void);
void hardware_update(const GameState_t* game);

#endif /* HARDWARE_H */
