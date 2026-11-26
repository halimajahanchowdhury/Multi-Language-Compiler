CC = gcc
CFLAGS = -Wall -Wextra -g -I../src/ast -I../src/symbol_table -I../src/semantic -I../src/tac -I.
FLEX = flex
BISON = bison

SHARED_SRCS = ../src/ast/ast.c \
              ../src/ast/ast_print.c \
              ../src/ast/ast_optimize.c \
              ../src/symbol_table/symtab.c \
              ../src/semantic/semantic.c \
              ../src/semantic/typecheck.c \
              ../src/tac/tac.c \
              ../src/tac/tac_expr.c \
              ../src/tac/tac_stmt.c

TARGET = c_compiler.exe

all: $(TARGET)

c_parser.tab.c c_parser.tab.h: c_parser.y
	$(BISON) -d -o c_parser.tab.c c_parser.y

c_lex.yy.c: c_lexer.l c_parser.tab.h
	$(FLEX) -o c_lex.yy.c c_lexer.l

$(TARGET): c_lex.yy.c c_parser.tab.c c_main.c $(SHARED_SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) c_lex.yy.c c_parser.tab.c c_main.c $(SHARED_SRCS)

clean:
	rm -f $(TARGET) c_lex.yy.c c_parser.tab.c c_parser.tab.h

.PHONY: all clean
