%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "ast.h"

extern int yylex(void);
extern int line_num;
extern int col_num;
extern FILE *yyin;

int syntax_errors = 0;
ASTNode *ast_root = NULL;

void yyerror(const char *s) {
    fprintf(stderr, "[Syntax Error] %s at line %d, near column %d\n", s, line_num, col_num);
    syntax_errors++;
}
%}

%error-verbose

/* Semantic Values Union */
%union {
    int int_val;
    double float_val;
    char *str_val;
    DataType data_type;
    ASTNode *ast_node;
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

/* Operator Precedence and Associativity */
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

/* Non-terminal Types */
%type <ast_node> program unit_list unit header_item
%type <ast_node> class_definition method_list method_definition function_definition
%type <ast_node> statement_list statement declaration_statement assignment_statement
%type <ast_node> if_statement while_statement for_statement print_statement io_statement
%type <ast_node> cout_statement cout_arg cin_statement block_statement return_statement
%type <ast_node> for_init_opt for_step_opt expression_opt expression primary print_args_list
%type <data_type> type

%start program

%%

/* Top-level Program Structure */
program:
      unit_list { ast_root = $1; $$ = $1; }
    ;

unit_list:
      unit                { $$ = $1; }
    | unit_list unit      { $$ = ast_append_stmt($1, $2); }
    ;

unit:
      header_item         { $$ = $1; }
    | class_definition    { $$ = $1; }
    | function_definition { $$ = $1; }
    | statement           { $$ = $1; }
    ;

/* Headers and Imports */
header_item:
      TOK_INCLUDE                                   { $$ = NULL; }
    | TOK_IMPORT                                    { $$ = NULL; }
    | TOK_USING TOK_NAMESPACE TOK_IDENT TOK_SEMI    { free($3); $$ = NULL; }
    ;

/* Java Class Definition */
class_definition:
      TOK_PUBLIC TOK_CLASS TOK_IDENT TOK_LBRACE method_list TOK_RBRACE
        { free($3); $$ = $5; }
    | TOK_CLASS TOK_IDENT TOK_LBRACE method_list TOK_RBRACE
        { free($2); $$ = $4; }
    ;

method_list:
      method_definition              { $$ = $1; }
    | method_list method_definition  { $$ = ast_append_stmt($1, $2); }
    ;

method_definition:
      TOK_PUBLIC TOK_STATIC TOK_VOID TOK_MAIN TOK_LPAREN TOK_STRING TOK_LBRACKET TOK_RBRACKET TOK_IDENT TOK_RPAREN block_statement
        { free($9); $$ = $11; }
    | type TOK_IDENT TOK_LPAREN parameter_list_opt TOK_RPAREN block_statement
        { free($2); $$ = $6; }
    ;

/* C / C++ Function Definition */
function_definition:
      type TOK_MAIN TOK_LPAREN parameter_list_opt TOK_RPAREN block_statement
        { $$ = $6; }
    | type TOK_IDENT TOK_LPAREN parameter_list_opt TOK_RPAREN block_statement
        { free($2); $$ = $6; }
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
      type TOK_IDENT { free($2); }
    ;

/* Types */
type:
      TOK_INT       { $$ = TYPE_INT; }
    | TOK_FLOAT     { $$ = TYPE_FLOAT; }
    | TOK_BOOL      { $$ = TYPE_BOOL; }
    | TOK_BOOLEAN   { $$ = TYPE_BOOL; }
    | TOK_STRING    { $$ = TYPE_STRING; }
    | TOK_VOID      { $$ = TYPE_VOID; }
    ;

/* Statement List */
statement_list:
      statement                   { $$ = $1; }
    | statement_list statement    { $$ = ast_append_stmt($1, $2); }
    ;

/* Statements */
statement:
      declaration_statement { $$ = $1; }
    | assignment_statement  { $$ = $1; }
    | if_statement          { $$ = $1; }
    | while_statement       { $$ = $1; }
    | for_statement         { $$ = $1; }
    | print_statement       { $$ = $1; }
    | io_statement          { $$ = $1; }
    | block_statement       { $$ = $1; }
    | return_statement      { $$ = $1; }
    | error TOK_SEMI        { yyerrok; $$ = NULL; }
    ;

/* Variable Declarations */
declaration_statement:
      type TOK_IDENT TOK_SEMI
        { $$ = ast_new_var_decl($1, $2, NULL, line_num); free($2); }
    | type TOK_IDENT TOK_ASSIGN expression TOK_SEMI
        { $$ = ast_new_var_decl($1, $2, $4, line_num); free($2); }
    ;

/* Assignments */
assignment_statement:
      TOK_IDENT TOK_ASSIGN expression TOK_SEMI
        { $$ = ast_new_assign($1, $3, line_num); free($1); }
    | TOK_IDENT TOK_INC TOK_SEMI
        {
            ASTNode *one = ast_new_literal_int(1, line_num);
            ASTNode *add = ast_new_binary_op("+", ast_new_ident($1, line_num), one, line_num);
            $$ = ast_new_assign($1, add, line_num);
            free($1);
        }
    | TOK_IDENT TOK_DEC TOK_SEMI
        {
            ASTNode *one = ast_new_literal_int(1, line_num);
            ASTNode *sub = ast_new_binary_op("-", ast_new_ident($1, line_num), one, line_num);
            $$ = ast_new_assign($1, sub, line_num);
            free($1);
        }
    ;

/* Control Flow: if / if-else */
if_statement:
      TOK_IF TOK_LPAREN expression TOK_RPAREN statement %prec LOWER_THAN_ELSE
        { $$ = ast_new_if($3, $5, NULL, line_num); }
    | TOK_IF TOK_LPAREN expression TOK_RPAREN statement TOK_ELSE statement
        { $$ = ast_new_if($3, $5, $7, line_num); }
    ;

/* Control Flow: while loop */
while_statement:
      TOK_WHILE TOK_LPAREN expression TOK_RPAREN statement
        { $$ = ast_new_while($3, $5, line_num); }
    ;

/* Control Flow: for loop (Bonus) */
for_statement:
      TOK_FOR TOK_LPAREN for_init_opt TOK_SEMI expression_opt TOK_SEMI for_step_opt TOK_RPAREN statement
        { $$ = ast_new_for($3, $5, $7, $9, line_num); }
    ;

for_init_opt:
      /* empty */                                 { $$ = NULL; }
    | type TOK_IDENT TOK_ASSIGN expression        { $$ = ast_new_var_decl($1, $2, $4, line_num); free($2); }
    | TOK_IDENT TOK_ASSIGN expression             { $$ = ast_new_assign($1, $3, line_num); free($1); }
    ;

expression_opt:
      /* empty */   { $$ = NULL; }
    | expression    { $$ = $1; }
    ;

for_step_opt:
      /* empty */                         { $$ = NULL; }
    | TOK_IDENT TOK_ASSIGN expression     { $$ = ast_new_assign($1, $3, line_num); free($1); }
    | TOK_IDENT TOK_INC
        {
            ASTNode *one = ast_new_literal_int(1, line_num);
            ASTNode *add = ast_new_binary_op("+", ast_new_ident($1, line_num), one, line_num);
            $$ = ast_new_assign($1, add, line_num);
            free($1);
        }
    | TOK_IDENT TOK_DEC
        {
            ASTNode *one = ast_new_literal_int(1, line_num);
            ASTNode *sub = ast_new_binary_op("-", ast_new_ident($1, line_num), one, line_num);
            $$ = ast_new_assign($1, sub, line_num);
            free($1);
        }
    ;

/* Standard Print Statement (Section 5) */
print_statement:
      TOK_PRINT expression TOK_SEMI
        { $$ = ast_new_print($2, line_num); }
    ;

/* Multi-Language I/O */
io_statement:
      TOK_PRINTF TOK_LPAREN print_args_list TOK_RPAREN TOK_SEMI
        { $$ = ast_new_print($3, line_num); }
    | TOK_SYSTEM_OUT_PRINTLN TOK_LPAREN print_args_list TOK_RPAREN TOK_SEMI
        { $$ = ast_new_print($3, line_num); }
    | cout_statement TOK_SEMI
        { $$ = $1; }
    | cin_statement TOK_SEMI
        { $$ = $1; }
    ;

cout_statement:
      TOK_COUT TOK_LSHIFT cout_arg
        { $$ = ast_new_print($3, line_num); }
    | cout_statement TOK_LSHIFT cout_arg
        { $$ = ast_append_stmt($1, ast_new_print($3, line_num)); }
    ;

cout_arg:
      expression  { $$ = $1; }
    | TOK_ENDL    { $$ = ast_new_literal_string("\n", line_num); }
    ;

cin_statement:
      TOK_CIN TOK_RSHIFT TOK_IDENT
        { free($3); $$ = NULL; }
    | cin_statement TOK_RSHIFT TOK_IDENT
        { free($3); $$ = NULL; }
    ;

print_args_list:
      expression                        { $$ = $1; }
    | print_args_list TOK_COMMA expression { $$ = ast_append_stmt($1, $3); }
    ;

/* Scoped Nested Blocks { ... } */
block_statement:
      TOK_LBRACE statement_list TOK_RBRACE { $$ = ast_new_block($2, line_num); }
    | TOK_LBRACE TOK_RBRACE                { $$ = ast_new_block(NULL, line_num); }
    ;

return_statement:
      TOK_RETURN expression_opt TOK_SEMI { $$ = ast_new_return($2, line_num); }
    ;

/* Expressions */
expression:
      expression TOK_OR expression      { $$ = ast_new_binary_op("||", $1, $3, line_num); }
    | expression TOK_AND expression     { $$ = ast_new_binary_op("&&", $1, $3, line_num); }
    | expression TOK_EQ expression      { $$ = ast_new_binary_op("==", $1, $3, line_num); }
    | expression TOK_NE expression      { $$ = ast_new_binary_op("!=", $1, $3, line_num); }
    | expression TOK_LT expression      { $$ = ast_new_binary_op("<",  $1, $3, line_num); }
    | expression TOK_LE expression      { $$ = ast_new_binary_op("<=", $1, $3, line_num); }
    | expression TOK_GT expression      { $$ = ast_new_binary_op(">",  $1, $3, line_num); }
    | expression TOK_GE expression      { $$ = ast_new_binary_op(">=", $1, $3, line_num); }
    | expression TOK_PLUS expression    { $$ = ast_new_binary_op("+",  $1, $3, line_num); }
    | expression TOK_MINUS expression   { $$ = ast_new_binary_op("-",  $1, $3, line_num); }
    | expression TOK_MUL expression     { $$ = ast_new_binary_op("*",  $1, $3, line_num); }
    | expression TOK_DIV expression     { $$ = ast_new_binary_op("/",  $1, $3, line_num); }
    | expression TOK_MOD expression     { $$ = ast_new_binary_op("%",  $1, $3, line_num); }
    | TOK_NOT expression                { $$ = ast_new_unary_op("!", $2, line_num); }
    | TOK_MINUS expression %prec UMINUS { $$ = ast_new_unary_op("-", $2, line_num); }
    | TOK_LPAREN expression TOK_RPAREN  { $$ = $2; }
    | primary                           { $$ = $1; }
    ;

primary:
      TOK_IDENT       { $$ = ast_new_ident($1, line_num); free($1); }
    | TOK_INT_LIT     { $$ = ast_new_literal_int($1, line_num); }
    | TOK_FLOAT_LIT   { $$ = ast_new_literal_float($1, line_num); }
    | TOK_STRING_LIT  { $$ = ast_new_literal_string($1, line_num); free($1); }
    | TOK_TRUE        { $$ = ast_new_literal_bool(1, line_num); }
    | TOK_FALSE       { $$ = ast_new_literal_bool(0, line_num); }
    ;

%%
