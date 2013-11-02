CC=gcc
CFLAGS=-O2

SRC_DIR = src
INC_DIR = include

LEXER_EXEC=tokenout
TMP_TOKENOUT_ARCHIVE=tmp/tokenout
TOKENOUT_ARCHIVE=tokenout.tar.gz

PARSER_EXEC=parse
TMP_PARSER_ARCHIVE=parser
PARSER_ARCHIVE=parser.tar.gz

.PHONY: clean lex_test lex_test2 parse_test parse_test_legal

#$(LEXER_EXEC): tokenout.l
#	flex $<
#	$(CC) $(CFLAGS) -o $@ lex.yy.c

$(INC_DIR)/clike.tab.h $(SRC_DIR)/clike.tab.c: $(SRC_DIR)/clike.y
	bison --defines=$(INC_DIR)/clike.tab.h -o $(SRC_DIR)/clike.tab.c $(SRC_DIR)/clike.y # makes clike.tab.h and clike.tab.c

$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/tokenout.l
	flex -o $(SRC_DIR)/lex.yy.c $(SRC_DIR)/tokenout.l # makes lex.yy.c

$(PARSER_EXEC): $(SRC_DIR)/clike.tab.c $(SRC_DIR)/lex.yy.c $(SRC_DIR)/clike_fn.c
	$(CC) $(CFLAGS) -o $@ -I $(INC_DIR) $(SRC_DIR)/lex.yy.c $(SRC_DIR)/clike.tab.c $(SRC_DIR)/clike_fn.c -ly -lfl

#debug: tokenout.l clike.y clike_fn.h clike_fn.c
#	bison -d -t -v clike.y # makes clike.tab.h and clike.tab.c
#	flex tokenout.l # makes lex.yy.c
#	$(CC) $(CFLAGS) -DDEBUG=1 -o $@ lex.yy.c clike.tab.c clike_fn.c -ly -lfl

$(TOKENOUT_ARCHIVE): tokenout.l Makefile
	mkdir -p $(TMP_TOKENOUT_ARCHIVE)
	cp tokenout.l Makefile $(TMP_TOKENOUT_ARCHIVE)
	tar zcvf $(TOKENOUT_ARCHIVE) $(TMP_TOKENOUT_ARCHIVE)
	rm -rf tmp

$(PARSER_ARCHIVE): tokenout.l clike.y clike_fn.h clike_fn.c
	mkdir -p $(TMP_PARSER_ARCHIVE)
	cp tokenout.l clike.y clike_fn.h clike_fn.c Makefile $(TMP_PARSER_ARCHIVE)
	tar zcvf $(PARSER_ARCHIVE) $(TMP_PARSER_ARCHIVE)
	rm -rf $(TMP_PARSER_ARCHIVE)

clean:
	rm -f $(SRC_DIR)/lex.yy.c $(LEXER_EXEC) $(ARCHIVE) $(SRC_DIR)/clike.tab.c $(INC_DIR)/clike.tab.h $(PARSER_EXEC)

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
