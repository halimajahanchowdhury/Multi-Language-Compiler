# Multi-Language Compiler

A modular compiler front-end and intermediate code generator implemented in C using Flex and Bison. The compiler parses and translates imperative source programs written in subsets of C, C++, and Java into linear Three Address Code (TAC). 

Developed for the Compiler Construction Laboratory course at the Department of Computer Science and Engineering, Metropolitan University, Bangladesh.

---

## 1. Project Overview

The compiler implements a full front-end pipeline that verifies lexical, syntactic, and semantic correctness before emitting optimized Three Address Code.

### Core Capabilities
- **Multi-Language Support**: Recognizes imperative constructs across C, C++, and Java, including distinct I/O mechanisms (`printf`, `cout`/`cin`, and `System.out.println`) and standard program entry structures (`int main()` and `public class Main`).
- **Lexical Analysis (Flex)**: Tokenizes language keywords, identifiers, numeric literals, boolean constants, operators, and delimiters, discarding single-line and multi-line comments.
- **Syntax Analysis (Bison)**: Enforces an unambiguous Context-Free Grammar with zero shift/reduce conflicts, supporting operator precedence and error recovery.
- **Abstract Syntax Tree (AST)**: Constructs an in-memory hierarchical syntax tree with structured traversal utilities and indented visualization.
- **Scoped Symbol Table**: Manages nested block scopes (`{ ... }`) using a hash table and scope stack, enforcing identifier visibility and tracking declaration lines.
- **Semantic Analysis**: Detects undeclared identifier usage, duplicate declarations within the same scope level, and type incompatibilities across expressions and control flow conditions.
- **Intermediate Code Generation (TAC)**: Emits linear Three Address Code using sequential temporaries (`t1`, `t2`, ...) and jump labels (`L1`, `L2`, ...) for conditional and iterative control flow.
- **Compile-Time Optimization**: Includes a constant folding pass that evaluates constant binary and unary expressions at compile time.
- **Interactive Web Interface**: Provides a local browser-based IDE featuring language selection, source code editing, and side-by-side output inspection.

---

## 2. Compiler Pipeline Architecture (The 6 Phases)

```text
Source Program (.c / .cpp / .java)
                 │
                 ▼
      [ Phase 1: Lexer ]          Flex Lexical Analyzer (Tokens Table)
                 │  Token Stream
                 ▼
      [ Phase 2: Parser ]         Bison Context-Free Grammar (LALR(1))
                 │  Parse Actions
                 ▼
      [ Phase 2: AST ]            Abstract Syntax Tree (src/ast/ast.c)
                 │
                 ▼
      [ Phase 3: SymTab ]         Scoped Symbol Table & djb2 Hash Table
                 │  Scope & Type Resolution
                 ▼
      [ Phase 3: Semantic ]       Semantic Analyzer & Type Compatibility
                 │  Validated AST
                 ▼
      [ Phase 4: TAC ]            Intermediate Code (Three Address Code)
                 │  Quadruples
                 ▼
      [ Phase 5: Optimize ]       Compile-Time Constant Folding
                 │  Folded AST & IR
                 ▼
      [ Phase 6: Code Gen ]       Target Code Generation:
                 │                  • C / C++ ──> x86 Assembly (Intel syntax)
                 │                  • Java    ──> JVM Bytecode (javap -c)
                 ▼
      [ Program Execution ]       Native Execution Console (Program Output)
```

### Architectural Boundaries
In accordance with Section 6 of the course specification, compiler backend targets (such as target assembly generation, register allocation, machine code generation, and binary linking) are explicitly out of scope. The compilation pipeline formally terminates at validated Three Address Code.

---

## 3. Technology Stack and Toolchain

| Component | Tool / Standard | Purpose |
| :--- | :--- | :--- |
| Implementation Language | ISO C (C99 / C11) | Core compiler logic, AST, and symbol table |
| Lexical Analyzer Generator | Flex (v2.5+) | Token specification and lexical analysis (`lexer.l`) |
| Parser Generator | Bison (v2.4+) | Context-Free Grammar and LALR(1) parsing (`parser.y`) |
| C Compiler | GCC (MinGW / MSYS) | Compilation of generated and handwritten C sources |
| Build System | GNU Make | Build automation via `Makefile` |
| Web Runtime | Python 3 | Standard library HTTP server for local Web IDE (`server.py`) |
| Version Control | Git / GitHub | Distributed source code management |

---

## 4. Module Implementation Status

All mandatory modules (Sections 4 and 5) and two bonus features (Section 14) are fully implemented and verified.

| Module | Location | Status | Implementation Details |
| :--- | :--- | :---: | :--- |
| Lexical Analyzer | `src/lexer/` | Completed | Flex specification tokenizing C, C++, Java, and baseline keywords, operators, and literals with line and column tracking. |
| Syntax Analyzer | `src/parser/` | Completed | Bison CFG with declared operator precedence and associativity; builds with zero shift/reduce and zero reduce/reduce conflicts. |
| Abstract Syntax Tree | `src/ast/` | Completed | Dynamic node definitions (`ASTNode`), constructor helpers, hierarchical indented printer (`ast_print.c`), and recursive memory cleanup (`ast_free()`). |
| Scoped Symbol Table | `src/symbol_table/` | Completed | Nested scope stack with djb2 hash table buckets; supports scope entry/exit and variable shadowing. |
| Semantic Analyzer | `src/semantic/` | Completed | Validates undeclared variables, prevents scope leaks, detects duplicate declarations, and enforces type compatibility rules. |
| Intermediate Code (TAC) | `src/tac/` | Completed | Linear quadruple data structures emitting expressions, conditional branches (`if_false`), unconditional jumps (`goto`), and labels. |
| Constant Folding (Bonus) | `src/ast/ast_optimize.c` | Completed | Traverses AST to evaluate constant arithmetic, relational, and boolean operations at compile time. |
| Web Compiler IDE (Bonus) | `frontend/`, `server.py` | Completed | Local browser interface hosted on port 5000 with C, C++, and Java mode selection, light/dark themes, 6-phase tabs, and native execution. |

