#include "semantic.h"

int semantic_errors = 0;

static void check_node(ASTNode *node, SymbolTable *st) {
    if (!node) return;

    switch (node->node_type) {
        case NODE_PROGRAM:
        case NODE_STMT_LIST:
            break;

        case NODE_BLOCK:
            symtab_enter_scope(st);
            if (node->body) {
                check_node(node->body, st);
            }
            symtab_exit_scope(st);
            break;

        case NODE_VAR_DECL: {
            Symbol *existing = symtab_lookup_current_scope(st, node->attr.ident_name);
            if (existing) {
                fprintf(stderr, "[Semantic Error] Redeclaration of variable '%s' at line %d (previously declared at line %d)\n",
                        node->attr.ident_name, node->line, existing->line_declared);
                semantic_errors++;
            } else {
                symtab_insert(st, node->attr.ident_name, node->eval_type, node->line);
            }
            if (node->right) {
                check_node(node->right, st);
            }
            break;
        }

        case NODE_ASSIGN: {
            Symbol *sym = symtab_lookup(st, node->attr.ident_name);
            if (!sym) {
                fprintf(stderr, "[Semantic Error] Assignment to undeclared variable '%s' at line %d\n",
                        node->attr.ident_name, node->line);
                semantic_errors++;
                node->eval_type = TYPE_UNKNOWN;
            } else {
                node->eval_type = sym->type;
            }
            if (node->right) {
                check_node(node->right, st);
            }
            break;
        }

        case NODE_IDENT: {
            Symbol *sym = symtab_lookup(st, node->attr.ident_name);
            if (!sym) {
                fprintf(stderr, "[Semantic Error] Use of undeclared variable '%s' at line %d\n",
                        node->attr.ident_name, node->line);
                semantic_errors++;
                node->eval_type = TYPE_UNKNOWN;
            } else {
                node->eval_type = sym->type;
            }
            break;
        }

        case NODE_IF:
            check_node(node->cond, st);
            check_node(node->then_branch, st);
            if (node->else_branch) {
                check_node(node->else_branch, st);
            }
            break;

        case NODE_WHILE:
            check_node(node->cond, st);
            check_node(node->body, st);
            break;

        case NODE_FOR:
            symtab_enter_scope(st);
            if (node->init) check_node(node->init, st);
            if (node->cond) check_node(node->cond, st);
            if (node->step) check_node(node->step, st);
            if (node->body) check_node(node->body, st);
            symtab_exit_scope(st);
            break;

        case NODE_PRINT:
        case NODE_RETURN:
            if (node->left) {
                check_node(node->left, st);
            }
            break;

        case NODE_BINARY_OP:
            check_node(node->left, st);
            check_node(node->right, st);
            break;

        case NODE_UNARY_OP:
            check_node(node->left, st);
            break;

        case NODE_LITERAL_INT:
        case NODE_LITERAL_FLOAT:
        case NODE_LITERAL_BOOL:
        case NODE_LITERAL_STRING:
            /* Terminals evaluated already */
            break;

        default:
            break;
    }

    /* Process next statement in sequence */
    if (node->next) {
        check_node(node->next, st);
    }
}

int semantic_analysis(ASTNode *root, SymbolTable *st) {
    if (!root || !st) return 0;
    semantic_errors = 0;
    check_node(root, st);
    return semantic_errors;
}
