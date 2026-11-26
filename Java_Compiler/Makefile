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

TARGET = java_compiler.exe

all: $(TARGET)

java_parser.tab.c java_parser.tab.h: java_parser.y
	$(BISON) -d -o java_parser.tab.c java_parser.y

java_lex.yy.c: java_lexer.l java_parser.tab.h
	$(FLEX) -o java_lex.yy.c java_lexer.l

$(TARGET): java_lex.yy.c java_parser.tab.c java_main.c $(SHARED_SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) java_lex.yy.c java_parser.tab.c java_main.c $(SHARED_SRCS)

clean:
	rm -f $(TARGET) java_lex.yy.c java_parser.tab.c java_parser.tab.h

.PHONY: all clean
