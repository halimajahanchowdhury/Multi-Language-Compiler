# Multi-Language Compiler

A modular compiler front-end with intermediate code generation implemented in C using Flex and Bison. This system analyzes and translates core imperative programs written in C, C++, and Java into Three Address Code (TAC). Developed for the Compiler Construction Laboratory at Metropolitan University, Bangladesh.

---

## Overview

The compiler processes standard constructs across C, C++, and Java, verifying source syntax and semantics before emitting low-level intermediate code:

- **Language Support**: Supports core subsets of C, C++, and Java, including distinct I/O mechanisms (`printf`, `cout`/`cin`, and `System.out.println`) and boilerplate wrappers.
- **Lexical Analysis (Flex)**: Tokenizes language keywords, identifiers, constants, arithmetic, relational, and logical operators while filtering out comments and whitespace.
- **Syntax Analysis (Bison)**: Implements an LALR(1) Context-Free Grammar with defined precedence rules and syntax error recovery.
- **Abstract Syntax Tree (AST)**: Constructs an in-memory tree representation of the input program and provides hierarchical text-based visualization.
- **Symbol Table Management**: Enforces lexical scoping rules using a scoped symbol table that tracks identifier types, depths, and line declarations.
- **Semantic Analysis**: Detects undeclared variables, duplicate declarations within the same scope, and type mismatches in expressions and control flow conditions.
- **Intermediate Code Generation**: Emits linear Three Address Code (TAC) with sequential temporary variables and jump labels for conditional and iterative control flow.

---

## Compiler Pipeline

```
 Source Program (.c / .cpp / .java)
                 │
                 ▼
       [ Lexical Analyzer ]      Flex (lexer.l)
                 │  Tokens
                 ▼
       [  Syntax Analyzer ]      Bison (parser.y)
                 │  Parse Actions
                 ▼
     [ Abstract Syntax Tree ]    AST (ast.c)
                 │  Hierarchy
                 ▼
     [ Scoped Symbol Table ]     Symbol Table (symtab.c)
                 │  Scope Resolution
                 ▼
     [   Semantic Checks   ]     Type Checker (semantic.c, typecheck.c)
                 │  Validated AST
                 ▼
     [    TAC Generator    ]     Code Generator (tac.c, tac_expr.c, tac_stmt.c)
                 │
                 ▼
     Three Address Code Output / Error Diagnostics
```

---

## Project Structure

```text
Multi-Language-Compiler/
├── docs/                      # Language specifications and design documents
├── src/
│   ├── lexer/                 # Flex specification (lexer.l) and token definitions (tokens.h)
│   ├── parser/                # Bison grammar (parser.y)
│   ├── ast/                   # AST node structures, constructors, printer, and optimization
│   ├── symbol_table/          # Symbol table implementation and scope management
│   ├── semantic/              # Scope checking and type validation logic
│   ├── tac/                   # Three Address Code data structures and emitters
│   └── main.c                 # Compiler entry point and CLI pipeline driver
├── web/
│   └── index.html             # Local Web IDE interface
├── server.py                  # Local Python server for running the web compiler
├── tests/
│   ├── valid/                 # Valid programs for nested scopes and arithmetic
│   └── invalid/               # Test cases for lexical, syntax, and semantic errors
├── examples/                  # Sample C, C++, Java, and specification test files
├── Makefile                   # Build automation configuration
└── README.md                  # Project documentation
```

---

## Build Instructions

### Prerequisites
- GCC (GNU Compiler Collection / MinGW)
- Flex (Fast Lexical Analyzer Generator)
- Bison (GNU Parser Generator)
- GNU Make

### Compilation
To compile the project from source:
```bash
make
```

To clean generated binaries and intermediate build files:
```bash
make clean
```

---

## Usage

### Command-Line Interface
The compiler accepts the source file path and an optional language target flag:

```bash
# Compile a C source file:
./compiler.exe --lang=c examples/sample_c.c

# Compile a C++ source file:
./compiler.exe --lang=cpp examples/sample_cpp.cpp

# Compile a Java source file:
./compiler.exe --lang=java examples/sample_java.java
```

If executed without arguments, the compiler enters interactive mode:
```bash
./compiler.exe
```

### Local Web Interface
To run the compiler through a browser-based interface:
```bash
python server.py
```
This starts a local server on port 5000 and opens `http://localhost:5000`, providing an editor, language selection, and side-by-side visualization of TAC, AST, Symbol Table, and diagnostics.

---

## Authors

- **Halima Jahan Chowdhury** ([@halimajahanchowdhury](https://github.com/halimajahanchowdhury))
- **Fatema Jannat Ema** ([@fatemajannatema](https://github.com/fatemajannatema))

Department of Computer Science and Engineering  
Metropolitan University, Bangladesh
