/**
 * @file regex_error.h
 * @brief Error handling for the LibRift regex engine
 *
 * This file defines the error codes, error structure, and functions for error
 * handling in the LibRift regex engine. It provides a consistent interface
 * for reporting and handling errors across the regex components.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef LIBRIFT_REGEX_ERRORS_REGEX_ERROR_H
#define LIBRIFT_REGEX_ERRORS_REGEX_ERROR_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum length of an error message
 */
#ifndef RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH
#define RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH 256
#endif

/**
 * @brief Maximum length of an error buffer
 */
#ifndef RIFT_REGEX_MAX_ERROR_LENGTH
#define RIFT_REGEX_MAX_ERROR_LENGTH 256
#endif

#define rift_regex_error_set_macro(error, code, format, ...)                                       \
    rift_regex_error_set_formatted(error, (rift_regex_error_code_t)(code), format, ##__VA_ARGS__)

#define rift_regex_error_return_macro(error, code, format, ...)                                    \
    do {                                                                                           \
        rift_regex_error_set_macro(error, code, format, ##__VA_ARGS__);                            \
        return false;                                                                              \
    } while (0)

#define rift_regex_error_return_null_macro(error, code, format, ...)                               \
    do {                                                                                           \
        rift_regex_error_set_macro(error, code, format, ##__VA_ARGS__);                            \
        return NULL;                                                                               \
    } while (0)

/* Convenience macro for returning with an error */
#define RIFT_REGEX_RETURN_ERROR(err, code_val, fmt, ...)                                           \
    do {                                                                                           \
        RIFT_REGEX_SET_ERROR(err, code_val, fmt, ##__VA_ARGS__);                                   \
        return false;                                                                              \
    } while (0)

/* Convenience macro for returning NULL with an error */
#define RIFT_REGEX_RETURN_NULL_ERROR(err, code_val, fmt, ...)                                      \
    do {                                                                                           \
        RIFT_REGEX_SET_ERROR(err, code_val, fmt, ##__VA_ARGS__);                                   \
        return NULL;                                                                               \
    } while (0)

/* Convenience macro for checking memory allocation */
#define RIFT_REGEX_CHECK_MEMORY(ptr, err, fmt, ...)                                                \
    do {                                                                                           \
        if (!(ptr)) {                                                                              \
            RIFT_REGEX_RETURN_NULL_ERROR(err, RIFT_REGEX_ERROR_MEMORY, fmt, ##__VA_ARGS__);        \
        }                                                                                          \
    } while (0)
#define rift_regex_error_set_macro(error, code, format, ...)                                       \
    do {                                                                                           \
        if (error) {                                                                               \
            (error)->code = (code);                                                                \
            snprintf((error)->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH, (format),              \
                     __VA_ARGS__);                                                                 \
        }                                                                                          \
    } while (0)

#define rift_regex_error_return_macro(error, code, format, ...)                                    \
    do {                                                                                           \
        rift_regex_error_set_macro(error, code, format, __VA_ARGS__);                              \
        return NULL;                                                                               \
    } while (0)

#define rift_regex_error_return_null_macro(error, code, format, ...)                               \
    do {                                                                                           \
        rift_regex_error_set_macro(error, code, format, __VA_ARGS__);                              \
        return NULL;                                                                               \
    } while (0)
/**
 * @brief Legacy error code alias for memory allocation failures
 */
#define RIFT_REGEX_ERROR_MEMORY_ALLOCATION RIFT_REGEX_ERROR_MEMORY

/**
 * @brief Regex-specific error codes
 */
typedef enum rift_regex_error_code {
    RIFT_REGEX_ERROR_NONE = 0,                /**< No error */
    RIFT_REGEX_ERROR_INTERNAL,                /**< Internal error */
    RIFT_REGEX_ERROR_INVALID_PARAMETER,       /**< Invalid parameter */
    RIFT_REGEX_ERROR_MEMORY,                  /**< Memory allocation failed */
    RIFT_REGEX_ERROR_INVALID_AUTOMATON,       /**< Invalid automaton */
    RIFT_REGEX_ERROR_EMPTY_AUTOMATON,         /**< Empty automaton */
    RIFT_REGEX_ERROR_SYNTAX,                  /**< Syntax error */
    RIFT_REGEX_ERROR_UNSUPPORTED_FEATURE,     /**< Unsupported feature */
    RIFT_REGEX_ERROR_INVALID_QUANTIFIER,      /**< Invalid quantifier */
    RIFT_REGEX_ERROR_INVALID_ESCAPE,          /**< Invalid escape sequence */
    RIFT_REGEX_ERROR_INVALID_CHARACTER_CLASS, /**< Invalid character class */
    RIFT_REGEX_ERROR_INVALID_BACKREFERENCE,   /**< Invalid backreference */
    RIFT_REGEX_ERROR_UNBALANCED_PARENTHESES,  /**< Unbalanced parentheses */
    RIFT_REGEX_ERROR_UNBALANCED_BRACKETS,     /**< Unbalanced brackets */
    RIFT_REGEX_ERROR_TRAILING_BACKSLASH,      /**< Trailing backslash */
    RIFT_REGEX_ERROR_BACKTRACKING_LIMIT,      /**< Backtracking limit exceeded */
    RIFT_REGEX_ERROR_TIMEOUT,                 /**< Operation timed out */
    RIFT_REGEX_ERROR_UNSUPPORTED_OPERATION,   /**< Unsupported operation */
    RIFT_REGEX_ERROR_UNEXPECTED_TOKEN,        /**< Unexpected token during parsing */
    RIFT_REGEX_ERROR_LIMIT_EXCEEDED,          /**< Some limit exceeded (e.g., states) */
    RIFT_REGEX_ERROR_BUFFER_OVERFLOW,         /**< Buffer overflow error */
    RIFT_REGEX_ERROR_INVALID_ARGUMENT,        /**< Invalid argument */
    RIFT_REGEX_ERROR_CONVERSION_FAILED,       /**< Conversion failed */
    RIFT_REGEX_ERROR_MINIMIZATION_FAILED,     /**< Minimization failed */
    RIFT_REGEX_ERROR_INITIALIZATION_FAILED,   /**< Initialization failed error */
    RIFT_REGEX_ERROR_PARTITION_FAILED,        /**< Partition computation failed error */
    RIFT_REGEX_ERROR_REFINEMENT_FAILED        /**< Refinement failed error */
} rift_regex_error_code_t;

typedef struct rift_regex_error {
    rift_regex_error_code_t code; /**< Error code indicating the type of error */
    char message[RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH]; /**< Fixed-size message buffer */
    size_t position;                                   /**< Position where the error occurred */
} rift_regex_error_t;

/**
 * @brief Get a string description for a regex error code
 *
 * @param error_code The error code
 * @return A string describing the error
 */
const char *rift_regex_error_code_to_string(rift_regex_error_code_t error_code);

/**
 * @brief Get a descriptive string for a regex error code
 *
 * @param error The regex error code
 * @return A string describing the error
 */
const char *rift_regex_get_error_string(rift_regex_error_t error);

/**
 * @brief Initialize a regex error object
 *
 * @param error Pointer to the error object
 */
bool rift_regex_error_init(rift_regex_error_t *error);

/**
 * @brief Get a descriptive string for a regex error
 *
 * @param error The regex error structure
 * @return A string describing the error
 */
const char *rift_regex_get_error_string(rift_regex_error_t error);

/**
 * @brief Set an error object with a fixed message
 *
 * @param error Pointer to the error object (can be NULL)
 * @param code The error code
 * @param message The error message
 */
void rift_regex_error_set_with_message(rift_regex_error_t *error, int code, const char *message);

/**
 * @brief Set a formatted regex error message
 *
 * @param error The regex error object
 * @param code The error code
 * @param format The format string
 * @param ... Variable arguments for the format
 */
void rift_regex_error_set_formatted(rift_regex_error_t *error, rift_regex_error_code_t code,
                                    const char *format, ...);

/* Convenience macro for setting errors */
#define RIFT_REGEX_SET_ERROR(err, code_val, fmt, ...)                                              \
    do {                                                                                           \
        if (err) {                                                                                 \
            rift_regex_error_set_formatted(err, code_val, fmt, ##__VA_ARGS__);                     \
        }                                                                                          \
    } while (0)
/**
 * @brief Set an error object (main function, uses formatted version)
 *
 * @param error Pointer to the error object (can be NULL)
 * @param code The error code
 * @param format Format string for the error message
 * @param ... Variable arguments for the format string
 */
/**
 * @brief Clear an error object
 *
 * @param error Pointer to the error object (can be NULL)
 */
void rift_regex_error_clear(rift_regex_error_t *error);

/**
 * @brief Create a new error
 *
 * @param code The error code
 * @param message The error message
 * @return A new error object (must be freed by the caller)
 */
rift_regex_error_t *rift_regex_error_create(rift_regex_error_code_t code, const char *message);

/**
 * @brief Free an error object
 *
 * @param error Pointer to the error object (can be NULL)
 */
void rift_regex_error_free(rift_regex_error_t *error);

/**
 * @brief Format an error object to a string
 *
 * @param error Pointer to the error object
 * @param buffer Buffer to store the string
 * @param buffer_size Size of the buffer
 * @return true if successful, false otherwise
 */
bool rift_regex_error_format(const rift_regex_error_t *error, char *buffer, size_t buffer_size);

/**
 * @brief Check if an error exists
 *
 * @param error Pointer to the error object
 * @return true if the error code is not RIFT_REGEX_ERROR_NONE, false otherwise
 */
bool rift_regex_error_exists(const rift_regex_error_t *error);

/**
 * @brief Copy an error object
 *
 * @param dest Destination error object
 * @param src Source error object
 * @return true if successful, false otherwise
 */
bool rift_regex_error_copy(rift_regex_error_t *dest, const rift_regex_error_t *src);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_ERRORS_REGEX_ERROR_H */