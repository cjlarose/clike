CC=gcc
CFLAGS=-O2
LEXER_EXEC=tokenout
PARSER_EXEC=parse
ARCHIVE=tokenout.tar.gz
TMPARCHIVE=tmp/tokenout

.PHONY: clean test test2

$(LEXER_EXEC): tokenout.l
	flex $<
	$(CC) $(CFLAGS) -o $@ lex.yy.c

$(PARSER_EXEC): tokenout.l clike.y clike_fn.h clike_fn.c
	bison -d clike.y # makes clike.tab.h and clike.tab.c
	flex tokenout.l # makes lex.yy.c
	$(CC) $(CFLAGS) -o $@ lex.yy.c clike.tab.c clike_fn.c

$(ARCHIVE): tokenout.l Makefile
	mkdir -p $(TMPARCHIVE)
	cp tokenout.l Makefile $(TMPARCHIVE)
	tar zcvf $(ARCHIVE) $(TMPARCHIVE)
	rm -rf tmp

clean:
	rm -f lex.yy.c $(LEXER_EXEC) $(ARCHIVE) clike.tab.c clike.tab.h $(PARSER_EXEC)

test: $(LEXER_EXEC)
	python test/testharness.py "/home/cjlarose/csc453/lexer/tokenout" test/tokens

test2: $(LEXER_EXEC)
	python test/testharness.py "/home/cjlarose/csc453/lexer/tokenout" test/TestProg1
