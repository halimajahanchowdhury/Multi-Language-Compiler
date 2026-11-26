%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/symbol_table/symtab.h"
#include "../src/ast/ast.h"

extern int yylex(void);
extern int java_line_num;
extern int java_col_num;
extern int java_lexical_errors;
extern char *yytext;

void yyerror(const char *s);

ASTNode *java_ast_root = NULL;
int java_syntax_errors = 0;
%}

%union {
    int ival;
    double fval;
    char *sval;
    DataType type_val;
    ASTNode *node_val;
}

%error-verbose

/* Keywords */
%token TOK_INT TOK_FLOAT TOK_BOOLEAN TOK_STRING TOK_VOID
%token TOK_CLASS TOK_PUBLIC TOK_STATIC TOK_MAIN TOK_IMPORT
%token TOK_IF TOK_ELSE TOK_WHILE TOK_FOR TOK_RETURN TOK_TRUE TOK_FALSE
%token TOK_SYSTEM_OUT_PRINTLN TOK_SYSTEM_OUT_PRINT

/* Operators */
%token TOK_ASSIGN TOK_INC TOK_DEC
%token TOK_PLUS TOK_MINUS TOK_MUL TOK_DIV TOK_MOD
%token TOK_EQ TOK_NE TOK_LT TOK_GT TOK_LE TOK_GE
%token TOK_AND TOK_OR TOK_NOT

/* Delimiters */
%token TOK_SEMI TOK_COMMA
%token TOK_LPAREN TOK_RPAREN TOK_LBRACE TOK_RBRACE TOK_LBRACKET TOK_RBRACKET

/* Literals and Identifiers */
%token <ival> TOK_INT_LIT
%token <fval> TOK_FLOAT_LIT
%token <sval> TOK_STRING_LIT TOK_IDENT

/* Non-terminal Types */
%type <type_val> type
%type <node_val> compilation_unit import_list_opt class_declaration
%type <node_val> method_list method_declaration
%type <node_val> block_statement statement_list statement
%type <node_val> declaration_statement var_init_list var_init_item
%type <node_val> assignment_statement if_statement while_statement for_statement
%type <node_val> print_statement return_statement
%type <node_val> expression print_args_list
%type <node_val> for_init_opt expression_opt for_step_opt

/* Operator Precedence and Associativity */
%nonassoc LOWER_THAN_ELSE
%nonassoc TOK_ELSE

%right TOK_ASSIGN
%left TOK_OR
%left TOK_AND
%left TOK_EQ TOK_NE
%left TOK_LT TOK_GT TOK_LE TOK_GE
%left TOK_PLUS TOK_MINUS
%left TOK_MUL TOK_DIV TOK_MOD
%right TOK_NOT UMINUS
%left TOK_INC TOK_DEC

%%

program:
      compilation_unit
        { java_ast_root = $1; }
    | /* empty */
        { java_ast_root = NULL; }
    ;

compilation_unit:
      import_list_opt class_declaration
        { $$ = $2; }
    ;

import_list_opt:
      /* empty */
        { $$ = NULL; }
    | import_list_opt TOK_IMPORT
        { $$ = NULL; }
    ;

class_declaration:
      TOK_PUBLIC TOK_CLASS TOK_IDENT TOK_LBRACE method_list TOK_RBRACE
        { free($3); $$ = $5; }
    | TOK_CLASS TOK_IDENT TOK_LBRACE method_list TOK_RBRACE
        { free($2); $$ = $4; }
    ;

method_list:
      method_declaration
        { $$ = $1; }
    | method_list method_declaration
        { $$ = ast_append_stmt($1, $2); }
    ;

method_declaration:
      TOK_PUBLIC TOK_STATIC TOK_VOID TOK_MAIN TOK_LPAREN TOK_STRING TOK_LBRACKET TOK_RBRACKET TOK_IDENT TOK_RPAREN block_statement
        { free($9); $$ = $11; }
    | TOK_PUBLIC TOK_STATIC type TOK_IDENT TOK_LPAREN parameter_list_opt TOK_RPAREN block_statement
        { free($4); $$ = $8; }
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

