/**
 * @file error.h
 * @brief Centralized error handling for the LibRift library
 *
 * This file defines the error handling facilities for the LibRift library,
 * providing standardized error codes and reporting mechanisms.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#ifndef LIBRIFT_CORE_ERROR_H
#define LIBRIFT_CORE_ERROR_H


#ifdef __cplusplus
extern "C" {
#endif

/* Maximum lengths for error information */
#define RIFT_ERROR_MAX_FILE_LENGTH 128
#define RIFT_ERROR_MAX_MESSAGE_LENGTH 256

/**
 * @brief Status codes for LibRift operations
 */
typedef enum rift_status {
    RIFT_OK = 0,                      /**< Operation succeeded */
    RIFT_ERROR_UNKNOWN,               /**< Unknown error */
    RIFT_ERROR_INTERNAL,              /**< Internal error */
    RIFT_ERROR_INVALID_PARAMETER,     /**< Invalid parameter */
    RIFT_ERROR_MEMORY_ALLOCATION,     /**< Memory allocation failed */
    RIFT_ERROR_BUFFER_OVERFLOW,       /**< Buffer overflow */
    RIFT_ERROR_NOT_IMPLEMENTED,       /**< Feature not implemented */
    RIFT_ERROR_IO,                    /**< I/O error */
    RIFT_ERROR_FORMAT,                /**< Format error */
    RIFT_ERROR_SYNTAX,                /**< Syntax error */
    RIFT_ERROR_INVALID_STATE,         /**< Invalid state */
    RIFT_ERROR_TIMEOUT,               /**< Operation timed out */
    RIFT_ERROR_LIMIT_EXCEEDED,        /**< Resource limit exceeded */
    RIFT_ERROR_INVALID_REGEX,         /**< Invalid regular expression */
    RIFT_ERROR_REGEX_COMPILE,         /**< Regular expression compilation failed */
    RIFT_ERROR_REGEX_MATCH,           /**< Regular expression matching failed */
    RIFT_ERROR_REGEX_BACKTRACK_LIMIT, /**< Regular expression backtracking limit exceeded */
    RIFT_ERROR_REGEX_INVALID_FLAG,    /**< Invalid regular expression flag */
    RIFT_ERROR_REGEX_GROUP_NOT_FOUND, /**< Capture group not found */
    RIFT_ERROR_REGEX_TOO_MANY_GROUPS  /**< Too many capture groups */
} rift_status_t;

/**
 * @brief Error information structure
 */
typedef struct rift_error_info {
    rift_status_t status;                        /**< Error status code */
    int line;                                    /**< Line number where the error occurred */
    char file[RIFT_ERROR_MAX_FILE_LENGTH];       /**< File where the error occurred */
    char message[RIFT_ERROR_MAX_MESSAGE_LENGTH]; /**< Detailed error message */
} rift_error_info_t;

/**
 * @brief Error callback function type
 */
typedef void (*rift_error_callback_t)(const rift_error_info_t *info);

/**
 * @brief Set the current error information
 *
 * @param status Error status code
 * @param line Line number where the error occurred
 * @param file Source file where the error occurred
 * @param message Error message format string
 * @param ... Additional arguments for formatting
 */
void rift_error_set(rift_status_t status, int line, const char *file, const char *message, ...);

/**
 * @brief Get the current error information
 *
 * @return Pointer to the current error information
 */
const rift_error_info_t *rift_error_get(void);

/**
 * @brief Clear the current error
 */
void rift_error_clear(void);

/**
 * @brief Register an error callback function
 *
 * @param callback Function to call when an error occurs
 * @return Previous callback function
 */
rift_error_callback_t rift_error_set_callback(rift_error_callback_t callback);

/**
 * @brief Get the description for a status code
 *
 * @param status Status code
 * @return Description string
 */
const char *rift_error_status_string(rift_status_t status);

/**
 * @brief Format an error into a string
 *
 * @param info Error information to format
 * @param buffer Buffer to store the formatted error
 * @param buffer_size Size of the buffer
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t rift_error_format(const rift_error_info_t *info, char *buffer, size_t buffer_size);

/**
 * @brief Check if a status code indicates an error
 *
 * @param status Status code to check
 * @return true if the status indicates an error, false otherwise
 */
bool rift_error_is_error(rift_status_t status);

/**
 * @brief Format the current error into a string
 *
 * @param buffer Buffer to store the formatted error
 * @param buffer_size Size of the buffer
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t rift_error_current_format(char *buffer, size_t buffer_size);

/**
 * @brief Log an error message with a status code
 *
 * @param status Status code
 * @param line Line number
 * @param file Source file
 * @param message Error message format
 * @param ... Variable arguments for the error message
 * @return The status code passed in
 */
rift_status_t rift_error_log(rift_status_t status, int line, const char *file, const char *message,
                             ...);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_CORE_ERROR_H */