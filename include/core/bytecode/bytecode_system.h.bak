/**
 * @file bytecode_system.h
 * @brief Cross-platform bytecode system for LibRift
 *
 * This file defines system-level functionality for the bytecode module,
 * including endianness detection, platform optimization, and bytecode
 * initialization.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "core/errors/error.h"
#include "core/errors/regex_error.h"
#include "core/memory/memory.h"
#include "core/errors/regex_error_compat.h"
#include "bytecode.h"
#include "core/bytecode/bytecode.h"
#include "core/regex/regex.h"
#ifndef LIBRIFT_BYTECODE_SYSTEM_H
#define LIBRIFT_BYTECODE_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Bytecode format version */
#define BYTECODE_FORMAT_VERSION 1

/* Magic number for bytecode serialization format */
#define BYTECODE_MAGIC 0x52494654 /* "RIFT" in ASCII */

/* Endianness marker */
#define BYTECODE_LITTLE_ENDIAN 0x01020304
#define BYTECODE_BIG_ENDIAN 0x04030201

/* Header structure for serialized bytecode */
typedef struct {
    uint32_t magic;             /* Magic number (BYTECODE_MAGIC) */
    uint32_t endianness;        /* Endianness marker */
    uint32_t version;           /* Bytecode format version */
    uint32_t flags;             /* Compilation flags */
    uint32_t instruction_count; /* Number of instructions */
    uint32_t group_count;       /* Number of capture groups */
    uint32_t pattern_length;    /* Length of original pattern string */
} bytecode_header_t;

/**
 * @brief Global initialization of the bytecode system
 *
 * This should be called once during library initialization.
 *
 * @return true if successful, false on failure
 */
bool rift_bytecode_system_initialize(void);

/**
 * @brief Check if the bytecode system has been initialized
 *
 * @return true if initialized, false otherwise
 */
bool rift_bytecode_system_is_initialized(void);

/**
 * @brief Shut down the bytecode system
 *
 * This should be called during library cleanup.
 */
void rift_bytecode_system_shutdown(void);

/**
 * @brief Get the native endianness of the current system
 *
 * @return rift_endian_marker_t The system's endianness
 */
rift_endian_marker_t rift_detect_endianness(void);

/**
 * @brief Check if the current system is little-endian
 *
 * @return true if little-endian, false if big-endian
 */
bool rift_is_little_endian(void);

/**
 * @brief Swap the endianness of a 32-bit integer
 *
 * @param value The value to swap
 * @return uint32_t The value with reversed byte order
 */
uint32_t rift_swap_endianness(uint32_t value);

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
 * @brief Create a bytecode program optimized for the current platform
 *
 * @param pattern The regex pattern
 * @param flags Compilation flags
 * @param error Error information (can be NULL)
 * @return rift_bytecode_program_t* The bytecode program or NULL on failure
 */
rift_bytecode_program_t *rift_create_platform_bytecode(const char *pattern, uint32_t flags,
                                                       rift_regex_error_t *error);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_BYTECODE_SYSTEM_H */