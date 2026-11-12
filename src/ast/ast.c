#include "ast.h"

static ASTNode *ast_alloc(NodeType type, int line) {
    ASTNode *node = (ASTNode *)calloc(1, sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "[Fatal Error] Out of memory creating AST node\n");
        exit(EXIT_FAILURE);
    }
    node->node_type = type;
    node->eval_type = TYPE_UNKNOWN;
    node->line = line;
    return node;
}

ASTNode *ast_new_literal_int(int val, int line) {
    ASTNode *n = ast_alloc(NODE_LITERAL_INT, line);
    n->eval_type = TYPE_INT;
    n->attr.int_val = val;
    return n;
}

ASTNode *ast_new_literal_float(double val, int line) {
    ASTNode *n = ast_alloc(NODE_LITERAL_FLOAT, line);
    n->eval_type = TYPE_FLOAT;
    n->attr.float_val = val;
    return n;
}

ASTNode *ast_new_literal_bool(int val, int line) {
    ASTNode *n = ast_alloc(NODE_LITERAL_BOOL, line);
    n->eval_type = TYPE_BOOL;
    n->attr.bool_val = val;
    return n;
}

ASTNode *ast_new_literal_string(const char *str, int line) {
    ASTNode *n = ast_alloc(NODE_LITERAL_STRING, line);
    n->eval_type = TYPE_STRING;
    n->attr.str_val = str ? strdup(str) : NULL;
    return n;
}

ASTNode *ast_new_ident(const char *name, int line) {
    ASTNode *n = ast_alloc(NODE_IDENT, line);
    n->attr.ident_name = name ? strdup(name) : NULL;
    return n;
}

ASTNode *ast_new_var_decl(DataType type, const char *name, ASTNode *init_expr, int line) {
    ASTNode *n = ast_alloc(NODE_VAR_DECL, line);
    n->eval_type = type;
    n->attr.ident_name = name ? strdup(name) : NULL;
    n->right = init_expr; /* optional initialization expression */
    return n;
}

ASTNode *ast_new_assign(const char *name, ASTNode *expr, int line) {
    ASTNode *n = ast_alloc(NODE_ASSIGN, line);
    n->attr.ident_name = name ? strdup(name) : NULL;
    n->right = expr;
    return n;
}

ASTNode *ast_new_binary_op(const char *op, ASTNode *left, ASTNode *right, int line) {
    ASTNode *n = ast_alloc(NODE_BINARY_OP, line);
    n->attr.op = op ? strdup(op) : NULL;
    n->left = left;
    n->right = right;
    return n;
}

ASTNode *ast_new_unary_op(const char *op, ASTNode *operand, int line) {
    ASTNode *n = ast_alloc(NODE_UNARY_OP, line);
    n->attr.op = op ? strdup(op) : NULL;
    n->left = operand;
    return n;
}

ASTNode *ast_new_if(ASTNode *cond, ASTNode *then_b, ASTNode *else_b, int line) {
    ASTNode *n = ast_alloc(NODE_IF, line);
    n->cond = cond;
    n->then_branch = then_b;
    n->else_branch = else_b;
    return n;
}

ASTNode *ast_new_while(ASTNode *cond, ASTNode *body, int line) {
    ASTNode *n = ast_alloc(NODE_WHILE, line);
    n->cond = cond;
    n->body = body;
    return n;
}

ASTNode *ast_new_for(ASTNode *init, ASTNode *cond, ASTNode *step, ASTNode *body, int line) {
    ASTNode *n = ast_alloc(NODE_FOR, line);
    n->init = init;
    n->cond = cond;
    n->step = step;
    n->body = body;
    return n;
}

ASTNode *ast_new_print(ASTNode *expr, int line) {
    ASTNode *n = ast_alloc(NODE_PRINT, line);
    n->left = expr;
    return n;
}

ASTNode *ast_new_block(ASTNode *stmt_list, int line) {
    ASTNode *n = ast_alloc(NODE_BLOCK, line);
    n->body = stmt_list;
    return n;
}

ASTNode *ast_new_return(ASTNode *expr, int line) {
    ASTNode *n = ast_alloc(NODE_RETURN, line);
    n->left = expr;
    return n;
}

ASTNode *ast_append_stmt(ASTNode *list, ASTNode *stmt) {
    if (!list) return stmt;
    if (!stmt) return list;
    ASTNode *curr = list;
    while (curr->next) {
        curr = curr->next;
    }
    curr->next = stmt;
    return list;
}

void ast_free(ASTNode *node) {
    if (!node) return;

    /* Free string attributes */
    if (node->node_type == NODE_LITERAL_STRING) {
        free(node->attr.str_val);
    } else if (node->node_type == NODE_IDENT || node->node_type == NODE_VAR_DECL || node->node_type == NODE_ASSIGN) {
        free(node->attr.ident_name);
    } else if (node->node_type == NODE_BINARY_OP || node->node_type == NODE_UNARY_OP) {
        free(node->attr.op);
    }

    /* Recursively free children */
    ast_free(node->left);
    ast_free(node->right);
    ast_free(node->cond);
    ast_free(node->then_branch);
    ast_free(node->else_branch);
    ast_free(node->body);
    ast_free(node->init);
    ast_free(node->step);
    ast_free(node->next);

    free(node);
}
