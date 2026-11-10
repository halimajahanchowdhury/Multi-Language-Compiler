%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex(void);
extern int line_num;
extern int col_num;
extern FILE *yyin;

int syntax_errors = 0;

void yyerror(const char *s) {
    fprintf(stderr, "[Syntax Error] %s at line %d, near column %d\n", s, line_num, col_num);
    syntax_errors++;
}
%}

%error-verbose

/* Bison Semantic Values Union */
%union {
    int int_val;
    double float_val;
    char *str_val;
}

/* Core Keywords */
%token TOK_INT TOK_FLOAT TOK_BOOL TOK_BOOLEAN
%token TOK_IF TOK_ELSE TOK_WHILE TOK_FOR TOK_DO
%token TOK_PRINT TOK_TRUE TOK_FALSE TOK_RETURN TOK_VOID

/* Multi-Language C/C++ Keywords */
%token TOK_PRINTF TOK_SCANF TOK_COUT TOK_CIN TOK_ENDL
%token TOK_INCLUDE TOK_USING TOK_NAMESPACE TOK_MAIN

/* Multi-Language Java Keywords */
%token TOK_CLASS TOK_PUBLIC TOK_STATIC TOK_SYSTEM_OUT_PRINTLN
%token TOK_IMPORT TOK_STRING

/* Identifiers & Literals */
%token <str_val>   TOK_IDENT
%token <int_val>   TOK_INT_LIT
%token <float_val> TOK_FLOAT_LIT
%token <str_val>   TOK_STRING_LIT

/* Operators */
%token TOK_PLUS TOK_MINUS TOK_MUL TOK_DIV TOK_MOD
%token TOK_INC TOK_DEC
%token TOK_LT TOK_GT TOK_LE TOK_GE TOK_EQ TOK_NE
%token TOK_AND TOK_OR TOK_NOT
%token TOK_ASSIGN TOK_LSHIFT TOK_RSHIFT

/* Delimiters */
%token TOK_LBRACE TOK_RBRACE TOK_LPAREN TOK_RPAREN
%token TOK_LBRACKET TOK_RBRACKET TOK_SEMI TOK_COMMA TOK_DOT TOK_COLON
%token TOK_ERROR

/* Precedence and Associativity Rules */
%nonassoc LOWER_THAN_ELSE
%nonassoc TOK_ELSE

%left TOK_OR
%left TOK_AND
%left TOK_EQ TOK_NE
%left TOK_LT TOK_LE TOK_GT TOK_GE
%left TOK_LSHIFT TOK_RSHIFT
%left TOK_PLUS TOK_MINUS
%left TOK_MUL TOK_DIV TOK_MOD
%right TOK_NOT UMINUS TOK_INC TOK_DEC

%start program

%%

/* Top-level Program Structure */
program:
      unit_list
    ;

unit_list:
      unit
    | unit_list unit
    ;

unit:
      header_item
    | class_definition
    | function_definition
    | statement
    ;

header_item:
      TOK_INCLUDE
    | TOK_IMPORT
    | TOK_USING TOK_NAMESPACE TOK_IDENT TOK_SEMI
    ;

/* Java Class Definition */
class_definition:
      TOK_PUBLIC TOK_CLASS TOK_IDENT TOK_LBRACE method_list TOK_RBRACE
    | TOK_CLASS TOK_IDENT TOK_LBRACE method_list TOK_RBRACE
    ;

method_list:
      method_definition
    | method_list method_definition
    ;

method_definition:
      TOK_PUBLIC TOK_STATIC TOK_VOID TOK_MAIN TOK_LPAREN TOK_STRING TOK_LBRACKET TOK_RBRACKET TOK_IDENT TOK_RPAREN block_statement
    | type TOK_IDENT TOK_LPAREN parameter_list_opt TOK_RPAREN block_statement
    ;

/* C / C++ Function Definition */
function_definition:
      type TOK_MAIN TOK_LPAREN parameter_list_opt TOK_RPAREN block_statement
    | type TOK_IDENT TOK_LPAREN parameter_list_opt TOK_RPAREN block_statement
    ;

parameter_list_opt:
      /* empty */
    | parameter_list
    ;

parameter_list:
      parameter
    | parameter_list TOK_COMMA parameter
    ;

