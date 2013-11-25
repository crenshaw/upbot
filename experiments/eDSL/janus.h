
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void header_contents();
void construct_event_responder();

// CONSTANTS
#define DRIVE_LOW 0
#define DRIVE_MED 1
#define DRIVE_HI  2

#define BUMP_LEFT  0
#define BUMP_RIGHT 1
#define BUMP_BOTH  2

#define BLINK       0
#define BLINK_RED   1
#define BLINK_GREEN 2

typedef struct transition_tag {
    char *event;
    char *responder;

    int next_state;
} transition;

typedef struct state_list_tag {
    transition *state;
    struct state_list_tag *next_state;
} state_list;

typedef struct syntax_tree_tag {
    int current_state;
    int total_states;
    state_list *states_head;

    state_list *states;
} syntax_tree;

