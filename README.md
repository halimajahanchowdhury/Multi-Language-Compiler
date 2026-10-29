# Multi-Language Compiler (C, C++, Java)

A modular Compiler Front-End with Intermediate Code Generation (Three Address Code) designed and implemented using **Flex (Lexical Analyzer)** and **Bison (Parser Generator)** in C for the Compiler Construction Laboratory at Metropolitan University, Bangladesh.

---

## 📌 Project Overview

This compiler processes programs written in **C**, **C++**, and **Java**, supporting core imperative language constructs:
- **Language Mode Selection**: Choose between **C**, **C++**, and **Java** via interactive CLI or command-line flags.
- **Lexical Analysis (Flex)**: Tokenizes keywords, identifiers, constants, arithmetic/relational/logical operators, delimiters, and discards whitespace and single/block comments.
- **Syntax Analysis (Bison)**: Validates Context-Free Grammar (CFG) with robust operator precedence and syntax error diagnostics.
- **Abstract Syntax Tree (AST)**: Builds an in-memory hierarchical syntax tree with readable text-based indented visualization and Graphviz export.
- **Scoped Symbol Table**: Implements nested block scope resolution (`{ ... }`), tracking identifiers, data types, and declaration line numbers.
- **Semantic Analysis**: Enforces type compatibility, detects undeclared variables, prevents scope violations, and catches duplicate declarations.
- **Intermediate Code Generation**: Emits low-level **Three Address Code (TAC)** using temporaries (`t1`, `t2`, ...) and jump labels (`L1`, `L2`, ...) for control flow.

---

## 🛠️ Compiler Architecture & Pipeline

```
 Source Code (.c / .cpp / .java / .mc)
                  │
                  ▼
         [ Lexical Analyzer ]  (Flex - lexer.l)
                  │  Token Stream
                  ▼
         [  Syntax Analyzer ]  (Bison - parser.y)
                  │  Parse Tree Actions
                  ▼
       [ Abstract Syntax Tree ]  (AST - ast.c)
                  │  Hierarchical Tree
                  ▼
      [ Scoped Symbol Table ]  (symtab.c)
                  │  Scope & Identifier Metadata
                  ▼
      [  Semantic Analyzer  ]  (semantic.c, typecheck.c)
                  │  Type Checking & Scope Validation
                  ▼
    [ Three Address Code (TAC) ]  (tac.c)
                  │
                  ▼
      Linear TAC Output / Diagnostic Errors
```

---

## 📂 Directory Structure

```text
Multi-Language-Compiler/
├── docs/                      # Architectural designs, CFG specifications, and reports
├── src/
│   ├── lexer/                 # Flex specification (lexer.l) and token definitions
│   ├── parser/                # Bison grammar (parser.y) and CFG rules
│   ├── ast/                   # AST node structures and tree printing utilities
│   ├── symbol_table/          # Scoped symbol table and hash table operations
│   ├── semantic/              # Type checking and semantic error detection
│   ├── tac/                   # Three Address Code data structures and generator
│   └── main.c                 # Compiler entry point, language selector, and CLI driver
├── tests/
│   ├── valid/                 # Valid test programs for C, C++, and Java
│   └── invalid/               # Test cases with deliberate lexical, syntax, and semantic errors
├── examples/                  # Demo input programs and sample TAC outputs
├── Makefile                   # Build automation rules
└── README.md                  # Project documentation
```

---

## 🚀 Building and Running

### Prerequisites
- **GCC** (GNU Compiler Collection / MinGW)
- **Flex** (Fast Lexical Analyzer Generator)
- **Bison** (GNU Parser Generator)
- **Make** (GNU Make)

### Build
To build the compiler executable:
```bash
make
```

### Execution
Run the compiler interactively (prompts for language selection):
```bash
./compiler
```

Or pass the target language and input file via command-line:
```bash
# Compile a C source file:
./compiler --lang=c examples/sample_c.c

# Compile a C++ source file:
./compiler --lang=cpp examples/sample_cpp.cpp

# Compile a Java source file:
./compiler --lang=java examples/sample_java.java
```

### Cleaning Build Artifacts
```bash
make clean
```

---

## 👥 Contributors

- **Halima Jahan Chowdhury** ([@halimajahanchowdhury](https://github.com/halimajahanchowdhury))
- **Fatema Jannat Ema** ([@fatemajannatema](https://github.com/fatemajannatema))

**Department of Computer Science and Engineering**  
**Metropolitan University, Bangladesh**
