# RIFT Stage 0: Enhanced Tokenization/Lexical Analysis

**CRITICAL: Fixed Library Architecture to Prevent Segmentation Faults**

Part of the OBINexus RIFT POC implementing AST-Aware bytecode generation with AEGIS automaton theory.

## 🚨 Library Selection (IMPORTANT)

**CHOOSE ONE LIBRARY TYPE** to avoid linker conflicts and segmentation faults:

```bash
# ✅ RECOMMENDED: Shared Library (Development)
make LIB=SO bootstrap

# ✅ ALTERNATIVE: Static Library (Distribution)  
make LIB=A bootstrap

# ⚠️  AVOID: Don't mix library types in same build
```

### Library Architecture

| Library Type | Use Case | Advantages | Command |
|--------------|----------|------------|---------|
| **Shared (.so)** | Development, Testing | Smaller executables, runtime updates | `make LIB=SO` |
| **Static (.a)** | Distribution, Embedded | Self-contained, no dependencies | `make LIB=A` |
| **Both** | Library distribution only | Generate both for users | `make LIB=BOTH lib` |

## Architecture

RIFT Stage 0 implements the **AEGIS** (Automaton Engine for Generative Interpretation & Syntax) framework using a 5-tuple automaton (Q, Σ, δ, q0, F) for tokenization.

### Core Components

1. **RegexAutomatonEngine** - Finite state machine with regex patterns
2. **Pattern Parser** - RIFT syntax: `R"pattern/flags[mode]"`  
3. **Token Generator** - Type/value separated IR tokens
4. **CLI System** - Enhanced command-line interface
5. **Library System** - Shared/static library generation
6. **Configuration System** - `.riftrc` support

### Fixed Architecture Issues

- ✅ **Multiple Definition Errors**: Moved duplicate functions to single source file
- ✅ **Library Linking Conflicts**: Proper library selection mechanism  
- ✅ **Segmentation Faults**: Fixed library loading order
- ✅ **Memory Management**: Thread-safe token creation
- ✅ **Compiler Compliance**: Zero warnings with `-Werror`

## Quick Start

```bash
# 1. Bootstrap with shared library (recommended)
make LIB=SO bootstrap

# 2. Test tokenization
./bin/rift0.exe examples/sample.rift

# 3. View generated IR
cat output/examples/sample.rift.0

# 4. Run demo
./bin/rift0_demo

# 5. Run QA tests  
make test
```

## CLI Usage

```bash
# Basic tokenization
./bin/rift0.exe input.rift

# With verbose output
./bin/rift0.exe --verbose input.rift

# With custom config
./bin/rift0.exe --config custom.riftrc input.rift

# Show help
./bin/rift0.exe --help
```

## Configuration (.riftrc)

```ini
# Enhanced RIFT Stage 0 Configuration
output_dir=./output
stage_name=rift.0
debug=false
verbose=false
lib_mode=SO
```

## Building

```bash
# Complete build with library selection
make LIB=SO all        # Shared library build
make LIB=A all         # Static library build

# Individual targets
make lib               # Build library only
make test              # Run QA tests
make config            # Generate .riftrc
make clean             # Clean artifacts
make install           # System installation

# Bootstrap (recommended)
make LIB=SO bootstrap  # Complete setup
```

## Library Integration

### Using RIFT in Your Projects

```bash
# With shared library
gcc -I./include -L./lib -lrift0 your_program.c -o your_program

# With static library  
gcc -I./include -L./lib -lrift0 -static your_program.c -o your_program
```

### Example Integration

```c
#include <rift.h>

int main() {
    // Create RIFT engine
    RiftEngine* engine = rift_engine_create();
    
    // Process input
    rift_engine_process_input(engine, "function hello() {}");
    
    // Generate tokens
    printf("Generated %zu tokens\n", engine->token_count);
    
    // Cleanup
    rift_engine_destroy(engine);
    return 0;
}
```

## RIFT Pattern Syntax

```
R"regex_pattern/flags[mode]"
```

- **regex_pattern**: POSIX extended regex
- **flags**: `g` (global), `m` (multiline), `i` (case-insensitive)  
- **mode**: `t` (top-down) or `b` (bottom-up) matching

### Examples

```c
// Top-down identifier matching
R"[a-zA-Z_][a-zA-Z0-9_]*/gi[t]"

// Bottom-up number matching  
R"[0-9]+(\.[0-9]+)?/g[b]"

// Case-insensitive keyword
R"function|class|struct/gi[t]"
```

