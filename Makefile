CC=gcc
CFLAGS=-Wall
EXECUTABLE=tokenout

.PHONY: clean test test2

$(EXECUTABLE): tokenout.l
	flex $<
	$(CC) $(CFLAGS) -o $@ lex.yy.c

clean:
	rm -f lex.yy.c $(EXECUTABLE)

test:
	python test/testharness.py "/home/cjlarose/csc453/lexer/tokenout" test/tokens

test2:
	python test/testharness.py "/home/cjlarose/csc453/lexer/tokenout" test/TestProg1