type:
      TOK_INT       { $$ = TYPE_INT; }
    | TOK_FLOAT     { $$ = TYPE_FLOAT; }
    | TOK_BOOLEAN   { $$ = TYPE_BOOL; }
    | TOK_STRING    { $$ = TYPE_STRING; }
    | TOK_VOID      { $$ = TYPE_VOID; }
    ;

block_statement:
      TOK_LBRACE statement_list TOK_RBRACE { $$ = ast_new_block($2, java_line_num); }
    | TOK_LBRACE TOK_RBRACE                { $$ = ast_new_block(NULL, java_line_num); }
    ;

statement_list:
      statement                   { $$ = $1; }
    | statement_list statement    { $$ = ast_append_stmt($1, $2); }
    ;

statement:
      declaration_statement { $$ = $1; }
    | assignment_statement  { $$ = $1; }
    | if_statement          { $$ = $1; }
    | while_statement       { $$ = $1; }
    | for_statement         { $$ = $1; }
    | print_statement       { $$ = $1; }
    | block_statement       { $$ = $1; }
    | return_statement      { $$ = $1; }
    | error TOK_SEMI        { yyerrok; $$ = NULL; }
    ;

/* Variable Declarations with comma lists */
declaration_statement:
      type var_init_list TOK_SEMI
        {
            ASTNode *curr = $2;
            while (curr) {
                curr->eval_type = $1;
                curr = curr->next;
            }
            $$ = $2;
        }
    ;

var_init_list:
      var_init_item
        { $$ = $1; }
    | var_init_list TOK_COMMA var_init_item
        { $$ = ast_append_stmt($1, $3); }
    ;

var_init_item:
      TOK_IDENT
        { $$ = ast_new_var_decl(TYPE_INT, $1, NULL, java_line_num); free($1); }
    | TOK_IDENT TOK_ASSIGN expression
        { $$ = ast_new_var_decl(TYPE_INT, $1, $3, java_line_num); free($1); }
    ;

assignment_statement:
      TOK_IDENT TOK_ASSIGN expression TOK_SEMI
        { $$ = ast_new_assign($1, $3, java_line_num); free($1); }
    | TOK_IDENT TOK_INC TOK_SEMI
        {
            ASTNode *one = ast_new_literal_int(1, java_line_num);
            ASTNode *add = ast_new_binary_op("+", ast_new_ident($1, java_line_num), one, java_line_num);
            $$ = ast_new_assign($1, add, java_line_num);
            free($1);
        }
    | TOK_IDENT TOK_DEC TOK_SEMI
        {
            ASTNode *one = ast_new_literal_int(1, java_line_num);
            ASTNode *sub = ast_new_binary_op("-", ast_new_ident($1, java_line_num), one, java_line_num);
            $$ = ast_new_assign($1, sub, java_line_num);
            free($1);
        }
    ;

if_statement:
      TOK_IF TOK_LPAREN expression TOK_RPAREN statement %prec LOWER_THAN_ELSE
        { $$ = ast_new_if($3, $5, NULL, java_line_num); }
    | TOK_IF TOK_LPAREN expression TOK_RPAREN statement TOK_ELSE statement
        { $$ = ast_new_if($3, $5, $7, java_line_num); }
    ;

while_statement:
      TOK_WHILE TOK_LPAREN expression TOK_RPAREN statement
        { $$ = ast_new_while($3, $5, java_line_num); }
    ;

for_statement:
      TOK_FOR TOK_LPAREN for_init_opt TOK_SEMI expression_opt TOK_SEMI for_step_opt TOK_RPAREN statement
        { $$ = ast_new_for($3, $5, $7, $9, java_line_num); }
    ;

for_init_opt:
      /* empty */                                 { $$ = NULL; }
    | type TOK_IDENT TOK_ASSIGN expression        { $$ = ast_new_var_decl($1, $2, $4, java_line_num); free($2); }
    | TOK_IDENT TOK_ASSIGN expression             { $$ = ast_new_assign($1, $3, java_line_num); free($1); }
    ;

expression_opt:
      /* empty */   { $$ = NULL; }
    | expression    { $$ = $1; }
    ;

