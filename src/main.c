#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "ast.h"
#include "semantic.h"
#include "tac.h"
#include "parser.tab.h"

extern FILE *yyin;
extern int yyparse(void);
extern ASTNode *ast_root;
extern int syntax_errors;
extern int lexical_errors;

typedef enum {
    LANG_UNKNOWN = 0,
    LANG_C,
    LANG_CPP,
    LANG_JAVA,
    LANG_MINI
} TargetLanguage;

static const char *lang_name(TargetLanguage lang) {
    switch (lang) {
        case LANG_C:    return "C (ISO C99/C11 Subset)";
        case LANG_CPP:  return "C++ (C++11 Subset)";
        case LANG_JAVA: return "Java (Core Language Subset)";
        default:        return "Custom Mini-Language (Section 5)";
    }
}

static TargetLanguage detect_language_from_ext(const char *filename) {
    if (!filename) return LANG_MINI;
    const char *dot = strrchr(filename, '.');
    if (!dot) return LANG_MINI;
    if (strcmp(dot, ".c") == 0) return LANG_C;
    if (strcmp(dot, ".cpp") == 0 || strcmp(dot, ".cc") == 0 || strcmp(dot, ".cxx") == 0) return LANG_CPP;
    if (strcmp(dot, ".java") == 0) return LANG_JAVA;
    return LANG_MINI;
}

static void print_banner(void) {
    printf("============================================================\n");
    printf("        MULTI-LANGUAGE COMPILER (C, C++, Java)              \n");
    printf("   Department of Computer Science & Engineering, MU         \n");
    printf("============================================================\n");
}

int main(int argc, char **argv) {
    TargetLanguage target_lang = LANG_UNKNOWN;
    const char *input_path = NULL;

    /* Parse CLI Arguments */
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--lang=", 7) == 0) {
            const char *l = argv[i] + 7;
            if (strcmp(l, "c") == 0) target_lang = LANG_C;
            else if (strcmp(l, "cpp") == 0) target_lang = LANG_CPP;
            else if (strcmp(l, "java") == 0) target_lang = LANG_JAVA;
            else target_lang = LANG_MINI;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_banner();
            printf("Usage:\n");
            printf("  %s [--lang=c|cpp|java] <source_file>\n", argv[0]);
            printf("  %s (Runs interactive language selector mode)\n", argv[0]);
            return 0;
        } else if (!input_path) {
            input_path = argv[i];
        }
    }

    char buffer[256];
    /* Interactive Mode if no file supplied */
    if (!input_path) {
        print_banner();
        printf("Select Target Language Mode (like an Online Compiler):\n");
        printf("  [1] C Language Mode\n");
        printf("  [2] C++ Language Mode\n");
        printf("  [3] Java Language Mode\n");
        printf("------------------------------------------------------------\n");
        printf("Enter your choice (1-3) [default: 1]: ");
        if (fgets(buffer, sizeof(buffer), stdin)) {
            int choice = atoi(buffer);
            if (choice == 2) target_lang = LANG_CPP;
            else if (choice == 3) target_lang = LANG_JAVA;
            else target_lang = LANG_C;
        } else {
            target_lang = LANG_C;
        }

        printf("Enter source file path: ");
        if (fgets(buffer, sizeof(buffer), stdin)) {
            size_t len = strlen(buffer);
            while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r' || buffer[len - 1] == ' ')) {
                buffer[--len] = '\0';
            }
            input_path = buffer;
        }
    }

    if (!input_path || strlen(input_path) == 0) {
        fprintf(stderr, "[Error] No source file provided for compilation.\n");
        return 1;
    }

    if (target_lang == LANG_UNKNOWN) {
        target_lang = detect_language_from_ext(input_path);
    }

    FILE *f = fopen(input_path, "r");
    if (!f) {
        fprintf(stderr, "[Error] Cannot open input file: '%s'\n", input_path);
        return 1;
    }

    printf("\n>>> Target Language Selected: %s\n", lang_name(target_lang));
    printf(">>> Compiling: %s\n\n", input_path);

    /* Phase 1 & 2: Lexical and Syntax Analysis */
    yyin = f;
    syntax_errors = 0;
    lexical_errors = 0;

    int parse_res = yyparse();
    fclose(f);

    if (lexical_errors > 0 || syntax_errors > 0 || parse_res != 0) {
        fprintf(stderr, "\n[Compilation Failed] Found %d lexical error(s) and %d syntax error(s).\n",
                lexical_errors, syntax_errors);
        if (ast_root) {
            ast_free(ast_root);
            ast_root = NULL;
        }
        return 1;
    }

    /* Abstract Syntax Tree Output */
    printf("============================================================\n");
    printf("                ABSTRACT SYNTAX TREE (AST)                  \n");
    printf("============================================================\n");
    if (ast_root) {
        ast_print(ast_root, 0);
    } else {
        printf("(Empty AST)\n");
    }

    /* Phase 3: Semantic Analysis and Symbol Table */
    printf("\n============================================================\n");
    printf("                 RUNNING SEMANTIC ANALYSIS                  \n");
    printf("============================================================\n");
    SymbolTable *st = symtab_create();
    int sem_errors = semantic_analysis(ast_root, st);

    symtab_print(st);

    if (sem_errors > 0) {
        fprintf(stderr, "[Compilation Failed] Found %d semantic error(s).\n", sem_errors);
        symtab_free(st);
        ast_free(ast_root);
        return 1;
    }

    printf("[Validation Succeeded] Front-end parsing and semantic validation passed with 0 errors.\n");

    /* Optimization Phase: Constant Folding (Section 14 Bonus) */
    ast_root = ast_constant_fold(ast_root);

    /* Phase 4: Intermediate Code Generation (Three Address Code) */
    printf("\n============================================================\n");
    printf("            THREE ADDRESS CODE (TAC) OUTPUT                 \n");
    printf("============================================================\n");
    TACList *tac = tac_generate(ast_root);
    if (tac) {
        tac_print(tac);
        tac_list_free(tac);
    }
    printf("============================================================\n");

    /* Memory Cleanup */
    symtab_free(st);
    if (ast_root) {
        ast_free(ast_root);
        ast_root = NULL;
    }

    return 0;
}

