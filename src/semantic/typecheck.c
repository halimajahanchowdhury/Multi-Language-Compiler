#include "semantic.h"

static int is_numeric_type(DataType t) {
    return (t == TYPE_INT || t == TYPE_FLOAT);
}

static int is_boolean_type(DataType t) {
    return (t == TYPE_BOOL);
}

static int are_types_compatible(DataType target, DataType source) {
    if (target == source) return 1;
    /* Allow widening: int can be assigned to float */
    if (target == TYPE_FLOAT && source == TYPE_INT) return 1;
    return 0;
}

DataType typecheck_expression(ASTNode *expr) {
    if (!expr) return TYPE_UNKNOWN;

    switch (expr->node_type) {
        case NODE_LITERAL_INT:
            expr->eval_type = TYPE_INT;
            return TYPE_INT;

        case NODE_LITERAL_FLOAT:
            expr->eval_type = TYPE_FLOAT;
            return TYPE_FLOAT;

        case NODE_LITERAL_BOOL:
            expr->eval_type = TYPE_BOOL;
            return TYPE_BOOL;

        case NODE_LITERAL_STRING:
            expr->eval_type = TYPE_STRING;
            return TYPE_STRING;

        case NODE_IDENT:
            /* Already resolved from symbol table during scope pass */
            return expr->eval_type;

        case NODE_UNARY_OP: {
            DataType operand_type = typecheck_expression(expr->left);
            if (strcmp(expr->attr.op, "!") == 0) {
                if (!is_boolean_type(operand_type)) {
                    fprintf(stderr, "[Semantic Error] Logical NOT '!' operator requires boolean operand at line %d (got '%s')\n",
                            expr->line, data_type_to_string(operand_type));
                    semantic_errors++;
                }
                expr->eval_type = TYPE_BOOL;
                return TYPE_BOOL;
            } else if (strcmp(expr->attr.op, "-") == 0) {
                if (!is_numeric_type(operand_type)) {
                    fprintf(stderr, "[Semantic Error] Unary negation '-' operator requires numeric operand at line %d (got '%s')\n",
                            expr->line, data_type_to_string(operand_type));
                    semantic_errors++;
                }
                expr->eval_type = operand_type;
                return operand_type;
            }
            expr->eval_type = operand_type;
            return operand_type;
        }

        case NODE_BINARY_OP: {
            DataType lt = typecheck_expression(expr->left);
            DataType rt = typecheck_expression(expr->right);
            const char *op = expr->attr.op ? expr->attr.op : "";

            /* Arithmetic operators: +, -, *, /, % */
            if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 ||
                strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) {
                
                if (!is_numeric_type(lt) || !is_numeric_type(rt)) {
                    fprintf(stderr, "[Semantic Error] Arithmetic operator '%s' cannot be applied to '%s' and '%s' at line %d\n",
                            op, data_type_to_string(lt), data_type_to_string(rt), expr->line);
                    semantic_errors++;
                    expr->eval_type = TYPE_UNKNOWN;
                    return TYPE_UNKNOWN;
                }

                if (strcmp(op, "%") == 0) {
                    if (lt != TYPE_INT || rt != TYPE_INT) {
                        fprintf(stderr, "[Semantic Error] Modulo operator '%%' requires integer operands at line %d\n", expr->line);
                        semantic_errors++;
                    }
                    expr->eval_type = TYPE_INT;
                    return TYPE_INT;
                }

                /* Type promotion: if either operand is float, result is float */
                DataType res = (lt == TYPE_FLOAT || rt == TYPE_FLOAT) ? TYPE_FLOAT : TYPE_INT;
                expr->eval_type = res;
                return res;
            }

            /* Relational operators: <, >, <=, >= */
            if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
                strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0) {
                
                if (!is_numeric_type(lt) || !is_numeric_type(rt)) {
                    fprintf(stderr, "[Semantic Error] Relational operator '%s' requires numeric operands at line %d (got '%s' and '%s')\n",
                            op, expr->line, data_type_to_string(lt), data_type_to_string(rt));
                    semantic_errors++;
                }
                expr->eval_type = TYPE_BOOL;
                return TYPE_BOOL;
            }

            /* Equality operators: ==, != */
            if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
                if (!are_types_compatible(lt, rt) && !are_types_compatible(rt, lt)) {
                    fprintf(stderr, "[Semantic Error] Equality comparison between incompatible types '%s' and '%s' at line %d\n",
                            data_type_to_string(lt), data_type_to_string(rt), expr->line);
                    semantic_errors++;
                }
                expr->eval_type = TYPE_BOOL;
                return TYPE_BOOL;
            }

            /* Logical operators: &&, || */
            if (strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
                if (!is_boolean_type(lt) || !is_boolean_type(rt)) {
                    fprintf(stderr, "[Semantic Error] Logical operator '%s' requires boolean operands at line %d (got '%s' and '%s')\n",
                            op, expr->line, data_type_to_string(lt), data_type_to_string(rt));
                    semantic_errors++;
                }
                expr->eval_type = TYPE_BOOL;
                return TYPE_BOOL;
            }

            expr->eval_type = TYPE_UNKNOWN;
            return TYPE_UNKNOWN;
        }

        default:
            return expr->eval_type;
    }
}

void typecheck_statement(ASTNode *stmt) {
    if (!stmt) return;

    switch (stmt->node_type) {
        case NODE_VAR_DECL:
            if (stmt->right) {
                DataType init_type = typecheck_expression(stmt->right);
                if (init_type != TYPE_UNKNOWN && !are_types_compatible(stmt->eval_type, init_type)) {
                    fprintf(stderr, "[Semantic Error] Type mismatch in variable declaration '%s' at line %d: cannot assign '%s' to variable of type '%s'\n",
                            stmt->attr.ident_name ? stmt->attr.ident_name : "(anonymous)",
                            stmt->line,
                            data_type_to_string(init_type),
                            data_type_to_string(stmt->eval_type));
                    semantic_errors++;
                }
            }
            break;

        case NODE_ASSIGN:
            if (stmt->right) {
                DataType rhs_type = typecheck_expression(stmt->right);
                if (stmt->eval_type != TYPE_UNKNOWN && rhs_type != TYPE_UNKNOWN) {
                    if (!are_types_compatible(stmt->eval_type, rhs_type)) {
                        fprintf(stderr, "[Semantic Error] Type mismatch in assignment to '%s' at line %d: cannot assign '%s' to variable of type '%s'\n",
                                stmt->attr.ident_name ? stmt->attr.ident_name : "(anonymous)",
                                stmt->line,
                                data_type_to_string(rhs_type),
                                data_type_to_string(stmt->eval_type));
                        semantic_errors++;
                    }
                }
            }
            break;

        case NODE_IF:
        case NODE_WHILE:
        case NODE_FOR:
            if (stmt->cond) {
                DataType cond_type = typecheck_expression(stmt->cond);
                if (cond_type != TYPE_UNKNOWN && !is_boolean_type(cond_type)) {
                    fprintf(stderr, "[Semantic Error] Condition expression at line %d must be boolean (got '%s')\n",
                            stmt->line, data_type_to_string(cond_type));
                    semantic_errors++;
                }
            }
            break;

        case NODE_PRINT:
            if (stmt->left) {
                typecheck_expression(stmt->left);
            }
            break;

        default:
            break;
    }
}
