# RIFT Token Model Specification
## Comprehensive Token Type, Value, and Memory Architecture

**Status**: OBINexus RIFT - Active Development  
**Version**: 1.0.0  
**Date**: May 28, 2026  
**Purpose**: Define the complete token lifecycle from RIFT-0 (tokenization) through RIFT-1 (parsing)

---

## Executive Summary

The RIFT Token Model defines a unified representation for symbols, keywords, operators, and identifiers flowing through the compilation pipeline. It combines:

1. **Token Type** - What kind of symbol (keyword, operator, identifier, etc.)
2. **Token Value** - The actual lexeme or literal value
3. **Token Memory** - Metadata about context, scope, type information

This model enables:
- ✅ Symbol-accurate parsing
- ✅ Semantic intent resolution
- ✅ Type-aware compilation
- ✅ Memory safety verification
- ✅ Cross-language code generation

---

## 1. Token Type Enumeration

### 1.1 Core Token Categories

```c
typedef enum {
    // Literals and Values
    TOKEN_INTEGER = 0,          // 42, 0xFF, 0b1010
    TOKEN_FLOAT = 1,            // 3.14, 1e-5
    TOKEN_STRING = 2,           // "hello", 'c'
    TOKEN_RAW_STRING = 3,       // r"raw\nstring"
    TOKEN_BOOLEAN = 4,          // true, false
    TOKEN_NULL = 5,             // null, nil, none
    
    // Identifiers and Keywords
    TOKEN_IDENTIFIER = 10,      // variable_name, functionName
    TOKEN_KEYWORD = 11,         // if, while, for, fn, pattern
    TOKEN_TYPE_KEYWORD = 12,    // int, string, bool, auto
    TOKEN_RESERVED = 13,        // (future keywords)
    
    // Operators
    TOKEN_ARITHMETIC = 20,      // +, -, *, /, %
    TOKEN_COMPARISON = 21,      // ==, !=, <, >, <=, >=
    TOKEN_LOGICAL = 22,         // &&, ||, !
    TOKEN_BITWISE = 23,         // &, |, ^, ~, <<, >>
    TOKEN_ASSIGNMENT = 24,      // =, +=, -=, *=, /=
    TOKEN_MEMBER = 25,          // ., ->, ::
    TOKEN_SCOPE = 26,           // ::, .
    TOKEN_ARROW = 27,           // ->, =>
    
    // Delimiters and Brackets
    TOKEN_LPAREN = 30,          // (
    TOKEN_RPAREN = 31,          // )
    TOKEN_LBRACE = 32,          // {
    TOKEN_RBRACE = 33,          // }
    TOKEN_LBRACKET = 34,        // [
    TOKEN_RBRACKET = 35,        // ]
    TOKEN_SEMICOLON = 36,       // ;
    TOKEN_COMMA = 37,           // ,
    TOKEN_DOT = 38,             // .
    TOKEN_COLON = 39,           // :
    TOKEN_QUESTION = 40,        // ?
    TOKEN_PIPE = 41,            // |
    TOKEN_AT = 42,              // @
    
    // Pattern-Specific
    TOKEN_PATTERN = 50,         // Regex pattern literal
    TOKEN_CAPTURE_GROUP = 51,   // (...)
    TOKEN_ALTERNATION = 52,     // |
    TOKEN_QUANTIFIER = 53,      // *, +, ?, {n,m}
    TOKEN_ANCHOR = 54,          // ^, $, \b
    TOKEN_ESCAPE = 55,          // \x, \u, \U
    
    // Special Tokens
    TOKEN_NEWLINE = 60,         // \n
    TOKEN_INDENT = 61,          // Significant whitespace
    TOKEN_DEDENT = 62,          // Dedent (Python-style)
    TOKEN_EOF = 63,             // End of file
    TOKEN_UNKNOWN = 64,         // Unknown/error token
    
    // DSL-Specific (RiftLang)
    TOKEN_RLANG_LET = 70,       // let keyword
    TOKEN_RLANG_PATTERN = 71,   // pattern keyword
    TOKEN_RLANG_FN = 72,        // fn keyword
    TOKEN_RLANG_IMPORT = 73,    // import keyword
    TOKEN_RLANG_TRANSFORM = 74, // transform keyword
} TokenType;
```

### 1.2 Token Type Semantic Classes

```c
typedef enum {
    SEMANTIC_LITERAL,       // Literal values
    SEMANTIC_IDENTIFIER,    // Names and symbols
    SEMANTIC_KEYWORD,       // Language keywords
    SEMANTIC_OPERATOR,      // Operators (any type)
    SEMANTIC_DELIMITER,     // Brackets, punctuation
    SEMANTIC_PATTERN,       // Regex/DSL patterns
    SEMANTIC_WHITESPACE,    // Whitespace/formatting
    SEMANTIC_SPECIAL,       // Special tokens (EOF, etc)
} SemanticClass;
```