for_step_opt:
      /* empty */                         { $$ = NULL; }
    | TOK_IDENT TOK_ASSIGN expression     { $$ = ast_new_assign($1, $3, java_line_num); free($1); }
    | TOK_IDENT TOK_INC
        {
            ASTNode *one = ast_new_literal_int(1, java_line_num);
            ASTNode *add = ast_new_binary_op("+", ast_new_ident($1, java_line_num), one, java_line_num);
            $$ = ast_new_assign($1, add, java_line_num);
            free($1);
        }
    | TOK_IDENT TOK_DEC
        {
            ASTNode *one = ast_new_literal_int(1, java_line_num);
            ASTNode *sub = ast_new_binary_op("-", ast_new_ident($1, java_line_num), one, java_line_num);
            $$ = ast_new_assign($1, sub, java_line_num);
            free($1);
        }
    ;

print_statement:
      TOK_SYSTEM_OUT_PRINTLN TOK_LPAREN print_args_list TOK_RPAREN TOK_SEMI
        { $$ = ast_new_print($3, java_line_num); }
    | TOK_SYSTEM_OUT_PRINT TOK_LPAREN print_args_list TOK_RPAREN TOK_SEMI
        { $$ = ast_new_print($3, java_line_num); }
    ;

print_args_list:
      expression                        { $$ = $1; }
    | print_args_list TOK_PLUS expression { $$ = ast_append_stmt($1, $3); }
    ;

return_statement:
      TOK_RETURN expression_opt TOK_SEMI { $$ = ast_new_return($2, java_line_num); }
    ;

expression:
      expression TOK_OR expression      { $$ = ast_new_binary_op("||", $1, $3, java_line_num); }
    | expression TOK_AND expression     { $$ = ast_new_binary_op("&&", $1, $3, java_line_num); }
    | expression TOK_EQ expression      { $$ = ast_new_binary_op("==", $1, $3, java_line_num); }
    | expression TOK_NE expression      { $$ = ast_new_binary_op("!=", $1, $3, java_line_num); }
    | expression TOK_LT expression      { $$ = ast_new_binary_op("<", $1, $3, java_line_num); }
    | expression TOK_GT expression      { $$ = ast_new_binary_op(">", $1, $3, java_line_num); }
    | expression TOK_LE expression      { $$ = ast_new_binary_op("<=", $1, $3, java_line_num); }
    | expression TOK_GE expression      { $$ = ast_new_binary_op(">=", $1, $3, java_line_num); }
    | expression TOK_PLUS expression    { $$ = ast_new_binary_op("+", $1, $3, java_line_num); }
    | expression TOK_MINUS expression   { $$ = ast_new_binary_op("-", $1, $3, java_line_num); }
    | expression TOK_MUL expression     { $$ = ast_new_binary_op("*", $1, $3, java_line_num); }
    | expression TOK_DIV expression     { $$ = ast_new_binary_op("/", $1, $3, java_line_num); }
    | expression TOK_MOD expression     { $$ = ast_new_binary_op("%", $1, $3, java_line_num); }
    | TOK_NOT expression                { $$ = ast_new_unary_op("!", $2, java_line_num); }
    | TOK_MINUS expression %prec UMINUS { $$ = ast_new_unary_op("-", $2, java_line_num); }
    | TOK_LPAREN expression TOK_RPAREN  { $$ = $2; }
    | TOK_IDENT                         { $$ = ast_new_ident($1, java_line_num); free($1); }
    | TOK_INT_LIT                       { $$ = ast_new_literal_int($1, java_line_num); }
    | TOK_FLOAT_LIT                     { $$ = ast_new_literal_float($1, java_line_num); }
    | TOK_STRING_LIT                    { $$ = ast_new_literal_string($1, java_line_num); free($1); }
    | TOK_TRUE                          { $$ = ast_new_literal_bool(1, java_line_num); }
    | TOK_FALSE                         { $$ = ast_new_literal_bool(0, java_line_num); }
    ;

%%

void yyerror(const char *s) {
    java_syntax_errors++;
    fprintf(stderr, "[Syntax Error] %s at line %d, near column %d\n", s, java_line_num, java_col_num);
}
