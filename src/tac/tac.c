#include "tac.h"

static int temp_count = 0;
static int label_count = 0;

TACList *tac_list_create(void) {
    TACList *list = (TACList *)calloc(1, sizeof(TACList));
    if (!list) {
        fprintf(stderr, "[Fatal Error] Out of memory creating TAC list\n");
        exit(EXIT_FAILURE);
    }
    return list;
}

void tac_list_free(TACList *list) {
    if (!list) return;
    TACInstr *curr = list->head;
    while (curr) {
        TACInstr *next = curr->next;
        free(curr->arg1);
        free(curr->arg2);
        free(curr->result);
        free(curr);
        curr = next;
    }
    free(list);
}

char *tac_new_temp(void) {
    char buf[32];
    snprintf(buf, sizeof(buf), "t%d", ++temp_count);
    return strdup(buf);
}

char *tac_new_label(void) {
    char buf[32];
    snprintf(buf, sizeof(buf), "L%d", ++label_count);
    return strdup(buf);
}

void tac_reset_counters(void) {
    temp_count = 0;
    label_count = 0;
}

TACInstr *tac_emit(TACList *list, TACOp op, const char *arg1, const char *arg2, const char *result) {
    if (!list) return NULL;

    TACInstr *instr = (TACInstr *)calloc(1, sizeof(TACInstr));
    if (!instr) {
        fprintf(stderr, "[Fatal Error] Out of memory allocating TAC instruction\n");
        exit(EXIT_FAILURE);
    }

    instr->op = op;
    instr->arg1 = arg1 ? strdup(arg1) : NULL;
    instr->arg2 = arg2 ? strdup(arg2) : NULL;
    instr->result = result ? strdup(result) : NULL;
    instr->next = NULL;

    if (!list->head) {
        list->head = instr;
        list->tail = instr;
    } else {
        list->tail->next = instr;
        list->tail = instr;
    }
    list->count++;
    return instr;
}

void tac_append_list(TACList *dest, TACList *src) {
    if (!dest || !src || !src->head) return;
    if (!dest->head) {
        dest->head = src->head;
        dest->tail = src->tail;
    } else {
        dest->tail->next = src->head;
        dest->tail = src->tail;
    }
    dest->count += src->count;
    src->head = NULL;
    src->tail = NULL;
    src->count = 0;
}

static const char *op_to_symbol(TACOp op) {
    switch (op) {
        case TAC_ADD: return "+";
        case TAC_SUB: return "-";
        case TAC_MUL: return "*";
        case TAC_DIV: return "/";
        case TAC_MOD: return "%";
        case TAC_EQ:  return "==";
        case TAC_NE:  return "!=";
        case TAC_LT:  return "<";
        case TAC_LE:  return "<=";
        case TAC_GT:  return ">";
        case TAC_GE:  return ">=";
        case TAC_AND: return "&&";
        case TAC_OR:  return "||";
        default:      return "?";
    }
}

void tac_fprint(FILE *out, const TACList *list) {
    if (!out || !list) return;

    for (TACInstr *i = list->head; i != NULL; i = i->next) {
        switch (i->op) {
            case TAC_LABEL:
                if (i->result && strcmp(i->result, "end") == 0) {
                    fprintf(out, "end\n");
                } else {
                    fprintf(out, "%s:\n", i->result);
                }
                break;

            case TAC_GOTO:
                fprintf(out, "    goto %s\n", i->result);
                break;

            case TAC_IF_FALSE:
                fprintf(out, "    if_false %s goto %s\n", i->arg1, i->result);
                break;

            case TAC_IF_TRUE:
                fprintf(out, "    if_true %s goto %s\n", i->arg1, i->result);
                break;

            case TAC_ASSIGN:
                fprintf(out, "    %s = %s\n", i->result, i->arg1);
                break;

            case TAC_NEG:
                fprintf(out, "    %s = -%s\n", i->result, i->arg1);
                break;

            case TAC_NOT:
                fprintf(out, "    %s = !%s\n", i->result, i->arg1);
                break;

            case TAC_PRINT:
                fprintf(out, "    print %s\n", i->arg1);
                break;

            case TAC_ADD:
            case TAC_SUB:
            case TAC_MUL:
            case TAC_DIV:
            case TAC_MOD:
            case TAC_EQ:
            case TAC_NE:
            case TAC_LT:
            case TAC_LE:
            case TAC_GT:
            case TAC_GE:
            case TAC_AND:
            case TAC_OR:
                fprintf(out, "    %s = %s %s %s\n", i->result, i->arg1, op_to_symbol(i->op), i->arg2);
                break;

            default:
                break;
        }
    }
}

void tac_print(const TACList *list) {
    tac_fprint(stdout, list);
}
