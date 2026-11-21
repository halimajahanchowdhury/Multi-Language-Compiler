#ifndef TAC_H
#define TAC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* Three Address Code Operation Codes */
typedef enum {
    TAC_NOP = 0,
    TAC_ASSIGN,     /* result = arg1 */
    TAC_ADD,        /* result = arg1 + arg2 */
    TAC_SUB,        /* result = arg1 - arg2 */
    TAC_MUL,        /* result = arg1 * arg2 */
    TAC_DIV,        /* result = arg1 / arg2 */
    TAC_MOD,        /* result = arg1 % arg2 */
    TAC_NEG,        /* result = -arg1 */
    TAC_NOT,        /* result = !arg1 */
    TAC_EQ,         /* result = arg1 == arg2 */
    TAC_NE,         /* result = arg1 != arg2 */
    TAC_LT,         /* result = arg1 < arg2 */
    TAC_LE,         /* result = arg1 <= arg2 */
    TAC_GT,         /* result = arg1 > arg2 */
    TAC_GE,         /* result = arg1 >= arg2 */
    TAC_AND,        /* result = arg1 && arg2 */
    TAC_OR,         /* result = arg1 || arg2 */
    TAC_LABEL,      /* result: */
    TAC_GOTO,       /* goto result */
    TAC_IF_FALSE,   /* if_false arg1 goto result */
    TAC_IF_TRUE,    /* if_true arg1 goto result */
    TAC_PRINT       /* print arg1 */
} TACOp;

/* A Single Linear TAC Instruction (Quadruple) */
typedef struct TACInstr {
    TACOp op;
    char *arg1;
    char *arg2;
    char *result;
    struct TACInstr *next;
} TACInstr;

/* Linear List of TAC Instructions */
typedef struct TACList {
    TACInstr *head;
    TACInstr *tail;
    int count;
} TACList;

/* Lifecycle and Memory */
TACList *tac_list_create(void);
void tac_list_free(TACList *list);

/* Temporary and Label Generators */
char *tac_new_temp(void);
char *tac_new_label(void);
void tac_reset_counters(void);

/* Instruction Emission */
TACInstr *tac_emit(TACList *list, TACOp op, const char *arg1, const char *arg2, const char *result);
void tac_append_list(TACList *dest, TACList *src);

/* Printing / Formatting */
void tac_print(const TACList *list);
void tac_fprint(FILE *out, const TACList *list);

#endif /* TAC_H */
