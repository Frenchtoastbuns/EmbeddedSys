#ifndef GAME_STORY_H
#define GAME_STORY_H

/* Dialogue sequence IDs and static story table used by game.c. */

#include <stdint.h>

typedef enum {
    STORY_LAB_INTRO = 0,
    STORY_INTRO,
    STORY_BOARD_ENTRY,
    STORY_SPEAKER_ENTRY,
    STORY_SPEAKER_COMPLETE,
    STORY_DISPLAY_ENTRY,
    STORY_BOSS_INVULNERABLE,
    STORY_BOSS_VULNERABLE,
    STORY_ENDING,
    STORY_SEQUENCE_COUNT
} StorySequenceId_t;

typedef enum {
    STORY_FLAG_INTRO_DONE = 1u << 0,
    STORY_FLAG_BOARD_DONE = 1u << 1,
    STORY_FLAG_SPEAKER_ENTRY_DONE = 1u << 2,
    STORY_FLAG_DISPLAY_ENTRY_DONE = 1u << 3,
    STORY_FLAG_BOSS_INVULN_DONE = 1u << 4,
    STORY_FLAG_BOSS_VULN_DONE = 1u << 5
} StoryFlag_t;

typedef struct {
    const char* speaker;
    const char* text;
} DialogueLine_t;

typedef struct {
    const DialogueLine_t* lines;
    uint8_t length;
} StoryDef_t;

extern const StoryDef_t story_defs[STORY_SEQUENCE_COUNT];

#endif /* GAME_STORY_H */
