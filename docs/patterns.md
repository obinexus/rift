# LibRift Baseline Patterns

This document describes the baseline regex patterns used in the LibRift engine for core language functionality.

## Purpose

The baseline patterns serve multiple purposes in the LibRift engine:

1. Define standard states and transitions in the automaton
2. Provide a consistent way to recognize common language constructs
3. Serve as building blocks for more complex patterns
4. Enable extension and customization without modifying core functionality

## Available Patterns

| Pattern | Macro | Description |
|---------|-------|-------------|
| Empty State | `RIFT_PATTERN_EMPTY_STATE` | Matches non-alphanumeric, non-whitespace characters. Used to represent an initial/empty automaton state. |
| Whitespace | `RIFT_PATTERN_WHITESPACE` | Matches any whitespace character. Used for transitions between states where whitespace is ignored. |
| Word Boundary | `RIFT_PATTERN_WORD_BOUNDARY` | Matches a word boundary. Used for transition points in the automaton. |
| Identifier | `RIFT_PATTERN_IDENTIFIER` | Matches a valid identifier (letter or underscore followed by letters, numbers, or underscores). |
| Numeric | `RIFT_PATTERN_NUMERIC` | Matches integers and floating-point numbers, including scientific notation. |
| String | `RIFT_PATTERN_STRING` | Matches quoted strings with proper escape handling. |
| Comment | `RIFT_PATTERN_COMMENT` | Matches single-line (//) and multi-line (/* */) comments. |
| Operator | `RIFT_PATTERN_OPERATOR` | Matches common operators like +, -, *, /, =, etc. |
| Function | `RIFT_PATTERN_FUNCTION` | Matches function declarations with name and parameter list. |
| State Transition | `RIFT_PATTERN_STATE_TRANSITION` | Matches transition markers (-> or →) in the engine. |

## Usage

The baseline patterns can be used in several ways:

### Direct Usage

```c
#include "librift/core/regex/patterns/baseline_patterns.h"

// Compile a pattern directly
rift_regex_pattern_t *pattern = rift_regex_compile(RIFT_PATTERN_IDENTIFIER, 
                                                 RIFT_REGEX_FLAG_RIFT_SYNTAX, 
                                                 &error);

if (!pattern) {
    // Handle error
}
// Use the pattern for matching
rift_regex_match(pattern, input_string, &match_result);
// Cleanup
rift_regex_pattern_free(pattern);
```
### Using Precompiled Patterns
```c
#include "librift/core/regex/patterns/pattern_extensions.h"
#include "librift/core/regex/patterns/baseline_patterns.h"
#include "librift/core/regex/errors/regex_error.h"
#include "librift/core/regex/engine/pattern.h"
#include "librift/core/regex/compiler/compiler.h"
#include "librift/core/regex/errors/regex_error.h"
#include "librift/core/regex/patterns/pattern_types.h"
#include "librift/core/regex/patterns/pattern_extensions.h"                     
#include "librift/core/regex/patterns/baseline_patterns.h"
#include "librift/core/regex/errors/regex_error.h"
#include "librift/core/regex/engine/pattern.h"                  


#include "librift/core/regex/compiler/compiler.h"
```
