onfig/config.h"/a #include "core/errors/regex_error.h"
librift/config.h
tmp/librift_analysis/librift-4933472-W/backup/v1/src/core/config.h
tmp/librift_analysis/librift-4933472-W/backup/v1/src/core copy/config.h
tmp/librift_analysis/librift-4933472-W/src/core/config/config.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/core/config.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/core copy/config.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/config/config.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/core/config.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/core copy/config.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/config/config.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
librift/config.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
onfig/config.h"/a #include "core/errors/regex_error.h"
/**
 * @file config.c
 * @brief Configuration management for the LibRift library
 *
 * This file implements the configuration management facilities for the
 * LibRift library, including support for regex engine parameters.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/config/config.h
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
onfig/config.h"/a #include "core/errors/regex_error.h"

onfig/config.h"/a #include "core/errors/regex_error.h"
/**
 * @brief Default configuration values
 */
static const rift_config_t DEFAULT_CONFIG = {
    .regex = {.max_pattern_length = 4096,
              .max_states = 10000,
              .max_backtrack_depth = 10000,
              .default_timeout_ms = 5000,
              .optimize_automaton = true,
              .use_dfa_when_possible = true,
              .enable_rift_syntax = true,
              .max_capture_groups = 100},
    .memory = {.allocation_limit = 0, /* 0 means no limit */
               .use_custom_allocator = false,
               .custom_malloc = NULL,
               .custom_realloc = NULL,
               .custom_free = NULL}};

onfig/config.h"/a #include "core/errors/regex_error.h"
/**
 * @brief Global configuration instance
 */
static rift_config_t global_config;

onfig/config.h"/a #include "core/errors/regex_error.h"
/**
 * @brief Initialization flag to track if the config has been initialized
 */
static bool config_initialized = false;

onfig/config.h"/a #include "core/errors/regex_error.h"
/**
 * @brief Initialize the configuration system
 *
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t
rift_config_initialize(void)
{
    if (config_initialized) {
        return RIFT_OK;
    }

    /* Copy default configuration to global instance */
    memcpy(&global_config, &DEFAULT_CONFIG, sizeof(rift_config_t));

    config_initialized = true;
    return RIFT_OK;
}

onfig/config.h"/a #include "core/errors/regex_error.h"
/**
 * @brief Cleanup the configuration system
 *
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t
rift_config_cleanup(void)
{
    if (!config_initialized) {
        return RIFT_OK;
    }

    /* No dynamic resources to clean up at the moment */

    config_initialized = false;
    return RIFT_OK;
}

onfig/config.h"/a #include "core/errors/regex_error.h"
/**
 * @brief Get the global configuration
 *
 * @return Pointer to the global configuration instance
 */
const rift_config_t *
rift_config_get(void)
{
    if (!config_initialized) {
        rift_config_initialize();
    }

    return &global_config;
}

onfig/config.h"/a #include "core/errors/regex_error.h"
/**
 * @brief Set configuration values
 *
 * @param config Pointer to configuration values to set
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t
rift_config_set(const rift_config_t *config)
{
    if (!config) {
        return RIFT_ERROR_INVALID_PARAMETER;
    }

    if (!config_initialized) {
        rift_config_initialize();
    }

    /* Copy the provided configuration to the global instance */
    memcpy(&global_config, config, sizeof(rift_config_t));

    return RIFT_OK;
}

onfig/config.h"/a #include "core/errors/regex_error.h"
/**
 * @brief Reset configuration to default values
 *
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t
rift_config_reset(void)
{
    if (!config_initialized) {
        return rift_config_initialize();
    }

    /* Copy default configuration to global instance */
    memcpy(&global_config, &DEFAULT_CONFIG, sizeof(rift_config_t));

    return RIFT_OK;
}

