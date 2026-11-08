#include "symtab.h"

static unsigned int hash_string(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % HASH_SIZE;
}

const char *data_type_to_string(DataType type) {
    switch (type) {
        case TYPE_INT:     return "int";
        case TYPE_FLOAT:   return "float";
        case TYPE_BOOL:    return "bool";
        case TYPE_VOID:    return "void";
        case TYPE_STRING:  return "string";
        default:           return "unknown";
    }
}

DataType string_to_data_type(const char *str) {
    if (!str) return TYPE_UNKNOWN;
    if (strcmp(str, "int") == 0) return TYPE_INT;
    if (strcmp(str, "float") == 0) return TYPE_FLOAT;
    if (strcmp(str, "bool") == 0 || strcmp(str, "boolean") == 0) return TYPE_BOOL;
    if (strcmp(str, "void") == 0) return TYPE_VOID;
    if (strcmp(str, "string") == 0 || strcmp(str, "String") == 0) return TYPE_STRING;
    return TYPE_UNKNOWN;
}

static Scope *scope_create(int level, Scope *parent) {
    Scope *s = (Scope *)calloc(1, sizeof(Scope));
    if (!s) {
        fprintf(stderr, "[Fatal Error] Out of memory creating scope\n");
        exit(EXIT_FAILURE);
    }
    s->level = level;
    s->parent = parent;
    return s;
}

static void scope_free(Scope *scope) {
    if (!scope) return;
    Symbol *sym = scope->symbol_list;
    while (sym) {
        Symbol *next = sym->scope_next;
        free(sym->name);
        free(sym);
        sym = next;
    }
    free(scope);
}

SymbolTable *symtab_create(void) {
    SymbolTable *st = (SymbolTable *)calloc(1, sizeof(SymbolTable));
    if (!st) {
        fprintf(stderr, "[Fatal Error] Out of memory creating symbol table\n");
        exit(EXIT_FAILURE);
    }
    st->current_level = 0;
    st->total_scopes_created = 1;
    st->current_scope = scope_create(0, NULL); /* Global scope level 0 */
    return st;
}

void symtab_free(SymbolTable *st) {
    if (!st) return;
    Scope *curr = st->current_scope;
    while (curr) {
        Scope *parent = curr->parent;
        scope_free(curr);
        curr = parent;
    }
    free(st);
}

void symtab_enter_scope(SymbolTable *st) {
    if (!st) return;
    st->current_level++;
    st->total_scopes_created++;
    Scope *new_scope = scope_create(st->current_level, st->current_scope);
    st->current_scope = new_scope;
}

void symtab_exit_scope(SymbolTable *st) {
    if (!st || !st->current_scope) return;
    if (st->current_scope->parent == NULL) {
        fprintf(stderr, "[Warning] Attempted to exit global scope\n");
        return;
    }
    Scope *old_scope = st->current_scope;
    st->current_scope = old_scope->parent;
    st->current_level--;
    scope_free(old_scope);
}

int symtab_get_current_scope_level(SymbolTable *st) {
    return st ? st->current_level : 0;
}

Symbol *symtab_insert(SymbolTable *st, const char *name, DataType type, int line) {
    if (!st || !st->current_scope || !name) return NULL;

    /* Check if already declared in CURRENT scope level */
    if (symtab_lookup_current_scope(st, name) != NULL) {
        return NULL; /* Redeclaration error detected */
    }

    unsigned int idx = hash_string(name);
    Symbol *sym = (Symbol *)calloc(1, sizeof(Symbol));
    if (!sym) {
        fprintf(stderr, "[Fatal Error] Out of memory allocating symbol\n");
        exit(EXIT_FAILURE);
    }

    sym->name = strdup(name);
    sym->type = type;
    sym->scope_level = st->current_level;
    sym->line_declared = line;
    sym->is_initialized = 0;

    /* Insert into hash bucket */
    sym->next = st->current_scope->buckets[idx];
    st->current_scope->buckets[idx] = sym;

    /* Add to scope symbol chain */
    sym->scope_next = st->current_scope->symbol_list;
    st->current_scope->symbol_list = sym;

    return sym;
}

Symbol *symtab_lookup(SymbolTable *st, const char *name) {
    if (!st || !name) return NULL;
    unsigned int idx = hash_string(name);

    /* Search from current scope outward through parent scopes */
    for (Scope *s = st->current_scope; s != NULL; s = s->parent) {
        for (Symbol *sym = s->buckets[idx]; sym != NULL; sym = sym->next) {
            if (strcmp(sym->name, name) == 0) {
                return sym;
            }
        }
    }
    return NULL; /* Not found in any visible scope */
}

Symbol *symtab_lookup_current_scope(SymbolTable *st, const char *name) {
    if (!st || !st->current_scope || !name) return NULL;
    unsigned int idx = hash_string(name);

    for (Symbol *sym = st->current_scope->buckets[idx]; sym != NULL; sym = sym->next) {
        if (strcmp(sym->name, name) == 0) {
            return sym;
        }
    }
    return NULL;
}

void symtab_print(SymbolTable *st) {
    if (!st) return;
    printf("\n============================================================\n");
    printf("                  SYMBOL TABLE SNAPSHOT                     \n");
    printf("============================================================\n");
    printf("%-15s | %-10s | %-6s | %-12s\n", "Name", "Type", "Scope", "Declared Line");
    printf("------------------------------------------------------------\n");

    for (Scope *s = st->current_scope; s != NULL; s = s->parent) {
        for (Symbol *sym = s->symbol_list; sym != NULL; sym = sym->scope_next) {
            printf("%-15s | %-10s | %-6d | %-12d\n",
                   sym->name, data_type_to_string(sym->type), sym->scope_level, sym->line_declared);
        }
    }
    printf("============================================================\n\n");
}
