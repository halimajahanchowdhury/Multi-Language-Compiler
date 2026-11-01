#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Token Definitions for Multi-Language Compiler (C, C++, Java)
 * Supports standard keywords, C/C++ constructs, and Java class/system constructs.
 */

typedef enum {
    TOK_EOF = 0,
    TOK_ERROR = 257,

    /* Core Language Keywords */
    TOK_INT = 258,
    TOK_FLOAT,
    TOK_BOOL,
    TOK_IF,
    TOK_ELSE,
    TOK_WHILE,
    TOK_FOR,
    TOK_DO,
    TOK_PRINT,
    TOK_TRUE,
    TOK_FALSE,
    TOK_RETURN,
    TOK_VOID,

    /* C and C++ Specific Tokens */
    TOK_PRINTF,
    TOK_SCANF,
    TOK_COUT,
    TOK_CIN,
    TOK_ENDL,
    TOK_INCLUDE,
    TOK_USING,
    TOK_NAMESPACE,
    TOK_MAIN,

    /* Java Specific Tokens */
    TOK_CLASS,
    TOK_PUBLIC,
    TOK_STATIC,
    TOK_SYSTEM_OUT_PRINTLN,
    TOK_IMPORT,
    TOK_STRING,
    TOK_BOOLEAN,

    /* Identifiers and Literals */
    TOK_IDENT,
    TOK_INT_LIT,
    TOK_FLOAT_LIT,
    TOK_STRING_LIT,

    /* Arithmetic Operators */
    TOK_PLUS,       /* + */
    TOK_MINUS,      /* - */
    TOK_MUL,        /* * */
    TOK_DIV,        /* / */
    TOK_MOD,        /* % */
    TOK_INC,        /* ++ */
    TOK_DEC,        /* -- */

    /* Relational Operators */
    TOK_LT,         /* < */
    TOK_GT,         /* > */
    TOK_LE,         /* <= */
    TOK_GE,         /* >= */
    TOK_EQ,         /* == */
    TOK_NE,         /* != */

    /* Logical Operators */
    TOK_AND,        /* && */
    TOK_OR,         /* || */
    TOK_NOT,        /* ! */

    /* Assignment and Stream Operators */
    TOK_ASSIGN,     /* = */
    TOK_LSHIFT,     /* << (C++ cout stream) */
    TOK_RSHIFT,     /* >> (C++ cin stream) */

    /* Delimiters and Punctuation */
    TOK_LBRACE,     /* { */
    TOK_RBRACE,     /* } */
    TOK_LPAREN,     /* ( */
    TOK_RPAREN,     /* ) */
    TOK_LBRACKET,   /* [ */
    TOK_RBRACKET,   /* ] */
    TOK_SEMI,       /* ; */
    TOK_COMMA,      /* , */
    TOK_DOT,        /* . */
    TOK_COLON       /* : */
} TokenType;

/* Token representation structure */
typedef struct {
    int type;
    char *lexeme;
    int int_val;
    double float_val;
    int line;
    int column;
} Token;

/* Helper to convert token type enum to readable string representation */
static inline const char *token_name(int type) {
    switch (type) {
        case TOK_EOF:                 return "EOF";
        case TOK_ERROR:               return "ERROR";
        case TOK_INT:                 return "KEYWORD_INT";
        case TOK_FLOAT:               return "KEYWORD_FLOAT";
        case TOK_BOOL:                return "KEYWORD_BOOL";
        case TOK_IF:                  return "KEYWORD_IF";
        case TOK_ELSE:                return "KEYWORD_ELSE";
        case TOK_WHILE:               return "KEYWORD_WHILE";
        case TOK_FOR:                 return "KEYWORD_FOR";
        case TOK_DO:                  return "KEYWORD_DO";
        case TOK_PRINT:               return "KEYWORD_PRINT";
        case TOK_TRUE:                return "KEYWORD_TRUE";
        case TOK_FALSE:               return "KEYWORD_FALSE";
        case TOK_RETURN:              return "KEYWORD_RETURN";
        case TOK_VOID:                return "KEYWORD_VOID";
        case TOK_PRINTF:              return "KEYWORD_PRINTF";
        case TOK_SCANF:               return "KEYWORD_SCANF";
        case TOK_COUT:                return "KEYWORD_COUT";
        case TOK_CIN:                 return "KEYWORD_CIN";
        case TOK_ENDL:                return "KEYWORD_ENDL";
        case TOK_INCLUDE:             return "KEYWORD_INCLUDE";
        case TOK_USING:               return "KEYWORD_USING";
        case TOK_NAMESPACE:           return "KEYWORD_NAMESPACE";
        case TOK_MAIN:                return "KEYWORD_MAIN";
        case TOK_CLASS:               return "KEYWORD_CLASS";
        case TOK_PUBLIC:              return "KEYWORD_PUBLIC";
        case TOK_STATIC:              return "KEYWORD_STATIC";
        case TOK_SYSTEM_OUT_PRINTLN:  return "KEYWORD_SYSTEM_OUT_PRINTLN";
        case TOK_IMPORT:              return "KEYWORD_IMPORT";
        case TOK_STRING:              return "KEYWORD_STRING";
        case TOK_BOOLEAN:             return "KEYWORD_BOOLEAN";
        case TOK_IDENT:               return "IDENTIFIER";
        case TOK_INT_LIT:             return "LITERAL_INT";
        case TOK_FLOAT_LIT:           return "LITERAL_FLOAT";
        case TOK_STRING_LIT:          return "LITERAL_STRING";
        case TOK_PLUS:                return "OP_PLUS";
        case TOK_MINUS:               return "OP_MINUS";
        case TOK_MUL:                 return "OP_MUL";
        case TOK_DIV:                 return "OP_DIV";
        case TOK_MOD:                 return "OP_MOD";
        case TOK_INC:                 return "OP_INC";
        case TOK_DEC:                 return "OP_DEC";
        case TOK_LT:                  return "OP_LT";
        case TOK_GT:                  return "OP_GT";
        case TOK_LE:                  return "OP_LE";
        case TOK_GE:                  return "OP_GE";
        case TOK_EQ:                  return "OP_EQ";
        case TOK_NE:                  return "OP_NE";
        case TOK_AND:                 return "OP_AND";
        case TOK_OR:                  return "OP_OR";
        case TOK_NOT:                 return "OP_NOT";
        case TOK_ASSIGN:              return "OP_ASSIGN";
        case TOK_LSHIFT:              return "OP_LSHIFT";
        case TOK_RSHIFT:              return "OP_RSHIFT";
        case TOK_LBRACE:              return "DELIM_LBRACE";
        case TOK_RBRACE:              return "DELIM_RBRACE";
        case TOK_LPAREN:              return "DELIM_LPAREN";
        case TOK_RPAREN:              return "DELIM_RPAREN";
        case TOK_LBRACKET:            return "DELIM_LBRACKET";
        case TOK_RBRACKET:            return "DELIM_RBRACKET";
        case TOK_SEMI:                return "DELIM_SEMI";
        case TOK_COMMA:               return "DELIM_COMMA";
        case TOK_DOT:                 return "DELIM_DOT";
        case TOK_COLON:               return "DELIM_COLON";
        default:                      return "UNKNOWN_TOKEN";
    }
}

#endif /* TOKENS_H */
