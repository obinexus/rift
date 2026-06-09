/**
 * @file regex_error_compat.h
 * @brief Compatibility layer between LibRift core errors and regex errors
 *
 * This file provides compatibility functions and macros to convert between
 * the core error system and the regex-specific error system.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/errors/error.h"
#include "core/errors/regex_error.h"
#ifndef LIBRIFT_REGEX_ERRORS_REGEX_ERROR_COMPAT_H
#define LIBRIFT_REGEX_ERRORS_REGEX_ERROR_COMPAT_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set a regex error with a code and message
 *
 * @param error The regex error to set (can be NULL)
 * @param code The error code
 * @param message The error message
 */
void rift_regex_error_set(rift_regex_error_t *error, int code, const char *message);

/**
 * @brief Convert a regex error code to a core status code
 *
 * @param error_code The regex error code
 * @return The equivalent core status code
 */
rift_status_t rift_regex_error_to_status(rift_regex_error_code_t error_code);

/**
 * @brief Convert a core status code to a regex error code
 *
 * @param status The core status code
 * @return The equivalent regex error code
 */
rift_regex_error_code_t rift_status_to_regex_error(rift_status_t status);

/**
 * @brief Convert a regex error to a core error
 *
 * @param regex_error The regex error
 * @param core_error The core error to fill (can be NULL)
 * @return The core status code
 */
rift_status_t rift_regex_error_to_core_error(const rift_regex_error_t *regex_error,
                                             rift_error_info_t *core_error);

/**
 * @brief Set both a regex error and the global core error
 *
 * @param regex_error The regex error to set (can be NULL)
 * @param error_code The regex error code
 * @param file The source file where the error occurred
 * @param line The line number where the error occurred
 * @param format The format string for the error message
 * @param ... Variable arguments for the format string
 * @return The equivalent core status code
 */
rift_status_t rift_regex_set_both_errors(rift_regex_error_t *regex_error,
                                         rift_regex_error_code_t error_code, const char *file,
                                         int line, const char *format, ...);

/* Convenience macro for setting both errors and returning from a function */
#define RIFT_REGEX_RETURN_BOTH_ERRORS(regex_err, code_val, status_val, fmt, ...)                   \
    do {                                                                                           \
        rift_regex_set_both_errors(regex_err, code_val, __FILE__, __LINE__, fmt, ##__VA_ARGS__);   \
        return status_val;                                                                         \
    } while (0)

/* Convenience macro for setting both errors and returning false */
#define RIFT_REGEX_RETURN_BOTH_ERRORS_FALSE(regex_err, code_val, fmt, ...)                         \
    RIFT_REGEX_RETURN_BOTH_ERRORS(regex_err, code_val, false, fmt, ##__VA_ARGS__)

/* Convenience macro for setting both errors and returning NULL */
#define RIFT_REGEX_RETURN_BOTH_ERRORS_NULL(regex_err, code_val, fmt, ...)                          \
    RIFT_REGEX_RETURN_BOTH_ERRORS(regex_err, code_val, NULL, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_ERRORS_REGEX_ERROR_COMPAT_H */