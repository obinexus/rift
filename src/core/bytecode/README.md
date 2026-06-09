# LibRift Bytecode Module

The bytecode module is a core component of LibRift, providing a portable and efficient execution engine for regular expression patterns. This module translates regex patterns into bytecode instructions that can be efficiently executed across different platforms.

## Overview

The bytecode system enables LibRift to:

- Compile regex patterns to platform-independent bytecode
- Execute pattern matching with consistent behavior across systems
- Optimize pattern matching for performance
- Support complex regex features through a comprehensive instruction set
- Handle platform differences like endianness automatically

## Architecture

The bytecode implementation consists of these main components:

- **Compiler**: Translates regex patterns to bytecode instructions
- **Virtual Machine (VM)**: Executes bytecode to perform pattern matching
- **Instruction Set**: Defines operations for pattern matching logic
- **Optimization Engine**: Improves bytecode efficiency
- **Platform Adaptation Layer**: Handles cross-platform compatibility

## Instruction Set

The bytecode VM supports these core instructions:

| Opcode | Description |
|--------|-------------|
| `NOP` | No operation |
| `MATCH_CHAR` | Match a single character |
| `MATCH_CLASS` | Match a character class |
| `MATCH_ANY` | Match any character |
| `JUMP` | Unconditional jump |
| `SPLIT` | Split execution path (non-deterministic choice) |
| `SAVE_START` | Save capture group start position |
| `SAVE_END` | Save capture group end position |
| `ACCEPT` | Accept the current match |
| `FAIL` | Fail the current path |
| `REPEAT_START` | Start repetition construct |
| `REPEAT_END` | End repetition construct |
| `BOUNDARY` | Word boundary assertion |
| `BACKREF` | Backreference to a previous capture |
| `LOOKAHEAD` | Positive lookahead assertion |
| `NEG_LOOKAHEAD` | Negative lookahead assertion |

## Usage

```c
// Compile a pattern to bytecode
rift_regex_error_t error;
rift_bytecode_program_t *program = rift_create_platform_bytecode(
    "a(b+)c", RIFT_REGEX_DEFAULT, &error);

// Create a VM for execution
rift_bytecode_vm_t *vm = rift_bytecode_vm_create(
    program, "abbbc", (size_t)-1);

// Execute the pattern
rift_regex_match_t match;
bool result = rift_bytecode_execute(program, vm, &match);

// Check capture groups
uint32_t start, end;
if (rift_bytecode_vm_get_group(vm, 1, &start, &end)) {
    // Use captured group 1 (the "b+")
}

// Clean up
rift_bytecode_vm_free(vm);
rift_bytecode_program_free(program);
```

## Optimization

The bytecode system includes optimization capabilities:

- Redundant instruction elimination
- Jump optimization
- Instruction reordering for better cache locality
- Various optimization levels (controlled via flags)

```c
// Optimize a bytecode program
rift_bytecode_optimize(program, &error);
```

## Cross-Platform Support

The bytecode system automatically handles platform differences:

- Endianness detection and correction
- Architecture-specific adjustments
- Serialization for cross-platform storage

## Debug Support

The module includes debugging tools:

```c
// Generate human-readable debug information
rift_bytecode_debug_info(program, stdout);
```

## Internal Implementation

The VM uses a backtracking algorithm with these key components:

- Instruction pointer management
- Input cursor position tracking
- Capture group storage
- Backtracking stack for non-deterministic choices
- Infinite loop protection through instruction counting

## Performance Considerations

- The VM includes protection against catastrophic backtracking
- Execution time limits can be configured
- Memory usage scales with pattern complexity

## See Also

- [Regex Engine Documentation](../engine/README.md)
- [Automaton Module](../automaton/README.md)
- [CLI Tools](../../cli/README.md)