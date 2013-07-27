/*** Definition section ***/

%{
#include <stdio.h>
%}

/* only read one file at a time */
%option noyywrap

%%

"begin state"   printf(" STATE_O "); return STATE_O
"end state"     printf(" STATE_C "); return STATE_C
"("             printf(" PAREN_O "); return PAREN_O
")"             printf(" PAREN_C "); return PAREN_C

\"[a-zA-Z]+\"   printf(" LABEL "); return LABEL;

"nothing"   |
"otherwise" |
"_"             printf(" TRUE "); return TRUE;

"bump"          printf(" BUMP "); return BUMP;
"left"  |
"right" |
"both"          {
                    printf(" BUMP_MOD ");
                    struct symtab *sp = symlook(yytext);
                    yylval.symp = sp;
                    return BUMP_MOD;
                }

"stop"          printf(" STOP "); return STOP;
[0-9]+          {
                    printf(" INT_MOD "); // A modifier for both "time" and "stop"
                    yylval.dval = atoi(yytext);
                    return INT_MOD;
                }
"now"           printf(" STOP_MOD "); return STOP_MOD; // Stop now!!!

"time"          printf(" TIME "); return TIME;

"drive"         printf(" DIRVE "); return DRIVE;
"low"  |
"med"  |
"high"          {
                    printf(" DRIVE_MOD ");
                    struct symtab *sp = symlook(yytext);
                    yylval.symp = sp;
                    return DRIVE_MOD;
                }

"wheel"         printf(" WHEEL "); return WHEEL;
"cliff"         printf(" CLIFF "); return CLIFF;
"vwall"         printf(" VWALL "); return VWALL;
"turn"          printf(" TURN ");  return TURN;
"song"          printf(" SONG ");  return SONG;
"blink"         printf(" BLINK "); return BLINK;



"->"            printf(" PAIRED_WITH "); return PAIRED_WITH;

":"             printf(" YIELDS ");      return YIELDS;

\n              printf("\n");            return EOL;
"//".*          // Do nothing for comments
[\t]+           // Do nothing for whitespace

%%

