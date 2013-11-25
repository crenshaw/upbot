/**
 * janus.l
 *
 * The token definition for the Janus DSL.
 *
 * @author Fernando Freire
 * @since 04 Aug 2013
 *
 */

%{

#include <stdio.h>
#include <string.h>
#include "janus.tab.h"

%}

%option noyywrap

%%

"begin state"   return STATE_O;
"end state"     return STATE_C;
"("             return PAREN_O;
")"             return PAREN_C;

(\"|\')[a-zA-Z]+[_\-0-9a-zA-z]*(\"|\')  {
                                            yylval.label = yytext;
                                            return LABEL;
                                        }

"nothing"   |
"otherwise" |
"_"             return NOOP;

"bump"          return BUMP;

"left"  |
"right" |
"both"          {
                    if (strcmp(yytext, "left") == 0) {
                        yylval.bump_mod = 0;
                    } else if (strcmp(yytext, "right") == 0) {
                        yylval.bump_mod = 1;
                    } else {
                        yylval.bump_mod = 2;
                    }

                    return BUMP_MOD;
                }

"stop"          return STOP;

[0-9]+          {
                    yylval.int_mod = atoi(yytext);
                    return INT_MOD;
                }

"now"           return STOP_MOD; // Stop now!!!

"time"          return TIME;

"drive"         return DRIVE;
"low"  |
"med"  |
"high"          {

                    if (strcmp(yytext, "low") == 0) {
                        yylval.drive_mod = 0;
                    } else if (strcmp(yytext, "med") == 0) {
                        yylval.drive_mod = 1;
                    } else {
                        yylval.drive_mod = 2;
                    }

                    return DRIVE_MOD;
                }

"blink"         return LED;
"red"   |
"green"         {
                    if (strcmp(yytext, "red") == 0) {
                        yylval.drive_mod = 0;
                    } else {
                        yylval.drive_mod = 1;
                    }

                    return LED_MOD;
                }

"turn"          return TURN;

"random"        return TURN_MOD;

"vwall"         return VWALL;
"wheel"         return WHEEL;
"cliff"         return CLIFF;
"song"          return SONG;

"->"            return PAIRED_WITH;

":"             return YIELDS;

"//".*\n        // Comments do not require a token (they are ignored by the compiler).
[\t\n\r]+       // Whitespace does not require a token (it is ignored by the compiler).
.               // Catchall for tokens that do not mach anything
%%
