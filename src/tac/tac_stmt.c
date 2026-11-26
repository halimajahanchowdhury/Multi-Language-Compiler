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
            tac_emit(list, TAC_GOTO, NULL, NULL, "end");
            break;

        default:
            break;
    }
}

static void tac_optimize_terminal_jumps(TACList *list) {
    if (!list || !list->head) return;

    /* 1. Redirect jumps to any label that immediately precedes 'goto end' */
    for (TACInstr *i = list->head; i != NULL; i = i->next) {
        if (i->op == TAC_LABEL && i->next && i->next->op == TAC_GOTO &&
            i->next->result && strcmp(i->next->result, "end") == 0) {
            char *target = i->result;
            if (target && strcmp(target, "end") != 0) {
                for (TACInstr *j = list->head; j != NULL; j = j->next) {
                    if (j != i && j->result && strcmp(j->result, target) == 0) {
                        free(j->result);
                        j->result = strdup("end");
                    }
                }
            }
        }
    }

    /* 2. Remove redundant fall-through goto end and unused labels directly preceding 'end' */
    while (list->tail && list->tail != list->head) {
        TACInstr *penult = NULL;
        for (TACInstr *p = list->head; p && p->next != list->tail; p = p->next) {
            penult = p;
        }
        if (!penult) {
            if (list->head->next == list->tail) penult = list->head;
            else break;
        }

        if (list->tail->op == TAC_LABEL && list->tail->result && strcmp(list->tail->result, "end") == 0) {
            if (penult->op == TAC_GOTO && penult->result && strcmp(penult->result, "end") == 0) {
                /* Remove penult goto end */
                if (penult == list->head) {
                    list->head = list->tail;
                } else {
                    TACInstr *antepenult = list->head;
                    while (antepenult && antepenult->next != penult) antepenult = antepenult->next;
                    if (antepenult) antepenult->next = list->tail;
                }
                free(penult->arg1);
                free(penult->arg2);
                free(penult->result);
                free(penult);
                list->count--;
                continue;
            }
            if (penult->op == TAC_LABEL) {
                int referenced = 0;
                for (TACInstr *j = list->head; j != NULL; j = j->next) {
                    if (j != penult && (j->op == TAC_GOTO || j->op == TAC_IF_FALSE || j->op == TAC_IF_TRUE)) {
                        if (j->result && strcmp(j->result, penult->result) == 0) {
                            referenced = 1;
                            break;
                        }
                    }
                }
                if (!referenced) {
                    if (penult == list->head) {
                        list->head = list->tail;
                    } else {
                        TACInstr *antepenult = list->head;
                        while (antepenult && antepenult->next != penult) antepenult = antepenult->next;
                        if (antepenult) antepenult->next = list->tail;
                    }
                    free(penult->arg1);
                    free(penult->arg2);
                    free(penult->result);
                    free(penult);
                    list->count--;
                    continue;
                }
            }
        }
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

    /* Finalize TAC: ensure compilation concludes with 'end' and trailing jumps route to 'end' */
    if (list->tail) {
        if (list->tail->op == TAC_LABEL) {
            char *old_label = list->tail->result ? strdup(list->tail->result) : NULL;
            if (old_label && strcmp(old_label, "end") != 0) {
                for (TACInstr *instr = list->head; instr != NULL; instr = instr->next) {
                    if (instr->result && strcmp(instr->result, old_label) == 0) {
                        free(instr->result);
                        instr->result = strdup("end");
                    }
                }
            }
            if (old_label) free(old_label);
        } else {
            if (!(list->tail->result && strcmp(list->tail->result, "end") == 0 && list->tail->op == TAC_LABEL)) {
                tac_emit(list, TAC_LABEL, NULL, NULL, "end");
            }
        }
    } else {
        tac_emit(list, TAC_LABEL, NULL, NULL, "end");
    }

    tac_optimize_terminal_jumps(list);

    return list;
}
