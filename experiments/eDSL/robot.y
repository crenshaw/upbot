/**
 * robot.y
 *
 * The grammar definition for the UPBOT DSL.
 *
 * @author Fernando Freire
 * @since 04 Aug 2013
 *
 */
%{

#include <stdio.h>
#include <string.h>

%}

%union {
    int ival;
    char *bval;
    char *dval;
}

%token <ival> INT_MOD
%token <bval> BUMP_MOD
%token <dval> DRIVE_MOD

%token STATE_O STATE_C PAREN_O PAREN_C
%token LABEL TRUE BUMP STOP TIME WHEEL
%token CLIFF VWALL TURN SONG DRIVE BLINK
%token STOP_MOD TURN_MOD
%token PAIRED_WITH YIELDS EOL

%%

state : STATE_O LABEL EOL statement_list STATE_C EOL;

statement_list
        : statement EOL
        | statement_list statement EOL
        ;

statement : event_list PAIRED_WITH responder_list YIELDS LABEL;

event_list
        : event
        | PAREN_O event PAREN_C
        | PAREN_O event_list event PAREN_C
        ;

responder_list
        : responder
        | PAREN_O responder PAREN_C
        | PAREN_O responder_list responder PAREN_C
        ;

event
        : BUMP BUMP_MOD
        | TIME INT_MOD
        ;

responder
        : DRIVE DRIVE_MOD
        | TURN TURN_MOD
        | STOP STOP_MOD
        ;

%%

int main(int argc, char** argv) {
    yyparse();
}

yyerror(char *s) {
  fprintf(stderr, "error: %s\n", s);
}
