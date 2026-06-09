/**
 * @file bytecode.h
 * @brief Core definitions for LibRift bytecode generation and execution
 *
 * This file defines the structures and functions necessary for
 * generating platform-independent bytecode from regex patterns.
 * VM-specific functionality is in bytecode_vm.h.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

 #ifndef LIBRIFT_BYTECODE_H
 #define LIBRIFT_BYTECODE_H
 
 #include <stdbool.h>
 #include <stdint.h>
 #include <stdio.h>
 #include <stdlib.h>
 
 #include "core/automaton/regex.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Opcodes for the LibRift bytecode virtual machine
  */
 typedef enum {
     RIFT_OP_NOP = 0,     /* No operation */
     RIFT_OP_MATCH_CHAR,  /* Match a single character */
     RIFT_OP_MATCH_CLASS, /* Match a character class */
     RIFT_OP_JUMP,        /* Unconditional jump */
     RIFT_OP_SPLIT,       /* Split execution (for alternation) */
     RIFT_OP_SAVE_START,  /* Save start position for a capture group */
     RIFT_OP_SAVE_END,    /* Save end position for a capture group */
     RIFT_OP_MATCH_ANY,   /* Match any character (.) */
     RIFT_OP_ACCEPT,      /* Accept the current input */
     RIFT_OP_FAIL,        /* Fail the current path */
 
     /* Advanced operations */
     RIFT_OP_REPEAT_START, /* Start of a repetition construct */
     RIFT_OP_REPEAT_END,   /* End of a repetition construct */
     RIFT_OP_BOUNDARY,     /* Word boundary assertion */
     RIFT_OP_BACKREF,      /* Backreference to a previous capture */
     RIFT_OP_LOOKAHEAD,    /* Positive lookahead assertion */
     RIFT_OP_NEG_LOOKAHEAD /* Negative lookahead assertion */
 } rift_bytecode_opcode_t;
 
 /**
  * @brief Single instruction in the LibRift bytecode
  */
 typedef struct {
     rift_bytecode_opcode_t opcode;
     union {
         char character;       /* For MATCH_CHAR */
         uint32_t jump_target; /* For JUMP, SPLIT */
         uint32_t group_index; /* For SAVE_START, SAVE_END, BACKREF */
         struct {              /* For MATCH_CLASS */
             char *class_pattern;
             uint32_t pattern_length;
         } char_class;
         struct { /* For REPEAT_START */
             uint32_t min;
             uint32_t max; /* UINT32_MAX for unbounded */
             bool greedy;
         } repeat;
     } operand;
 } rift_bytecode_instruction_t;
 
 /**
  * @brief Collection of bytecode instructions representing a compiled pattern
  */
 typedef struct {
     rift_bytecode_instruction_t *instructions;
     uint32_t instruction_count;
     uint32_t capacity;
     uint32_t group_count;
     rift_regex_flags_t flags;
     char *original_pattern;   /* For debugging */
     uint32_t *char_class_map; /* Optimized character class lookup */
 } rift_bytecode_program_t;
 
 /**
  * @brief Endianness markers for bytecode serialization
  */
 typedef enum {
     RIFT_ENDIAN_LITTLE = 0x01020304, // Serialized as 04 03 02 01
     RIFT_ENDIAN_BIG = 0x04030201     // Serialized as 04 03 02 01
 } rift_endian_marker_t;
 
 /* Forward declaration of the VM type (defined in bytecode_vm.h) */
 typedef struct rift_bytecode_vm rift_bytecode_vm_t;
 
 /**
  * @brief Compile a pattern directly to bytecode
  *
  * @param pattern Pattern string to compile
  * @param flags Compilation flags
  * @param error Error information (can be NULL)
  * @return Compiled bytecode program or NULL on failure
  */
 rift_bytecode_program_t *rift_bytecode_compile(const char *pattern, rift_regex_flags_t flags,
                                                rift_regex_error_t *error);
 
 /**
  * @brief Convert an existing automaton to bytecode
  *
  * @param automaton Automaton to convert
  * @param flags Compilation flags
  * @param error Error information (can be NULL)
  * @return Compiled bytecode program or NULL on failure
  */
 rift_bytecode_program_t *rift_bytecode_from_automaton(rift_regex_automaton_t *automaton,
                                                       rift_regex_flags_t flags,
                                                       rift_regex_error_t *error);
 
 /**
  * @brief Create a pattern from bytecode program
  *
  * @param program Bytecode program to convert
  * @param error Error information (can be NULL)
  * @return rift_regex_pattern_t* Reconstructed pattern or NULL on failure
  */
 rift_regex_pattern_t *rift_regex_pattern_from_bytecode(rift_bytecode_program_t *program,
                                                        rift_regex_error_t *error);
 
 /**
  * @brief Free a bytecode program
  *
  * @param program Program to free
  */
 void rift_bytecode_program_free(rift_bytecode_program_t *program);
 
 /**
  * @brief Serialize bytecode to a binary format
  *
  * @param program Bytecode program to serialize
  * @param data Output buffer for serialized data (can be NULL to get size)
  * @param size Size of output buffer or pointer to store required size
  * @return true if successful, false otherwise
  */
 bool rift_bytecode_serialize(rift_bytecode_program_t *program, uint8_t *data, size_t *size);
 
 /**
  * @brief Deserialize bytecode from a binary format
  *
  * @param data Serialized bytecode data
  * @param size Size of serialized data
  * @param error Error information (can be NULL)
  * @return Deserialized bytecode program or NULL on failure
  */
 rift_bytecode_program_t *rift_bytecode_deserialize(const uint8_t *data, size_t size,
                                                    rift_regex_error_t *error);
 
 /**
  * @brief Initialize bytecode system
  *
  * @return bool True if successful, false otherwise
  */
 bool rift_bytecode_system_initialize(void);
 
 /**
  * @brief Detect system endianness
  *
  * @return rift_endian_marker_t The detected endianness
  */
 rift_endian_marker_t rift_detect_endianness(void);
 
 /**
  * @brief Swap endianness of a 32-bit value if needed
  *
  * @param value The value to swap
  * @param source_endian The source endianness
  * @param target_endian The target endianness
  * @return uint32_t The endian-corrected value
  */
 uint32_t rift_swap_endian32(uint32_t value, rift_endian_marker_t source_endian,
                             rift_endian_marker_t target_endian);
 
 /**
  * @brief Create bytecode program with proper platform adjustments
  *
  * @param pattern The regex pattern
  * @param flags Compilation flags
  * @param error Error information (can be NULL)
  * @return rift_bytecode_program_t* The bytecode program or NULL on failure
  */
 rift_bytecode_program_t *rift_create_platform_bytecode(const char *pattern, uint32_t flags,
                                                        rift_regex_error_t *error);
 
 /**
  * @brief Validate a bytecode program
  *
  * @param program Bytecode program to validate
  * @param error Error information (can be NULL)
  * @return bool True if valid, false otherwise
  */
 bool rift_bytecode_validate(rift_bytecode_program_t *program, rift_regex_error_t *error);
 
 /**
  * @brief Optimize a bytecode program
  *
  * @param program Bytecode program to optimize
  * @param error Error information (can be NULL)
  * @return bool True if optimization was successful, false otherwise
  */
 bool rift_bytecode_optimize(rift_bytecode_program_t *program, rift_regex_error_t *error);
 
 /**
  * @brief Generate debug information for a bytecode program
  *
  * @param program Bytecode program
  * @param output Output file or NULL to print to stdout
  * @return bool True if debug info generated successfully
  */
 bool rift_bytecode_debug_info(rift_bytecode_program_t *program, FILE *output);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* LIBRIFT_BYTECODE_H */