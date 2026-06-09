/**
 * @file webbridge.h
 * @brief WebAssembly bridge for LibRift
 *
 * This file defines the interfaces necessary for compiling LibRift
 * to WebAssembly and exposing its functionality to JavaScript.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdint.h>
#include <stdlib.h>
#include "core/bytecode.h"
#include <emscripten.h>
#ifndef LIBRIFT_WEBBRIDGE_H
#define LIBRIFT_WEBBRIDGE_H



// When compiling for WebAssembly, we need to export functions
#ifdef __EMSCRIPTEN__
#define RIFT_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define RIFT_EXPORT
#endif

/**
 * @brief Handle representing a compiled pattern
 *
 * Opaque handle used to reference a pattern across the WebBridge
 */
typedef uint32_t rift_pattern_handle_t;

/**
 * @brief Handle representing a matcher instance
 *
 * Opaque handle used to reference a matcher across the WebBridge
 */
typedef uint32_t rift_matcher_handle_t;

/**
 * @brief Initialize the WebBridge
 *
 * @return 1 if successful, 0 otherwise
 */
RIFT_EXPORT int rift_webbridge_init(void);

/**
 * @brief Cleanup the WebBridge
 */
RIFT_EXPORT void rift_webbridge_cleanup(void);

/**
 * @brief Compile a pattern
 *
 * @param pattern Pattern string to compile
 * @param flags Compilation flags
 * @return Handle to the compiled pattern or 0 on failure
 */
RIFT_EXPORT rift_pattern_handle_t rift_webbridge_compile(const char *pattern, uint32_t flags);

/**
 * @brief Get last error message
 *
 * @param buffer Buffer to store error message
 * @param buffer_size Size of buffer
 * @return Length of error message or 0 if no error
 */
RIFT_EXPORT uint32_t rift_webbridge_get_last_error(char *buffer, uint32_t buffer_size);

/**
 * @brief Free a compiled pattern
 *
 * @param handle Handle to the pattern
 */
RIFT_EXPORT void rift_webbridge_free_pattern(rift_pattern_handle_t handle);

/**
 * @brief Create a matcher for a pattern
 *
 * @param pattern_handle Handle to the compiled pattern
 * @param options Matcher options
 * @return Handle to the matcher or 0 on failure
 */
RIFT_EXPORT rift_matcher_handle_t
rift_webbridge_create_matcher(rift_pattern_handle_t pattern_handle, uint32_t options);

/**
 * @brief Set input for a matcher
 *
 * @param matcher_handle Handle to the matcher
 * @param input Input string
 * @param length Length of input or -1 to use strlen
 * @return 1 if successful, 0 otherwise
 */
RIFT_EXPORT int rift_webbridge_set_input(rift_matcher_handle_t matcher_handle, const char *input,
                                         int32_t length);

/**
 * @brief Check if input matches pattern
 *
 * @param matcher_handle Handle to the matcher
 * @return 1 if matches, 0 otherwise
 */
RIFT_EXPORT int rift_webbridge_matches(rift_matcher_handle_t matcher_handle);

/**
 * @brief Find next match
 *
 * @param matcher_handle Handle to the matcher
 * @param start_pos Pointer to store start position of match
 * @param end_pos Pointer to store end position of match
 * @return 1 if match found, 0 otherwise
 */
RIFT_EXPORT int rift_webbridge_find_next(rift_matcher_handle_t matcher_handle, uint32_t *start_pos,
                                         uint32_t *end_pos);

/**
 * @brief Get capture group information
 *
 * @param matcher_handle Handle to the matcher
 * @param group_index Group index (0 for whole match)
 * @param start_pos Pointer to store start position
 * @param end_pos Pointer to store end position
 * @return 1 if group exists, 0 otherwise
 */
RIFT_EXPORT int rift_webbridge_get_group(rift_matcher_handle_t matcher_handle, uint32_t group_index,
                                         uint32_t *start_pos, uint32_t *end_pos);

/**
 * @brief Get group by name
 *
 * @param matcher_handle Handle to the matcher
 * @param name Group name
 * @param start_pos Pointer to store start position
 * @param end_pos Pointer to store end position
 * @return 1 if group exists, 0 otherwise
 */
RIFT_EXPORT int rift_webbridge_get_named_group(rift_matcher_handle_t matcher_handle,
                                               const char *name, uint32_t *start_pos,
                                               uint32_t *end_pos);

/**
 * @brief Free a matcher
 *
 * @param matcher_handle Handle to the matcher
 */
RIFT_EXPORT void rift_webbridge_free_matcher(rift_matcher_handle_t matcher_handle);

/**
 * @brief Serialize a compiled pattern to bytecode
 *
 * @param pattern_handle Handle to the pattern
 * @param buffer Buffer to store serialized data (can be NULL to get size)
 * @param buffer_size Size of buffer or pointer to store required size
 * @return Actual size of serialized data or 0 on failure
 */
RIFT_EXPORT uint32_t rift_webbridge_serialize_pattern(rift_pattern_handle_t pattern_handle,
                                                      uint8_t *buffer, uint32_t *buffer_size);

/**
 * @brief Deserialize a pattern from bytecode
 *
 * @param buffer Buffer containing serialized data
 * @param buffer_size Size of buffer
 * @return Handle to the deserialized pattern or 0 on failure
 */
RIFT_EXPORT rift_pattern_handle_t rift_webbridge_deserialize_pattern(const uint8_t *buffer,
                                                                     uint32_t buffer_size);

/**
 * @brief Run a bytecode program directly
 *
 * Advanced function for direct bytecode execution
 *
 * @param bytecode Serialized bytecode
 * @param bytecode_size Size of bytecode
 * @param input Input string
 * @param input_length Length of input
 * @param match_start Pointer to store match start position
 * @param match_end Pointer to store match end position
 * @return 1 if match found, 0 otherwise
 */
RIFT_EXPORT int rift_webbridge_run_bytecode(const uint8_t *bytecode, uint32_t bytecode_size,
                                            const char *input, uint32_t input_length,
                                            uint32_t *match_start, uint32_t *match_end);

#endif /* LIBRIFT_WEBBRIDGE_H */