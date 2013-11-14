CC=gcc
CFLAGS=-O2

SRC_DIR = src
INC_DIR = include
TEST_DIR = test
BUILD_DIR = build

LEXER_EXEC=tokenout
TMP_TOKENOUT_ARCHIVE=tmp/tokenout
TOKENOUT_ARCHIVE=tokenout.tar.gz

PARSER_EXEC=parse
TMP_PARSER_ARCHIVE=parser
PARSER_ARCHIVE=parser.tar.gz

TMP_SEMANTICS_ARCHIVE=semantics
SEMANTICS_ARCHIVE=semantics.tar.gz

NITTY_GRITTY_OBJS=nitty_gritty/build/map.o nitty_gritty/build/utils.o nitty_gritty/build/linked_list.o nitty_gritty/build/array.o
COMPONENTS = env.c semantics.c declarations.c expressions.c str_table.c statements.c procedure.c
COMPONENT_OBJS = $(patsubst %, $(BUILD_DIR)/%, $(COMPONENTS:.c=.o))


.PHONY: clean lex_test lex_test2 parse_test parse_test_legal

################################################################################
## Components                                                                 ##
################################################################################

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	gcc -g -I $(INC_DIR) -I nitty_gritty/include -c $< -o $@

.PHONY: nitty_gritty
nitty_gritty:
	cd nitty_gritty && make all

################################################################################
## Executables                                                                ##
################################################################################

$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/tokenout.l
	flex -o $(SRC_DIR)/lex.yy.c $(SRC_DIR)/tokenout.l # makes lex.yy.c

#$(LEXER_EXEC): $(SRC_DIR)/lex.yy.c
#	$(CC) -D TOKENOUT_MAIN -I $(INC_DIR) -o $@ $(SRC_DIR)/lex.yy.c

$(LEXER_EXEC): $(SRC_DIR)/lex.yy.c $(SRC_DIR)/clike.tab.c $(SRC_DIR)/clike_fn.c
	$(CC) -D TOKENOUT_MAIN -I $(INC_DIR) -o $@ $^

$(INC_DIR)/clike.tab.h $(SRC_DIR)/clike.tab.c: $(SRC_DIR)/clike.y
	bison --defines=$(INC_DIR)/clike.tab.h -o $(SRC_DIR)/clike.tab.c $(SRC_DIR)/clike.y # makes clike.tab.h and clike.tab.c

#$(PARSER_EXEC): $(SRC_DIR)/clike.tab.c $(SRC_DIR)/lex.yy.c $(SRC_DIR)/clike_fn.c
#	$(CC) $(CFLAGS) -o $@ -I $(INC_DIR) $^ -ly -lfl

$(PARSER_EXEC): $(SRC_DIR)/clike.tab.c $(SRC_DIR)/lex.yy.c $(SRC_DIR)/clike_fn.c nitty_gritty $(COMPONENT_OBJS)
	$(CC) $(CFLAGS) -o $@ -I $(INC_DIR) -I nitty_gritty/include $(SRC_DIR)/clike.tab.c $(SRC_DIR)/lex.yy.c $(SRC_DIR)/clike_fn.c $(COMPONENT_OBJS) $(NITTY_GRITTY_OBJS) 

################################################################################
## Debugging                                                                  ##
################################################################################

#debug: tokenout.l clike.y clike_fn.h clike_fn.c
#	bison -d -t -v clike.y # makes clike.tab.h and clike.tab.c
#	flex tokenout.l # makes lex.yy.c
#	$(CC) $(CFLAGS) -DDEBUG=1 -o $@ lex.yy.c clike.tab.c clike_fn.c -ly -lfl


################################################################################
## Archiving                                                                  ##
################################################################################

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

$(SEMANTICS_ARCHIVE):
	make clean
	mkdir -p $(TMP_SEMANTICS_ARCHIVE)
	cp -R src nitty_gritty include Makefile $(TMP_SEMANTICS_ARCHIVE)
	tar zcvf $(SEMANTICS_ARCHIVE) $(TMP_SEMANTICS_ARCHIVE) --exclude=".*" --exclude="*.swp"
	rm -rf $(TMP_SEMANTICS_ARCHIVE)

################################################################################
## Testing                                                                    ##
################################################################################

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

env_test: $(TEST_DIR)/test_env.c $(BUILD_DIR)/env.o nitty_gritty
	gcc -g -I include -I $(TEST_DIR)/include -I nitty_gritty/include $(BUILD_DIR)/env.o $(NITTY_GRITTY_OBJS) $(TEST_DIR)/test_env.c

sem_test: $(PARSER_EXEC)
	python semantics_test/parse_test.py parse semantics_test

sem_test2_legal: $(PARSER_EXEC)
	python semantics_test2/parse_test.py parse semantics_test2/ legal

sem_test2_illegal: $(PARSER_EXEC)
	python semantics_test2/parse_test.py parse semantics_test2/ illegal

################################################################################
## Cleaning                                                                   ##
################################################################################

clean:
	rm -rf $(SRC_DIR)/lex.yy.c $(LEXER_EXEC) $(ARCHIVE) $(SRC_DIR)/clike.tab.c $(INC_DIR)/clike.tab.h $(PARSER_EXEC) $(BUILD_DIR) $(SEMANTICS_ARCHIVE)
	cd nitty_gritty && make clean
