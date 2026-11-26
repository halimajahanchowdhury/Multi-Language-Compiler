#include "ast.h"

/* Helper to render structured tree hierarchy with connectors */
static void ast_print_node(const ASTNode *node, const char *prefix, int is_last);

static void print_child(const char *label, const ASTNode *child, const char *prefix, int is_last) {
    char new_prefix[512];
    snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, is_last ? "    " : "|   ");

    printf("%s%s%s\n", prefix, is_last ? "`-- " : "|-- ", label);
    if (child) {
        /* Print child statements */
        const ASTNode *curr = child;
        while (curr) {
            ast_print_node(curr, new_prefix, curr->next == NULL);
            curr = curr->next;
        }
    }
}

static void ast_print_node(const ASTNode *node, const char *prefix, int is_last) {
    if (!node) return;

    char new_prefix[512];
    snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, is_last ? "    " : "|   ");

    printf("%s%s", prefix, is_last ? "`-- " : "|-- ");

    switch (node->node_type) {
        case NODE_PROGRAM:
            printf("Program\n");
            break;

        case NODE_STMT_LIST:
            printf("StatementList\n");
            break;

        case NODE_BLOCK:
            printf("Block [Line %d]\n", node->line);
            if (node->body) {
                const ASTNode *curr = node->body;
                while (curr) {
                    ast_print_node(curr, new_prefix, curr->next == NULL);
                    curr = curr->next;
                }
            }
            break;

        case NODE_VAR_DECL:
            printf("VarDecl: %s %s [Line %d]\n",
                   data_type_to_string(node->eval_type),
                   node->attr.ident_name ? node->attr.ident_name : "(anonymous)",
                   node->line);
            if (node->right) {
                print_child("InitExpr", node->right, new_prefix, 1);
            }
            break;

        case NODE_ASSIGN:
            printf("Assign: %s [Line %d]\n",
                   node->attr.ident_name ? node->attr.ident_name : "(anonymous)",
                   node->line);
            if (node->right) {
                ast_print_node(node->right, new_prefix, 1);
            }
            break;

        case NODE_IF:
            printf("IfStatement [Line %d]\n", node->line);
            print_child("Condition", node->cond, new_prefix, 0);
            print_child("ThenBranch", node->then_branch, new_prefix, node->else_branch == NULL);
            if (node->else_branch) {
                print_child("ElseBranch", node->else_branch, new_prefix, 1);
            }
            break;

        case NODE_WHILE:
            printf("WhileLoop [Line %d]\n", node->line);
            print_child("Condition", node->cond, new_prefix, 0);
            print_child("Body", node->body, new_prefix, 1);
            break;

        case NODE_FOR:
            printf("ForLoop [Line %d]\n", node->line);
            if (node->init) print_child("Init", node->init, new_prefix, 0);
            if (node->cond) print_child("Condition", node->cond, new_prefix, 0);
            if (node->step) print_child("Step", node->step, new_prefix, 0);
            print_child("Body", node->body, new_prefix, 1);
            break;

        case NODE_PRINT:
            printf("PrintStatement [Line %d]\n", node->line);
            if (node->left) {
                const ASTNode *curr = node->left;
                while (curr) {
                    ast_print_node(curr, new_prefix, curr->next == NULL);
                    curr = curr->next;
                }
            }
            break;

        case NODE_RETURN:
            printf("ReturnStatement [Line %d]\n", node->line);
            if (node->left) {
                ast_print_node(node->left, new_prefix, 1);
            }
            break;

        case NODE_BINARY_OP:
            printf("BinaryOp (%s) [Type: %s, Line %d]\n",
                   node->attr.op ? node->attr.op : "?",
                   data_type_to_string(node->eval_type),
                   node->line);
            if (node->left) ast_print_node(node->left, new_prefix, 0);
            if (node->right) ast_print_node(node->right, new_prefix, 1);
            break;

        case NODE_UNARY_OP:
            printf("UnaryOp (%s) [Type: %s, Line %d]\n",
                   node->attr.op ? node->attr.op : "?",
                   data_type_to_string(node->eval_type),
                   node->line);
            if (node->left) ast_print_node(node->left, new_prefix, 1);
            break;

        case NODE_IDENT:
            printf("Ident: %s [Type: %s, Line %d]\n",
                   node->attr.ident_name ? node->attr.ident_name : "?",
                   data_type_to_string(node->eval_type),
                   node->line);
            break;

        case NODE_LITERAL_INT:
            printf("LiteralInt: %d [Line %d]\n", node->attr.int_val, node->line);
            break;

        case NODE_LITERAL_FLOAT:
            printf("LiteralFloat: %g [Line %d]\n", node->attr.float_val, node->line);
            break;

        case NODE_LITERAL_BOOL:
            printf("LiteralBool: %s [Line %d]\n", node->attr.bool_val ? "true" : "false", node->line);
            break;

        case NODE_LITERAL_STRING:
            printf("LiteralString: %s [Line %d]\n",
                   node->attr.str_val ? node->attr.str_val : "\"\"", node->line);
            break;

        default:
            printf("UnknownNode\n");
            break;
    }
}

void ast_print(const ASTNode *node, int indent) {
    (void)indent;
    if (!node) return;

    /* If top-level node is a Block, print root and its children with tree branches */
    if (node->node_type == NODE_BLOCK) {
        printf("Block [Line %d]\n", node->line);
        const ASTNode *curr = node->body;
        while (curr) {
            ast_print_node(curr, "", curr->next == NULL);
            curr = curr->next;
        }
    } else {
        const ASTNode *curr = node;
        while (curr) {
            ast_print_node(curr, "", curr->next == NULL);
            curr = curr->next;
        }
    }
}
