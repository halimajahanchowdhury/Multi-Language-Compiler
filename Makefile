CC = gcc
CFLAGS = -Wall -Wextra -Isrc -Isrc/lexer -Isrc/parser -Isrc/ast -Isrc/symbol_table -Isrc/semantic -Isrc/tac
FLEX = flex
BISON = bison
RM = rm -f

# Target executable
TARGET = compiler.exe

# Directories
SRC_DIR = src
LEXER_DIR = src/lexer
PARSER_DIR = src/parser
AST_DIR = src/ast
SYM_DIR = src/symbol_table
SEM_DIR = src/semantic
TAC_DIR = src/tac

all: $(TARGET)

# Parser generation
$(PARSER_DIR)/parser.tab.c $(PARSER_DIR)/parser.tab.h: $(PARSER_DIR)/parser.y
	$(BISON) -d -o $(PARSER_DIR)/parser.tab.c $(PARSER_DIR)/parser.y

# Lexer generation
$(LEXER_DIR)/lex.yy.c: $(LEXER_DIR)/lexer.l $(PARSER_DIR)/parser.tab.h
	$(FLEX) -o $(LEXER_DIR)/lex.yy.c $(LEXER_DIR)/lexer.l

# Build compiler executable
$(TARGET): $(LEXER_DIR)/lex.yy.c $(PARSER_DIR)/parser.tab.c
	$(CC) $(CFLAGS) -o $@ \
		$(SRC_DIR)/main.c \
		$(LEXER_DIR)/lex.yy.c \
		$(PARSER_DIR)/parser.tab.c \
		$(AST_DIR)/ast.c \
		$(AST_DIR)/ast_print.c \
		$(SYM_DIR)/symtab.c \
		$(SEM_DIR)/semantic.c \
		$(SEM_DIR)/typecheck.c \
		$(TAC_DIR)/tac.c \
		$(TAC_DIR)/tac_expr.c \
		$(TAC_DIR)/tac_stmt.c

clean:
	$(RM) $(TARGET)
	$(RM) $(LEXER_DIR)/lex.yy.c
	$(RM) $(PARSER_DIR)/parser.tab.c $(PARSER_DIR)/parser.tab.h
	$(RM) *.o

.PHONY: all clean
