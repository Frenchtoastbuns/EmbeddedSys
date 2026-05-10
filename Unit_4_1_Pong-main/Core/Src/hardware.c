#include "hardware.h"

/*
 * Optional hardware hook.
 *
 * The final demo does not depend on the old LED bar/audio experiments. Keeping
 * these empty functions lets main.c stay simple while avoiding unstable
 * external hardware side effects.
 */

/* starts optional extra hardware if it is enabled */
void start_extra_stuff(void)
{
}

/* updates optional hardware once per frame */
void update_extra_stuff(const GameState_t *game)
{
    (void)game;
}
