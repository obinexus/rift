/**
 * @file rift_dsl_compiler.h
 * @brief Header for the compiler for the .rift DSL
 *
 * This file defines the public API for compiling patterns from .rift DSL files
 * into bytecode for the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "core/bytecode/bytecode.h"
#include "core/errors/regex_error.h"
#ifndef RIFT_DSL_COMPILER_H
#define RIFT_DSL_COMPILER_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compile a .rift DSL source to bytecode
 * 
 * @param source The .rift DSL source code
 * @return Opaque handle to compiled bytecode or NULL on error
 */
void *rift_dsl_compile(const char *source);

/**
 * @brief Free a compilation handle
 * 
 * @param handle The compilation handle to free
 */
void rift_dsl_free_compilation(void *handle);

/**
 * @brief Get error message from a failed compilation
 * 
 * @param handle The compilation handle
 * @return Error message or NULL if no error
 */
const char *rift_dsl_get_compilation_error(void *handle);

/**
 * @brief Get the number of compiled programs
 * 
 * @param handle The compilation handle
 * @return Number of compiled programs
 */
size_t rift_dsl_get_compiled_count(void *handle);

/**
 * @brief Get a compiled program by index
 * 
 * @param handle The compilation handle
 * @param index Program index
 * @return Bytecode program or NULL if index is invalid
 */
const rift_bytecode_program_t *rift_dsl_get_compiled_program(void *handle, size_t index);

/**
 * @brief Serialize a compilation to binary data
 * 
 * @param handle The compilation handle
 * @param data Pointer to receive the serialized data
 * @param size Pointer to receive the size of the serialized data
 * @return true if successful, false otherwise
 */
bool rift_dsl_serialize_compilation(void *handle, uint8_t **data, size_t *size);

/**
 * @brief Deserialize a compilation from binary data
 * 
 * @param data The serialized data
 * @param size Size of the serialized data
 * @return New compilation handle or NULL on error
 */
void *rift_dsl_deserialize_compilation(const uint8_t *data, size_t size);

/**
 * @brief Execute a compiled program on input text
 * 
 * @param handle The compilation handle
 * @param index Program index
 * @param input Input text
 * @param input_length Length of input text
 * @param match Pointer to match structure to fill
 * @return true if match found, false otherwise
 */
bool rift_dsl_execute(void *handle, size_t index, const char *input, 
					 size_t input_length, rift_regex_match_t *match);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_DSL_COMPILER_H */
