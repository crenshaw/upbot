FLEX=flex
CC=gcc

all: lexer parser compile

lexer:
	$(FLEX) robot.l

parser:
	$(CC) lex.yy.c -o rbt.out -lfl

compile:
	echo "Compiling\!"
