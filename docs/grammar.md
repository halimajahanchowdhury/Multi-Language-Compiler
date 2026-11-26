# Multi-Language Grammar Specifications

This document defines the formal grammar rules supported by the C, C++, and Java front-end compilers.

---

## 1. C Subset Grammar

```ebnf
program             ::= translation_unit
translation_unit    ::= external_decl*
external_decl       ::= include_stmt | function_def | declaration_stmt

function_def        ::= type ('main' | IDENT) '(' param_list? ')' block_stmt
param_list          ::= param (',' param)*
param               ::= type IDENT

type                ::= 'int' | 'float' | 'double' | 'char' | 'void'

block_stmt          ::= '{' stmt* '}'
stmt                ::= decl_stmt
                      | assign_stmt
                      | if_stmt
                      | while_stmt
                      | for_stmt
                      | printf_stmt
                      | return_stmt

decl_stmt           ::= type var_init (',' var_init)* ';'
var_init            ::= IDENT ('=' expr)?

assign_stmt         ::= IDENT '=' expr ';'
                      | IDENT '++' ';'
                      | IDENT '--' ';'

if_stmt             ::= 'if' '(' expr ')' stmt ('else' stmt)?
while_stmt          ::= 'while' '(' expr ')' stmt
for_stmt            ::= 'for' '(' for_init? ';' expr? ';' for_step? ')' stmt

printf_stmt         ::= 'printf' '(' expr (',' expr)* ')' ';'
return_stmt         ::= 'return' expr? ';'
```

---

## 2. C++ Subset Grammar

Extends the C grammar with stream I/O and boolean semantics:

```ebnf
cpp_external_decl   ::= 'using' 'namespace' 'std' ';'
                      | include_stmt
                      | function_def
                      | declaration_stmt

cout_stmt           ::= 'cout' '<<' cout_arg ('<<' cout_arg)* ';'
cout_arg            ::= expr | 'endl'

cin_stmt            ::= 'cin' '>>' IDENT ('>>' IDENT)* ';'
```

---

## 3. Java Subset Grammar

Enforces object-oriented class packaging and JVM standard main signatures:

```ebnf
java_program        ::= import_stmt* class_decl
class_decl          ::= ('public')? 'class' IDENT '{' method_decl* '}'

method_decl         ::= 'public' 'static' 'void' 'main' '(' 'String' '[' ']' IDENT ')' block_stmt
                      | 'public' 'static' type IDENT '(' param_list? ')' block_stmt

print_stmt          ::= 'System.out.println' '(' expr ('+' expr)* ')' ';'
                      | 'System.out.print' '(' expr ('+' expr)* ')' ';'
```
