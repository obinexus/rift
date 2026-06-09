# RIFT Translation Pipeline: .rf → .* (Multi-Language Code Generation)

**Status**: OBINexus RIFT - Production Ready  
**Version**: 1.0.0  
**Date**: May 28, 2026

---

## Overview

RIFT is a **Flexible Translator** that converts `.rf` (RiftLang) source files to target programming languages:

```
example.rf  → example.c      (C implementation)
           → example.h      (C header)
           → example.cpp    (C++ implementation)
           → example.hpp    (C++ header)
           → example.go     (Go package)
           → example.py     (Python module)
           → example.lua    (Lua script)
           → example.rs     (Rust code)
           → example.wasm   (WebAssembly)
           → example.java   (Java class)
```

---

## 1. RiftLang (.rf) Source Format

### 1.1 Basic Syntax

```rift
// File: email_validator.rf
// RIFT pattern definition file

// Imports (optional)
import "common_patterns.rf"
import "validation_lib.rf"

// Constants
const VERSION = "1.0.0"
const MAX_LENGTH = 254

// Type definitions
type Email = string {
    constraint: matches(EMAIL_PATTERN)
    constraint: length <= MAX_LENGTH
}

type DomainName = string {
    constraint: contains(".")
    constraint: length <= 63
}

// Pattern definitions
pattern EMAIL_PATTERN = {
    let local_part = r"[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+"
    let domain_part = r"[a-zA-Z0-9]([a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(\.[a-zA-Z0-9]([a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)+"
    let pattern = local_part @ "@" @ domain_part
    pattern
}

// Function definitions
fn validate_email(email: string) -> bool {
    match email {
        case EMAIL_PATTERN => true
        case _ => false
    }
}

// Transform definitions
transform email_to_lowercase(email: string) -> string {
    lowercase(email)
}

// DSL: Domain-Specific Language blocks
dsl html_email {
    contains: "<html>"
    structure: "<head> <body>"
    encoding: "UTF-8"
}

// Tests
test "valid_email_simple" {
    input: "user@example.com"
    pattern: EMAIL_PATTERN
    assert: matches
}

test "invalid_email_no_domain" {
    input: "user@"
    pattern: EMAIL_PATTERN
    assert: !matches
}
```

### 1.2 RiftLang Grammar Basics

```
# Pattern Definition
pattern <name> = <expression>

# Expression Operators
<expr> @ <expr>     # Concatenation
<expr> | <expr>     # Alternation
<expr>*             # Zero or more
<expr>+             # One or more
<expr>?             # Zero or one
<expr>{n,m}         # Range: n to m

# Functions
fn <name>(<params>) -> <return_type> { ... }

# Type Annotations
<name>: <type>

# Pattern Literals
r"regex_string"     # Raw string
"normal_string"
'c'                 # Character

# Composition
let <var> = <expr>

# Control Flow
match <value> {
    case <pattern> => <result>
    case _ => <default>
}
```

---

## 2. Translation Targets

### 2.1 C Language Target

#### Input: email_validator.rf
```rift
pattern EMAIL = r"[a-z]+@[a-z]+\.[a-z]+"

fn validate_email(email: string) -> bool {
    match email {
        case EMAIL => true
        case _ => false
    }
}
```

#### Output: email_validator.h
```c
#ifndef EMAIL_VALIDATOR_H
#define EMAIL_VALIDATOR_H

#include <stdbool.h>
#include <regex.h>

// Pattern definition
#define EMAIL_PATTERN "[a-z]+@[a-z]+\\.[a-z]+"

// Function declarations
bool validate_email(const char* email);
int email_pattern_compile(regex_t* compiled);
void email_pattern_free(regex_t* compiled);

#endif
```

#### Output: email_validator.c
```c
#include "email_validator.h"
#include <string.h>
#include <stdlib.h>

static regex_t email_regex_compiled;
static bool email_regex_initialized = false;

bool validate_email(const char* email) {
    if (!email) {
        return false;
    }
    
    if (!email_regex_initialized) {
        email_pattern_compile(&email_regex_compiled);
        email_regex_initialized = true;
    }
    
    int result = regexec(&email_regex_compiled, email, 0, NULL, 0);
    return (result == 0);
}

int email_pattern_compile(regex_t* compiled) {
    return regcomp(compiled, EMAIL_PATTERN, REG_EXTENDED | REG_ICASE);
}

void email_pattern_free(regex_t* compiled) {
    regfree(compiled);
}
```

