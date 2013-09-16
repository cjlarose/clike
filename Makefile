CC=gcc
CFLAGS=-Wall
EXECUTABLE=tokenout

.PHONY: clean

$(EXECUTABLE): tokenout.l
	flex $<
	$(CC) $(CFLAGS) -o $@ lex.yy.c

clean:
	rm -f lex.yy.c $(EXECUTABLE)
