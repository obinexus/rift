# RIFT Usage Guide - RiftLang (.rf) Compiler

**Version**: 1.0.0  
**Date**: May 28, 2026  
**Status**: Complete Developer Handbook  
**Audience**: RIFT Developers and End Users

---

## Quick Overview

RIFT is a **Flexible Translator** that compiles RiftLang (`.rf`) files to **9 target languages**:
- **C, C++, Go, Python, Rust, Java, JavaScript, Lua, WebAssembly**

A single `.rf` file becomes production-ready code in all target languages with:
- ✓ Full pattern validation
- ✓ Type constraints
- ✓ Semantic validation
- ✓ Comprehensive testing
- ✓ Performance optimization

---

## Table of Contents

1. [Quick Start](#quick-start) - Get running in 5 minutes
2. [Installation and Setup](#installation-and-setup) - Full environment setup
3. [Creating Your First .rf File](#creating-your-first-rf-file) - Basic structure
4. [Language Syntax](#language-syntax) - Complete syntax reference
5. [CLI Commands](#cli-commands) - All available commands
6. [Compilation Workflow](#compilation-workflow) - Step-by-step examples
7. [Testing Framework](#testing-framework) - Unit, property-based, and benchmarks
8. [Performance Optimization](#performance-optimization) - Built-in optimizations
9. [Multi-Language Compilation](#multi-language-compilation) - Generate all targets
10. [Best Practices](#best-practices) - Design recommendations
11. [Troubleshooting](#troubleshooting) - Common issues and solutions

---

## Quick Start

### 5-Minute Getting Started

```bash
# 1. Set up development environment
cd /path/to/RIFT
./tools/devsetup-enhanced.sh

# 2. Create your first .rf file
cat > examples/hello.rf << 'EOF'
package patterns.hello;

pattern GREETING = /hello|hi|hey/;
pattern NAME = /[a-zA-Z]+/;

fn greet(message: string) -> bool {
    match message {
        GREETING => true,
        _ => false
    }
}

test greet {
    config { framework: "unit" }
    assert greet("hello") == true;
    assert greet("invalid") == false;
}
EOF

# 3. Syntax check
rift check examples/hello.rf

# 4. Run tests
rift test examples/hello.rf

# 5. Compile to C
rift compile examples/hello.rf --target c --output examples/generated/

# 6. Compile to all targets
rift compile examples/hello.rf --targets all --optimize all --output examples/generated/

# 7. View generated code
cat examples/generated/hello.h
cat examples/generated/hello.c
```

---

## Installation and Setup

### Prerequisites

- **Operating System**: Linux (Debian/RedHat/Arch), macOS, or Windows (WSL2)
- **Build Tools**: CMake 3.14+, GCC 7+ or Clang 6+
- **Languages**: For compilation to specific targets, install:
  - C/C++: GCC/Clang (automatic via devsetup.sh)
  - Go: Go 1.16+
  - Python: Python 3.7+
  - Rust: Rust 1.50+ (installed via rustup)
  - Java: JDK 11+

### Step 1: Clone the Repository

```bash
git clone https://github.com/OBINexus/RIFT.git
cd RIFT
```

### Step 2: Run Setup Script

```bash
# Make setup script executable
chmod +x tools/devsetup-enhanced.sh

# Run setup (installs dependencies, configures build, runs initial tests)
./tools/devsetup-enhanced.sh
```

The setup script will:
- ✓ Detect your OS and distribution
- ✓ Install all required dependencies
- ✓ Verify CMake and compiler versions
- ✓ Configure parallel builds (using all available CPU cores)
- ✓ Create build directories (debug, release, coverage)
- ✓ Set up git hooks for pre-commit checks
- ✓ Configure code formatting with clang-format
- ✓ Set environment variables

### Step 3: Verify Installation

```bash
# Check RIFT version
rift --version

# Run self-tests
rift test --all

# Benchmark compilation
rift bench --quick
```

---

## Creating Your First .rf File

### Basic Structure

Every RiftLang (.rf) file follows this structure:

```rift
// Comments (single line)
/* Comments (multi-line) */

// Package declaration (required, first statement)
package module.submodule;

// Imports (optional)
import patterns.email;
import validators.url;

// Pattern definitions (optional)
pattern DIGIT = /[0-9]/;
pattern DIGITS = (DIGIT)+;

// Type definitions (optional)
type UserId {
    pattern: DIGITS,
    constraints: {
        length: { min: 1, max: 10 },
        unique: true
    }
}

// Function declarations (optional)
fn validate_id(id: string) -> bool {
    match id {
        DIGITS => true,
        _ => false
    }
}

// Tests (optional but recommended)
test validate_id {
    config { framework: "unit" }
    assert validate_id("12345") == true;
    assert validate_id("abc") == false;
}
```

### Example: Email Pattern Validator

Create `examples/simple_email.rf`:

```rift
package patterns.email_simple;

// Basic patterns
pattern LETTER = /[a-zA-Z]/;
pattern DIGIT = /[0-9]/;
pattern SPECIAL = /[._\-]/;

// Character classes
pattern WORD_CHAR = LETTER | DIGIT | SPECIAL;
pattern LOCAL = WORD_CHAR+;
pattern DOMAIN = WORD_CHAR @ /\./ @ LETTER+;
pattern EMAIL = LOCAL @ /@/ @ DOMAIN;

// Type definition with constraints
type SimpleEmail {
    pattern: EMAIL,
    constraints: {
        length: { min: 5, max: 100 },
        required: true
    }
}

// Validation function
fn is_email(input: string) -> bool {
    match input {
        EMAIL => true,
        _ => false
    }
}

// Test block
test is_email {
    config {
        framework: "unit",
        timeout_ms: 1000
    }

    // Valid emails
    assert is_email("user@example.com") == true,
           "Basic email should be valid";

    assert is_email("john.doe@company.co.uk") == true,
           "Email with dots should be valid";

    // Invalid emails
    assert is_email("@example.com") == false,
           "Missing local part should be invalid";

    assert is_email("user@") == false,
           "Missing domain should be invalid";

    assert is_email("plaintext") == false,
           "No @ symbol should be invalid";
}
```

---

## Language Syntax

### Patterns

Patterns are the core building block of RiftLang.

#### Regex Literals

```rift
pattern DIGIT = /[0-9]/;
pattern WHITESPACE = /\s/;
pattern WORD = /\w/;
pattern RAW = r#"literal\string\with\\backslashes"#;
```

#### Pattern Composition

```rift
// Concatenation (implicit or @)
pattern AB = A @ B;
pattern AB2 = A B;  // Same as above

// Alternation (|)
pattern DIGIT_OR_LETTER = /[0-9]/ | /[a-zA-Z]/;

// Quantifiers
pattern DIGITS = (/[0-9]/)+ ;      // One or more
pattern OPTIONAL = (/[0-9]/)? ;    // Zero or one
pattern MULTIPLE = (/[0-9]/)* ;    // Zero or more
pattern RANGE = (/[0-9]/){3,5} ;   // 3 to 5 occurrences
pattern EXACT = (/[0-9]/){5} ;     // Exactly 5

// Grouping (parentheses)
pattern GROUPED = (A @ B) | C;
```

#### Examples

```rift
// Phone pattern
pattern AREA = (/[0-9]/){3};
pattern EXCHANGE = (/[0-9]/){3};
pattern SUBSCRIBER = (/[0-9]/){4};
pattern PHONE = /\(/ @ AREA @ /)/ @ EXCHANGE @ /-/ @ SUBSCRIBER;

// URL pattern
pattern PROTOCOL = /https?:\/\//;
pattern DOMAIN = /[a-z0-9\-]+/ @ (/\./ @ /[a-z]{2,}/)+ ;
pattern PATH = (// @ /[^?]*/)? ;
pattern QUERY = (/\?/ @ /[^#]*)? ;
pattern URL = PROTOCOL @ DOMAIN @ PATH @ QUERY;
```

### Types

Types define structured data with validation constraints.

```rift
// Simple type
type Age {
    pattern: /[0-9]+/,
    constraints: {
        range: { min: 0, max: 150 }
    }
}

// Type with multiple constraints
type Email {
    pattern: EMAIL,
    constraints: {
        length: { min: 5, max: 254 },
        regex: /^[^@]+@[^@]+\.[^@]+$/,
        unique: true,
        required: true
    }
}

// Generic type
type List<T> {
    // Generic type definition
}

type Optional<T> {
    // Optional value
}
```

### Functions

Functions perform pattern matching and transformations.

```rift
// Simple function
fn is_digit(c: string) -> bool {
    match c {
        /[0-9]/ => true,
        _ => false
    }
}

// Function with complex matching
fn extract_domain(email: string) -> string | null {
    match email {
        (EMAIL_LOCAL) @ /@/ @ (EMAIL_DOMAIN) => {
            return EMAIL_DOMAIN;
        },
        _ => {
            return null;
        }
    }
}

// Function with multiple statements
fn validate_and_normalize(email: string) -> string | null {
    if !is_email(email) {
        return null;
    }

    let normalized = email.to_lowercase();
    let trimmed = normalized.trim();

    return trimmed;
}
```

### Control Flow

```rift
// Match statement
match value {
    PATTERN1 => action1,
    PATTERN2 => action2,
    PATTERN3 @ /constraint/ => action3,
    _ => default_action
}

// If statement
if condition {
    do_something();
} else if other_condition {
    do_other_thing();
} else {
    default_action();
}

// For loop
for item in collection {
    process(item);
}

// While loop
while condition {
    do_work();
}
```

### Test Blocks

Tests validate pattern behavior and performance.

```rift
test my_pattern {
    config {
        framework: "unit",          // unit, property-based, benchmark
        timeout_ms: 5000,           // Timeout for test
        max_iterations: 1000        // For property-based tests
    }

    // Simple assertion
    assert validate_email("user@example.com") == true;

    // Assertion with message
    assert validate_email("invalid") == false,
           "Invalid email should return false";

    // Multiple assertions
    assert extract_domain("user@example.com") == "example.com";
    assert extract_domain("invalid") == null;
}

test performance_test {
    config {
        framework: "benchmark",
        iterations: 10000,
        timeout_ms: 5000,
        performance_assertions: {
            max_time_per_op_us: 100,
            max_memory_bytes: 1024,
            throughput_min_ops_per_sec: 10000
        }
    }

    for i in 0..10000 {
        validate_email("user" @ i @ "@example.com");
    }
}
```

---

## CLI Commands

### Command Reference

#### rift check
Syntax check without compilation.

```bash
# Check single file
rift check examples/email.rf

# Check directory
rift check examples/

# Check with detailed output
rift check examples/email.rf --verbose

# Check and report statistics
rift check examples/email.rf --stats
```

#### rift compile
Compile .rf files to target languages.

```bash
# Compile to C
rift compile examples/email.rf --target c --output output/

# Compile to multiple targets
rift compile examples/email.rf --targets c,cpp,go,python --output output/

# Compile all targets with optimizations
rift compile examples/email.rf --targets all --optimize all --output output/

# Compile with specific optimization passes
rift compile examples/email.rf --target rust \
  --optimize "dfa-minimization,pattern-inlining" \
  --output output/

# Compile and generate documentation
rift compile examples/email.rf --target go \
  --docs output/docs \
  --output output/

# Show compilation details
rift compile examples/email.rf --target cpp \
  --verbose \
  --output output/
```

#### rift test
Run embedded tests.

```bash
# Run unit tests
rift test examples/email.rf

# Run all test types
rift test examples/email.rf --framework all

# Run benchmarks
rift test examples/email.rf --framework benchmark

# Property-based tests with high iteration count
rift test examples/email.rf --framework property-based --max-iterations 10000

# Run tests for all .rf files in directory
rift test examples/ --recursive

# Run with detailed output
rift test examples/email.rf --verbose

# Save test results
rift test examples/email.rf --output test_results.json

# Run tests with coverage
rift test examples/email.rf --coverage --output coverage_report.html
```

#### rift doc
Generate documentation.

```bash
# Generate HTML documentation
rift doc examples/email.rf --output docs/html/

# Generate Markdown documentation
rift doc examples/email.rf --format markdown --output docs/

# Generate with API reference
rift doc examples/email.rf --include-api --output docs/

# Generate combined documentation for directory
rift doc examples/ --recursive --format html --output docs/
```

#### rift build
Build complete projects.

```bash
# Build with debug configuration
rift build --config debug

# Build with release optimization
rift build --config release --optimize all

# Build specific target
rift build --target go --config release

# Build all targets
rift build --targets all --config release

# Incremental build
rift build --incremental
```

#### rift watch
Watch for changes and rebuild.

```bash
# Watch and rebuild on changes
rift watch examples/ --target c

# Watch and run tests
rift watch examples/ --test

# Watch with custom interval
rift watch examples/ --target go --interval 500ms
```

#### rift bench
Performance benchmarking.

```bash
# Quick benchmark
rift bench --quick

# Full benchmark suite
rift bench --all

# Benchmark specific file
rift bench examples/email.rf

# Generate benchmark report
rift bench --report benchmark_results.html

# Compare with previous benchmark
rift bench --compare baseline_results.json
```

---

## Compilation Workflow

### Step-by-Step Compilation

#### 1. Write RiftLang Pattern

Create `examples/url_validator.rf`:

```rift
package patterns.url;

pattern PROTOCOL = /https?:\/\//;
pattern DOMAIN = /[a-z0-9\-]+/;
pattern TLD = /[a-z]{2,6}/;
pattern URL = PROTOCOL @ DOMAIN @ (/\./ @ DOMAIN)* @ (/\./ @ TLD);

fn is_valid_url(input: string) -> bool {
    match input {
        URL => true,
        _ => false
    }
}

test is_valid_url {
    config { framework: "unit" }
    assert is_valid_url("https://example.com") == true;
    assert is_valid_url("http://sub.example.co.uk") == true;
    assert is_valid_url("invalid") == false;
}
```

#### 2. Syntax Check

```bash
$ rift check examples/url_validator.rf
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Syntax Check: examples/url_validator.rf
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✓ Lexical analysis: PASSED
✓ Pattern parsing: PASSED
✓ Type definitions: PASSED
✓ Function signatures: PASSED
✓ Test declarations: PASSED

File is syntactically valid.
Lines: 28, Patterns: 5, Functions: 1, Tests: 1
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

#### 3. Run Tests

```bash
$ rift test examples/url_validator.rf
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Test Results: examples/url_validator.rf
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✓ is_valid_url (unit): PASSED (3 assertions)

Summary:
  Assertions: 3/3 passed
  Time: 42ms
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

#### 4. Compile to Single Target

```bash
$ rift compile examples/url_validator.rf --target c --output generated/
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Compilation: url_validator.rf → C
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✓ Tokenization (RIFT-0): 128 tokens
✓ Parsing (RIFT-1): AST generated
✓ Semantic analysis: Type checking passed
✓ Optimization: DFA minimization (45% reduction)
✓ Code generation: C target
  - url_validator.h (850 bytes)
  - url_validator.c (2.3 KB)

Generated files:
  generated/url_validator.h
  generated/url_validator.c

Compilation time: 128ms
Target size: 3.2 KB
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

#### 5. Compile to All Targets

```bash
$ rift compile examples/url_validator.rf --targets all --optimize all --output generated/
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Compilation: url_validator.rf → All Targets
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Targets:   C, C++, Go, Python, Rust, Java, JS, Lua, WASM
Optimizations: All (4 passes)

Progress:
  [████████████████████████████████████████] 100%

Results:
✓ C:           url_validator.h/c (3.2 KB)
✓ C++:         url_validator.hpp/cpp (4.1 KB)
✓ Go:          url_validator.go (5.2 KB)
✓ Python:      url_validator.py (3.8 KB)
✓ Rust:        url_validator.rs (6.4 KB)
✓ Java:        UrlValidator.java (7.1 KB)
✓ JavaScript:  url_validator.js (4.3 KB)
✓ Lua:         url_validator.lua (2.9 KB)
✓ WebAssembly: url_validator.wasm (12 KB)

Total output: 48.9 KB
Compilation time: 1.2s
Average target size: 5.4 KB
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

#### 6. Use Generated Code

**C Example:**
```c
#include "url_validator.h"
#include <stdio.h>

int main() {
    const char* url = "https://example.com";
    if (validate_url(url)) {
        printf("Valid URL: %s\n", url);
    }
    return 0;
}
```

**Python Example:**
```python
from url_validator import validate_url

url = "https://example.com"
if validate_url(url):
    print(f"Valid URL: {url}")
```

**Go Example:**
```go
package main

import (
    "fmt"
    "urlvalidator"
)

func main() {
    url := "https://example.com"
    if urlvalidator.ValidateURL(url) {
        fmt.Printf("Valid URL: %s\n", url)
    }
}
```

---

## Testing Framework

### Test Types

#### Unit Tests

```rift
test validate_email {
    config {
        framework: "unit",
        timeout_ms: 1000
    }

    // Simple assertions
    assert validate_email("user@example.com") == true;
    assert validate_email("invalid") == false;

    // Assertions with messages
    assert validate_email("a@b.co") == true,
           "Minimal email should be valid";

    // Multiple conditions
    let email = "john.doe@company.com";
    let valid = validate_email(email);
    assert valid == true;
    assert extract_domain(email) == "company.com";
}
```

#### Property-Based Tests

```rift
test email_property_based {
    config {
        framework: "property-based",
        max_iterations: 1000,
        seed: 42  // For reproducibility
    }

    // Generate and test random valid emails
    property "valid emails parse consistently" {
        for i in 0..1000 {
            let email = generate_email();
            assert validate_email(email) == true;
        }
    }

    // Roundtrip test
    property "extract and reconstruct preserves data" {
        for i in 0..1000 {
            let original = generate_email();
            let local = extract_local_part(original);
            let domain = extract_domain_part(original);
            let reconstructed = local @ "@" @ domain;
            assert reconstructed == original;
        }
    }
}
```

#### Performance Tests

```rift
test email_performance {
    config {
        framework: "benchmark",
        iterations: 10000,
        timeout_ms: 5000,
        performance_assertions: {
            max_time_per_op_us: 100,
            max_memory_bytes: 1024,
            throughput_min_ops_per_sec: 10000
        }
    }

    for i in 0..10000 {
        validate_email("user" @ i @ "@example.com");
    }
}
```

### Running Tests

```bash
# Run all tests in file
rift test examples/email.rf

# Run specific test
rift test examples/email.rf --test validate_email

# Run with output
rift test examples/email.rf --verbose --output results.json

# Run with coverage
rift test examples/email.rf --coverage

# Run benchmarks only
rift test examples/email.rf --framework benchmark

# Run property-based tests with high iteration
rift test examples/email.rf --framework property-based --max-iterations 10000
```

---

## Performance Optimization

### Built-in Optimization Passes

RIFT automatically applies 4 optimization passes during compilation:

#### 1. Constant Folding

```rift
// Before optimization
pattern YEAR = (/[1-9][0-9]{3}/);

// After constant folding
pattern YEAR = /[1-9][0-9]{3}/;
```

#### 2. DFA Minimization (Hopcroft Algorithm)

```rift
// Complex patterns with redundant states
pattern DATE = (/[0-3][0-9]/ @ /-/ @ /[0-1][0-9]/ @ /-/ @ /[1-9][0-9]{3}/);

// Minimized to efficient DFA
```

#### 3. Pattern Inlining

```rift
// Before inlining
pattern DIGIT = /[0-9]/;
pattern DIGITS = (DIGIT)+;

// After inlining
pattern DIGITS = (/[0-9]/)+;
```

#### 4. Code Generation Optimization

- SIMD where applicable (C/C++)
- Loop unrolling for quantifiers
- Fast-path code generation
- Inline pattern matching

### Enabling/Disabling Optimizations

```bash
# Enable all optimizations (default)
rift compile examples/email.rf --target c --optimize all

# Enable specific passes
rift compile examples/email.rf --target c \
  --optimize "dfa-minimization,pattern-inlining"

# Disable optimization (for debugging)
rift compile examples/email.rf --target c --optimize none

# Show optimization report
rift compile examples/email.rf --target c \
  --optimize all \
  --optimization-report
```

### Performance Profiling

```bash
# Run performance test
rift test examples/email.rf --framework benchmark \
  --verbose \
  --output benchmark.json

# Generate performance report
rift bench --report performance_summary.html

# Compare with baseline
rift bench --compare baseline.json --current current.json
```

---

## Multi-Language Compilation

### Compilation to All 9 Targets

#### 1. Compile Command

```bash
rift compile examples/email.rf --targets all --optimize all --output generated/
```

#### 2. Target-Specific Features

| Target | Feature | Example |
|--------|---------|---------|
| **C** | POSIX regex, memory management | Manual `malloc`/`free` |
| **C++** | `std::regex`, RAII, exceptions | Automatic cleanup |
| **Go** | Goroutine-safe, error handling | Goroutines, channels |
| **Python** | LRU caching, decorators | `@functools.lru_cache` |
| **Rust** | Zero-copy, memory safety | Lifetime, borrowing |
| **Java** | JIT optimization, pattern caching | `HashMap<String, Pattern>` |
| **JavaScript** | Async support, TypeScript defs | `async/await`, `.d.ts` |
| **Lua** | Minimal memory, table-based | Tables, coroutines |
| **WebAssembly** | Binary execution, fast | `.wasm` modules |

#### 3. Using Generated Code Across Languages

**C:**
```c
#include "email_validator.h"
bool is_valid = validate_email("user@example.com");
```

**C++:**
```cpp
#include "email_validator.hpp"
bool is_valid = rift::patterns::email::EmailValidator::get_instance()
                    .validate("user@example.com");
```

**Go:**
```go
import "emailvalidator"
isValid := emailvalidator.ValidateEmail("user@example.com")
```

**Python:**
```python
from email_validator import validate_email
is_valid = validate_email("user@example.com")
```

**Rust:**
```rust
use email_validator::validate_email;
let is_valid = validate_email("user@example.com");
```

**Java:**
```java
import patterns.email.EmailValidator;
boolean isValid = EmailValidator.getInstance()
                      .validate("user@example.com");
```

**JavaScript:**
```javascript
import { validateEmail } from './email_validator.js';
const isValid = validateEmail("user@example.com");
```

**Lua:**
```lua
local email_validator = require("email_validator")
local is_valid = email_validator.validate_email("user@example.com")
```

**WebAssembly:**
```javascript
import init, { validate_email } from './email_validator.js';

init().then(() => {
    const isValid = validate_email("user@example.com");
    console.log("Valid:", isValid);
});
```

---

## Best Practices

### 1. Pattern Design

```rift
// ✓ GOOD: Clear, composable patterns
pattern DIGIT = /[0-9]/;
pattern DIGITS = (DIGIT)+;
pattern PHONE = /\(/ @ DIGITS @ /)/ @ DIGITS @ DIGITS;

// ✗ AVOID: Complex monolithic patterns
pattern PHONE = /\(\d+\)\d{3}\d{4}/;
```

### 2. Type Constraints

```rift
// ✓ GOOD: Comprehensive constraints
type Email {
    pattern: EMAIL,
    constraints: {
        length: { min: 5, max: 254 },
        regex: /^[^@]+@[^@]+\.[^@]+$/,
        unique: true,
        required: true
    }
}

// ✗ AVOID: Missing constraints
type Email {
    pattern: EMAIL
}
```

### 3. Function Design

```rift
// ✓ GOOD: Clear separation, descriptive names
fn is_valid_email(input: string) -> bool {
    match input {
        EMAIL => true,
        _ => false
    }
}

fn extract_domain(email: string) -> string | null {
    // Implementation
}

// ✗ AVOID: Overloaded functions
fn process(x: string) -> bool | string | null {
    // Unclear return type
}
```

### 4. Test Coverage

```rift
test validation {
    config { framework: "unit" }

    // ✓ GOOD: Cover valid, invalid, edge cases
    assert is_email("user@example.com") == true;     // Valid
    assert is_email("@example.com") == false;        // Missing local
    assert is_email("a@b.co") == true;               // Minimal valid
    assert is_email("") == false;                    // Empty string
    assert is_email(" user@example.com ") == false;  // Whitespace
}
```

### 5. Performance Optimization

```rift
// ✓ GOOD: Fast-path validation
fn validate_email(email: string) -> bool {
    // Length check first (O(1))
    if len(email) < 5 || len(email) > 254 {
        return false;
    }
    // @ count check (O(n), short-circuits)
    if email.count("@") != 1 {
        return false;
    }
    // Regex only if other checks pass
    return EMAIL.match(email);
}

// ✗ AVOID: Immediate regex matching
fn validate_email_bad(email: string) -> bool {
    return EMAIL.match(email);
}
```

---

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Compilation Fails - "Unknown Pattern"

**Error:**
```
error: undefined pattern 'UNKNOWN' at line 42
```

**Solution:**
```rift
// Ensure pattern is defined before use
pattern EMAIL = LOCAL @ /@/ @ DOMAIN;  // Must define DOMAIN first

fn validate(e: string) -> bool {
    match e {
        EMAIL => true,  // EMAIL must be defined above
        _ => false
    }
}
```

#### Issue 2: Test Timeout

**Error:**
```
error: test timeout after 1000ms
```

**Solution:**
```rift
test long_running {
    config {
        framework: "benchmark",
        timeout_ms: 10000  // Increase timeout
    }
    // Implementation
}
```

#### Issue 3: Memory Constraints

**Error:**
```
error: regex memory exceeds 1MB limit
```

**Solution:**
```rift
// Simplify complex patterns
pattern COMPLEX = SIMPLE1 | SIMPLE2 | SIMPLE3;  // Avoid large alternations
```

#### Issue 4: Generated Code Size Too Large

**Solution:**
```bash
# Enable aggressive optimizations
rift compile examples/large.rf --target c \
  --optimize all \
  --inline-aggressive \
  --output minimal/
```

### Debugging Commands

```bash
# Verbose compilation output
rift compile examples/email.rf --target c --verbose

# Show AST
rift compile examples/email.rf --target c --show-ast

# Show optimization report
rift compile examples/email.rf --target c --optimization-report

# Profile compilation
rift compile examples/email.rf --target c --profile

# Validate patterns
rift validate examples/email.rf --patterns

# Dry-run (no output)
rift compile examples/email.rf --target c --dry-run
```

### Getting Help

```bash
# View help for specific command
rift compile --help
rift test --help
rift check --help

# View version and system info
rift --version
rift --system-info

# Report issue
rift --report-issue
```

---

## Next Steps

1. **Explore Examples**: Review `examples/` directory for more patterns
2. **Read Specifications**: Check RIFTLANG_SPECIFICATION.md for detailed syntax
3. **Try Compilation**: Compile examples to your target language
4. **Write Tests**: Add comprehensive tests for your patterns
5. **Optimize**: Use profiling tools to optimize performance
6. **Integrate**: Embed generated code in your project

---

## Resources

- **Official Documentation**: [docs/](../docs/)
- **API Reference**: [docs/api/](../docs/api/)
- **Examples**: [examples/](../examples/)
- **Specifications**: [RIFTLANG_SPECIFICATION.md](../RIFTLANG_SPECIFICATION.md)
- **Token Model**: [RIFT_TOKEN_MODEL.md](../RIFT_TOKEN_MODEL.md)
- **Translation Pipeline**: [RIFT_TRANSLATION.md](../RIFT_TRANSLATION.md)

---

**Document Created**: May 28, 2026  
**Version**: 1.0.0  
**Status**: Complete and Ready for Use  
**Audience**: RIFT Developers
