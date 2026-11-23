#include "tac.h"

void tac_gen_stmt(TACList *list, ASTNode *stmt) {
    if (!list || !stmt) return;

    switch (stmt->node_type) {
        case NODE_PROGRAM:
        case NODE_STMT_LIST:
            break;

        case NODE_BLOCK: {
            for (ASTNode *s = stmt->body; s != NULL; s = s->next) {
                tac_gen_stmt(list, s);
            }
            break;
        }

        case NODE_VAR_DECL:
        case NODE_ASSIGN:
            tac_gen_assign(list, stmt);
            break;

        case NODE_PRINT: {
            char *arg = tac_gen_expr(list, stmt->left);
            tac_emit(list, TAC_PRINT, arg, NULL, NULL);
            free(arg);
            break;
        }

        case NODE_IF: {
            char *cond = tac_gen_expr(list, stmt->cond);

            if (stmt->else_branch) {
                char *lbl_else = tac_new_label();
                char *lbl_end = tac_new_label();

                tac_emit(list, TAC_IF_FALSE, cond, NULL, lbl_else);
                tac_gen_stmt(list, stmt->then_branch);
                tac_emit(list, TAC_GOTO, NULL, NULL, lbl_end);

                tac_emit(list, TAC_LABEL, NULL, NULL, lbl_else);
                tac_gen_stmt(list, stmt->else_branch);

                tac_emit(list, TAC_LABEL, NULL, NULL, lbl_end);

                free(lbl_else);
                free(lbl_end);
            } else {
                char *lbl_end = tac_new_label();

                tac_emit(list, TAC_IF_FALSE, cond, NULL, lbl_end);
                tac_gen_stmt(list, stmt->then_branch);
                tac_emit(list, TAC_LABEL, NULL, NULL, lbl_end);

                free(lbl_end);
            }
            free(cond);
            break;
        }

        case NODE_WHILE: {
            char *lbl_start = tac_new_label();
            char *lbl_end = tac_new_label();

            tac_emit(list, TAC_LABEL, NULL, NULL, lbl_start);

            char *cond = tac_gen_expr(list, stmt->cond);
            tac_emit(list, TAC_IF_FALSE, cond, NULL, lbl_end);

            tac_gen_stmt(list, stmt->body);
            tac_emit(list, TAC_GOTO, NULL, NULL, lbl_start);

            tac_emit(list, TAC_LABEL, NULL, NULL, lbl_end);

            free(cond);
            free(lbl_start);
            free(lbl_end);
            break;
        }

        case NODE_FOR: {
            if (stmt->init) {
                tac_gen_stmt(list, stmt->init);
            }

            char *lbl_start = tac_new_label();
            char *lbl_end = tac_new_label();

            tac_emit(list, TAC_LABEL, NULL, NULL, lbl_start);

            if (stmt->cond) {
                char *cond = tac_gen_expr(list, stmt->cond);
                tac_emit(list, TAC_IF_FALSE, cond, NULL, lbl_end);
                free(cond);
            }

            tac_gen_stmt(list, stmt->body);

            if (stmt->step) {
                tac_gen_stmt(list, stmt->step);
            }

            tac_emit(list, TAC_GOTO, NULL, NULL, lbl_start);
            tac_emit(list, TAC_LABEL, NULL, NULL, lbl_end);

            free(lbl_start);
            free(lbl_end);
            break;
        }

        case NODE_RETURN:
            if (stmt->left) {
                char *ret = tac_gen_expr(list, stmt->left);
                free(ret);
            }
            break;

        default:
            break;
    }
}

TACList *tac_generate(ASTNode *root) {
    if (!root) return NULL;

    tac_reset_counters();
    TACList *list = tac_list_create();

    for (ASTNode *curr = root; curr != NULL; curr = curr->next) {
        tac_gen_stmt(list, curr);
    }

    return list;
}
