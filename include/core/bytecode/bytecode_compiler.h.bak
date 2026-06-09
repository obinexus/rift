/**
 * @file bytecode_compiler.h
 * @brief Header for the bytecode compiler for LibRift
 *
 * This file declares the interfaces for compiling regex patterns to portable bytecode,
 * supporting cross-platform serialization and execution.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stdint.h>
#include "core/automaton/automaton.h"
#include "core/bytecode/bytecode.h"
#include "core/errors/regex_error.h"
#ifndef LIBRIFT_BYTECODE_COMPILER_H
#define LIBRIFT_BYTECODE_COMPILER_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compile a regex automaton into bytecode
 *
 * @param automaton The automaton to compile
 * @param flags Compilation flags
 * @param error Error information (can be NULL)
 * @return Compiled bytecode program or NULL on failure
 */
rift_bytecode_program_t *rift_bytecode_from_automaton(rift_regex_automaton_t *automaton,
                                                      rift_regex_flags_t flags,
                                                      rift_regex_error_t *error);

/**
 * @brief Compile a regex pattern string into bytecode
 *
 * @param pattern The pattern string to compile
 * @param flags Compilation flags
 * @param error Error information (can be NULL)
 * @return Compiled bytecode program or NULL on failure
 */
rift_bytecode_program_t *rift_bytecode_compile(const char *pattern, rift_regex_flags_t flags,
                                               rift_regex_error_t *error);

/**
 * @brief Free a bytecode program
 *
 * @param program The program to free
 */
void rift_bytecode_program_free(rift_bytecode_program_t *program);

/**
 * @brief Serialize a bytecode program to binary data
 *
 * @param program The bytecode program to serialize
 * @param data Buffer to hold the serialized data
 * @param size Pointer to receive the serialized size
 * @return true on success, false on failure
 */
bool rift_bytecode_serialize(rift_bytecode_program_t *program, uint8_t *data, size_t *size);

/**
 * @brief Deserialize a bytecode program from binary data
 *
 * @param data The serialized bytecode data
 * @param size Size of the serialized data
 * @param error Error information (can be NULL)
 * @return Reconstructed bytecode program or NULL on failure
 */
rift_bytecode_program_t *rift_bytecode_deserialize(const uint8_t *data, size_t size,
                                                   rift_regex_error_t *error);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_BYTECODE_COMPILER_H */
