/**
 * janus.y
 *
 * The grammar definition for the Janus DSL.
 *
 * @author Fernando Freire
 * @since 04 Aug 2013
 *
 */
%{

#include "janus.h"

syntax_tree *janus;

// Since the UPBOT system does not support
// string state names, then it is silly for Janus
// to do so as well. For now just assign an arbitrary
// integer to represent a state.
int next_state_idx;

%}

/**
 * UNION
 *
 * This union definition allows us to extend the YYLVAL variable that gets
 * thrown around between FLEX and BISON.
 *
 */
%union {
    int int_mod;        // Integer value that is received from INT_MOD
    int bump_mod;       // Integer value that is received from BUMP_MOD
    int drive_mod;      // Integer value that is received from DRIVE_MOD
    int led_mod;        // Integer value that is received from LED_MOD
    char *label;        // String value used to identify a state
    char *string;       // String type
    struct transition *transition;  // Struct defining a Janus statement
}

/**
 * TOKENS
 *
 * These are the supported tokens that FLEX is currently catching. In the case
 * of the first three tokens, we are actually capturing input, so we need to
 * specify their type before proceeding.
 *
 */
%token <int_mod>    INT_MOD
%token <bump_mod>   BUMP_MOD
%token <drive_mod>  DRIVE_MOD
%token <led_mod>    LED_MOD
%token <label>      LABEL
%token TURN_MOD /* At this time only turning randomly is supported */

%token STATE_O STATE_C PAREN_O PAREN_C
%token NOOP BUMP STOP TIME WHEEL CLIFF
%token VWALL TURN SONG DRIVE LED
%token STOP_MOD TURN_MOD
%token PAIRED_WITH YIELDS

/* better error messages */
%error-verbose


%type <transition>  state
%type <transition>  statement
%type <string>      event
%type <string>      responder

/* Where should we start parsing? */
%start state_list

%%

state_list
        : state_list state
        | state {
                    janus->total_states++;

                    janus->states = malloc(sizeof(state_list));
                    janus->states_head = janus->states;
                    janus->states->state = $1;
                    janus->states->next_state = NULL;
                }
        ;

state
        : STATE_O LABEL statement STATE_C   {
                                                $$ = $3;
                                            }
        ;

statement
        : event PAIRED_WITH responder YIELDS LABEL  {
                                                        transition *stmt = malloc(sizeof(transition));
                                                        stmt->event = $1;
                                                        stmt->responder = $3;

                                                        stmt->next_state = (int) next_state_idx;
                                                        next_state_idx++;

                                                        $$ = stmt;
                                                    }
        ;

event
        : BUMP BUMP_MOD {
                            switch($2) {
                                case BUMP_LEFT:
                                    $$ = "eventBumpLeft";
                                    break;
                                case BUMP_RIGHT:
                                    $$ = "eventBumpRight";
                                    break;
                                case BUMP_BOTH:
                                    $$ = "eventBump";
                                    break;
                            }
                        }

        | TIME INT_MOD  { $$ = "eventAlarm"; }

        | NOOP          { $$ = "eventTrue"; }

        | VWALL         { $$ = "eventVWall"; }
        ;

responder
        : DRIVE DRIVE_MOD   {
                                switch($2) {
                                    case DRIVE_LOW:
                                        $$ = "respondDriveLow";
                                        break;
                                    case DRIVE_MED:
                                        $$ = "respondDriveMed";
                                        break;
                                    case DRIVE_HI:
                                        $$ = "respondDriveHigh";
                                        break;
                                }
                            }

        | LED LED_MOD       {
                                switch($2) {
                                    case BLINK:
                                        $$ = "respondLedBlink";
                                        break;
                                    case BLINK_RED:
                                        $$ = "respondLedRed";
                                        break;
                                    case BLINK_GREEN:
                                        $$ = "respondLedGreen";
                                        break;
                                }
                            }

        | TURN TURN_MOD     { $$ = "respondTurn"; }

        | STOP STOP_MOD     { $$ = "respondStop"; }

        ;

%%

/**
 * main()
 *
 * Responsible for running the yyparse function, which will apply the above
 * grammar definition to the tokenized input returned from FLEX.
 *
 * @param[in] argc The number of arguments passed to main.
 * @param[in] argv An array of arguments passed to main.
 *
 */
int main(int argc, char** argv) {
    next_state_idx = 0; // Init out state counter.

    janus = malloc(sizeof(syntax_tree));
    janus->current_state = 0;
    janus->total_states = 0;

    yyparse();
    header_contents();
    construct_event_responder();

    exit(EXIT_SUCCESS);
}

/**
 * yyerror()
 *
 * This function is called if yyparse encounters an error during the parsing of
 * the input file.
 *
 * @param[in] s The error string generated by BISON.
 *
 */
yyerror(char *s) {
  fprintf(stderr, "error: %s\n", s);
}

/**
 * headerContents()
 *
 * Output a static set of header contents into a header file that will be included
 * in the generated C file from Janus.
 *
 */
void header_contents() {
    FILE* header = fopen("janusER.h", "w");

    fputs("/* This file was generated by the Janus DSL parser. */\n", header);
    fputs("#include \"eventresponder.h\"\n", header);
    fputs("#include \"events.h\"\n", header);
    fputs("#include \"responders.h\"\n", header);
    fputs("#ifndef JANUS_EVENT_RESPONDER\n", header);
    fputs("#define JANUS_EVENT_RESPONDER\n", header);
    fputs("void initializeJanusER(eventResponder* jER);\n", header);
    fputs("void selectNextER(char* erName, eventResponder* myER);\n", header);
    fputs("#endif\n", header);

    fclose(header);
}

void construct_event_responder() {
    // A pointer to the head of the transition list.
    state_list *tmp_head;



    printf("\n");
    printf("#include \"janusER.h\"\n");
    printf("void initializeJanusER(eventResponder* jER) {\n");
    printf("  jER->curState = %d;\n", janus->current_state);
    printf("  jER->stateCount = %d;\n", janus->total_states);
    printf("  jER->states = malloc(sizeof(state)*%d);\n", janus->total_states);

    tmp_head = janus->states_head;
    int transition_size = 0;  // The number of transitions we have.
    while(tmp_head != NULL) {
        transition_size++;
        tmp_head = tmp_head->next_state;
    }

    tmp_head = janus->states_head;
    int state_idx = 0;  // The number of states we have.
    while(tmp_head != NULL) {
        printf("    jER->states[%d].count = %d;\n", state_idx, transition_size);
        printf("    jER->states[%d].clockTime = 0;\n", state_idx);
        printf("    jER->states[%d].transitions = malloc(sizeof(transition) * %d);\n", state_idx, transition_size);

        printf("      jER->states[%d].transitions[%d].e = %s;\n", state_idx, state_idx, janus->states->state->event);
        printf("      jER->states[%d].transitions[%d].r = %s;\n", state_idx, state_idx, janus->states->state->responder);
        printf("      jER->states[%d].transitions[%d].n = %d;\n", state_idx, state_idx, janus->states->state->next_state);

        tmp_head = tmp_head->next_state;
        state_idx++;
    }

    printf("}\n");

}

