%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/symbol_table/symtab.h"
#include "../src/ast/ast.h"
#include "java_parser.tab.h"

int java_line_num = 1;
int java_col_num = 1;
int java_lexical_errors = 0;

static void count_cols(const char *text) {
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\n') {
            java_line_num++;
            java_col_num = 1;
        } else if (text[i] == '\t') {
            java_col_num += 4 - (java_col_num % 4);
        } else {
            java_col_num++;
        }
    }
}
%}

%option noyywrap
%option yylineno

DIGIT       [0-9]
LETTER      [a-zA-Z_]
IDENT       {LETTER}({LETTER}|{DIGIT})*
INT_LIT     {DIGIT}+
FLOAT_LIT   {DIGIT}+\.{DIGIT}+
STRING_LIT  \"([^"\\\n]|\\.)*\"
WHITESPACE  [ \t\r]+
NEWLINE     \n

%%

"//".*                      { count_cols(yytext); }
"/*"([^*]|\*+[^*/])*\*+"/"  { count_cols(yytext); }

{WHITESPACE}                { count_cols(yytext); }
{NEWLINE}                   { java_line_num++; java_col_num = 1; }

"int"                       { count_cols(yytext); return TOK_INT; }
"float"                     { count_cols(yytext); return TOK_FLOAT; }
"double"                    { count_cols(yytext); return TOK_FLOAT; }
"boolean"                   { count_cols(yytext); return TOK_BOOLEAN; }
"String"                    { count_cols(yytext); return TOK_STRING; }
"void"                      { count_cols(yytext); return TOK_VOID; }

"class"                     { count_cols(yytext); return TOK_CLASS; }
"public"                    { count_cols(yytext); return TOK_PUBLIC; }
"static"                    { count_cols(yytext); return TOK_STATIC; }
"main"                      { count_cols(yytext); return TOK_MAIN; }
"import"[ \t]+[^\n;]+";"    { count_cols(yytext); return TOK_IMPORT; }

"if"                        { count_cols(yytext); return TOK_IF; }
"else"                      { count_cols(yytext); return TOK_ELSE; }
"while"                     { count_cols(yytext); return TOK_WHILE; }
"for"                       { count_cols(yytext); return TOK_FOR; }
"return"                    { count_cols(yytext); return TOK_RETURN; }
"true"                      { count_cols(yytext); return TOK_TRUE; }
"false"                     { count_cols(yytext); return TOK_FALSE; }

"System.out.println"        { count_cols(yytext); return TOK_SYSTEM_OUT_PRINTLN; }
"System.out.print"          { count_cols(yytext); return TOK_SYSTEM_OUT_PRINT; }

"++"                        { count_cols(yytext); return TOK_INC; }
"--"                        { count_cols(yytext); return TOK_DEC; }
"+"                         { count_cols(yytext); return TOK_PLUS; }
"-"                         { count_cols(yytext); return TOK_MINUS; }
"*"                         { count_cols(yytext); return TOK_MUL; }
"/"                         { count_cols(yytext); return TOK_DIV; }
"%"                         { count_cols(yytext); return TOK_MOD; }

"=="                        { count_cols(yytext); return TOK_EQ; }
"!="                        { count_cols(yytext); return TOK_NE; }
"<="                        { count_cols(yytext); return TOK_LE; }
">="                        { count_cols(yytext); return TOK_GE; }
"<"                         { count_cols(yytext); return TOK_LT; }
">"                         { count_cols(yytext); return TOK_GT; }

"&&"                        { count_cols(yytext); return TOK_AND; }
"||"                        { count_cols(yytext); return TOK_OR; }
"!"                         { count_cols(yytext); return TOK_NOT; }

"="                         { count_cols(yytext); return TOK_ASSIGN; }

";"                         { count_cols(yytext); return TOK_SEMI; }
","                         { count_cols(yytext); return TOK_COMMA; }
"("                         { count_cols(yytext); return TOK_LPAREN; }
")"                         { count_cols(yytext); return TOK_RPAREN; }
"{"                         { count_cols(yytext); return TOK_LBRACE; }
"}"                         { count_cols(yytext); return TOK_RBRACE; }
"["                         { count_cols(yytext); return TOK_LBRACKET; }
"]"                         { count_cols(yytext); return TOK_RBRACKET; }

{INT_LIT}                   { count_cols(yytext); yylval.ival = atoi(yytext); return TOK_INT_LIT; }
{FLOAT_LIT}                 { count_cols(yytext); yylval.fval = atof(yytext); return TOK_FLOAT_LIT; }
{STRING_LIT}                { count_cols(yytext); yylval.sval = strdup(yytext); return TOK_STRING_LIT; }
{IDENT}                     { count_cols(yytext); yylval.sval = strdup(yytext); return TOK_IDENT; }

.                           {
                                fprintf(stderr, "[Lexical Error] Unrecognized character '%s' at line %d, column %d\n",
                                        yytext, java_line_num, java_col_num);
                                java_lexical_errors++;
                                count_cols(yytext);
                            }

%%
