#include "tac.h"

static TACOp get_binary_tac_op(const char *op) {
    if (!op) return TAC_NOP;
    if (strcmp(op, "+") == 0) return TAC_ADD;
    if (strcmp(op, "-") == 0) return TAC_SUB;
    if (strcmp(op, "*") == 0) return TAC_MUL;
    if (strcmp(op, "/") == 0) return TAC_DIV;
    if (strcmp(op, "%") == 0) return TAC_MOD;
    if (strcmp(op, "==") == 0) return TAC_EQ;
    if (strcmp(op, "!=") == 0) return TAC_NE;
    if (strcmp(op, "<") == 0) return TAC_LT;
    if (strcmp(op, "<=") == 0) return TAC_LE;
    if (strcmp(op, ">") == 0) return TAC_GT;
    if (strcmp(op, ">=") == 0) return TAC_GE;
    if (strcmp(op, "&&") == 0) return TAC_AND;
    if (strcmp(op, "||") == 0) return TAC_OR;
    return TAC_NOP;
}

char *tac_gen_expr(TACList *list, ASTNode *expr) {
    if (!expr) return NULL;

    char buf[64];

    switch (expr->node_type) {
        case NODE_LITERAL_INT:
            snprintf(buf, sizeof(buf), "%d", expr->attr.int_val);
            return strdup(buf);

        case NODE_LITERAL_FLOAT:
            snprintf(buf, sizeof(buf), "%g", expr->attr.float_val);
            return strdup(buf);

        case NODE_LITERAL_BOOL:
            return strdup(expr->attr.bool_val ? "true" : "false");

        case NODE_LITERAL_STRING:
            return strdup(expr->attr.str_val ? expr->attr.str_val : "\"\"");

        case NODE_IDENT:
            return strdup(expr->attr.ident_name ? expr->attr.ident_name : "");

        case NODE_UNARY_OP: {
            char *operand = tac_gen_expr(list, expr->left);
            char *temp = tac_new_temp();
            if (strcmp(expr->attr.op, "-") == 0) {
                tac_emit(list, TAC_NEG, operand, NULL, temp);
            } else if (strcmp(expr->attr.op, "!") == 0) {
                tac_emit(list, TAC_NOT, operand, NULL, temp);
            }
            free(operand);
            return temp;
        }

        case NODE_BINARY_OP: {
            char *left_val = tac_gen_expr(list, expr->left);
            char *right_val = tac_gen_expr(list, expr->right);
            char *temp = tac_new_temp();
            TACOp op = get_binary_tac_op(expr->attr.op);

            tac_emit(list, op, left_val, right_val, temp);

            free(left_val);
            free(right_val);
            return temp;
        }

        default:
            return NULL;
    }
}

void tac_gen_assign(TACList *list, ASTNode *assign) {
    if (!list || !assign) return;

    if (assign->node_type == NODE_ASSIGN) {
        char *rhs = tac_gen_expr(list, assign->right);
        tac_emit(list, TAC_ASSIGN, rhs, NULL, assign->attr.ident_name);
        free(rhs);
    } else if (assign->node_type == NODE_VAR_DECL) {
        if (assign->right) {
            char *rhs = tac_gen_expr(list, assign->right);
            tac_emit(list, TAC_ASSIGN, rhs, NULL, assign->attr.ident_name);
            free(rhs);
        }
    }
}
