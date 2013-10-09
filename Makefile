CC=gcc
CFLAGS=-O2
LEXER_EXEC=tokenout
PARSER_EXEC=parse
ARCHIVE=tokenout.tar.gz
TMPARCHIVE=tmp/tokenout

.PHONY: clean lex_test lex_test2 parse_test parse_test_legal

$(LEXER_EXEC): tokenout.l
	flex $<
	$(CC) $(CFLAGS) -o $@ lex.yy.c

$(PARSER_EXEC): tokenout.l clike.y clike_fn.h clike_fn.c
	bison -d clike.y # makes clike.tab.h and clike.tab.c
	flex tokenout.l # makes lex.yy.c
	$(CC) $(CFLAGS) -o $@ lex.yy.c clike.tab.c clike_fn.c -ly -lfl

debug: tokenout.l clike.y clike_fn.h clike_fn.c
	bison -d -t -v clike.y # makes clike.tab.h and clike.tab.c
	flex tokenout.l # makes lex.yy.c
	$(CC) $(CFLAGS) -DDEBUG=1 -o $@ lex.yy.c clike.tab.c clike_fn.c -ly -lfl

$(ARCHIVE): tokenout.l Makefile
	mkdir -p $(TMPARCHIVE)
	cp tokenout.l Makefile $(TMPARCHIVE)
	tar zcvf $(ARCHIVE) $(TMPARCHIVE)
	rm -rf tmp

clean:
	rm -f lex.yy.c $(LEXER_EXEC) $(ARCHIVE) clike.tab.c clike.tab.h $(PARSER_EXEC)

lex_test: $(LEXER_EXEC)
	python lex_test/testharness.py "/home/cjlarose/csc453/lexer/tokenout" lex_test/tokens

lex_test2: $(LEXER_EXEC)
	python lex_test/testharness.py "/home/cjlarose/csc453/lexer/tokenout" lex_test/TestProg1

parse_test: $(PARSER_EXEC)
	python parse_test/parse_test.py parse parse_test

parse_test_legal: $(PARSER_EXEC)
	python parse_test/parse_test.py parse parse_test legal

parse_test_illegal: $(PARSER_EXEC)
	python parse_test/parse_test.py parse parse_test illegal
