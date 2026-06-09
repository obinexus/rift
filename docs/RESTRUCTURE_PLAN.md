# RIFT Project Restructuring Plan
## Production-Ready Directory Organization

**Status**: OBINexus RIFT - Resumed & Active Development  
**Date**: May 28, 2026  
**Version**: 1.0.0

---

## Executive Summary

This document outlines the restructuring of the RIFT project to achieve:
- ✅ **Flat, consolidated header structure** - All `.h` files in `/include` root
- ✅ **Organized asset management** - PDFs, PNGs in `/docs` (flat structure)
- ✅ **Binary separation** - Clear distinction between binaries, libraries, and headers
- ✅ **Production-ready tooling** - Enhanced `devsetup.sh` with parallel builds
- ✅ **Token model clarity** - Explicit specification of RIFT-0 → RIFT-1 pipeline
- ✅ **Comprehensive examples** - `.rf` to `.c`, `.h`, `.cpp`, `.hpp` translations

---

## Current Issues

```
❌ Headers scattered across:
   - include/core/**/*.h
   - include/cli/**/*.h
   - include/automaton/*.h
   - include/engine/*.h
   - include/runtime/*.h
   - include/rift/*.h (duplicates)

❌ Documentation assets mixed with code:
   - docs/docs/specification/ (PDFs, duplicates)
   - docs/rifters_way/RIFT/ (PNGs, SVGs scattered)
   - docs/images/ (mixed types)

❌ No clear binary output structure
❌ devsetup.sh sequential (single-threaded builds)
❌ Token model not explicitly documented
❌ .rf → .* translation pipeline unclear
```

---

## Target Structure

```
RIFT/
├── include/                      # ✨ CONSOLIDATED HEADERS
│   ├── rift/
│   │   ├── rift.h             # Main API (R"")
│   │   ├── riftlang.h         # DSL (RLANG)
│   │   ├── rifttest.h         # Testing (RTEST"")
│   │   ├── version.h
│   │   └── platform.h
│   ├── core/                   # Core engine modules (flat namespace)
│   │   ├── automaton.h
│   │   ├── parser.h
│   │   ├── compiler.h
│   │   ├── engine.h
│   │   ├── matcher.h
│   │   ├── runtime.h
│   │   ├── bytecode.h
│   │   ├── config.h
│   │   └── memory.h
│   ├── lang/                   # Language & DSL (flat namespace)
│   │   ├── lexer.h
│   │   ├── tokenizer.h
│   │   ├── token.h
│   │   ├── token_type.h
│   │   └── syntax.h
│   ├── cli/                    # Command-line interface (flat namespace)
│   │   ├── command.h
│   │   ├── options.h
│   │   └── cli_main.h
│   └── bindings/               # Language bindings
│       ├── c.h
│       ├── go.h
│       ├── python.h
│       ├── lua.h
│       ├── node.h
│       └── wasm.h
│
├── src/                        # Source implementations
│   ├── core/
│   │   ├── automaton.c
│   │   ├── parser.c
│   │   ├── compiler.c
│   │   └── ...
│   ├── lang/
│   │   ├── lexer.c
│   │   ├── tokenizer.c
│   │   └── ...
│   ├── cli/
│   │   └── main.c
│   └── bindings/
│       ├── c.c
│       └── ...
│
├── bin/                        # ✨ BINARY OUTPUTS
│   ├── rift                    # Main executable (rift lang, rift test, rift)
│   ├── rift-lang               # DSL compiler
│   ├── rift-test               # Test runner
│   └── librift.a / librift.so   # Compiled library
│
├── lib/                        # ✨ LIBRARY OUTPUTS
│   ├── librift.a
│   ├── librift.so
│   ├── libriftlang.a
│   └── librifttest.a
│
├── docs/                       # ✨ CONSOLIDATED DOCUMENTATION
│   ├── assets/                 # Flat structure for all media
│   │   ├── diagrams/
│   │   │   ├── rift-automaton.svg
│   │   │   ├── rift-pipeline.png
│   │   │   └── ...
│   │   ├── specs/
│   │   │   ├── rift-token-spec.pdf
│   │   │   ├── riftlang-impl.pdf
│   │   │   └── ...
│   │   └── images/
│   │       ├── architecture.png
│   │       └── ...
│   ├── api/
│   │   ├── core.md
│   │   ├── riftlang.md
│   │   └── rifttest.md
│   ├── guides/
│   │   ├── getting-started.md
│   │   ├── token-model.md
│   │   └── translation-pipeline.md
│   ├── examples/
│   │   ├── .rf-to-.c.example
│   │   ├── .rf-to-.h.example
│   │   ├── .rf-to-.cpp.example
│   │   └── .rf-to-.hpp.example
│   └── ARCHITECTURE.md
│
├── examples/                   # Example programs
│   ├── hello.rf
│   ├── email-validator.rf
│   ├── pattern-compiler.rf
│   └── quantum-bridge.rf
│
├── tests/                      # Test suite
│   ├── unit/
│   ├── integration/
│   └── fixtures/
│
├── tools/                      # Development tools
│   ├── devsetup.sh             # Enhanced, parallel builds
│   ├── format.sh               # Code formatting
│   ├── lint.sh                 # Static analysis
│   └── coverage.sh             # Code coverage
│
├── build/                      # Build outputs (not committed)
│   ├── debug/
│   ├── release/
│   └── coverage/
│
├── CMakeLists.txt              # Main CMake configuration
├── Makefile                    # Top-level Makefile
├── .clang-format               # Code style
├── .cppcheck_suppressions      # Static analysis
├── .gitignore                  # Git ignore rules
├── CMakeCache.txt              # Build cache
├── CHANGELOG.md                # Version history
├── RIFT_TOKEN_MODEL.md         # ✨ NEW: Token specification
├── RIFT_TRANSLATION.md         # ✨ NEW: .rf translation guide
└── README.md                   # Project overview
```

