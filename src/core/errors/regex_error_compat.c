/**
 * @file regex_error_compat.c
 * @brief Compatibility implementation between LibRift core errors and regex errors
 *
 * This file implements the compatibility functions declared in regex_error_compat.h
 * for converting between the core error system and the regex-specific error system.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/errors/regex_error_compat.h
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "core/errors/error.h"

tmp/librift_analysis/librift-4933472-W/backup/v1/src/errors/regex_error_compat.h
tmp/librift_analysis/librift-4933472-W/backup/v2/errors/regex_error_compat.h
tmp/librift_analysis/librift-4933472-W/src/core/errors/regex_error_compat.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/errors/regex_error_compat.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v2/errors/regex_error_compat.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/errors/regex_error_compat.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/errors/regex_error_compat.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v2/errors/regex_error_compat.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/errors/regex_error_compat.h"

/**
 * @brief Set a regex error with a code and message
 *
 * @param error The regex error to set (can be NULL)
 * @param code The error code
 * @param message The error message
 */
void
rift_regex_error_set(rift_regex_error_t *error, int code, const char *message)
{
    if (!error) {
        return;
    }

    rift_regex_error_set_with_message(error, code, message);
}

/**
 * @brief Convert a regex error code to a core status code
 *
 * @param error_code The regex error code
 * @return The equivalent core status code
 */
rift_status_t
rift_regex_error_to_status(rift_regex_error_code_t error_code)
{
    switch (error_code) {
    case RIFT_REGEX_ERROR_NONE:
        return RIFT_OK;
    case RIFT_REGEX_ERROR_MEMORY:
        return RIFT_ERROR_MEMORY_ALLOCATION;
    case RIFT_REGEX_ERROR_INVALID_PARAMETER:
        return RIFT_ERROR_INVALID_PARAMETER;
    case RIFT_REGEX_ERROR_INTERNAL:
        return RIFT_ERROR_INTERNAL;
    case RIFT_REGEX_ERROR_BUFFER_OVERFLOW:
        return RIFT_ERROR_BUFFER_OVERFLOW;
    case RIFT_REGEX_ERROR_LIMIT_EXCEEDED:
        return RIFT_ERROR_LIMIT_EXCEEDED;
    case RIFT_REGEX_ERROR_TIMEOUT:
        return RIFT_ERROR_TIMEOUT;
    default:
        return RIFT_ERROR_UNKNOWN;
    }
}

/**
 * @brief Convert a core status code to a regex error code
 *
 * @param status The core status code
 * @return The equivalent regex error code
 */
rift_regex_error_code_t
rift_status_to_regex_error(rift_status_t status)
{
    switch (status) {
    case RIFT_OK:
        return RIFT_REGEX_ERROR_NONE;
    case RIFT_ERROR_MEMORY_ALLOCATION:
        return RIFT_REGEX_ERROR_MEMORY;
    case RIFT_ERROR_INVALID_PARAMETER:
        return RIFT_REGEX_ERROR_INVALID_PARAMETER;
    case RIFT_ERROR_INTERNAL:
        return RIFT_REGEX_ERROR_INTERNAL;
    case RIFT_ERROR_BUFFER_OVERFLOW:
        return RIFT_REGEX_ERROR_BUFFER_OVERFLOW;
    case RIFT_ERROR_LIMIT_EXCEEDED:
        return RIFT_REGEX_ERROR_LIMIT_EXCEEDED;
    case RIFT_ERROR_TIMEOUT:
        return RIFT_REGEX_ERROR_TIMEOUT;
    default:
        return RIFT_REGEX_ERROR_INTERNAL;
    }
}

/**
 * @brief Convert a regex error to a core error
 *
 * @param regex_error The regex error
 * @param core_error The core error to fill (can be NULL)
 * @return The core status code
 */
rift_status_t
rift_regex_error_to_core_error(const rift_regex_error_t *regex_error, rift_error_info_t *core_error)
{
    if (!regex_error) {
        return RIFT_ERROR_INVALID_PARAMETER;
    }

    rift_status_t status = rift_regex_error_to_status(regex_error->code);

    if (core_error) {
        core_error->status = status;
        strncpy(core_error->message, regex_error->message, RIFT_ERROR_MAX_MESSAGE_LENGTH - 1);
        core_error->message[RIFT_ERROR_MAX_MESSAGE_LENGTH - 1] = '\0';
        // Other fields would typically be filled by the calling function
    }

    return status;
}

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
rift_status_t
rift_regex_set_both_errors(rift_regex_error_t *regex_error, rift_regex_error_code_t error_code,
                           const char *file, int line, const char *format, ...)
{
    char message[RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH];

    // Format the message
    if (format) {
        va_list args;
        va_start(args, format);
        vsnprintf(message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH, format, args);
        va_end(args);
    } else {
        const char *error_string = rift_regex_error_code_to_string(error_code);
        if (error_string) {
            strncpy(message, error_string, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
        } else {
            strncpy(message, "Unknown error", RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
        }
        message[RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1] = '\0';
    }

    // Set the regex error
    if (regex_error) {
        rift_regex_error_set_with_message(regex_error, error_code, message);
    }

    // Set the global core error
    rift_status_t status = rift_regex_error_to_status(error_code);
    rift_error_set(status, line, file, message);

    return status;
}