## Directory Structure

```
rift0/
├── src/core/              # Core library implementation
│   ├── rift_automaton.c   # Automaton engine
│   ├── rift_pattern.c     # Pattern parser (fixed)
│   ├── rift_engine.c      # Token engine (fixed) 
│   ├── rift_config.c      # Configuration & utilities
│   └── main.c             # Demo program
├── src/cli/               # CLI interface
│   ├── main.c             # CLI main program
│   └── command/           # CLI commands
│       ├── config_handler.c
│       └── stage_processor.c
├── include/               # Public headers
│   └── rift.h             # Enhanced API
├── tests/qa_mocks/        # QA framework
├── examples/              # Sample files  
├── lib/                   # Generated libraries
│   ├── librift0.so        # Shared library
│   └── librift0.a         # Static library
├── bin/                   # Executables
└── output/                # IR output
```

## IR Output Format (.rift.0)

```
# RIFT Intermediate Representation
stage=rift.0
version=1.0.0
source=examples/sample.rift
token_count=42

# Tokens (Type/Value Separated)
TOKEN 0: type="CHAR" value="f" pos=(1,1)
TOKEN 1: type="CHAR" value="u" pos=(1,2)
TOKEN 2: type="CHAR" value="n" pos=(1,3)
...
```

## Compiler Compliance

Built with strict enforcement to prevent runtime errors:

```bash
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude -g -O2
```

- **Zero Warnings**: All warnings treated as errors
- **Memory Safety**: Comprehensive cleanup and validation
- **Thread Safety**: Atomic operations for token memory
- **POSIX Compliance**: Portable across Unix systems

## Error Handling

Comprehensive error system with proper resource cleanup:

| Error Code | Description | Resolution |
|------------|-------------|------------|
| `RIFT_SUCCESS` | Operation successful | - |
| `RIFT_ERROR_NULL_POINTER` | Invalid parameters | Check input validation |
| `RIFT_ERROR_MEMORY_ALLOCATION` | Out of memory | Increase system memory |
| `RIFT_ERROR_PATTERN_INVALID` | Invalid RIFT pattern | Fix pattern syntax |
| `RIFT_ERROR_COMPILATION_FAILED` | Regex compilation failed | Check regex syntax |
| `RIFT_ERROR_IO` | File I/O error | Check file permissions |

## Troubleshooting

### Common Issues

**Multiple Definition Errors**:
```bash
# ❌ Problem: Functions defined in multiple files
# ✅ Solution: Updated - functions in single source file
make clean && make LIB=SO all
```

**Segmentation Faults**:
```bash
# ❌ Problem: Library linking conflicts  
# ✅ Solution: Choose one library type
make LIB=SO bootstrap  # Use shared library only
```

**Linker Errors**:
```bash
# ❌ Problem: Missing library path
# ✅ Solution: Set LD_LIBRARY_PATH for shared libraries
export LD_LIBRARY_PATH=$(pwd)/lib:$LD_LIBRARY_PATH
./bin/rift0.exe examples/sample.rift
```

## Performance

RIFT Stage 0 achieves O(n) tokenization performance through:

- **State Minimization**: Automaton optimization  
- **Memory Pool**: Pre-allocated token storage
- **Single Pass**: Direct tokenization without backtracking
- **Zero Copy**: String references instead of duplication

## Integration Pipeline

RIFT Stage 0 outputs `.rift.0` IR files ready for:

1. **Stage 1**: Parsing/Grammar Analysis
2. **Stage 2**: Semantic Analysis  
3. **Stage 3**: Code Generation
4. **Stage 4**: Optimization

## Zero Trust Future

Planned integration with cryptographic governance:

```bash
make zero-trust-init  # Generate RSA-4096 keys
make verify-keys      # Verify key integrity
```

## OBINexus Computing

*"Structure IS the syntax."*

RIFT implements structural solutions to language engineering complexity through automaton theory, achieving deterministic behavior with comprehensive error handling and memory safety.

**Technical Achievement**: Zero-warning compilation with proper library architecture preventing segmentation faults and linker conflicts.

---

## Quick Reference

```bash
# Essential Commands
make LIB=SO bootstrap     # Complete setup (shared lib)
./bin/rift0.exe file.rift # Tokenize file
make test                 # Run QA tests
make clean                # Clean build

# Library Usage
gcc -I./include -L./lib -lrift0 program.c -o program
```