---

## Phase 1: Header Consolidation

### Step 1.1 - Flatten Core Headers
**Move from**: `include/core/**/*.h` → `include/core/`

```bash
# Automaton module
include/core/automaton/*.h → include/core/automaton.h (merged)

# Parser module  
include/core/parser/*.h → include/core/parser.h (merged)

# Compiler module
include/core/compiler/*.h → include/core/compiler.h (merged)

# Engine module
include/core/engine/*.h → include/core/engine.h (merged)

# Runtime module
include/core/runtime/*.h → include/core/runtime.h (merged)

# Other core modules
include/core/bytecode/*.h → include/core/bytecode.h
include/core/config/*.h → include/core/config.h
include/core/errors/*.h → include/core/errors.h
include/core/memory/*.h → include/core/memory.h
include/core/patterns/*.h → include/core/patterns.h
include/core/tokenizer/*.h → include/lang/tokenizer.h
include/core/syntax/*.h → include/lang/syntax.h
```

### Step 1.2 - Create Lang Headers
**Move from**: `include/core/tokenizer/`, `include/core/syntax/` → `include/lang/`

```
include/lang/
├── lexer.h
├── tokenizer.h
├── token.h
├── token_type.h
├── syntax.h
└── dsl.h
```

### Step 1.3 - Create CLI Headers
**Move from**: `include/cli/**/*.h` → `include/cli/`

```
include/cli/
├── command.h
├── options.h
├── cli_main.h
├── accessibility.h
└── visualizer.h
```

---

## Phase 2: Documentation Asset Migration

### Step 2.1 - Consolidate Assets
```bash
# PDFs
docs/docs/specification/*.pdf → docs/assets/specs/

# Diagrams
docs/rifters_way/RIFT/*.{png,svg,mermaid} → docs/assets/diagrams/

# Images
docs/images/*.{png,svg} → docs/assets/images/

# Marked-down docs (keep referenced originals)
docs/docs/marked-down/*.md → docs/references/markdown/
```

### Step 2.2 - Update Asset References
- Update all markdown files to reference `../assets/specs/filename.pdf`
- Update CMakeLists.txt to copy assets to build output

---

## Phase 3: Binary & Library Output Structure

### Step 3.1 - Configure CMake Output
```cmake
# In CMakeLists.txt
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
```

### Step 3.2 - Binary Organization
```
bin/
├── rift              # Main CLI: rift lang <file>.rf, rift test, etc.
├── rift-lang         # RiftLang compiler
├── rift-test         # Test framework
└── rift-codegen      # Code generator

lib/
├── librift.a         # Static library
├── librift.so        # Shared library
├── libriftlang.a     # DSL library
└── librifttest.a     # Testing library
```

---

## Phase 4: Enhanced devsetup.sh

### Key Improvements
```bash
✅ Parallel build support (make -j$(nproc))
✅ Multi-threaded configuration
✅ Progress tracking
✅ Colored output with better logging
✅ Pre-commit hooks for code quality
✅ Automated code formatting
✅ Integration with clang-format, cppcheck, clang-tidy
✅ Environment variables for toolchain
✅ Docker support (optional)
✅ CI/CD preparation
```

---

## Phase 5: Token Model & Translation Pipeline

### Token Specification Document
File: `RIFT_TOKEN_MODEL.md`

**Content**:
```
1. Token Definition
   - TokenType enumeration
   - TokenValue union
   - TokenMemory metadata

2. Token Lifecycle
   - RIFT-0: Tokenization
   - RIFT-1: Parsing
   - RIFT-2: Compilation

3. Memory Model
   - Token memory associations
   - Scope tracking
   - Type bindings
```

### Translation Pipeline Document
File: `RIFT_TRANSLATION.md`

