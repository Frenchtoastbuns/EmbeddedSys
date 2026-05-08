#include "game_story.h"

/*
 * Static dialogue script.
 *
 * These strings live in Flash/rodata. The dialogue system in game.c only keeps
 * a sequence index and line index, so no dynamic memory is needed.
 */

static const DialogueLine_t story_lab_intro[] = {
    {"LOCATION", "In Lab 1.60"}
};

static const DialogueLine_t story_intro[] = {
    {"Player", "Professor, my game is cooked."},
    {"Professor", "Cooked how?"},
    {"Player", "Speaker, display... both gone."},
    {"Professor", "Perfect. I'll send you inside the board."},
    {"Player", "You're not coming?"},
    {"Professor", "No chance. I'll guide you over comms."},
    {"Player", "Of course you will."},
    {"Professor", "Step into the transporter."}
};

static const DialogueLine_t story_board_entry[] = {
    {"PROF COMMS", "Signal received. You're inside the circuit."},
    {"PROF COMMS", "This area links the MCU to the breadboard."},
    {"PROF COMMS", "Only the speaker module is open. Start there."}
};

static const DialogueLine_t story_speaker_entry[] = {
    {"PROF COMMS", "The speaker path is blocked."},
    {"Player", "Is that lint?"},
    {"PROF COMMS", "Worse. Droppings."},
    {"Player", "That is disgusting."},
    {"PROF COMMS", "Agreed. Clean it up before I ask questions."}
};

static const DialogueLine_t story_speaker_complete[] = {
    {"PROF COMMS", "Speaker output restored."},
    {"Player", "Please tell me we're almost done."},
    {"PROF COMMS", "One module left: the display controller."}
};

static const DialogueLine_t story_display_entry[] = {
    {"PROF COMMS", "Oh. There it is."},
    {"Player", "Is that a bug?"},
    {"PROF COMMS", "Unfortunately, yes. A literal bug in your circuit."},
    {"Player", "Good thing I conveniently have bug spray in my back pocket."},
    {"PROF COMMS", "That is not how embedded debugging works."},
    {"Player", "Worth a try."},
    {"PROF COMMS", "It appears to be eating RAM to stay invulnerable."},
    {"PROF COMMS", "Clear the cache nodes and cut off its memory supply."}
};

static const DialogueLine_t story_boss_invulnerable[] = {
    {"PROF COMMS", "Your shots are doing nothing."},
    {"Player", "So... bug spray is not enough?"},
    {"PROF COMMS", "Not while its shield is powered."},
    {"PROF COMMS", "Clear all six cache nodes first."}
};

static const DialogueLine_t story_boss_vulnerable[] = {
    {"PROF COMMS", "Shield down."},
    {"Player", "Finally. Bug spray time."},
    {"PROF COMMS", "Debug that thing."}
};

static const DialogueLine_t story_ending[] = {
    {"PROF COMMS", "System restored."},
    {"Player", "So my project works?"},
    {"PROF COMMS", "It should."},
    {"PROF COMMS", "Now submit it before the deadline."},
    {"Player", "Of course it is."}
};

#define STORY_LEN(sequence) ((uint8_t)(sizeof(sequence) / sizeof((sequence)[0])))

const StoryDef_t story_defs[STORY_SEQUENCE_COUNT] = {
    {story_lab_intro, STORY_LEN(story_lab_intro)},
    {story_intro, STORY_LEN(story_intro)},
    {story_board_entry, STORY_LEN(story_board_entry)},
    {story_speaker_entry, STORY_LEN(story_speaker_entry)},
    {story_speaker_complete, STORY_LEN(story_speaker_complete)},
    {story_display_entry, STORY_LEN(story_display_entry)},
    {story_boss_invulnerable, STORY_LEN(story_boss_invulnerable)},
    {story_boss_vulnerable, STORY_LEN(story_boss_vulnerable)},
    {story_ending, STORY_LEN(story_ending)}
};
