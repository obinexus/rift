/**
 * @file bytecode_program.h
 * @brief Interface for bytecode program operations in LibRift
 *
 * This header file defines operations for managing bytecode programs,
 * including creation, validation, and manipulation.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#include "core/bytecode/bytecode.h"
#include "core/errors/regex_error.h"
#ifndef RIFT_BYTECODE_PROGRAM_H
#define RIFT_BYTECODE_PROGRAM_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a new bytecode program
 *
 * @param initial_capacity Initial capacity for the bytecode instructions
 * @param flags Regex compilation flags
 * @return A new bytecode program or NULL on failure
 */
rift_bytecode_program_t *
rift_bytecode_program_create(size_t initial_capacity, rift_regex_flags_t flags);

/**
 * @brief Ensure the program has enough capacity for additional instructions
 *
 * @param program The bytecode program
 * @param capacity The minimum capacity required
 * @return true if successful, false on allocation failure
 */
bool
rift_bytecode_program_ensure_capacity(rift_bytecode_program_t *program, size_t capacity);

/**
 * @brief Add an instruction to the bytecode program
 *
 * @param program The bytecode program
 * @param opcode The operation code to add
 * @return The index of the new instruction or -1 on failure
 */
int32_t
rift_bytecode_program_add_instruction(rift_bytecode_program_t *program, rift_bytecode_opcode_t opcode);

/**
 * @brief Set a character operand for an instruction
 *
 * @param program The bytecode program
 * @param index The instruction index
 * @param character The character operand
 * @return true if successful, false otherwise
 */
bool
rift_bytecode_program_set_char_operand(rift_bytecode_program_t *program, int32_t index, char character);

/**
 * @brief Set a character class operand for an instruction
 *
 * @param program The bytecode program
 * @param index The instruction index
 * @param class_pattern The character class pattern
 * @param pattern_length Length of the pattern
 * @return true if successful, false otherwise
 */
bool
rift_bytecode_program_set_class_operand(rift_bytecode_program_t *program, int32_t index,
										const char *class_pattern, uint32_t pattern_length);

/**
 * @brief Set a jump target for an instruction
 *
 * @param program The bytecode program
 * @param index The instruction index
 * @param target_index The target instruction index
 * @return true if successful, false otherwise
 */
bool
rift_bytecode_program_set_jump_target(rift_bytecode_program_t *program, int32_t index,
									  uint32_t target_index);

/**
 * @brief Set a group operand for an instruction
 *
 * @param program The bytecode program
 * @param index The instruction index
 * @param group_index The capture group index
 * @return true if successful, false otherwise
 */
bool
rift_bytecode_program_set_group_operand(rift_bytecode_program_t *program, int32_t index,
										uint32_t group_index);

/**
 * @brief Set a repeat operand for an instruction
 *
 * @param program The bytecode program
 * @param index The instruction index
 * @param min Minimum repetition count
 * @param max Maximum repetition count
 * @param greedy Whether the repetition is greedy
 * @return true if successful, false otherwise
 */
bool
rift_bytecode_program_set_repeat_operand(rift_bytecode_program_t *program, int32_t index,
										 uint32_t min, uint32_t max, bool greedy);

/**
 * @brief Set the original pattern for a bytecode program
 *
 * @param program The bytecode program
 * @param pattern The original regex pattern
 * @return true if successful, false otherwise
 */
bool
rift_bytecode_program_set_pattern(rift_bytecode_program_t *program, const char *pattern);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_BYTECODE_PROGRAM_H */