---

## 2. Token Value Structure

### 2.1 Token Value Union

```c
typedef union {
    // Numeric values
    int64_t integer_value;      // -9223372036854775808 to 9223372036854775807
    double float_value;         // IEEE 754 double precision
    
    // String/Text values
    char* string_value;         // Heap-allocated string
    
    // Boolean/State
    bool boolean_value;         // true or false
    
    // Opaque pointer (for complex structures)
    void* pointer_value;        // Generic pointer
    
    // Reference to symbol table
    uint32_t symbol_id;         // Index in symbol table
} TokenValue;
```

### 2.2 Complete Token Structure

```c
typedef struct {
    // Identity
    TokenType type;             // What kind of token
    SemanticClass semantic;     // Semantic classification
    uint32_t token_id;          // Unique token identifier
    
    // Value
    TokenValue value;           // Actual value
    char* lexeme;               // Original text from source
    size_t lexeme_length;       // Length of lexeme
    
    // Position (row/column)
    uint32_t row;               // Line number (1-indexed)
    uint32_t column;            // Column number (1-indexed)
    uint32_t offset;            // Byte offset in source
    
    // Confidence
    double confidence;          // [0.0, 1.0] - Symbol confidence
    double context_confidence;  // [0.0, 1.0] - Positional confidence
    
    // Metadata
    uint32_t nesting_depth;     // Bracket nesting level
    uint16_t precedence;        // Operator precedence (if applicable)
    
    // References
    uint32_t parent_id;         // Parent token (for grouping)
    uint32_t next_id;           // Next token in stream
    uint32_t prev_id;           // Previous token in stream
    
    // Memory association (CRITICAL)
    TokenMemoryType memory_type; // How token uses memory
    uint32_t memory_scope_id;   // Scope identifier
    uint32_t memory_size;       // Bytes allocated
    bool memory_borrowed;       // Borrowed vs owned
    
    // Semantic intent
    SemanticIntent intent;      // Resolved meaning
    void* semantic_data;        // Extensible semantic info
} RIFTToken;
```

---

## 3. Token Memory Model

### 3.1 Memory Type Classification

```c
typedef enum {
    MEMORY_TYPE_LITERAL,        // Immediate value (on stack)
    MEMORY_TYPE_REFERENCE,      // Reference to symbol table
    MEMORY_TYPE_HEAP,           // Heap-allocated (string, array)
    MEMORY_TYPE_BORROWED,       // Borrowed reference
    MEMORY_TYPE_SHARED,         // Shared ownership (RC)
    MEMORY_TYPE_EXCLUSIVE,      // Exclusive ownership
    MEMORY_TYPE_STATIC,         // Static/global storage
    MEMORY_TYPE_THREAD_LOCAL,   // Thread-local storage
} TokenMemoryType;
```

### 3.2 Memory Scope Tracking

```c
typedef struct {
    uint32_t scope_id;          // Unique scope identifier
    uint32_t parent_scope;      // Parent scope ID
    TokenMemoryType memory_type; // Type of memory in this scope
    
    // Variable declarations in scope
    struct {
        uint32_t* variable_ids;
        size_t variable_count;
    } declarations;
    
    // Memory boundaries
    uint32_t memory_start;      // Start offset
    uint32_t memory_end;        // End offset
    size_t total_allocated;     // Total bytes in scope
    
    // Lifetime info
    uint32_t creation_token_id;  // Token that created scope
    uint32_t destruction_token_id; // Token that ends scope
} MemoryScope;
```

### 3.3 Type Association

```c
typedef struct {
    uint32_t token_id;          // Token this type belongs to
    
    // Base type info
    char* type_name;            // "int", "string", "custom_pattern"
    uint32_t type_size;         // Size in bytes
    bool is_signed;             // For numeric types
    bool is_mutable;            // Can be modified
    
    // Type hierarchy
    uint32_t base_type_id;      // Parent type (inheritance)
    uint32_t* trait_ids;        // Associated traits
    size_t trait_count;
    
    // Generic parameters
    uint32_t* generic_params;   // Type parameters
    size_t generic_count;
    
    // Encoding (for RIFT's sparse encoding)
    char* sparse_encoding;      // Hexadecimal sparse representation
    char* dense_encoding;       // Dense representation
} TypeAssociation;
```

---

## 4. Token Lifecycle

### 4.1 Stage 0: Source Code → RIFT-0 (Tokenization)

