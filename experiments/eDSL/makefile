FLEX=flex
BISON=bison
CC=gcc

all: parser lexer robot

parser:
	$(BISON) -d robot.y

lexer:
	$(FLEX) robot.l

robot:
	$(CC) -o $@ robot.tab.c lex.yy.c

clean:
	@rm *.h robot *.tab.c lex.yy.c > /dev/null 2>&1 || true

