CC=gcc
CFLAGS=-Wall

.PHONY: clean

tokenout: tokenout.l
	flex $<
	$(CC) $(CFLAGS) -o $@ lex.yy.c

clean:
	rm -f lex.yy.c
