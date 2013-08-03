/**
 * robot.l
 *
 * The token definition for the UPBOT DSL.
 *
 * @author Fernando Freire
 * @since 04 Aug 2013
 *
 */

%{

#include <stdio.h>
#include "robot.tab.h"

%}

%option noyywrap

%%

"begin state"   return STATE_O;
"end state"     return STATE_C;
"("             return PAREN_O;
")"             return PAREN_C;

\"[a-zA-Z]+\"   return LABEL;

"nothing"   |
"otherwise" |
"_"             return NOOP;

"bump"          return BUMP;
"left"  |
"right" |
"both"          {
                    yylval.bval = yytext;
                    return BUMP_MOD;
                }

"stop"          return STOP;
[0-9]+          {
                    yylval.ival = atoi(yytext);
                    return INT_MOD;
                }
"now"           return STOP_MOD; // Stop now!!!

"time"          return TIME;

"drive"         return DRIVE;
"low"  |
"med"  |
"high"          {
                    yylval.dval = yytext;
                    return DRIVE_MOD;
                }

"vwall"         return VWALL;
"wheel"         return WHEEL;
"cliff"         return CLIFF;
"turn"          return TURN;
"song"          return SONG;
"blink"         return BLINK;



"->"            return PAIRED_WITH;

":"             return YIELDS;

\n              return EOL;
"//".*\n        // Do nothing for comments
[\t]+           // Do nothing for whitespace

%%

