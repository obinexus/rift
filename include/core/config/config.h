/**
 * @file config.h
 * @brief Configuration management for the LibRift library
 *
 * This file defines the configuration structures and functions for the
 * LibRift library, including support for regex engine parameters.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "core/errors/error.h"  // For status codes
#include "core/memory/memory.h" // For memory functions
#ifndef LIBRIFT_CORE_CONFIG_H
#define LIBRIFT_CORE_CONFIG_H



#ifdef __cplusplus
extern "C" {
#endif



/**
 * @brief Configuration parameters for the regex engine
 */
typedef enum rift_regex_config_param {
    RIFT_REGEX_PARAM_MAX_PATTERN_LENGTH,    /**< Maximum length of regex patterns */
    RIFT_REGEX_PARAM_MAX_STATES,            /**< Maximum number of states in automaton */
    RIFT_REGEX_PARAM_MAX_BACKTRACK_DEPTH,   /**< Maximum backtracking depth */
    RIFT_REGEX_PARAM_DEFAULT_TIMEOUT_MS,    /**< Default timeout in milliseconds */
    RIFT_REGEX_PARAM_OPTIMIZE_AUTOMATON,    /**< Whether to optimize automaton */
    RIFT_REGEX_PARAM_USE_DFA_WHEN_POSSIBLE, /**< Use DFA when possible */
    RIFT_REGEX_PARAM_ENABLE_RIFT_SYNTAX,    /**< Enable LibRift r'' syntax */
    RIFT_REGEX_PARAM_MAX_CAPTURE_GROUPS     /**< Maximum number of capture groups */
} rift_regex_config_param_t;

/**
 * @brief Regex engine configuration structure
 */
typedef struct rift_regex_config {
    size_t max_pattern_length;   /**< Maximum length of regex patterns */
    size_t max_states;           /**< Maximum number of states in automaton */
    size_t max_backtrack_depth;  /**< Maximum backtracking depth */
    uint32_t default_timeout_ms; /**< Default timeout in milliseconds */
    bool optimize_automaton;     /**< Whether to optimize automaton */
    bool use_dfa_when_possible;  /**< Use DFA when possible */
    bool enable_rift_syntax;     /**< Enable LibRift r'' syntax */
    size_t max_capture_groups;   /**< Maximum number of capture groups */
} rift_regex_config_t;

/**
 * @brief Memory management configuration structure
 */
typedef struct rift_memory_config {
    size_t allocation_limit;            /**< Maximum memory allocation (0 = no limit) */
    bool use_custom_allocator;          /**< Whether to use custom allocator functions */
    rift_malloc_func_t custom_malloc;   /**< Custom malloc function */
    rift_realloc_func_t custom_realloc; /**< Custom realloc function */
    rift_free_func_t custom_free;       /**< Custom free function */
} rift_memory_config_t;

/**
 * @brief Global configuration structure
 */
typedef struct rift_config {
    rift_regex_config_t regex;   /**< Regex engine configuration */
    rift_memory_config_t memory; /**< Memory management configuration */
} rift_config_t;

/**
 * @brief Initialize the configuration system
 *
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t rift_config_initialize(void);

/**
 * @brief Cleanup the configuration system
 *
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t rift_config_cleanup(void);

/**
 * @brief Get the global configuration
 *
 * @return Pointer to the global configuration instance
 */
const rift_config_t *rift_config_get(void);

/**
 * @brief Set configuration values
 *
 * @param config Pointer to configuration values to set
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t rift_config_set(const rift_config_t *config);

/**
 * @brief Reset configuration to default values
 *
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t rift_config_reset(void);

/**
 * @brief Get a specific regex configuration parameter
 *
 * @param param The parameter to get
 * @param value Pointer to store the value
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t rift_config_get_regex_param(rift_regex_config_param_t param, void *value);

/**
 * @brief Set a specific regex configuration parameter
 *
 * @param param The parameter to set
 * @param value Pointer to the value
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t rift_config_set_regex_param(rift_regex_config_param_t param, const void *value);

/**
 * @brief Set custom memory allocator functions
 *
 * @param malloc_fn Custom malloc function
 * @param realloc_fn Custom realloc function
 * @param free_fn Custom free function
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t rift_config_set_memory_allocator(rift_malloc_func_t malloc_fn,
                                               rift_realloc_func_t realloc_fn,
                                               rift_free_func_t free_fn);

/**
 * @brief Get the configuration as a JSON string
 *
 * @param buffer Buffer to write the JSON string
 * @param buffer_size Size of the buffer
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t rift_config_to_json(char *buffer, size_t buffer_size);

/**
 * @brief Load configuration from a JSON string
 *
 * @param json JSON string containing configuration
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t rift_config_from_json(const char *json);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_CORE_CONFIG_H */