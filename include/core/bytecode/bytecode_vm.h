/**
 * @file bytecode_vm.h
 * @brief Header file for the bytecode virtual machine for LibRift
 *
 * This file defines the structures and functions for executing compiled bytecode,
 * providing platform-independent regex pattern matching.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

 #ifndef LIBRIFT_BYTECODE_VM_H
 #define LIBRIFT_BYTECODE_VM_H
 
 #include <stdbool.h>
 #include <stdint.h>
 #include <stdlib.h>
 #include <string.h>
 
 #include "core/bytecode/bytecode.h"
 #include "core/errors/regex_error.h"
 #include "core/engine/match_types.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Bytecode virtual machine state
  */
 struct rift_bytecode_vm {
     const char *input;            /* Input string being matched */
     size_t input_length;          /* Length of input string */
     uint32_t current_pos;         /* Current position in input */
     uint32_t stack_size;          /* Current size of backtrack stack */
     uint32_t stack_capacity;      /* Capacity of backtrack stack */
     uint32_t *backtrack_stack;    /* Stack for backtracking information */
     uint32_t capture_count;       /* Number of capture groups (including full match) */
     uint32_t *captures;           /* Capture group positions (start,end pairs) */
     bool timed_out;               /* Whether execution timed out */
     uint64_t max_instructions;    /* Maximum number of instructions to execute */
     uint64_t instruction_counter; /* Number of instructions executed */
 };
 
 /**
  * @brief Create a new VM for executing bytecode
  *
  * @param program Bytecode program to execute
  * @param input Input string to match against
  * @param input_length Length of input string or (size_t)-1 to use strlen
  * @return New VM instance or NULL on failure
  */
 rift_bytecode_vm_t *rift_bytecode_vm_create(rift_bytecode_program_t *program, const char *input,
                                             size_t input_length);
 
 /**
  * @brief Free resources associated with a bytecode VM
  *
  * @param vm The VM instance to free
  */
 void rift_bytecode_vm_free(rift_bytecode_vm_t *vm);
 
 /**
  * @brief Reset VM state for reuse
  *
  * @param vm The VM instance to reset
  */
 void rift_bytecode_vm_reset(rift_bytecode_vm_t *vm);
 
 /**
  * @brief Execute bytecode program on the given VM
  *
  * @param program Bytecode program to execute
  * @param vm VM instance to use
  * @param match Output match result (can be NULL)
  * @return true if pattern matched, false otherwise
  */
 bool rift_bytecode_execute(rift_bytecode_program_t *program, rift_bytecode_vm_t *vm,
                            rift_regex_match_t *match);
 
 /**
  * @brief Create VM with custom execution limits
  *
  * @param program Bytecode program to execute
  * @param input Input string to match against
  * @param input_length Length of input string or (size_t)-1 to use strlen
  * @param max_instructions Maximum number of instructions to execute
  * @return New VM instance or NULL on failure
  */
 rift_bytecode_vm_t *rift_bytecode_vm_create_with_options(rift_bytecode_program_t *program,
                                                          const char *input, size_t input_length,
                                                          uint64_t max_instructions);
 
 /**
  * @brief Check if VM execution timed out
  *
  * @param vm The VM instance to check
  * @return true if execution timed out, false otherwise
  */
 bool rift_bytecode_vm_timed_out(const rift_bytecode_vm_t *vm);
 
 /**
  * @brief Get capture group positions
  *
  * @param vm The VM instance to query
  * @param group_index Index of the capture group (0 for full match)
  * @param start Pointer to store start position
  * @param end Pointer to store end position
  * @return true if group was captured, false otherwise
  */
 bool rift_bytecode_vm_get_group(const rift_bytecode_vm_t *vm, uint32_t group_index, uint32_t *start,
                                 uint32_t *end);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* LIBRIFT_BYTECODE_VM_H */