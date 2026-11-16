#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* AST Node Classification */
typedef enum {
    /* Programs and Lists */
    NODE_PROGRAM,
    NODE_STMT_LIST,
    NODE_BLOCK,

    /* Declarations */
    NODE_VAR_DECL,

    /* Statements */
    NODE_ASSIGN,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_PRINT,
    NODE_RETURN,

    /* Expressions */
    NODE_BINARY_OP,
    NODE_UNARY_OP,

    /* Terminals / Primary */
    NODE_IDENT,
    NODE_LITERAL_INT,
    NODE_LITERAL_FLOAT,
    NODE_LITERAL_BOOL,
    NODE_LITERAL_STRING
} NodeType;

/* Forward declaration */
typedef struct ASTNode ASTNode;

/* AST Node Structure */
struct ASTNode {
    NodeType node_type;
    DataType eval_type;     /* Evaluated/inferred type during semantic analysis */
    int line;
    int col;

    /* Specific node attributes */
    union {
        /* Literals */
        int int_val;
        double float_val;
        int bool_val;
        char *str_val;

        /* Identifiers */
        char *ident_name;

        /* Operators: "+", "-", "*", "/", "%", "==", "!=", "<", "<=", ">", ">=", "&&", "||", "!", "++", "--" */
        char *op;
    } attr;

    /* Child pointers for AST traversal */
    ASTNode *left;          /* Binary op left, assignment LHS, var decl, etc. */
    ASTNode *right;         /* Binary op right, assignment RHS, etc. */
    ASTNode *cond;          /* If condition, while condition, for condition */
    ASTNode *then_branch;   /* If then branch */
    ASTNode *else_branch;   /* If else branch (optional) */
    ASTNode *body;          /* Loop body, function body, block statements */
    ASTNode *init;          /* For loop initialization */
    ASTNode *step;          /* For loop increment/decrement step */

    /* Statement list linkage */
    ASTNode *next;          /* Next statement in sequential execution */
};

/* Constructors */
ASTNode *ast_new_literal_int(int val, int line);
ASTNode *ast_new_literal_float(double val, int line);
ASTNode *ast_new_literal_bool(int val, int line);
ASTNode *ast_new_literal_string(const char *str, int line);
ASTNode *ast_new_ident(const char *name, int line);

ASTNode *ast_new_var_decl(DataType type, const char *name, ASTNode *init_expr, int line);
ASTNode *ast_new_assign(const char *name, ASTNode *expr, int line);
ASTNode *ast_new_binary_op(const char *op, ASTNode *left, ASTNode *right, int line);
ASTNode *ast_new_unary_op(const char *op, ASTNode *operand, int line);

ASTNode *ast_new_if(ASTNode *cond, ASTNode *then_b, ASTNode *else_b, int line);
ASTNode *ast_new_while(ASTNode *cond, ASTNode *body, int line);
ASTNode *ast_new_for(ASTNode *init, ASTNode *cond, ASTNode *step, ASTNode *body, int line);
ASTNode *ast_new_print(ASTNode *expr, int line);
ASTNode *ast_new_block(ASTNode *stmt_list, int line);
ASTNode *ast_new_return(ASTNode *expr, int line);

ASTNode *ast_append_stmt(ASTNode *list, ASTNode *stmt);

/* Memory Deallocation */
void ast_free(ASTNode *node);

/* Visualization */
void ast_print(const ASTNode *node, int indent);

#endif /* AST_H */
