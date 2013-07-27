/*** Definition Section ***/
%{ 
#include <stdio.h>
#include <string.h>

#define YYSTYPE char *

void yyerror(const char* str) {
    fprintf(stderr, "error: %s\n", str);
}

int yywrap() { return 1; }

main() { yyparse(); }

%}

%token STATE_O STATE_C PAREN_O PAREN_C
%token LABEL TRUE BUMP STOP TIME WHEEL
%token CLIFF VWALL TURN SONG DRIVE BLINK
%token BUMP_MOD INT_MOD STOP_MOD
%token PAIRED_WITH YIELDS EOL

%%

%%