onfig/config.h"/a #include "core/errors/regex_error.h"
/**
 * @brief Get a specific regex configuration parameter
 *
 * @param param The parameter to get
 * @param value Pointer to store the value
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t
rift_config_get_regex_param(rift_regex_config_param_t param, void *value)
{
    if (!value) {
        return RIFT_ERROR_INVALID_PARAMETER;
    }

    if (!config_initialized) {
        rift_config_initialize();
    }

    switch (param) {
    case RIFT_REGEX_PARAM_MAX_PATTERN_LENGTH:
        *(size_t *)value = global_config.regex.max_pattern_length;
        break;
    case RIFT_REGEX_PARAM_MAX_STATES:
        *(size_t *)value = global_config.regex.max_states;
        break;
    case RIFT_REGEX_PARAM_MAX_BACKTRACK_DEPTH:
        *(size_t *)value = global_config.regex.max_backtrack_depth;
        break;
    case RIFT_REGEX_PARAM_DEFAULT_TIMEOUT_MS:
        *(uint32_t *)value = global_config.regex.default_timeout_ms;
        break;
    case RIFT_REGEX_PARAM_OPTIMIZE_AUTOMATON:
        *(bool *)value = global_config.regex.optimize_automaton;
        break;
    case RIFT_REGEX_PARAM_USE_DFA_WHEN_POSSIBLE:
        *(bool *)value = global_config.regex.use_dfa_when_possible;
        break;
    case RIFT_REGEX_PARAM_ENABLE_RIFT_SYNTAX:
        *(bool *)value = global_config.regex.enable_rift_syntax;
        break;
    case RIFT_REGEX_PARAM_MAX_CAPTURE_GROUPS:
        *(size_t *)value = global_config.regex.max_capture_groups;
        break;
    default:
        return RIFT_ERROR_INVALID_PARAMETER;
    }

    return RIFT_OK;
}

onfig/config.h"/a #include "core/errors/regex_error.h"
/**
 * @brief Set a specific regex configuration parameter
 *
 * @param param The parameter to set
 * @param value Pointer to the value
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t
rift_config_set_regex_param(rift_regex_config_param_t param, const void *value)
{
    if (!value) {
        return RIFT_ERROR_INVALID_PARAMETER;
    }

    if (!config_initialized) {
        rift_config_initialize();
    }

    switch (param) {
    case RIFT_REGEX_PARAM_MAX_PATTERN_LENGTH:
        global_config.regex.max_pattern_length = *(const size_t *)value;
        break;
    case RIFT_REGEX_PARAM_MAX_STATES:
        global_config.regex.max_states = *(const size_t *)value;
        break;
    case RIFT_REGEX_PARAM_MAX_BACKTRACK_DEPTH:
        global_config.regex.max_backtrack_depth = *(const size_t *)value;
        break;
    case RIFT_REGEX_PARAM_DEFAULT_TIMEOUT_MS:
        global_config.regex.default_timeout_ms = *(const uint32_t *)value;
        break;
    case RIFT_REGEX_PARAM_OPTIMIZE_AUTOMATON:
        global_config.regex.optimize_automaton = *(const bool *)value;
        break;
    case RIFT_REGEX_PARAM_USE_DFA_WHEN_POSSIBLE:
        global_config.regex.use_dfa_when_possible = *(const bool *)value;
        break;
    case RIFT_REGEX_PARAM_ENABLE_RIFT_SYNTAX:
        global_config.regex.enable_rift_syntax = *(const bool *)value;
        break;
    case RIFT_REGEX_PARAM_MAX_CAPTURE_GROUPS:
        global_config.regex.max_capture_groups = *(const size_t *)value;
        break;
    default:
        return RIFT_ERROR_INVALID_PARAMETER;
    }

    return RIFT_OK;
}

onfig/config.h"/a #include "core/errors/regex_error.h"
/**
 * @brief Set custom memory allocator functions
 *
 * @param malloc_fn Custom malloc function
 * @param realloc_fn Custom realloc function
 * @param free_fn Custom free function
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t
rift_config_set_memory_allocator(rift_malloc_func_t malloc_fn, rift_realloc_func_t realloc_fn,
                                 rift_free_func_t free_fn)
{
    if (!malloc_fn || !realloc_fn || !free_fn) {
        return RIFT_ERROR_INVALID_PARAMETER;
    }

    if (!config_initialized) {
        rift_config_initialize();
    }

    global_config.memory.use_custom_allocator = true;
    global_config.memory.custom_malloc = malloc_fn;
    global_config.memory.custom_realloc = realloc_fn;
    global_config.memory.custom_free = free_fn;

    return RIFT_OK;
}

onfig/config.h"/a #include "core/errors/regex_error.h"
/**
 * @brief Get the configuration as a JSON string
 *
 * @param buffer Buffer to write the JSON string
 * @param buffer_size Size of the buffer
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t
rift_config_to_json(char *buffer, size_t buffer_size)
{
    if (!buffer || buffer_size == 0) {
        return RIFT_ERROR_INVALID_PARAMETER;
    }

    if (!config_initialized) {
        rift_config_initialize();
    }

    int written =
        snprintf(buffer, buffer_size,
                 "{\n"
                 "  \"regex\": {\n"
                 "    \"max_pattern_length\": %zu,\n"
                 "    \"max_states\": %zu,\n"
                 "    \"max_backtrack_depth\": %zu,\n"
                 "    \"default_timeout_ms\": %u,\n"
                 "    \"optimize_automaton\": %s,\n"
                 "    \"use_dfa_when_possible\": %s,\n"
                 "    \"enable_rift_syntax\": %s,\n"
                 "    \"max_capture_groups\": %zu\n"
                 "  },\n"
                 "  \"memory\": {\n"
                 "    \"allocation_limit\": %zu,\n"
                 "    \"use_custom_allocator\": %s\n"
                 "  }\n"
                 "}",
                 global_config.regex.max_pattern_length, global_config.regex.max_states,
                 global_config.regex.max_backtrack_depth, global_config.regex.default_timeout_ms,
                 global_config.regex.optimize_automaton ? "true" : "false",
                 global_config.regex.use_dfa_when_possible ? "true" : "false",
                 global_config.regex.enable_rift_syntax ? "true" : "false",
                 global_config.regex.max_capture_groups, global_config.memory.allocation_limit,
                 global_config.memory.use_custom_allocator ? "true" : "false");

    if (written < 0 || (size_t)written >= buffer_size) {
        return RIFT_ERROR_BUFFER_OVERFLOW;
    }

    return RIFT_OK;
}

onfig/config.h"/a #include "core/errors/regex_error.h"
/**
 * @brief Load configuration from a JSON string
 *
 * This is a simplified implementation that parses a basic JSON format.
 * In a production environment, you would use a proper JSON parser library.
 *
 * @param json JSON string containing configuration
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t
rift_config_from_json(const char *json)
{
    if (!json) {
        return RIFT_ERROR_INVALID_PARAMETER;
    }

    if (!config_initialized) {
        rift_config_initialize();
    }

    /*
     * Note: This is a placeholder for a proper JSON parser.
     * In a real implementation, you would use a JSON library to parse the string.
     * For now, we'll just indicate that this functionality is not implemented.
     */

    return RIFT_ERROR_NOT_IMPLEMENTED;
}
