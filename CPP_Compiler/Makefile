CXX = g++
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

TARGET = cpp_compiler.exe

all: $(TARGET)

cpp_parser.tab.c cpp_parser.tab.h: cpp_parser.y
	$(BISON) -d -o cpp_parser.tab.c cpp_parser.y

cpp_lex.yy.c: cpp_lexer.l cpp_parser.tab.h
	$(FLEX) -o cpp_lex.yy.c cpp_lexer.l

$(TARGET): cpp_lex.yy.c cpp_parser.tab.c cpp_main.c $(SHARED_SRCS)
	$(CXX) $(CFLAGS) -o $(TARGET) cpp_lex.yy.c cpp_parser.tab.c cpp_main.c $(SHARED_SRCS)

clean:
	rm -f $(TARGET) cpp_lex.yy.c cpp_parser.tab.c cpp_parser.tab.h

.PHONY: all clean
