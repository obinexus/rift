# RiftLang (.rf) Formal Specification
## Complete Language Definition for Pattern & Grammar Translation

**Status**: OBINexus RIFT - Official Standard  
**Version**: 1.0.0  
**Date**: May 28, 2026  
**Classification**: Language Specification

---

## Executive Summary

**RiftLang (.rf)** is a domain-specific language for defining patterns and grammars that compile to multiple target languages (C, C++, Go, Python, Rust, Java, Node.js, Lua, WebAssembly).

Key Features:
- ✅ **BNF-style grammar** for formal specifications
- ✅ **Type system** with semantic constraints
- ✅ **Pattern composition** with quantifiers and alternation
- ✅ **Multi-language compilation** (user-configurable targets)
- ✅ **Comprehensive testing** framework
- ✅ **Package system** for code organization
- ✅ **Compiler optimizations** (DFA minimization, inlining, etc.)
- ✅ **Memory safety** analysis

---

## 1. Language Grammar

### 1.1 EBNF Formal Specification

```ebnf
(* RiftLang Grammar - Extended Backus-Naur Form *)

Program         ::= PackageDecl? ImportList? DeclarationList
                    
PackageDecl     ::= 'package' Identifier ('/' Identifier)* ';'

ImportList      ::= ImportStmt+

ImportStmt      ::= 'import' StringLiteral ';'
                  | 'import' StringLiteral 'as' Identifier ';'

DeclarationList ::= Declaration+

Declaration     ::= TypeDecl
                  | PatternDecl
                  | FunctionDecl
                  | TestBlock
                  | ConstraintDecl

(* Type Declarations *)
TypeDecl        ::= 'type' Identifier '=' BaseType Constraints?

BaseType        ::= PrimitiveType
                  | PatternType
                  | CustomType
                  | GenericType

PrimitiveType   ::= 'string' | 'int' | 'float' | 'bool' | 'char'

PatternType     ::= 'pattern' '<' Identifier '>'

CustomType      ::= Identifier

GenericType     ::= Identifier '<' TypeList '>'

TypeList        ::= BaseType (',' BaseType)*

Constraints     ::= '{' ConstraintList '}'

ConstraintList  ::= Constraint (';' Constraint)*

Constraint      ::= ConstraintType ':' ConstraintExpr

ConstraintType  ::= 'length' | 'range' | 'regex' | 'unique'
                  | 'required' | 'custom'

ConstraintExpr  ::= Literal | FunctionCall | ComparisonOp

(* Pattern Declarations *)
PatternDecl     ::= 'pattern' Identifier '=' PatternExpr

PatternExpr     ::= Sequence
                  | PatternExpr '|' Sequence

Sequence        ::= Term
                  | Sequence Term

Term            ::= Factor
                  | Factor Quantifier

Factor          ::= Atom
                  | '(' PatternExpr ')'
                  | '[' CharSet ']'
                  | '[' '^' CharSet ']'

Quantifier      ::= '*' | '+' | '?' | '{' int '}'
                  | '{' int ',' '}'
                  | '{' int ',' int '}'

Atom            ::= StringLiteral
                  | Identifier
                  | CharLiteral
                  | AnchorExpr
                  | EscapeSeq

AnchorExpr      ::= '^' | '$' | '\\b' | '\\B'

EscapeSeq       ::= '\\' HexDigit+
                  | '\\u' HexDigit HexDigit HexDigit HexDigit
                  | '\\U' HexDigit HexDigit HexDigit HexDigit HexDigit HexDigit HexDigit HexDigit

CharSet         ::= CharSetItem+

CharSetItem     ::= CharRange | Identifier

CharRange       ::= CharLiteral '-' CharLiteral

(* Function Declarations *)
FunctionDecl    ::= 'fn' Identifier ParamList '->' ReturnType '{'
                      StatementList
                    '}'

ParamList       ::= '(' (Param (',' Param)*)? ')'

Param           ::= Identifier ':' BaseType

ReturnType      ::= BaseType | '()'

StatementList   ::= Statement*

Statement       ::= ExprStmt ';'
                  | VarDecl ';'
                  | MatchStmt
                  | IfStmt
                  | BlockStmt

ExprStmt        ::= Expression

VarDecl         ::= 'let' Identifier ':' BaseType '=' Expression
                  | 'let' Identifier '=' Expression

MatchStmt       ::= 'match' Expression '{' CaseList '}'

CaseList        ::= CaseExpr+

CaseExpr        ::= 'case' Pattern '=>' Expression

IfStmt          ::= 'if' Expression BlockStmt ('else' BlockStmt)?

BlockStmt       ::= '{' StatementList '}'

Expression      ::= Term
                  | Expression BinaryOp Term
                  | FunctionCall

FunctionCall    ::= Identifier '(' ArgumentList? ')'

ArgumentList    ::= Expression (',' Expression)*

(* Test Declarations *)
TestBlock       ::= 'test' StringLiteral '{'
                      TestConfig
                      TestAssertions
                    '}'

TestConfig      ::= ConfigItem+

ConfigItem      ::= 'pattern' ':' Identifier
                  | 'input' ':' StringLiteral
                  | 'expected' ':' StringLiteral
                  | 'tags' ':' '[' StringList ']'

StringList      ::= StringLiteral (',' StringLiteral)*

TestAssertions  ::= AssertionList

AssertionList   ::= Assertion+

Assertion       ::= 'assert' ':' AssertionType
                  | 'performance' ':' PerformanceConstraint
                  | 'memory' ':' MemoryConstraint

AssertionType   ::= 'matches' | '!matches' | 'equals' | 'contains'

PerformanceConstraint ::= ComparisonOp Duration

Duration        ::= int TimeUnit

TimeUnit        ::= 'µs' | 'ms' | 's'

MemoryConstraint ::= ComparisonOp MemorySize

MemorySize      ::= int MemoryUnit

MemoryUnit      ::= 'B' | 'KB' | 'MB' | 'GB'

(* Constraint Declarations *)
ConstraintDecl  ::= 'constraint' Identifier 'on' BaseType '{'
                      ConstraintBody
                    '}'

ConstraintBody  ::= ConstraintRule+

ConstraintRule  ::= ConstraintExpr '=>' 'valid' | 'invalid'

(* Literals *)
Literal         ::= StringLiteral | CharLiteral | NumericLiteral | BoolLiteral

StringLiteral   ::= '"' (~["\n])* '"'
                  | 'r"' (~["])* '"'

CharLiteral     ::= "'" (Char | EscapeSeq) "'"

NumericLiteral  ::= [0-9]+ ('.' [0-9]+)?

BoolLiteral     ::= 'true' | 'false'

Identifier      ::= [a-zA-Z_][a-zA-Z0-9_]*

BinaryOp        ::= '+' | '-' | '*' | '/' | '%'
                  | '==' | '!=' | '<' | '>' | '<=' | '>='
                  | '&&' | '||'

ComparisonOp    ::= '<' | '>' | '<=' | '>=' | '==' | '!='
```