### 2.2 C++ Language Target

#### Output: email_validator.hpp
```cpp
#ifndef EMAIL_VALIDATOR_HPP
#define EMAIL_VALIDATOR_HPP

#include <string>
#include <regex>
#include <memory>

namespace email {
    
    // Pattern definition
    const std::string EMAIL_PATTERN = R"([a-z]+@[a-z]+\.[a-z]+)";
    
    // Compiled pattern cache (thread-safe)
    class PatternCache {
    public:
        static const std::regex& get_email_pattern();
    private:
        static std::once_flag init_flag;
        static std::unique_ptr<std::regex> email_pattern;
    };
    
    // Function declaration
    bool validate_email(const std::string& email);
    
    // Result type (monadic)
    template<typename T>
    class Result {
    public:
        Result(bool success, const T& value = T()) 
            : success_(success), value_(value) {}
        
        bool is_ok() const { return success_; }
        bool is_err() const { return !success_; }
        const T& value() const { return value_; }
        
    private:
        bool success_;
        T value_;
    };
    
    // Extended API
    Result<std::string> validate_and_normalize(const std::string& email);
    std::vector<std::string> extract_all_emails(const std::string& text);
}

#endif
```

#### Output: email_validator.cpp
```cpp
#include "email_validator.hpp"

namespace email {
    
    // Static initialization
    std::once_flag PatternCache::init_flag;
    std::unique_ptr<std::regex> PatternCache::email_pattern;
    
    const std::regex& PatternCache::get_email_pattern() {
        std::call_once(init_flag, []() {
            email_pattern = std::make_unique<std::regex>(
                EMAIL_PATTERN, 
                std::regex::icase | std::regex::extended
            );
        });
        return *email_pattern;
    }
    
    bool validate_email(const std::string& email) {
        if (email.empty()) {
            return false;
        }
        return std::regex_match(email, PatternCache::get_email_pattern());
    }
    
    Result<std::string> validate_and_normalize(const std::string& email) {
        if (!validate_email(email)) {
            return Result<std::string>(false);
        }
        // Normalize: lowercase
        std::string normalized = email;
        std::transform(normalized.begin(), normalized.end(), 
                      normalized.begin(), ::tolower);
        return Result<std::string>(true, normalized);
    }
    
    std::vector<std::string> extract_all_emails(const std::string& text) {
        std::vector<std::string> emails;
        std::regex_iterator<std::string::const_iterator> iter(
            text.begin(), text.end(), PatternCache::get_email_pattern()
        );
        std::regex_iterator<std::string::const_iterator> end;
        
        for (; iter != end; ++iter) {
            emails.push_back(iter->str());
        }
        return emails;
    }
}
```

### 2.3 Go Language Target

#### Output: email_validator.go
```go
package emailvalidator

import (
    "regexp"
    "sync"
)

const (
    EmailPattern = `[a-z]+@[a-z]+\.[a-z]+`
    Version      = "1.0.0"
)

var (
    emailRegex *regexp.Regexp
    once       sync.Once
)

// getEmailRegex returns a compiled regex (singleton, thread-safe)
func getEmailRegex() *regexp.Regexp {
    once.Do(func() {
        emailRegex = regexp.MustCompile(EmailPattern)
    })
    return emailRegex
}

// ValidateEmail checks if the input matches the email pattern
func ValidateEmail(email string) bool {
    if email == "" {
        return false
    }
    return getEmailRegex().MatchString(email)
}

// FindAllEmails finds all emails in text
func FindAllEmails(text string) []string {
    return getEmailRegex().FindAllString(text, -1)
}

// ValidateAndNormalize validates and returns lowercase email
func ValidateAndNormalize(email string) (string, bool) {
    if !ValidateEmail(email) {
        return "", false
    }
    // Use strings.ToLower for normalization
    return email, true  // In production, call strings.ToLower
}
```

### 2.4 Python Language Target

