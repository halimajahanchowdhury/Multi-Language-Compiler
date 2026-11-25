#include "ast.h"

ASTNode *ast_constant_fold(ASTNode *node) {
    if (!node) return NULL;

    /* Recursively optimize children bottom-up */
    node->left = ast_constant_fold(node->left);
    node->right = ast_constant_fold(node->right);
    node->cond = ast_constant_fold(node->cond);
    node->then_branch = ast_constant_fold(node->then_branch);
    node->else_branch = ast_constant_fold(node->else_branch);
    node->body = ast_constant_fold(node->body);
    node->init = ast_constant_fold(node->init);
    node->step = ast_constant_fold(node->step);
    node->next = ast_constant_fold(node->next);

    /* Unary constant folding */
    if (node->node_type == NODE_UNARY_OP && node->left) {
        if (strcmp(node->attr.op, "-") == 0 && node->left->node_type == NODE_LITERAL_INT) {
            int val = -node->left->attr.int_val;
            ASTNode *next_node = node->next;
            ast_free(node->left);
            free(node->attr.op);
            free(node);
            ASTNode *res = ast_new_literal_int(val, 0);
            res->next = next_node;
            return res;
        } else if (strcmp(node->attr.op, "!") == 0 && node->left->node_type == NODE_LITERAL_BOOL) {
            int val = !node->left->attr.bool_val;
            ASTNode *next_node = node->next;
            ast_free(node->left);
            free(node->attr.op);
            free(node);
            ASTNode *res = ast_new_literal_bool(val, 0);
            res->next = next_node;
            return res;
        }
    }

    /* Binary constant folding */
    if (node->node_type == NODE_BINARY_OP && node->left && node->right) {
        const char *op = node->attr.op ? node->attr.op : "";

        /* Integer arithmetic and relational folding */
        if (node->left->node_type == NODE_LITERAL_INT && node->right->node_type == NODE_LITERAL_INT) {
            int l = node->left->attr.int_val;
            int r = node->right->attr.int_val;
            int folded = 0;
            int is_bool = 0;
            int can_fold = 1;

            if (strcmp(op, "+") == 0) folded = l + r;
            else if (strcmp(op, "-") == 0) folded = l - r;
            else if (strcmp(op, "*") == 0) folded = l * r;
            else if (strcmp(op, "/") == 0 && r != 0) folded = l / r;
            else if (strcmp(op, "%") == 0 && r != 0) folded = l % r;
            else if (strcmp(op, "==") == 0) { folded = (l == r); is_bool = 1; }
            else if (strcmp(op, "!=") == 0) { folded = (l != r); is_bool = 1; }
            else if (strcmp(op, "<") == 0)  { folded = (l < r);  is_bool = 1; }
            else if (strcmp(op, "<=") == 0) { folded = (l <= r); is_bool = 1; }
            else if (strcmp(op, ">") == 0)  { folded = (l > r);  is_bool = 1; }
            else if (strcmp(op, ">=") == 0) { folded = (l >= r); is_bool = 1; }
            else can_fold = 0;

            if (can_fold) {
                ASTNode *next_node = node->next;
                ast_free(node->left);
                ast_free(node->right);
                free(node->attr.op);
                free(node);

                ASTNode *res = is_bool ? ast_new_literal_bool(folded, 0) : ast_new_literal_int(folded, 0);
                res->next = next_node;
                return res;
            }
        }

        /* Boolean logical folding */
        if (node->left->node_type == NODE_LITERAL_BOOL && node->right->node_type == NODE_LITERAL_BOOL) {
            int l = node->left->attr.bool_val;
            int r = node->right->attr.bool_val;
            int folded = 0;
            int can_fold = 1;

            if (strcmp(op, "&&") == 0) folded = l && r;
            else if (strcmp(op, "||") == 0) folded = l || r;
            else if (strcmp(op, "==") == 0) folded = (l == r);
            else if (strcmp(op, "!=") == 0) folded = (l != r);
            else can_fold = 0;

            if (can_fold) {
                ASTNode *next_node = node->next;
                ast_free(node->left);
                ast_free(node->right);
                free(node->attr.op);
                free(node);

                ASTNode *res = ast_new_literal_bool(folded, 0);
                res->next = next_node;
                return res;
            }
        }
    }

    return node;
}