### 1.2 PEG (Parsing Expression Grammar)

```peg
(* RiftLang PEG Grammar - Parser Generator Ready *)

Program         ← PackageDecl? ImportList? DeclarationList

PackageDecl     ← 'package' _ Identifier ('/' Identifier)* _ ';'

ImportList      ← ImportStmt+

ImportStmt      ← 'import' _ StringLiteral (_ 'as' _ Identifier)? _ ';'

DeclarationList ← Declaration*

Declaration     ← TypeDecl / PatternDecl / FunctionDecl / TestBlock / ConstraintDecl

(* Type Declarations *)
TypeDecl        ← 'type' _ Identifier _ '=' _ BaseType Constraints?

BaseType        ← PrimitiveType / PatternType / GenericType / CustomType

PrimitiveType   ← ('string' / 'int' / 'float' / 'bool' / 'char') !IdentChar

PatternType     ← 'pattern' _ '<' _ Identifier _ '>'

GenericType     ← Identifier _ '<' _ TypeList _ '>'

CustomType      ← Identifier

TypeList        ← BaseType (_ ',' _ BaseType)*

Constraints     ← _ '{' _ ConstraintList _ '}'

ConstraintList  ← Constraint (_ ';' _ Constraint)* _ ';'?

Constraint      ← ('length' / 'range' / 'regex' / 'unique' / 'required' / 'custom') _ ':' _ ConstraintExpr

ConstraintExpr  ← FunctionCall / Literal

(* Pattern Declarations *)
PatternDecl     ← 'pattern' _ Identifier _ '=' _ PatternExpr

PatternExpr     ← Sequence (_ '|' _ Sequence)*

Sequence        ← Term+

Term            ← Factor Quantifier?

Factor          ← Atom / CharClass / '(' _ PatternExpr _ ')'

Quantifier      ← '*' / '+' / '?' / '{' [0-9]+ (',' [0-9]*)? '}'

Atom            ← StringLiteral / RawString / Identifier / AnchorExpr / EscapeSeq

RawString       ← 'r"' (~'"')* '"'

AnchorExpr      ← '^' / '$' / '\\b' / '\\B'

EscapeSeq       ← '\\' [xXuU] HexDigit+

CharClass       ← '[' ('^')? CharClassItem+ ']'

CharClassItem   ← CharRange / [^\]-]

CharRange       ← . '-' .

(* Function Declarations *)
FunctionDecl    ← 'fn' _ Identifier _ ParamList _ '->' _ ReturnType _ BlockStmt

ParamList       ← '(' _ (Param (_ ',' _ Param)*)? _ ')'

Param           ← Identifier _ ':' _ BaseType

ReturnType      ← BaseType / '()'

BlockStmt       ← '{' _ StatementList _ '}'

StatementList   ← Statement*

Statement       ← (VarDecl / ExprStmt / MatchStmt / IfStmt / BlockStmt) _ ';'?

VarDecl         ← 'let' _ Identifier (_ ':' _ BaseType)? _ '=' _ Expression

ExprStmt        ← Expression

MatchStmt       ← 'match' _ Expression _ '{' _ CaseList _ '}'

CaseList        ← CaseExpr+

CaseExpr        ← _ 'case' _ Pattern _ '=>' _ Expression

IfStmt          ← 'if' _ Expression _ BlockStmt (_ 'else' _ BlockStmt)?

Expression      ← Term (_ BinaryOp _ Term)*

Term            ← FunctionCall / Identifier / Literal / '(' _ Expression _ ')'

FunctionCall    ← Identifier _ '(' _ (Expression (_ ',' _ Expression)*)? _ ')'

Pattern         ← PatternExpr / Identifier / Literal

(* Test Declarations *)
TestBlock       ← 'test' _ StringLiteral _ '{' TestContent _ '}'

TestContent     ← TestConfigItem* TestAssertion+

TestConfigItem  ← ('pattern' / 'input' / 'expected') _ ':' _ (Identifier / StringLiteral) _ ';'?

TestAssertion   ← ('assert' / 'performance' / 'memory') _ ':' _ AssertionExpr _ ';'?

AssertionExpr   ← [a-zA-Z_][a-zA-Z0-9_!]* (_ (ComparisonOp / 'matches' / 'contains') _ [a-zA-Z0-9_"'.µmsMBGB]*)?

(* Literals *)
Literal         ← StringLiteral / CharLiteral / NumericLiteral / BoolLiteral

StringLiteral   ← '"' (~["\n])* '"'

CharLiteral     ← "'" (EscapeSeq / [^']) "'"

NumericLiteral  ← [0-9]+ ('.' [0-9]+)?

BoolLiteral     ← 'true' / 'false'

Identifier      ← [a-zA-Z_] [a-zA-Z0-9_]*

IdentChar       ← [a-zA-Z0-9_]

BinaryOp        ← ('+' / '-' / '*' / '/' / '%' / '==' / '!=' / '<=' / '>=' / '<' / '>' / '&&' / '||')

ComparisonOp    ← '<' / '>' / '<=' / '>=' / '==' / '!='

_ ← ([ \t\n] / Comment)*

Comment         ← '//' (~[\n])* / '/*' (!'*/' .)* '*/'

HexDigit        ← [0-9a-fA-F]
```

