# Abstract Syntax Tree (AST) Design & Specification

This document details the Abstract Syntax Tree (AST) data structures, node memory management, and visual tree dumping implemented in the compiler.

---

## 1. Node Classification

AST nodes are classified into structural categories via `NodeType` in `src/ast/ast.h`:
* **Program Structures**: `NODE_PROGRAM`, `NODE_STMT_LIST`, `NODE_BLOCK`
* **Declarations**: `NODE_VAR_DECL` (supporting types `int`, `float`, `double`, `bool`, `char`, `string`)
* **Statements**: `NODE_ASSIGN`, `NODE_IF`, `NODE_WHILE`, `NODE_FOR`, `NODE_PRINT`, `NODE_RETURN`
* **Expressions**: `NODE_BINARY_OP` (`+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `>`, `<=`, `>=`, `&&`, `||`), `NODE_UNARY_OP` (`!`, `-`)
* **Primary / Literals**: `NODE_IDENT`, `NODE_LITERAL_INT`, `NODE_LITERAL_FLOAT`, `NODE_LITERAL_BOOL`, `NODE_LITERAL_STRING`

---

## 2. Source Location Tracking
Every node structure records `line` and `col` coordinates captured from the lexical analyzer. This guarantees that semantic type checkers and diagnostics report accurate source coordinates.

---

## 3. Memory Ownership & Cleanup
AST nodes are dynamically allocated via constructor functions (`ast_new_binary_op`, `ast_new_var_decl`, etc.). The entire tree is freed in a single post-order traversal via `ast_free()` to prevent memory leaks.
