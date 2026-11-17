#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "symtab.h"
#include "ast.h"

extern int semantic_errors;

/* Entry point for semantic analysis */
int semantic_analysis(ASTNode *root, SymbolTable *st);

#endif /* SEMANTIC_H */