---

## 2. Syntax Examples

### 2.1 Basic Pattern Definition

```rift
// Simple email pattern
pattern EMAIL = r"[a-zA-Z0-9.]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}"

// Composed pattern with type constraint
type Email = pattern<EMAIL> {
    length: <= 254;
    regex: EMAIL;
}
```

### 2.2 Pattern Composition

```rift
// Define component patterns
pattern digits = r"[0-9]+"
pattern dash = "-"
pattern word = r"[a-zA-Z_][a-zA-Z0-9_]*"

// Compose into larger pattern
pattern phone = "(" digits ")" digits dash digits

// With quantifiers
pattern hostname = word ("." word)*
pattern domain = word ("." word) ("." word)?
pattern url = "http" "s"? "://" hostname (":" digits)?
```

### 2.3 Type Definitions with Constraints

```rift
// Integer with range constraint
type Age = int {
    range: >= 0, <= 150;
}

// String with semantic constraint
type Username = string {
    length: >= 3, <= 32;
    regex: r"^[a-zA-Z0-9_]+$";
    custom: is_not_reserved_word(value);
}

// Pattern type with properties
type HTTPMethod = pattern<HTTP_METHOD> {
    values: "GET", "POST", "PUT", "DELETE", "PATCH";
}
```

### 2.4 Functions

