#include <stdio.h>
#include <stdlib.h>
#include "../src/lexer/tokens.h"

extern int yylex(void);
extern FILE *yyin;
extern char *yytext;
extern int line_num;
extern int col_num;
extern int lexical_errors;

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("Failed to open source file");
        return 1;
    }

    yyin = f;
    printf("============================================================\n");
    printf("           LEXICAL ANALYSIS TOKEN STREAM OUTPUT             \n");
    printf("============================================================\n");
    printf("%-6s | %-6s | %-26s | %s\n", "Line", "Col", "Token Type", "Lexeme");
    printf("------------------------------------------------------------\n");

    int token;
    int token_count = 0;
    while ((token = yylex()) != 0) {
        printf("%-6d | %-6d | %-26s | %s\n", line_num, col_num, token_name(token), yytext);
        token_count++;
    }

    fclose(f);
    printf("------------------------------------------------------------\n");
    printf("Total Tokens: %d | Lexical Errors Encountered: %d\n", token_count, lexical_errors);
    printf("============================================================\n");
    return 0;
}
