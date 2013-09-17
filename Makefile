CC=gcc
CFLAGS=-O2
EXECUTABLE=tokenout
ARCHIVE=tokenout.tar.gz
TMPARCHIVE=tmp/tokenout

.PHONY: clean test test2

$(EXECUTABLE): tokenout.l
	flex $<
	$(CC) $(CFLAGS) -o $@ lex.yy.c


$(ARCHIVE): tokenout.l Makefile
	mkdir -p $(TMPARCHIVE)
	cp tokenout.l Makefile $(TMPARCHIVE)
	tar zcvf $(ARCHIVE) $(TMPARCHIVE)
	rm -rf tmp

clean:
	rm -f lex.yy.c $(EXECUTABLE) $(ARCHIVE)

test: $(EXECUTABLE)
	python test/testharness.py "/home/cjlarose/csc453/lexer/tokenout" test/tokens

test2: $(EXECUTABLE)
	python test/testharness.py "/home/cjlarose/csc453/lexer/tokenout" test/TestProg1