```
Input:  Raw source code string
        "pattern email = r"[a-z]+@[a-z]+\.[a-z]+""

Output: Token stream
        [
            RIFTToken { type: TOKEN_RLANG_PATTERN, lexeme: "pattern", ... },
            RIFTToken { type: TOKEN_IDENTIFIER, lexeme: "email", ... },
            RIFTToken { type: TOKEN_ASSIGNMENT, lexeme: "=", ... },
            RIFTToken { type: TOKEN_RAW_STRING, lexeme: r"[a-z]+...", ... },
        ]

Process:
1. Lexer scans source character-by-character
2. Recognizes token boundaries
3. Classifies token type
4. Computes confidence scores
5. Records position (row, column, offset)
6. Allocates token and value structures
```

### 4.2 Stage 1: RIFT-0 → RIFT-1 (Parsing)

```
Input:  Validated token stream from RIFT-0
        Token stream with confidence >= θ_min

Output: Abstract Syntax Tree (AST)
        PatternDefinition {
            name: "email",
            pattern: RegexNode { ... }
        }

Process:
1. Organize tokens into semantic matrix M[R×C]
2. Compute row/column confidence values
3. Apply semantic gating rules
4. Resolve query symbols (?)
5. Build AST nodes with token references
6. Validate semantic consistency
7. Track type associations
```

### 4.3 Stage 2: RIFT-1 → RIFT-2 (Compilation)

```
Input:  AST with semantic information
        Type associations and memory scopes

Output: Bytecode or target language code
        rift_bytecode or C/C++/Go/Python/etc.

Process:
1. Walk AST, visit each node
2. Resolve type information
3. Allocate memory scopes
4. Generate code for target language
5. Apply optimizations
6. Emit bytecode or source
```

---

## 5. Confidence Calculation

### 5.1 Token Confidence Formula

```
ψ(token, row, col) = α·κ(type) + β·ρ(row,col) + γ·τ(lexeme)

Where:
  κ(type) = Type confidence based on token classification
            [0.0 = unknown, 1.0 = exact match]
  
  ρ(row,col) = Positional context confidence
               [0.0 = isolated, 1.0 = well-contextualized]
  
  τ(lexeme) = Lexeme confidence based on string matching
              [0.0 = unknown, 1.0 = exact dictionary match]
  
  α, β, γ = Weights (typically 0.4, 0.3, 0.3)
  
Threshold: θ_min = 0.65 (tokens below this flagged for disambiguation)
```

### 5.2 Type Confidence Lookup

```c
double get_type_confidence(TokenType type) {
    switch(type) {
        case TOKEN_KEYWORD:
            return 0.95;  // Keywords are well-known
        case TOKEN_IDENTIFIER:
            return 0.85;  // Identifiers fairly confident
        case TOKEN_OPERATOR:
            return 0.90;  // Operators are deterministic
        case TOKEN_DELIMITER:
            return 0.99;  // Brackets/punctuation exact
        case TOKEN_UNKNOWN:
            return 0.10;  // Unknown tokens need help
        default:
            return 0.70;  // Default confidence
    }
}
```

---

## 6. Semantic Intent Resolution

### 6.1 Intent Categories

```c
typedef enum {
    INTENT_DECLARATION,     // Variable/function declaration
    INTENT_ASSIGNMENT,      // Assignment operation
    INTENT_DEFINITION,      // Pattern definition
    INTENT_INVOCATION,      // Function/macro call
    INTENT_CONDITION,       // Conditional (if/while/?)
    INTENT_ITERATION,       // Loop structure
    INTENT_TERMINATION,     // Statement end (;, })
    INTENT_GROUPING,        // Grouping ((..))
    INTENT_TRANSFORM,       // Transformation operation
    INTENT_UNKNOWN,         // Unknown intent
} SemanticIntent;
```

### 6.2 Intent Resolution Algorithm

```c
SemanticIntent resolve_intent(RIFTToken* token, RIFTToken* prev, RIFTToken* next) {
    
    // Rule 1: Keywords have fixed intent
    if (token->type == TOKEN_RLANG_PATTERN) {
        return INTENT_DEFINITION;
    }
    if (token->type == TOKEN_KEYWORD && strcmp(token->lexeme, "if") == 0) {
        return INTENT_CONDITION;
    }
    
    // Rule 2: Operators have context-dependent intent
    if (token->type == TOKEN_ASSIGNMENT) {
        if (prev && prev->type == TOKEN_IDENTIFIER) {
            return INTENT_ASSIGNMENT;  // x = ...
        }
    }
    
    // Rule 3: Query symbols (?) need context resolution
    if (token->type == TOKEN_QUESTION) {
        if (next && is_boolean_expression(next)) {
            return INTENT_CONDITION;  // x ? y : z
        } else if (prev && prev->type == TOKEN_IDENTIFIER) {
            return INTENT_QUERY_OPERATOR;
        }
    }
    
    // Rule 4: Brackets indicate grouping/invocation
    if (token->type == TOKEN_LPAREN) {
        if (prev && prev->type == TOKEN_IDENTIFIER) {
            return INTENT_INVOCATION;  // func(...)
        } else {
            return INTENT_GROUPING;   // (...)
        }
    }
    
    // Default: Unknown
    return INTENT_UNKNOWN;
}
```

