/**
 * @file rift_dsl_io.h
 * @brief Header for I/O utilities for the .rift DSL
 *
 * This file declares the public API for reading and writing .rift DSL files,
 * including serialization and deserialization of bytecode.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#ifndef RIFT_DSL_IO_H
#define RIFT_DSL_IO_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compile a .rift DSL source to binary format
 * 
 * @param source The .rift DSL source code
 * @return Opaque handle to binary data or NULL on error
 */
void *rift_dsl_compile_to_binary(const char *source);

/**
 * @brief Load a .rift file and compile it to binary format
 * 
 * @param filename Path to the .rift file
 * @return Opaque handle to binary data or NULL on error
 */
void *rift_dsl_load_compile_to_binary(const char *filename);

/**
 * @brief Free binary data
 * 
 * @param handle The binary data handle to free
 */
void rift_dsl_binary_free(void *handle);

/**
 * @brief Get error message from binary data
 * 
 * @param handle The binary data handle
 * @return Error message or NULL if no error
 */
const char *rift_dsl_binary_get_error(void *handle);

/**
 * @brief Save binary data to a file
 * 
 * @param handle The binary data handle
 * @param filename Path to save the binary data
 * @return true if successful, false otherwise
 */
bool rift_dsl_binary_save(void *handle, const char *filename);

/**
 * @brief Load binary data from a file
 * 
 * @param filename Path to the binary file
 * @return Opaque handle to binary data or NULL on error
 */
void *rift_dsl_binary_load(const char *filename);

/**
 * @brief Get raw data from binary handle
 * 
 * @param handle The binary data handle
 * @param data Pointer to receive the data pointer
 * @param size Pointer to receive the size of the data
 * @return true if successful, false otherwise
 */
bool rift_dsl_binary_get_data(void *handle, const uint8_t **data, size_t *size);

/**
 * @brief Create a compilation handle from binary data
 * 
 * @param handle The binary data handle
 * @return Compilation handle or NULL on error
 */
void *rift_dsl_compilation_from_binary(void *handle);

/**
 * @brief Validate binary data
 * 
 * @param handle The binary data handle
 * @return true if valid, false otherwise
 */
bool rift_dsl_binary_validate(void *handle);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_DSL_IO_H */
