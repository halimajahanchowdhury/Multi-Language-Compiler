# Compiler Pipeline Specification (The 6 Phases)

This document provides the formal academic specification for the 6-phase compilation pipeline implemented in the Multi-Language Compiler.

---

## High-Level Pipeline Architecture

```text
Source Code (.c / .cpp / .java)
             │
             ▼
  ┌────────────────────────────────────────┐
  │  Phase 1: Lexical Analysis (Flex)      │  ──> Token Stream <Token, Lexeme, Line, Col>
  └────────────────────────────────────────┘
             │
             ▼
  ┌────────────────────────────────────────┐
  │  Phase 2: Syntax Analysis (Bison)      │  ──> Abstract Syntax Tree (AST)
  └────────────────────────────────────────┘
             │
             ▼
  ┌────────────────────────────────────────┐
  │  Phase 3: Semantic Analysis & SymTab   │  ──> Type-Checked AST & Scope Stack
  └────────────────────────────────────────┘
             │
             ▼
  ┌────────────────────────────────────────┐
  │  Phase 4: Intermediate Code Generation │  ──> Three-Address Code (TAC) Quadruples
  └────────────────────────────────────────┘
             │
             ▼
  ┌────────────────────────────────────────┐
  │  Phase 5: Code Optimization            │  ──> Constant-Folded AST & Optimized TAC
  └────────────────────────────────────────┘
             │
             ▼
  ┌────────────────────────────────────────┐
  │  Phase 6: Target Code Generation       │  ──> C / C++: x86 Assembly (Intel syntax)
  │                                        │  ──> Java: JVM Bytecode (javap -c)
  └────────────────────────────────────────┘
```

---

## 1. Phase 1: Lexical Analysis
* **Technology**: Flex (Fast Lexical Analyzer Generator).
* **Role**: Scans raw source characters into lexical tokens, stripping whitespace and comments while tracking source locations (`line_num`, `col_num`).
* **Output Format**:
  ```text
  Line   | Token Category           | Lexeme
  --------------------------------------------------
  4      | KEYWORD (Type)           | int
  4      | IDENTIFIER/LITERAL       | a
  4      | OPERATOR                 | =
  4      | IDENTIFIER/LITERAL       | 10
  ```

---

## 2. Phase 2: Syntax Analysis
* **Technology**: Bison LALR(1) Context-Free Parser.
* **Role**: Validates syntactic correctness against context-free grammar rules, handles operator precedence, and builds the Abstract Syntax Tree.
* **Grammar Features**:
  * Function definitions and return statements.
  * Comma-separated variable declarations (`int a = 10, b = 3, c = 7;`).
  * Conditionals (`if`, `if-else`), loops (`while`, `for`), and I/O streams (`printf`, `cout`, `System.out.println`).
* **Output Format**: Indented N-ary tree representation.

---

## 3. Phase 3: Semantic Analysis & Symbol Table
* **Technology**: Scoped Symbol Table (djb2 hash table) & Recursive AST Visitor.
* **Role**:
  * Enforces declaration before use.
  * Disallows duplicate declarations within the same scope.
  * Validates type compatibility in assignments, arithmetic expressions, and conditional branches.

---

## 4. Phase 4: Intermediate Code Generation (IR)
* **Technology**: Three-Address Code (TAC) Quadruple Generator.
* **Role**: Translates high-level AST constructs into linear quadruples with temporary variables (`t1`, `t2`) and jump labels (`L1`, `L2`).
* **Example**:
  ```text
  t1 = a + b
  t2 = t1 + c
  sum = t2
  if_false sum > 50 goto L1
  ```

---

## 5. Phase 5: Code Optimization
* **Technology**: Compile-time Constant Folding.
* **Role**: Simplifies constant expressions during compilation, reducing runtime instruction overhead.
* **Example**: `int x = 10 + 20 * 2;` $\rightarrow$ folded to `int x = 50;`.

---

## 6. Phase 6: Target Code Generation
* **Role**: Synthesizes intermediate representations into executable target architectures.
* **Target Architectures**:
  * **C & C++**: Target **x86 Assembly Code** emitted in Intel syntax (`mov`, `push`, `call`, `add`).
  * **Java**: Target **Java Virtual Machine (JVM) Bytecode** instructions (`bipush`, `istore`, `iadd`, `invokevirtual`).
