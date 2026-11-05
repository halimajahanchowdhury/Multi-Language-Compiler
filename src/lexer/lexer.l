%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(HAVE_BISON) || defined(YYBISON)
#include "parser.tab.h"
#else
#include "tokens.h"
#endif

int line_num = 1;
int col_num = 1;
int lexical_errors = 0;

void count_columns(const char *text) {
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\n') {
            line_num++;
            col_num = 1;
        } else if (text[i] == '\t') {
            col_num += 4 - (col_num % 4);
        } else {
            col_num++;
        }
    }
}
%}

%option noyywrap
%option yylineno

/* Regular Definitions */
DIGIT       [0-9]
LETTER      [a-zA-Z_]
IDENT       {LETTER}({LETTER}|{DIGIT})*
INT_LIT     {DIGIT}+
FLOAT_LIT   {DIGIT}+\.{DIGIT}+
STRING_LIT  \"([^"\\\n]|\\.)*\"
WHITESPACE  [ \t\r]+
NEWLINE     \n

%%

"//".*                      { /* Discard single-line comment */ count_columns(yytext); }
"/*"([^*]|\*+[^*/])*\*+"/"  { /* Discard multi-line block comment */ count_columns(yytext); }

{WHITESPACE}                { count_columns(yytext); }
{NEWLINE}                   { line_num++; col_num = 1; }

"int"                       { count_columns(yytext); return TOK_INT; }
"float"                     { count_columns(yytext); return TOK_FLOAT; }
"bool"                      { count_columns(yytext); return TOK_BOOL; }
"boolean"                   { count_columns(yytext); return TOK_BOOLEAN; }
"if"                        { count_columns(yytext); return TOK_IF; }
"else"                      { count_columns(yytext); return TOK_ELSE; }
"while"                     { count_columns(yytext); return TOK_WHILE; }
"for"                       { count_columns(yytext); return TOK_FOR; }
"do"                        { count_columns(yytext); return TOK_DO; }
"print"                     { count_columns(yytext); return TOK_PRINT; }
"true"                      { count_columns(yytext); return TOK_TRUE; }
"false"                     { count_columns(yytext); return TOK_FALSE; }
"return"                    { count_columns(yytext); return TOK_RETURN; }
"void"                      { count_columns(yytext); return TOK_VOID; }

"printf"                    { count_columns(yytext); return TOK_PRINTF; }
"scanf"                     { count_columns(yytext); return TOK_SCANF; }
"cout"                      { count_columns(yytext); return TOK_COUT; }
"cin"                       { count_columns(yytext); return TOK_CIN; }
"endl"                      { count_columns(yytext); return TOK_ENDL; }
"#include"[ \t]*[<"][^>"\n]+[>"] { count_columns(yytext); return TOK_INCLUDE; }
"using"                     { count_columns(yytext); return TOK_USING; }
"namespace"                 { count_columns(yytext); return TOK_NAMESPACE; }
"main"                      { count_columns(yytext); return TOK_MAIN; }

"class"                     { count_columns(yytext); return TOK_CLASS; }
"public"                    { count_columns(yytext); return TOK_PUBLIC; }
"static"                    { count_columns(yytext); return TOK_STATIC; }
"System.out.println"        { count_columns(yytext); return TOK_SYSTEM_OUT_PRINTLN; }
"System.out.print"          { count_columns(yytext); return TOK_PRINT; }
"import"[ \t]+[^\n;]+";"    { count_columns(yytext); return TOK_IMPORT; }
"String"                    { count_columns(yytext); return TOK_STRING; }

"++"                        { count_columns(yytext); return TOK_INC; }
"--"                        { count_columns(yytext); return TOK_DEC; }
"+"                         { count_columns(yytext); return TOK_PLUS; }
"-"                         { count_columns(yytext); return TOK_MINUS; }
"*"                         { count_columns(yytext); return TOK_MUL; }
"/"                         { count_columns(yytext); return TOK_DIV; }
"%"                         { count_columns(yytext); return TOK_MOD; }

"=="                        { count_columns(yytext); return TOK_EQ; }
"!="                        { count_columns(yytext); return TOK_NE; }
"<="                        { count_columns(yytext); return TOK_LE; }
">="                        { count_columns(yytext); return TOK_GE; }
"<"                         { count_columns(yytext); return TOK_LT; }
">"                         { count_columns(yytext); return TOK_GT; }

"&&"                        { count_columns(yytext); return TOK_AND; }
"||"                        { count_columns(yytext); return TOK_OR; }
"!"                         { count_columns(yytext); return TOK_NOT; }

"<<"                        { count_columns(yytext); return TOK_LSHIFT; }
">>"                        { count_columns(yytext); return TOK_RSHIFT; }
"="                         { count_columns(yytext); return TOK_ASSIGN; }

"{"                         { count_columns(yytext); return TOK_LBRACE; }
"}"                         { count_columns(yytext); return TOK_RBRACE; }
"("                         { count_columns(yytext); return TOK_LPAREN; }
")"                         { count_columns(yytext); return TOK_RPAREN; }
"["                         { count_columns(yytext); return TOK_LBRACKET; }
"]"                         { count_columns(yytext); return TOK_RBRACKET; }
";"                         { count_columns(yytext); return TOK_SEMI; }
","                         { count_columns(yytext); return TOK_COMMA; }
"."                         { count_columns(yytext); return TOK_DOT; }
":"                         { count_columns(yytext); return TOK_COLON; }

{INT_LIT}                   { count_columns(yytext); return TOK_INT_LIT; }
{FLOAT_LIT}                 { count_columns(yytext); return TOK_FLOAT_LIT; }
{STRING_LIT}                { count_columns(yytext); return TOK_STRING_LIT; }
{IDENT}                     { count_columns(yytext); return TOK_IDENT; }

.                           {
                                fprintf(stderr, "[Lexical Error] Unrecognized character '%s' at line %d, column %d\n", yytext, line_num, col_num);
                                lexical_errors++;
                                count_columns(yytext);
                                return TOK_ERROR;
                            }

%%


