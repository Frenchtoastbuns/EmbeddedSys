#ifndef HARDWARE_H
#define HARDWARE_H

/* Safe optional hardware interface called by main.c once per frame. */

#include "game.h"

void start_extra_stuff(void);
void update_extra_stuff(const GameState_t* game);

#endif /* HARDWARE_H */
