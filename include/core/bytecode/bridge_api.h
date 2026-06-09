/**
 * @file bridge_api.h
 * @brief Declaration of platform-agnostic bridge API for LibRift
 *
 * This file defines the abstract bridge API that serves as the interface
 * between the LibRift regex engine and platform-specific implementations.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifndef LIBRIFT_BRIDGE_API_H
#define LIBRIFT_BRIDGE_API_H


#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Bridge API function type definitions
 */
typedef bool (*rift_bridge_initialize_fn)(rift_bridge_context_t *context);
typedef void (*rift_bridge_cleanup_fn)(rift_bridge_context_t *context);
typedef rift_pattern_handle_t (*rift_bridge_compile_fn)(rift_bridge_context_t *context,
                                                        const char *pattern, uint32_t flags);
typedef rift_matcher_handle_t (*rift_bridge_create_matcher_fn)(rift_bridge_context_t *context,
                                                               rift_pattern_handle_t pattern_handle,
                                                               uint32_t options);
typedef int (*rift_bridge_set_input_fn)(rift_bridge_context_t *context,
                                        rift_matcher_handle_t matcher_handle, const char *input,
                                        int32_t length);
typedef int (*rift_bridge_matches_fn)(rift_bridge_context_t *context,
                                      rift_matcher_handle_t matcher_handle);
typedef int (*rift_bridge_find_next_fn)(rift_bridge_context_t *context,
                                        rift_matcher_handle_t matcher_handle, uint32_t *start_pos,
                                        uint32_t *end_pos);
typedef void (*rift_bridge_free_pattern_fn)(rift_bridge_context_t *context,
                                            rift_pattern_handle_t handle);
typedef void (*rift_bridge_free_matcher_fn)(rift_bridge_context_t *context,
                                            rift_matcher_handle_t handle);
typedef uint32_t (*rift_bridge_get_last_error_fn)(rift_bridge_context_t *context, char *buffer,
                                                  uint32_t buffer_size);
typedef uint32_t (*rift_bridge_serialize_pattern_fn)(rift_bridge_context_t *context,
                                                     rift_pattern_handle_t pattern_handle,
                                                     uint8_t *buffer, uint32_t *buffer_size);
typedef rift_pattern_handle_t (*rift_bridge_deserialize_pattern_fn)(rift_bridge_context_t *context,
                                                                    const uint8_t *buffer,
                                                                    uint32_t buffer_size);
typedef int (*rift_bridge_run_bytecode_fn)(rift_bridge_context_t *context, const uint8_t *bytecode,
                                           uint32_t bytecode_size, const char *input,
                                           uint32_t input_length, uint32_t *match_start,
                                           uint32_t *match_end);
typedef int (*rift_bridge_reset_matcher_fn)(rift_bridge_context_t *context,
                                            rift_matcher_handle_t matcher_handle);

/**
 * @brief Bridge API function table
 *
 * This structure defines the interface that must be implemented
 * by platform-specific bridge implementations.
 */
typedef struct rift_bridge_api {
    rift_bridge_initialize_fn initialize;
    rift_bridge_cleanup_fn cleanup;
    rift_bridge_compile_fn compile;
    rift_bridge_create_matcher_fn create_matcher;
    rift_bridge_set_input_fn set_input;
    rift_bridge_matches_fn matches;
    rift_bridge_find_next_fn find_next;
    rift_bridge_free_pattern_fn free_pattern;
    rift_bridge_free_matcher_fn free_matcher;
    rift_bridge_get_last_error_fn get_last_error;
    rift_bridge_serialize_pattern_fn serialize_pattern;
    rift_bridge_deserialize_pattern_fn deserialize_pattern;
    rift_bridge_run_bytecode_fn run_bytecode;
    rift_bridge_reset_matcher_fn reset_matcher;
} rift_bridge_api_t;

/**
 * @brief Bridge initialization parameters
 */
typedef struct rift_bridge_params {
    uint32_t max_patterns;    /**< Maximum number of patterns */
    uint32_t max_matchers;    /**< Maximum number of matchers */
    size_t error_buffer_size; /**< Size of error buffer */
    void *user_data;          /**< User data pointer */
} rift_bridge_params_t;

/**
 * @brief Get the registered bridge API implementation
 *
 * @return The current bridge API implementation
 */
const rift_bridge_api_t *rift_get_bridge_api(void);

/**
 * @brief Register a bridge API implementation
 *
 * @param api The implementation to register
 * @return true if successful, false otherwise
 */
bool rift_register_bridge_implementation(const rift_bridge_api_t *api);

/**
 * @brief Create a new bridge context
 *
 * @param params Initialization parameters
 * @return New context or NULL on failure
 */
rift_bridge_context_t *rift_bridge_context_create(const rift_bridge_params_t *params);

/**
 * @brief Free a bridge context
 *
 * @param context The context to free
 */
void rift_bridge_context_free(rift_bridge_context_t *context);

/**
 * @brief Get user data from bridge context
 *
 * @param context The bridge context
 * @return User data pointer
 */
void *rift_bridge_context_get_user_data(rift_bridge_context_t *context);

/**
 * @brief Set user data for bridge context
 *
 * @param context The bridge context
 * @param user_data The user data to set
 */
void rift_bridge_context_set_user_data(rift_bridge_context_t *context, void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_BRIDGE_API_H */