parameter:
      type TOK_IDENT
    ;

/* Types */
type:
      TOK_INT
    | TOK_FLOAT
    | TOK_BOOL
    | TOK_BOOLEAN
    | TOK_STRING
    | TOK_VOID
    ;

/* Statement List */
statement_list:
      statement
    | statement_list statement
    ;

/* Statements */
statement:
      declaration_statement
    | assignment_statement
    | if_statement
    | while_statement
    | for_statement
    | print_statement
    | io_statement
    | block_statement
    | return_statement
    | error TOK_SEMI { yyerrok; }
    ;

/* Variable Declarations */
declaration_statement:
      type TOK_IDENT TOK_SEMI
    | type TOK_IDENT TOK_ASSIGN expression TOK_SEMI
    ;

/* Assignments */
assignment_statement:
      TOK_IDENT TOK_ASSIGN expression TOK_SEMI
    | TOK_IDENT TOK_INC TOK_SEMI
    | TOK_IDENT TOK_DEC TOK_SEMI
    ;

/* Control Flow: if / if-else */
if_statement:
      TOK_IF TOK_LPAREN expression TOK_RPAREN statement %prec LOWER_THAN_ELSE
    | TOK_IF TOK_LPAREN expression TOK_RPAREN statement TOK_ELSE statement
    ;

/* Control Flow: while loop */
while_statement:
      TOK_WHILE TOK_LPAREN expression TOK_RPAREN statement
    ;

/* Control Flow: for loop (Bonus feature) */
for_statement:
      TOK_FOR TOK_LPAREN for_init_opt TOK_SEMI expression_opt TOK_SEMI for_step_opt TOK_RPAREN statement
    ;

for_init_opt:
      /* empty */
    | type TOK_IDENT TOK_ASSIGN expression
    | TOK_IDENT TOK_ASSIGN expression
    ;

expression_opt:
      /* empty */
    | expression
    ;

for_step_opt:
      /* empty */
    | TOK_IDENT TOK_ASSIGN expression
    | TOK_IDENT TOK_INC
    | TOK_IDENT TOK_DEC
    ;

/* Standard Print Statement (Section 5) */
print_statement:
      TOK_PRINT expression TOK_SEMI
    ;

/* Multi-Language I/O: C printf, C++ cout/cin, Java System.out.println */
io_statement:
      TOK_PRINTF TOK_LPAREN print_args_list TOK_RPAREN TOK_SEMI
    | TOK_SYSTEM_OUT_PRINTLN TOK_LPAREN print_args_list TOK_RPAREN TOK_SEMI
    | cout_statement TOK_SEMI
    | cin_statement TOK_SEMI
    ;

cout_statement:
      TOK_COUT TOK_LSHIFT cout_arg
    | cout_statement TOK_LSHIFT cout_arg
    ;

cout_arg:
      expression
    | TOK_ENDL
    ;

cin_statement:
      TOK_CIN TOK_RSHIFT TOK_IDENT
    | cin_statement TOK_RSHIFT TOK_IDENT
    ;

print_args_list:
      expression
    | print_args_list TOK_COMMA expression
    ;

/* Scoped Nested Blocks { ... } */
block_statement:
      TOK_LBRACE statement_list TOK_RBRACE
    | TOK_LBRACE TOK_RBRACE
    ;

return_statement:
      TOK_RETURN expression_opt TOK_SEMI
    ;

/* Expressions */
expression:
      expression TOK_OR expression
    | expression TOK_AND expression
    | expression TOK_EQ expression
    | expression TOK_NE expression
    | expression TOK_LT expression
    | expression TOK_LE expression
    | expression TOK_GT expression
    | expression TOK_GE expression
    | expression TOK_PLUS expression
    | expression TOK_MINUS expression
    | expression TOK_MUL expression
    | expression TOK_DIV expression
    | expression TOK_MOD expression
    | TOK_NOT expression
    | TOK_MINUS expression %prec UMINUS
    | TOK_LPAREN expression TOK_RPAREN
    | primary
    ;

primary:
      TOK_IDENT
    | TOK_INT_LIT
    | TOK_FLOAT_LIT
    | TOK_STRING_LIT
    | TOK_TRUE
    | TOK_FALSE
    ;

%%
