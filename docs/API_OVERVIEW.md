# RIFT Library Public API Overview

## Three Public Headers

The LibRift library exposes three main public headers, each with distinct responsibilities:

### 1. **`<rift/rift.h>`** → R""
**Core Regex Engine API**

The main umbrella header that includes all core components of the regex engine.

**When to use:**
- Pattern compilation and execution
- Direct regex matching operations
- Access to all internal modules (automaton, parser, compiler, engine, etc.)
- Low-level regex functionality

**Key functions:**
```c
bool librift_initialize(void);      // Initialize library
void librift_cleanup(void);         // Clean up resources
bool librift_check_version(void);   // Version checking
```

**Module hierarchy included:**
- Memory management (`core/memory/memory.h`)
- Error handling (`core/errors/regex_error.h`)
- Automata operations (`core/automaton/automaton.h`)
- Parser (`core/parser/parser.h`)
- Tokenizer (`core/tokenizer/tokenizer.h`)
- Compiler (`core/compiler/compiler.h`)
- Regex engine (`core/regex/regex.h`)
- Matching engine (`core/engine/engine.h`)
- Bytecode VM (`core/bytecode/bytecode_vm.h`)
- Configuration management (`core/config/config.h`)

---

### 2. **`<rift/rifttest.h>`** → RTEST""
**Testing and Validation Framework**

Provides comprehensive testing utilities for validating regex patterns.

**When to use:**
- Creating test cases for regex patterns
- Running test suites
- Validating pattern behavior across multiple inputs
- Assertion-based testing
- Test reporting and results

**Key types and functions:**
```c
// Test case creation and management
RTestCase rtest_case_create(const char *name, const char *pattern,
                            const char *input, bool should_match);

// Test suite management
RTestSuite *rtest_suite_create(const char *name, size_t capacity);
bool rtest_suite_add(RTestSuite *suite, RTestCase test);

// Execution
RTestResult rtest_run_case(RTestCase *test);
bool rtest_run_suite(RTestSuite *suite);

// Results and reporting
void rtest_print_results(RTestSuite *suite);

// Assertions
RTEST_ASSERT_MATCH(pattern, input)
RTEST_ASSERT_NO_MATCH(pattern, input)
```

**Features:**
- Multiple test cases per suite
- Automated matching validation
- Capture group support
- Human-readable result reporting
- Test result enumeration (PASS, FAIL, ERROR, SKIP)

---

### 3. **`<rift/riftlang.h>`** → RLANG
**Domain-Specific Language (DSL) for Pattern Definition**

Provides a high-level, readable language for defining complex regex patterns.

**When to use:**
- Defining complex patterns in readable DSL syntax
- Building pattern libraries
- Pattern composition and reuse
- Generating optimized regex from declarative specifications
- Exporting patterns to C code

**Key types and functions:**
```c
// Context and compilation
RLangContext *rlang_context_create(void);
RLangCompileResult rlang_compile(RLangContext *ctx, const char *source);
RLangCompileResult rlang_load_file(RLangContext *ctx, const char *filepath);

// Pattern execution
char *rlang_execute_pattern(RLangProgram *program, const char *pattern_name);

// Utilities
bool rlang_get_patterns(RLangProgram *program, const char ***patterns,
                       size_t *count);
bool rlang_validate(const char *source, char **error);
bool rlang_export_c(RLangProgram *program, const char *filepath);

// Memory management
void rlang_context_free(RLangContext *ctx);
void rlang_program_free(RLangProgram *program);
```

**RiftLang DSL Features:**
- **Variable bindings:** `let name = expression`
- **Pattern definitions:** `pattern name = expression`
- **Concatenation:** `expr1 @ expr2`
- **Alternation:** `expr1 | expr2`
- **Quantifiers:** `expr+`, `expr*`, `expr?`
- **Grouping:** `(expr)`
- **Raw strings:** `r"regex_string"`

---

## Architecture and Integration

```
┌─────────────────────────────────────────────────────────┐
│                 Application Code                        │
├──────────────┬──────────────┬──────────────────────────┤
│   R""        │   RTEST""    │   RLANG                  │
│   (rift.h)   │  (rifttest.h)│ (riftlang.h)            │
├──────────────┴──────────────┴──────────────────────────┤
│          Unified Library Interface                      │
│  ┌──────────────────────────────────────────────────┐  │
│  │ Core Regex Engine                                │  │
│  │ ┌────────────────────────────────────────────┐   │  │
│  │ │ Parser → AST → Compiler → Bytecode VM      │   │  │
│  │ │ Automaton → Matcher → Engine                │   │  │
│  │ └────────────────────────────────────────────┘   │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

### Typical Usage Flow

1. **Development Phase:**
   - Write patterns using RLANG (more readable)
   - Create tests using RTEST
   - Validate with test suite

2. **Validation Phase:**
   - Run RTEST suites to verify patterns
   - Check edge cases and error conditions

3. **Production Phase:**
   - Export RLANG programs to C for deployment
   - Use R"" core API for runtime matching
   - Execute with bytecode VM for performance

---

## Quick Comparison Table

| Aspect | R"" (Core) | RTEST"" (Test) | RLANG (DSL) |
|--------|-----------|---|---|
| **Purpose** | Direct regex engine | Pattern validation | Readable pattern definition |
| **Best for** | Performance, low-level | Testing, QA | Development, readability |
| **Syntax** | C API calls | C test framework | DSL language |
| **Output** | Match results | Test reports | Generated regex / C code |
| **Learning curve** | Medium | Low | Low-Medium |
| **Flexibility** | Very high | High | High |
| **Production ready** | Yes | Yes | Yes (after export) |

---

## Example: Using All Three Together

```c
#include <rift/rift.h>
#include <rift/rifttest.h>
#include <rift/riftlang.h>

