CC=gcc
CFLAGS=-Wall
EXECUTABLE=tokenout

.PHONY: clean test

$(EXECUTABLE): tokenout.l
	flex $<
	$(CC) $(CFLAGS) -o $@ lex.yy.c

clean:
	rm -f lex.yy.c $(EXECUTABLE)

test:
	python test/testharness.py "/home/cjlarose/csc453/lexer/tokenout" test/TestProg1