**Content**:
```
.rf → .c     Translate to C code
.rf → .h     Generate C header
.rf → .cpp   Generate C++ code
.rf → .hpp   Generate C++ header
.rf → .go    Generate Go code
.rf → .py    Generate Python code
.rf → .lua   Generate Lua code
.rf → .wasm  Generate WebAssembly
```

---

## Phase 6: Test-Driven Examples

### Example Structure
```
examples/
├── 01-hello.rf              # Basic pattern
├── 02-email-validator.rf    # Intermediate
├── 03-json-parser.rf        # Advanced
├── 04-custom-dsl.rf         # Domain-specific
└── tests/
    ├── 01-hello.test
    ├── 02-email-validator.test
    ├── 03-json-parser.test
    └── 04-custom-dsl.test
```

### Test Format
```
# test file format
pattern: "pattern-name"
input: "test-input"
expected: "expected-output"
should-compile: true|false
performance-target: "< 100µs"
```

---

## Implementation Checklist

### Phase 1: Headers
- [ ] Identify all `.h` files in `include/**`
- [ ] Merge modular headers into single files
- [ ] Update `#include` directives
- [ ] Verify compilation
- [ ] Update CMakeLists.txt

### Phase 2: Documentation
- [ ] Copy PDFs to `docs/assets/specs/`
- [ ] Copy diagrams to `docs/assets/diagrams/`
- [ ] Copy images to `docs/assets/images/`
- [ ] Update reference links
- [ ] Remove old nested directories

### Phase 3: Binaries
- [ ] Configure CMake output directories
- [ ] Build test suite
- [ ] Verify binary generation
- [ ] Create shell wrappers

### Phase 4: devsetup.sh
- [ ] Add parallel build support
- [ ] Add progress indicators
- [ ] Add error handling
- [ ] Add Docker support (optional)
- [ ] Test on Linux, macOS, Windows

### Phase 5: Documentation
- [ ] Create RIFT_TOKEN_MODEL.md
- [ ] Create RIFT_TRANSLATION.md
- [ ] Update API documentation
- [ ] Create translation examples

### Phase 6: Examples & Tests
- [ ] Create example .rf files
- [ ] Create test fixtures
- [ ] Document expected behavior
- [ ] Verify all examples run

---

## Build Integration

### New CMake Targets
```cmake
# Main targets
add_executable(rift src/cli/main.c ...)
add_library(librift STATIC src/core/...)

# Language bindings
add_library(libriftlang STATIC src/lang/...)
add_library(librifttest STATIC src/test/...)

# Code generation
add_custom_target(rift-examples
    COMMAND rift lang examples/hello.rf
    COMMAND rift lang examples/email-validator.rf
)

# Testing
add_custom_target(rift-test
    COMMAND rift test tests/unit/
)

# Parallel builds
set(CMAKE_BUILD_PARALLEL_LEVEL 8)
```

---

## File Naming Conventions

### Headers
- Core API: `rift.h`, `riftlang.h`, `rifttest.h`
- Modules: `core/{module}.h`, `lang/{module}.h`, `cli/{module}.h`
- Bindings: `bindings/{lang}.h`

### Source
- Implementation: `src/{module}/{file}.c`
- Tests: `tests/{category}/{module}_test.c`

### Examples
- RiftLang files: `examples/{name}.rf`
- Test data: `examples/tests/{name}.test`
- Output samples: `examples/out/{name}.{ext}`

---

## Success Metrics

✅ **Code Organization**
- Single `include/` namespace
- No nested subdirectories in headers
- Clear separation of concerns

✅ **Documentation**
- All assets in `docs/assets/`
- No scattered PDFs/PNGs
- Updated reference links

✅ **Build Performance**
- Parallel build support
- Reduced compile times
- Clear error messages

✅ **Developer Experience**
- One-command setup: `./tools/devsetup.sh`
- Fast incremental builds
- Clear API documentation

✅ **Production Ready**
- Clean binary output
- Stable library interface
- Comprehensive test coverage

---

## Timeline

| Phase | Tasks | Estimated Time |
|-------|-------|-----------------|
| 1 | Header consolidation | 2-3 hours |
| 2 | Asset migration | 1-2 hours |
| 3 | Binary organization | 1 hour |
| 4 | devsetup.sh enhancement | 2 hours |
| 5 | Documentation | 3-4 hours |
| 6 | Examples & tests | 2-3 hours |
| **Total** | | **11-15 hours** |

---

## Notes for OBINexus RIFT

This restructuring maintains continuity with:
- **Milestone-based investment** philosophy
- **#NoGhosting** compliance architecture  
- **OpenSense recruitment** principles
- **Flexible Translator** core mission (`.rf` → `.*`)

The new structure supports the toolchain:
```
riftlang.exe → .so.a → rift.exe → gosilang
```

---

**Document Version**: 1.0.0  
**Last Updated**: May 28, 2026  
**Status**: APPROVED FOR IMPLEMENTATION
