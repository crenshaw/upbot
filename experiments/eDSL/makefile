# Filename: makefile
# Author: Fernando Freire
# Date created: 04 Aug, 2013

LEXER=flex
PARSER=bison
CC=gcc

all: parser lexer robot

parser:
	$(PARSER) -d robot.y

lexer:
	$(LEXER) robot.l

robot:
	$(CC) -o $@ robot.tab.c lex.yy.c

clean:
	@rm *.h robot *.tab.c lex.yy.c > /dev/null 2>&1 || true