int main(void) {
    librift_initialize();
    
    // Step 1: Define pattern using RLANG (readable, maintainable)
    RLangContext *ctx = rlang_context_create();
    const char *rlang_source = R"(
        pattern email = r"[a-z]+@[a-z]+\.[a-z]+"
    )";
    RLangCompileResult result = rlang_compile(ctx, rlang_source);
    char *pattern = rlang_execute_pattern(result.program, "email");
    
    // Step 2: Create tests using RTEST (validates behavior)
    RTestSuite *suite = rtest_suite_create("Email Tests", 5);
    rtest_suite_add(suite, rtest_case_create(
        "Valid email", pattern, "user@example.com", true
    ));
    
    // Step 3: Run tests using RTEST
    rtest_run_suite(suite);
    rtest_print_results(suite);
    
    // Step 4: Export pattern using RLANG for production
    rlang_export_c(result.program, "email_patterns.c");
    
    // Step 5: Use core API (R"") for runtime matching
    // (Engine module from rift.h handles actual matching)
    
    // Cleanup
    rtest_suite_free(suite);
    rlang_program_free(result.program);
    rlang_context_free(ctx);
    librift_cleanup();
    
    return 0;
}
```

---

## Build Integration

### Required Includes
```c
#include <rift/rift.h>     // Always include for core functionality
#include <rift/rifttest.h> // If using testing framework
#include <rift/riftlang.h> // If using DSL
```

### Compiler Flags
```bash
gcc -std=c11 -I/usr/local/include -L/usr/local/lib -lrift -lm
```

### CMake
```cmake
find_library(LIBRIFT rift)
target_link_libraries(myapp PUBLIC ${LIBRIFT} m)
target_include_directories(myapp PUBLIC /usr/local/include)
```

---

## API Stability and Versioning

All three APIs follow semantic versioning:

```c
// Check version at compile time
#ifndef LIBRIFT_VERSION_MAJOR
#define LIBRIFT_VERSION_MAJOR 1
#define LIBRIFT_VERSION_MINOR 0
#define LIBRIFT_VERSION_PATCH 0
#endif

// Check version at runtime
if (!librift_check_version()) {
    // Handle version mismatch
}
```

---

## Migration Guide: From C Regex to RIFT

### Before (Standard C regex)
```c
#include <regex.h>

regex_t regex;
regcomp(&regex, "[a-z]+@[a-z]+\\.[a-z]+", REG_EXTENDED);
regmatch_t match;
regexec(&regex, "user@example.com", 1, &match, 0);
regfree(&regex);
```

### After (RIFT Library)
```c
#include <rift/rift.h>

librift_initialize();
// Use engine module from rift.h for matching
// ... matching operations ...
librift_cleanup();
```

### With RLANG (Recommended)
```c
#include <rift/riftlang.h>

RLangContext *ctx = rlang_context_create();
RLangCompileResult result = rlang_compile(ctx, 
    "pattern email = r\"[a-z]+@[a-z]+\\.[a-z]+\""
);
char *pattern = rlang_execute_pattern(result.program, "email");
// Use pattern with engine
rlang_program_free(result.program);
```

---

## Performance Considerations

- **R"" (Core API):** Fastest - direct bytecode execution
- **RLANG:** Compilation overhead upfront, then fast execution
  - Export to C for production: no runtime compilation
  - Direct use: one-time compilation cost
- **RTEST"" (Testing):** Testing framework has minimal runtime overhead

**Optimization strategy:**
1. Develop and test with RLANG + RTEST
2. Export compiled patterns to C
3. Use R"" core API in production

---

## For More Information

- **Usage Guide:** See `USAGE_GUIDE.md`
- **Example:** See `examples/floyd/string_pattern_detector.c`
- **Header Documentation:** Read `include/rift.h`, `include/rifttest.h`, `include/riftlang.h`
- **Project Structure:** Explore `include/core/` for module details

---

**Last Updated:** May 28, 2026
**LibRift Version:** 1.0.0
**Status:** OBINexus - Resumed & Active Development
