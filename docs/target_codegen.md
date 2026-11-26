# Target Code Generation Specification

This document details Phase 6: converting optimized intermediate representations into target machine code and bytecode.

---

## 1. C & C++ Target: x86 Assembly Code (Intel Syntax)
* **Emitter**: GCC/G++ target code generation backend invoked in Intel syntax (`-masm=intel`).
* **Registers Used**: `eax` (accumulator / return value), `ebp` (base pointer), `esp` (stack pointer), `ecx`, `edx`.
* **Stack Frame Setup**:
  ```assembly
  push  ebp
  mov   ebp, esp
  sub   esp, <frame_size>
  call  ___main
  ```

---

## 2. Java Target: Java Virtual Machine (JVM) Bytecode
* **Emitter**: OpenJDK Java compiler & bytecode disassembler (`javac` and `javap -c`).
* **Key Bytecode Opcodes**:
  * `bipush <val>`: Push single-byte integer constant onto operand stack.
  * `istore_<n>`: Store top of stack into local variable `<n>`.
  * `iload_<n>`: Load integer from local variable `<n>` onto stack.
  * `iadd`, `isub`, `imul`, `idiv`: Arithmetic operations on operand stack.
  * `invokevirtual`: Invoke standard library methods (e.g. `PrintStream.println`).