```rift
fn validate_email(email: string) -> bool {
    match email {
        case EMAIL => true
        case _ => false
    }
}

fn extract_domain(email: string) -> string {
    let at_pos = email.find("@")
    match at_pos {
        case Some(pos) => email.substring(pos + 1)
        case None => ""
    }
}
```

### 2.5 Testing Framework

```rift
test "valid_email_simple" {
    pattern: EMAIL
    input: "user@example.com"
    expected: "user@example.com"
    assert: matches
    performance: < 1000µs
    memory: < 100B
    tags: ["email", "validation"]
}

test "invalid_email_no_domain" {
    pattern: EMAIL
    input: "user@"
    assert: !matches
    performance: < 1000µs
}

test "property_email_idempotent" {
    pattern: EMAIL
    property: "Validation is idempotent"
    assert: validate(validate(x)) == validate(x)
    iterations: 1000
}
```

### 2.6 Package System

```rift
// patterns/email.rf
package patterns/email

type Email = pattern<EMAIL_PATTERN> { ... }
pattern EMAIL_PATTERN = r"..."

fn validate_email(email: string) -> bool { ... }
```

```rift
// app/main.rf
package app

import "patterns/email.rf" as email

fn process_user_email(email_str: string) -> bool {
    email.validate_email(email_str)
}
```

### 2.7 Semantic Constraints

```rift
// Complex constraint: username must be unique and not reserved
constraint UniqueUsername on string {
    rule: is_unique_in_database(value) => valid
    rule: is_reserved_word(value) => invalid
}

constraint EmailDomainWhitelist on Email {
    rule: domain_in_whitelist(extract_domain(value)) => valid
    rule: is_business_email(value) => valid
    rule: _ => invalid
}

// Apply constraints
type Username = string {
    constraint: UniqueUsername
}

type CompanyEmail = Email {
    constraint: EmailDomainWhitelist
}
```

---

## 3. Compiler Implementation

### 3.1 Compilation Pipeline

```
.rf source
    ↓
[LEXER] → Tokens (RIFT-0)
    ↓
[PARSER] → AST (RIFT-1)
    ↓
[SEMANTIC ANALYSIS] → Validated AST
    ↓
[OPTIMIZATION] → Optimized AST
    ├─ Constant Folding
    ├─ DFA Minimization
    ├─ Pattern Inlining
    └─ Code Generation Optimization
    ↓
[CODEGEN] → Target Language
    ├─ C / C++ / Go / Python / Rust / Java / Lua / Node.js / WASM
    ↓
[TARGET COMPILER] → Binary/Library
```

### 3.2 Optimization Passes (in order)

1. **Constant Folding**
   - Evaluate constant expressions at compile-time
   - Example: `"a" | "a"` → `"a"`

