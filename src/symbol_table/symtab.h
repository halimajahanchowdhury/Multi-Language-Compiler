#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 211

/* Supported Data Types */
typedef enum {
    TYPE_UNKNOWN = 0,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_VOID,
    TYPE_STRING
} DataType;

const char *data_type_to_string(DataType type);
DataType string_to_data_type(const char *str);

/* Symbol Table Entry */
typedef struct Symbol {
    char *name;
    DataType type;
    int scope_level;
    int line_declared;
    int is_initialized;
    struct Symbol *next;        /* Collision chain in bucket */
    struct Symbol *scope_next;  /* Linked list of symbols in same scope */
} Symbol;

/* Nested Scope representation */
typedef struct Scope {
    int level;
    Symbol *buckets[HASH_SIZE];
    Symbol *symbol_list;        /* Linked list of all symbols in this scope level (head) */
    Symbol *symbol_tail;        /* Tail pointer for insertion order */
    struct Scope *parent;       /* Enclosing parent scope */
    struct Scope *all_next;     /* Chronological list of all created scopes */
} Scope;

/* Top-level Symbol Table manager */
typedef struct SymbolTable {
    Scope *current_scope;
    Scope *all_scopes_head;
    Scope *all_scopes_tail;
    int current_level;
    int total_scopes_created;
} SymbolTable;

/* Lifecycle */
SymbolTable *symtab_create(void);
void symtab_free(SymbolTable *st);

/* Scope Management */
void symtab_enter_scope(SymbolTable *st);
void symtab_exit_scope(SymbolTable *st);
int symtab_get_current_scope_level(SymbolTable *st);

/* Symbol Operations */
Symbol *symtab_insert(SymbolTable *st, const char *name, DataType type, int line);
Symbol *symtab_lookup(SymbolTable *st, const char *name);
Symbol *symtab_lookup_current_scope(SymbolTable *st, const char *name);

/* Diagnostics & Visualization */
void symtab_print(SymbolTable *st);

#endif /* SYMTAB_H */