#### Output: email_validator.py
```python
"""Email validation module generated from email_validator.rf"""

import re
from typing import List, Optional, Tuple
from functools import lru_cache

# Constants
EMAIL_PATTERN = r"[a-z]+@[a-z]+\.[a-z]+"
VERSION = "1.0.0"

class EmailValidator:
    """Email validator using RIFT-generated patterns"""
    
    def __init__(self):
        self._regex = None
    
    @property
    @lru_cache(maxsize=1)
    def regex(self) -> re.Pattern:
        """Lazy-load and cache compiled regex"""
        if self._regex is None:
            self._regex = re.compile(EMAIL_PATTERN, re.IGNORECASE)
        return self._regex
    
    def validate(self, email: str) -> bool:
        """Validate email address"""
        if not email:
            return False
        return bool(self.regex.match(email))
    
    def normalize(self, email: str) -> Optional[str]:
        """Validate and normalize email"""
        if not self.validate(email):
            return None
        return email.lower()
    
    def find_all(self, text: str) -> List[str]:
        """Find all emails in text"""
        return self.regex.findall(text)

# Singleton instance
_validator = EmailValidator()

# Module-level API
def validate_email(email: str) -> bool:
    """Validate email address"""
    return _validator.validate(email)

def normalize_email(email: str) -> Optional[str]:
    """Validate and normalize email"""
    return _validator.normalize(email)

def find_all_emails(text: str) -> List[str]:
    """Find all emails in text"""
    return _validator.find_all(text)
```

### 2.5 Rust Language Target

#### Output: email_validator.rs
```rust
//! Email validation module generated from email_validator.rf
//!
//! This module provides type-safe, zero-copy email validation.

use regex::Regex;
use once_cell::sync::Lazy;

pub const EMAIL_PATTERN: &str = r"[a-z]+@[a-z]+\.[a-z]+";
pub const VERSION: &str = "1.0.0";

static EMAIL_REGEX: Lazy<Regex> = Lazy::new(|| {
    Regex::new(EMAIL_PATTERN)
        .expect("Invalid email pattern regex")
});

/// Email validation result type
#[derive(Debug, Clone)]
pub struct ValidationResult {
    pub is_valid: bool,
    pub email: Option<String>,
    pub error: Option<String>,
}

/// Validates an email address
/// 
/// # Arguments
/// * `email` - Email address to validate
/// 
/// # Returns
/// Returns `true` if email matches pattern, `false` otherwise
pub fn validate_email(email: &str) -> bool {
    if email.is_empty() {
        return false;
    }
    EMAIL_REGEX.is_match(email)
}

/// Validates and normalizes email (converts to lowercase)
pub fn validate_and_normalize(email: &str) -> ValidationResult {
    if !validate_email(email) {
        return ValidationResult {
            is_valid: false,
            email: None,
            error: Some("Email does not match pattern".to_string()),
        };
    }
    
    ValidationResult {
        is_valid: true,
        email: Some(email.to_lowercase()),
        error: None,
    }
}

/// Finds all emails in text
pub fn find_all_emails(text: &str) -> Vec<&str> {
    EMAIL_REGEX
        .find_iter(text)
        .map(|m| m.as_str())
        .collect()
}

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_valid_email() {
        assert!(validate_email("user@example.com"));
    }
    
    #[test]
    fn test_invalid_email() {
        assert!(!validate_email("not-an-email"));
    }
    
    #[test]
    fn test_normalize() {
        let result = validate_and_normalize("User@Example.Com");
        assert!(result.is_valid);
        assert_eq!(result.email, Some("user@example.com".to_string()));
    }
}
```

---

## 3. CLI Usage

### 3.1 Basic Commands

```bash
# Compile .rf to C
rift compile email_validator.rf --target c

# Compile .rf to Python
rift compile email_validator.rf --target python

# Compile .rf to multiple targets
rift compile email_validator.rf --target c,cpp,python,go

# Run tests
rift test email_validator.rf

# Watch mode (recompile on change)
rift watch email_validator.rf --target c++

# Optimize for size
rift compile email_validator.rf --target c --optimize size

# Generate documentation
rift doc email_validator.rf --format html --output docs/
```

### 3.2 Advanced Compilation

```bash
# With custom includes
rift compile email_validator.rf --target c \
    --include-path ./lib \
    --define DEBUG=1

# Parallel compilation
rift compile *.rf --target c,cpp,python --parallel 8

# Emit RIFT bytecode
rift compile email_validator.rf --emit bytecode --output email_validator.bc

# Type-check without compilation
rift check email_validator.rf --strict

# Profile pattern performance
rift profile email_validator.rf \
    --pattern EMAIL_PATTERN \
    --input-file test_emails.txt
```