2. **DFA Minimization**
   - Convert NFA to minimal DFA
   - Reduces states and transitions
   - Applies Hopcroft's algorithm

3. **Pattern Inlining**
   - Inline small patterns into uses
   - Reduces indirection
   - Improves code locality

4. **Code Generation Optimization**
   - Target-specific optimizations
   - SSE/SIMD for vectorizable patterns
   - Loop unrolling for quantifiers

### 3.3 Code Generation Targets

```
Target    Strategy              Library             Performance
C         POSIX regex           regex.h             Fast (C built-in)
C++       std::regex            <regex>             Medium (JIT in C++)
Go        regexp                regexp package      Medium
Python    re module             re                  Medium (interpreted)
Rust      regex crate           regex               Fast (compiled)
Java      java.util.regex       java.util.regex     Medium (JIT)
WebAssembly Native pattern match WASM native        Very Fast
Lua       Lua regex             Lua re              Slow (scripted)
Node.js   JavaScript regex      /pattern/           Medium (JIT)
```

---

## 4. Type System & Constraints

### 4.1 Type Hierarchy

```
Type
├── Primitive
│   ├── int
│   ├── float
│   ├── string
│   ├── bool
│   └── char
├── Pattern
│   ├── StringPattern
│   ├── RegexPattern
│   └── CustomPattern
├── Generic<T>
│   ├── List<T>
│   ├── Option<T>
│   ├── Result<T, E>
│   └── Custom<T>
└── Custom (User-defined)
```

### 4.2 Constraint Categories

```
Constraint Type    Example                     Use Case
─────────────────────────────────────────────────────────────
length            length: >= 3, <= 32         String bounds
range             range: 0..150               Numeric bounds
regex             regex: EMAIL_PATTERN        Pattern matching
unique            unique: database_check      Database constraints
required          required: true              Mandatory fields
custom            custom: validator_fn(x)     Semantic checks
semantic          depends: other_field > x    Cross-field logic
```

---

## 5. CLI & Tool Integration

### 5.1 Compilation Commands

```bash
# Compile to specific target
rift compile email.rf --target c --output email.c

# Compile to multiple targets
rift compile email.rf --targets c,cpp,go,python

# Compile with optimization
rift compile email.rf --target c --optimize size

# Generate header files
rift compile email.rf --target cpp --emit header

# Check syntax without compilation
rift check email.rf --strict

# Run tests
rift test email.rf --verbose

# Generate documentation
rift doc email.rf --format html
```

### 5.2 Build Configuration (rift.toml)

```toml
[package]
name = "email-patterns"
version = "1.0.0"
edition = "2026"

[targets]
default = ["c", "cpp", "python"]
extended = ["c", "cpp", "go", "python", "rust", "java"]
all = ["c", "cpp", "go", "python", "rust", "java", "node", "lua", "wasm"]

[optimization]
enabled = true
dfa-minimize = true
pattern-inline = true
constant-fold = true
code-gen-optimize = true

[testing]
framework = "inline"
property-based = true
performance-checks = true
```

---

## 6. Implementation Checklist

- [ ] Lexer implementation (RIFT-0)
- [ ] Parser implementation (EBNF + PEG)
- [ ] AST definition
- [ ] Type system implementation
- [ ] Constraint validator
- [ ] Constant folding optimizer
- [ ] DFA minimizer
- [ ] Pattern inliner
- [ ] C code generator
- [ ] C++ code generator
- [ ] Go code generator
- [ ] Python code generator
- [ ] Rust code generator
- [ ] Java code generator
- [ ] WebAssembly code generator
- [ ] Lua code generator
- [ ] Node.js code generator
- [ ] Test framework
- [ ] CLI interface
- [ ] Documentation generator

---

## References

- **RIFT_TOKEN_MODEL.md** - Token specification and semantics
- **RIFT_TRANSLATION.md** - Code generation details
- **RIFTMemory_26MAY2026.txt** - Grammar traversal system
- **devsetup-enhanced.sh** - Build system

---

**Document Version**: 1.0.0  
**Status**: APPROVED FOR IMPLEMENTATION  
**Last Updated**: May 28, 2026  
**Next Review**: June 28, 2026
