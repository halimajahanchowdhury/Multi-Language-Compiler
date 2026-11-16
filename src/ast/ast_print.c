#include "ast.h"

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

void ast_print(const ASTNode *node, int indent) {
    if (!node) return;

    print_indent(indent);

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
                ast_print(node->body, indent + 1);
            }
            break;

        case NODE_VAR_DECL:
            printf("VarDecl: %s %s [Line %d]\n",
                   data_type_to_string(node->eval_type),
                   node->attr.ident_name ? node->attr.ident_name : "(anonymous)",
                   node->line);
            if (node->right) {
                print_indent(indent + 1);
                printf("InitExpr:\n");
                ast_print(node->right, indent + 2);
            }
            break;

        case NODE_ASSIGN:
            printf("Assign: %s [Line %d]\n",
                   node->attr.ident_name ? node->attr.ident_name : "(anonymous)",
                   node->line);
            if (node->right) {
                ast_print(node->right, indent + 1);
            }
            break;

        case NODE_IF:
            printf("IfStatement [Line %d]\n", node->line);
            print_indent(indent + 1);
            printf("Condition:\n");
            ast_print(node->cond, indent + 2);
            print_indent(indent + 1);
            printf("ThenBranch:\n");
            ast_print(node->then_branch, indent + 2);
            if (node->else_branch) {
                print_indent(indent + 1);
                printf("ElseBranch:\n");
                ast_print(node->else_branch, indent + 2);
            }
            break;

        case NODE_WHILE:
            printf("WhileLoop [Line %d]\n", node->line);
            print_indent(indent + 1);
            printf("Condition:\n");
            ast_print(node->cond, indent + 2);
            print_indent(indent + 1);
            printf("Body:\n");
            ast_print(node->body, indent + 2);
            break;

        case NODE_FOR:
            printf("ForLoop [Line %d]\n", node->line);
            if (node->init) {
                print_indent(indent + 1);
                printf("Init:\n");
                ast_print(node->init, indent + 2);
            }
            if (node->cond) {
                print_indent(indent + 1);
                printf("Condition:\n");
                ast_print(node->cond, indent + 2);
            }
            if (node->step) {
                print_indent(indent + 1);
                printf("Step:\n");
                ast_print(node->step, indent + 2);
            }
            print_indent(indent + 1);
            printf("Body:\n");
            ast_print(node->body, indent + 2);
            break;

        case NODE_PRINT:
            printf("PrintStatement [Line %d]\n", node->line);
            ast_print(node->left, indent + 1);
            break;

        case NODE_RETURN:
            printf("ReturnStatement [Line %d]\n", node->line);
            if (node->left) {
                ast_print(node->left, indent + 1);
            }
            break;

        case NODE_BINARY_OP:
            printf("BinaryOp (%s) [Type: %s, Line %d]\n",
                   node->attr.op ? node->attr.op : "?",
                   data_type_to_string(node->eval_type),
                   node->line);
            ast_print(node->left, indent + 1);
            ast_print(node->right, indent + 1);
            break;

        case NODE_UNARY_OP:
            printf("UnaryOp (%s) [Type: %s, Line %d]\n",
                   node->attr.op ? node->attr.op : "?",
                   data_type_to_string(node->eval_type),
                   node->line);
            ast_print(node->left, indent + 1);
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

    /* Print subsequent statements at same indentation */
    if (node->next) {
        ast_print(node->next, indent);
    }
}