---

## 7. Integration with RIFT Pipeline

### 7.1 Token → AST Bridge

```c
// Converting tokens to AST nodes while preserving metadata
typedef struct {
    RIFTToken* source_token;         // Original token
    TokenValue resolved_value;       // Computed/coerced value
    TypeAssociation* type_info;      // Type information
    MemoryScope* memory_context;     // Memory/scope info
    SemanticIntent semantic_intent;  // Resolved meaning
    double aggregate_confidence;     // Combined confidence score
} TokenToASTMapping;
```

### 7.2 Token Stream Organization

```c
typedef struct {
    RIFTToken** tokens;              // Array of tokens
    size_t token_count;              // Number of tokens
    
    // Spatial organization
    RIFTToken*** matrix;             // M[rows][cols] matrix
    size_t matrix_rows;
    size_t matrix_cols;
    
    // Metadata
    double min_confidence;           // Minimum confidence threshold
    double* row_confidences;         // Confidence per row
    double* col_confidences;         // Confidence per column
    
    // Statistics
    uint32_t total_symbols;
    uint32_t unknown_tokens;
    uint32_t ambiguous_tokens;
} TokenStream;
```

---

## 8. Memory Safety Guarantees

### 8.1 Token Lifetime

```
1. Allocation: Token created during tokenization
   - Lexeme allocated on heap
   - Value union populated
   - Position recorded
   
2. Validation: Token confidence verified
   - If confidence < θ_min, enter disambiguation
   - Type and value checked against grammar
   
3. Integration: Token incorporated into AST
   - Reference stored in AST node
   - Memory scope recorded
   - Lifetime extended to AST lifetime
   
4. Deallocation: When AST is freed
   - All token references released
   - Lexeme strings freed
   - Token structure freed
```

### 8.2 Token Reference Counting

```c
typedef struct {
    RIFTToken* token;
    uint32_t reference_count;        // Number of AST nodes referencing
    bool is_borrowed;                // Borrowed vs owned
    RIFTToken** referrers;          // Who's referencing this token
} TokenReference;
```

---

## 9. Example: Email Pattern Token Stream

### Input
```rift
pattern email = r"[a-z]+@[a-z]+\.[a-z]+"
```

### Token Stream Output

```json
[
  {
    "type": "TOKEN_RLANG_PATTERN",
    "lexeme": "pattern",
    "row": 1, "col": 1,
    "confidence": 0.99,
    "semantic_intent": "INTENT_DEFINITION",
    "memory_type": "MEMORY_TYPE_LITERAL"
  },
  {
    "type": "TOKEN_IDENTIFIER",
    "lexeme": "email",
    "row": 1, "col": 9,
    "confidence": 0.95,
    "semantic_intent": "INTENT_DECLARATION",
    "memory_type": "MEMORY_TYPE_REFERENCE",
    "type_association": {
      "type_name": "pattern",
      "is_mutable": false
    }
  },
  {
    "type": "TOKEN_ASSIGNMENT",
    "lexeme": "=",
    "row": 1, "col": 15,
    "confidence": 0.99,
    "semantic_intent": "INTENT_ASSIGNMENT",
    "memory_type": "MEMORY_TYPE_LITERAL"
  },
  {
    "type": "TOKEN_RAW_STRING",
    "lexeme": "r\"[a-z]+@[a-z]+\\.[a-z]+\"",
    "row": 1, "col": 17,
    "confidence": 0.98,
    "semantic_intent": "INTENT_DEFINITION",
    "memory_type": "MEMORY_TYPE_HEAP",
    "memory_size": 28,
    "type_association": {
      "type_name": "regex_pattern",
      "sparse_encoding": "0x1A2B3C..."
    }
  }
]
```

---

## 10. Implementation Checklist

- [ ] Define TokenType enumeration (C header)
- [ ] Implement TokenValue union and RIFTToken struct
- [ ] Create token allocation/deallocation functions
- [ ] Implement confidence calculation
- [ ] Implement semantic intent resolution
- [ ] Create token stream organization (matrix)
- [ ] Implement type association tracking
- [ ] Create memory scope management
- [ ] Implement token → AST conversion
- [ ] Write unit tests for token operations
- [ ] Document token API
- [ ] Create example token streams

---

## References

- **RIFT_TRANSLATION.md** - How tokens are used in code generation
- **RESTRUCTURE_PLAN.md** - Project organization
- **RIFTMemory_26MAY2026.txt** - Memory governance details
- **RIFTDir.txt** - Current directory structure

---

**Document Version**: 1.0.0  
**Status**: APPROVED FOR IMPLEMENTATION  
**Last Updated**: May 28, 2026
