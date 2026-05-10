#include "hardware.h"

/*
 * Optional hardware hook.
 *
 * The final demo does not depend on the old LED bar/audio experiments. Keeping
 * these empty functions lets main.c stay simple while avoiding unstable
 * external hardware side effects.
 */

void start_extra_stuff(void)
{
}

void update_extra_stuff(const GameState_t *game)
{
    (void)game;
}