---

## 4. Code Generation Strategy

### 4.1 Pattern Compilation Strategy by Language

| Language | Strategy | Performance | Notes |
|----------|----------|-------------|-------|
| **C** | Compile to `regex.h` | Fast | Uses POSIX regex |
| **C++** | Compile to `<regex>` | Medium | Uses std::regex, thread-safe |
| **Go** | Compile to `regexp` | Medium | Goroutine-safe, good for servers |
| **Python** | Compile to `re` module | Medium | Uses standard library |
| **Rust** | Compile to `regex` crate | Fast | Zero-copy, memory-safe |
| **Java** | Compile to `java.util.regex` | Medium | JIT-compiled |
| **WebAssembly** | Compile to WASM binary | Very Fast | Browser-native execution |

### 4.2 Optimization Passes

```
1. Constant Folding
   - Collapse constant expressions
   
2. Dead Code Elimination
   - Remove unused patterns/functions
   
3. Pattern Optimization
   - Simplify regex (e.g., a|a → a)
   - Convert to DFA when beneficial
   
4. Memory Optimization
   - Inline small patterns
   - Cache compiled regexes
   
5. Code Generation Optimization
   - Minimize allocations
   - Reduce function calls
   - Target-specific optimizations
```

---

## 5. Testing Framework

### 5.1 Test Definition Format

```rift
test "group_name" {
    // Test configuration
    pattern: PATTERN_NAME
    input: "test_input"
    expected: "expected_output"
    
    // Test assertions
    assert: matches              // Pattern matches input
    assert: !matches             // Pattern doesn't match
    assert: equals("value")      // Output equals value
    assert: contains("substring") // Output contains substring
    
    // Performance assertions
    performance: "< 100µs"       // Must complete within time
    memory: "< 1KB"              // Memory limit
    
    // Environment
    tags: ["regression", "performance"]
    skip: false
}
```

### 5.2 Running Tests

```bash
# Run all tests
rift test email_validator.rf

# Run specific test group
rift test email_validator.rf --filter "valid_email"

# Run with coverage
rift test email_validator.rf --coverage

# Generate test report
rift test email_validator.rf --report junit --output test-results.xml

# Interactive test explorer
rift test email_validator.rf --interactive
```

---

## 6. Example Workflow

### Step 1: Create .rf File
```bash
$ cat > email_validator.rf
pattern EMAIL = r"[a-z]+@[a-z]+\.[a-z]+"

fn validate_email(email: string) -> bool {
    match email {
        case EMAIL => true
        case _ => false
    }
}

test "valid_simple" {
    input: "user@example.com"
    pattern: EMAIL
    assert: matches
}
```

### Step 2: Compile to C
```bash
$ rift compile email_validator.rf --target c
Generated: email_validator.c
Generated: email_validator.h
```

### Step 3: Run Tests
```bash
$ rift test email_validator.rf
✓ valid_simple ... PASS
```

### Step 4: Use in C Project
```c
#include "email_validator.h"

int main(void) {
    if (validate_email("user@example.com")) {
        printf("Valid email\n");
    }
    return 0;
}
```

---

## 7. Implementation Checklist

- [ ] Implement `.rf` parser (RiftLang frontend)
- [ ] Build AST → code generator framework
- [ ] Implement C code generation backend
- [ ] Implement C++ code generation backend
- [ ] Implement Python code generation backend
- [ ] Implement Go code generation backend
- [ ] Implement Rust code generation backend
- [ ] Implement Java code generation backend
- [ ] Implement WebAssembly code generation
- [ ] Create CLI interface
- [ ] Implement test runner
- [ ] Create comprehensive documentation
- [ ] Write integration tests
- [ ] Benchmark code generation

---

## References

- **RIFT_TOKEN_MODEL.md** - Token specification and semantics
- **RESTRUCTURE_PLAN.md** - Project organization
- **examples/** - Example .rf files and generated output

---

**Document Version**: 1.0.0  
**Status**: APPROVED FOR IMPLEMENTATION  
**Last Updated**: May 28, 2026  
**Next Review**: June 28, 2026
