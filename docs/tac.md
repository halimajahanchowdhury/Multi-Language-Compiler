# Intermediate Representation: Three-Address Code (TAC)

This document specifies the Three-Address Code (TAC) intermediate representation used by the compiler middle-end.

---

## 1. Instruction Set Format

Each TAC instruction represents at most one elementary operation in the format:
`result = arg1 op arg2`

Supported Quadruple Types:
* **Binary Arithmetic**: `t1 = a + b`, `t2 = t1 * c`
* **Relational / Comparison**: `t1 = a > b`, `t2 = x == y`
* **Assignments**: `x = 10`, `y = t1`
* **Unconditional Jumps**: `goto L1`
* **Conditional Jumps**: `if_false t1 goto L2`
* **Labels**: `L1:`, `L2:`
* **I/O Operations**: `print x`, `print "Hello"`

---

## 2. Temporary Register & Label Management
* **Temporaries (`t1`, `t2`, ...)**: Emitted monotonically for sub-expression evaluation.
* **Labels (`L1`, `L2`, ...)**: Generated for control-flow branching (`if-else`, loops).