---

## 5. Project Directory Structure

```text
Multi-Language-Compiler/
├── C_Compiler/                # Dedicated C compiler (lexer, parser, driver, examples)
├── CPP_Compiler/              # Dedicated C++ compiler (streams, namespaces, driver, examples)
├── Java_Compiler/             # Dedicated Java compiler (classes, methods, driver, examples)
├── docs/                      # Formal 6-phase specifications and grammar documentation
│   ├── pipeline.md            # Complete 6-phase pipeline specification
│   ├── grammar.md             # EBNF grammar specifications for C, C++, and Java
│   ├── ast.md                 # AST node classification and hierarchy
│   ├── tac.md                 # Three-Address Code instruction set specification
│   └── target_codegen.md      # x86 Assembly and JVM Bytecode generation specification
├── src/                       # Shared compiler core engine
│   ├── lexer/                 # Shared Flex lexer specification and tokens header
│   ├── parser/                # Shared Bison LALR(1) Context-Free Grammar
│   ├── ast/                   # AST node hierarchy, constructors, and constant folding
│   ├── symbol_table/          # Scoped symbol table and djb2 hash table
│   ├── semantic/              # Scope checker and type compatibility validator
│   ├── tac/                   # Three-Address Code quadruples and label manager
│   └── main.c                 # Unified compiler driver
├── frontend/                  # Browser-based IDE with 6-phase tabs and Program Output
│   └── index.html             # Light/dark mode web interface
├── server.py                  # Local Python server handling compilation and native runs
├── tests/                     # Validation and regression test suites
│   ├── valid/                 # Positive test cases
│   └── invalid/               # Negative test cases (lexical, syntax, semantic)
├── Makefile                   # Master build script compiling all subcompilers
└── README.md                  # Comprehensive project documentation
```

---

## 6. Build Instructions

### Compilation
To build the compiler executable:
```bash
make
```
This invokes Bison to generate `parser.tab.c`, Flex to generate `lex.yy.c`, and GCC to compile the final `compiler.exe` binary.

### Clean Build Artifacts
To remove the compiled binary and generated files:
```bash
make clean
```

---

## 7. Execution and Usage

### Command-Line Execution
Run the compiler from the terminal by providing an input file path and an optional language target flag:

```bash
# Compile a C source file:
./compiler.exe --lang=c examples/sample_c.c

# Compile a C++ source file:
./compiler.exe --lang=cpp examples/sample_cpp.cpp

# Compile a Java source file:
./compiler.exe --lang=java examples/sample_java.java
```

If the language flag is omitted, the compiler automatically infers the target language from the file extension (`.c`, `.cpp`, `.java`, or `.mc`).

### Interactive Terminal Mode
Running the executable without arguments launches the interactive console selector:
```bash
./compiler.exe
```

### Local Web-Based IDE
To use the browser interface:
```bash
python server.py
```
This starts the local HTTP server and automatically opens `http://localhost:5000` in the default web browser, allowing code editing, language switching, and side-by-side inspection of generated TAC, AST, and Symbol Table snapshots.

---

## 8. Test Suite and Error Handling

The test suite in `tests/` covers both positive compilation paths and negative diagnostic cases as mandated by Section 15 of the course specification.

| Test Category | Test File | Description / Tested Rule | Expected Outcome |
| :--- | :--- | :--- | :--- |
| Valid Execution | `tests/valid/valid_arithmetic.mc` | Complex arithmetic expressions and operator precedence | Successful compilation through to TAC |
| Valid Execution | `tests/valid/valid_nested_scope.mc` | Multi-level nested block scoping and identifier visibility | Successful compilation through to TAC |
| Lexical Error | `tests/invalid/lexical_error.mc` | Unrecognized characters (`@`, `$`) | Lexical error reported with line number |
| Syntax Error | `tests/invalid/syntax_error.mc` | Missing delimiters and malformed grammar constructs | Syntax error reported with line number |
| Semantic Error | `tests/invalid/semantic_undeclared.mc` | Use of identifier prior to declaration | Undeclared identifier diagnostic |
| Semantic Error | `tests/invalid/semantic_redeclare.mc` | Duplicate variable declaration within the same scope | Redeclaration diagnostic |
| Semantic Error | `tests/invalid/semantic_scope_violation.mc` | Accessing an inner-block variable outside its block | Scope violation diagnostic |
| Semantic Error | `tests/invalid/semantic_type_mismatch.mc` | Incompatible assignment (assigning `bool` to numeric) | Type mismatch diagnostic |
| Semantic Error | `tests/invalid/semantic_invalid_expr.mc` | Applying logical operators to numeric operands | Expression type error diagnostic |

---

## 9. Authors

- **Halima Jahan Chowdhury** ([@halimajahanchowdhury](https://github.com/halimajahanchowdhury))
- **Fatema Jannat Ema** ([@fatemajannatema](https://github.com/fatemajannatema))

Department of Computer Science and Engineering  
Metropolitan University, Bangladesh
