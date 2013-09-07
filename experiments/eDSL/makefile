# Filename: makefile
# Author: Fernando Freire
# Date created: 04 Aug, 2013

LEXER=flex
PARSER=bison
CC=gcc

all: parser lexer janus

parser:
	$(PARSER) -d janus.y

lexer:
	$(LEXER) janus.l

janus:
	$(CC) -o $@ janus.tab.c lex.yy.c

clean:
	@rm janus janusER.h janus.c *.tab.h *.tab.c lex.yy.c > /dev/null 2>&1 || true

