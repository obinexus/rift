/**
 * @file pattern_extensions.c
 * @brief Implementation of pattern extension utilities
 *
 * This file implements the utilities for extending baseline patterns
 * defined in pattern_extensions.h.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/patterns/pattern_extensions.h
#include <string.h>
#include "core/memory/memory.h"
#include "core/patterns/baseline_patterns.h"
#include "librift/core/memory/memory.h"
#include "librift/patterns/baseline_patterns.h"
#include "librift/patterns/pattern_extensions.h"

tmp/librift_analysis/librift-4933472-W/backup/v1/src/patterns/pattern_extensions.h
tmp/librift_analysis/librift-4933472-W/backup/v2/patterns/pattern_extensions.h
tmp/librift_analysis/librift-4933472-W/src/core/patterns/pattern_extensions.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/patterns/pattern_extensions.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v2/patterns/pattern_extensions.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/patterns/pattern_extensions.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/patterns/pattern_extensions.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v2/patterns/pattern_extensions.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/patterns/pattern_extensions.h"


/**
 * @brief Get the string representation of a baseline pattern
 *
 * @param pattern_type The pattern type
 * @return The string representation of the pattern
 */
static const char *
get_pattern_string(rift_baseline_pattern_type_t pattern_type)
{
    switch (pattern_type) {
    case RIFT_BASELINE_PATTERN_EMPTY_STATE:
        return RIFT_PATTERN_EMPTY_STATE;
    case RIFT_BASELINE_PATTERN_WHITESPACE:
        return RIFT_PATTERN_WHITESPACE;
    case RIFT_BASELINE_PATTERN_WORD_BOUNDARY:
        return RIFT_PATTERN_WORD_BOUNDARY;
    case RIFT_BASELINE_PATTERN_IDENTIFIER:
        return RIFT_PATTERN_IDENTIFIER;
    case RIFT_BASELINE_PATTERN_NUMERIC:
        return RIFT_PATTERN_NUMERIC;
    case RIFT_BASELINE_PATTERN_STRING:
        return RIFT_PATTERN_STRING;
    case RIFT_BASELINE_PATTERN_COMMENT:
        return RIFT_PATTERN_COMMENT;
    case RIFT_BASELINE_PATTERN_OPERATOR:
        return RIFT_PATTERN_OPERATOR;
    case RIFT_BASELINE_PATTERN_FUNCTION:
        return RIFT_PATTERN_FUNCTION;
    case RIFT_BASELINE_PATTERN_STATE_TRANSITION:
        return RIFT_PATTERN_STATE_TRANSITION;
    default:
        return NULL;
    }
}

rift_regex_pattern_t *
rift_pattern_combine(const rift_baseline_pattern_type_t *pattern_types, size_t num_patterns,
                     rift_regex_flags_t flags, rift_regex_error_t *error)
{
    if (!pattern_types || num_patterns == 0) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
            snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                     "Invalid parameters: pattern_types=%p, num_patterns=%zu",
                     (void *)pattern_types, num_patterns);
        }
        return NULL;
    }

    /* Calculate the required buffer size */
    size_t buffer_size = 1; /* For null terminator */
    for (size_t i = 0; i < num_patterns; i++) {
        const char *pattern_str = get_pattern_string(pattern_types[i]);
        if (!pattern_str) {
            if (error) {
                error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
                snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                         "Invalid pattern type: %d", pattern_types[i]);
            }
            return NULL;
        }

        buffer_size += strlen(pattern_str) + 3; /* +3 for "|" and surrounding parentheses */
    }

    /* Allocate buffer for combined pattern */
    char *combined_pattern = (char *)rift_malloc(buffer_size);
    if (!combined_pattern) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_MEMORY;
            snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                     "Memory allocation failed");
        }
        return NULL;
    }

    /* Build the combined pattern */
    size_t offset = 0;
    strcpy(combined_pattern, "");

    for (size_t i = 0; i < num_patterns; i++) {
        const char *pattern_str = get_pattern_string(pattern_types[i]);

        /* Add separator if needed */
        if (i > 0) {
            strcat(combined_pattern, "|");
            offset += 1;
        }

        /* Add the pattern */
        strcat(combined_pattern, "(");
        strcat(combined_pattern, pattern_str);
        strcat(combined_pattern, ")");
        offset += strlen(pattern_str) + 2;
    }

    /* Compile the combined pattern */
    rift_regex_pattern_t *pattern =
        rift_regex_compile(combined_pattern, flags | RIFT_REGEX_FLAG_RIFT_SYNTAX, error);

    /* Free the temporary buffer */
    rift_free(combined_pattern);

    return pattern;
}

rift_regex_pattern_t *
rift_pattern_extend(rift_baseline_pattern_type_t base_type, const char *extension,
                    rift_regex_flags_t flags, rift_regex_error_t *error)
{
    const char *base_pattern = get_pattern_string(base_type);
    if (!base_pattern || !extension) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
            snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                     "Invalid parameters: base_pattern=%p, extension=%p", (void *)base_pattern,
                     (void *)extension);
        }
        return NULL;
    }

    /* Calculate the required buffer size */
    size_t base_len = strlen(base_pattern);
    size_t ext_len = strlen(extension);
    size_t buffer_size = base_len + ext_len + 3; /* +3 for parentheses and null terminator */

    /* Allocate buffer for extended pattern */
    char *extended_pattern = (char *)rift_malloc(buffer_size);
    if (!extended_pattern) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_MEMORY;
            snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                     "Memory allocation failed");
        }
        return NULL;
    }

    /* Build the extended pattern */
    snprintf(extended_pattern, buffer_size, "(%s%s)", base_pattern, extension);

    /* Compile the extended pattern */
    rift_regex_pattern_t *pattern =
        rift_regex_compile(extended_pattern, flags | RIFT_REGEX_FLAG_RIFT_SYNTAX, error);

    /* Free the temporary buffer */
    rift_free(extended_pattern);

    return pattern;
}

rift_regex_pattern_t *
rift_pattern_sequence(const rift_baseline_pattern_type_t *pattern_types, size_t num_patterns,
                      rift_regex_flags_t flags, rift_regex_error_t *error)
{
    if (!pattern_types || num_patterns == 0) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
            snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                     "Invalid parameters: pattern_types=%p, num_patterns=%zu",
                     (void *)pattern_types, num_patterns);
        }
        return NULL;
    }

    /* Calculate the required buffer size */
    size_t buffer_size = 1; /* For null terminator */
    for (size_t i = 0; i < num_patterns; i++) {
        const char *pattern_str = get_pattern_string(pattern_types[i]);
        if (!pattern_str) {
            if (error) {
                error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
                snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                         "Invalid pattern type: %d", pattern_types[i]);
            }
            return NULL;
        }

        buffer_size += strlen(pattern_str) + 2; /* +2 for surrounding parentheses */
    }

    /* Allocate buffer for sequenced pattern */
    char *sequenced_pattern = (char *)rift_malloc(buffer_size);
    if (!sequenced_pattern) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_MEMORY;
            snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                     "Memory allocation failed");
        }
        return NULL;
    }

    /* Build the sequenced pattern */
    size_t offset = 0;
    strcpy(sequenced_pattern, "");

    for (size_t i = 0; i < num_patterns; i++) {
        const char *pattern_str = get_pattern_string(pattern_types[i]);

        /* Add the pattern */
        strcat(sequenced_pattern, "(");
        strcat(sequenced_pattern, pattern_str);
        strcat(sequenced_pattern, ")");
        offset += strlen(pattern_str) + 2;
    }

    /* Compile the sequenced pattern */
    rift_regex_pattern_t *pattern =
        rift_regex_compile(sequenced_pattern, flags | RIFT_REGEX_FLAG_RIFT_SYNTAX, error);

    /* Free the temporary buffer */
    rift_free(sequenced_pattern);

    return pattern;
}

/**
 * @brief Find a pattern name placeholder in a template string
 *
 * @param template The template string
 * @param pos Starting position to search from
 * @param start_pos Pointer to store start position of placeholder
 * @param end_pos Pointer to store end position of placeholder
 * @param pattern_name Buffer to store the extracted pattern name
 * @param name_buffer_size Size of the pattern_name buffer
 * @return true if a placeholder was found, false otherwise
 */
static bool
find_pattern_placeholder(const char *template, size_t pos, size_t *start_pos, size_t *end_pos,
                         char *pattern_name, size_t name_buffer_size)
{
    const char *placeholder_start = strstr(template + pos, "{{");
    if (!placeholder_start) {
        return false;
    }

    const char *placeholder_end = strstr(placeholder_start, "}}");
    if (!placeholder_end) {
        return false;
    }

    *start_pos = placeholder_start - template;
    *end_pos = placeholder_end + 2 - template; /* +2 for the }} */

    /* Extract the pattern name (remove {{ and }}) */
    size_t name_start = *start_pos + 2;
    size_t name_length = *end_pos - name_start - 2;
    if (name_length >= name_buffer_size) {
        name_length = name_buffer_size - 1;
    }

    strncpy(pattern_name, template + name_start, name_length);
    pattern_name[name_length] = '\0';

    /* Trim whitespace */
    char *start = pattern_name;
    char *end = pattern_name + strlen(pattern_name) - 1;

    while (isspace(*start))
        start++;
    while (end > start && isspace(*end))
        end--;

    *(end + 1) = '\0';
    if (start != pattern_name) {
        memmove(pattern_name, start, strlen(start) + 1);
    }

    return true;
}

/**
 * @brief Get pattern type from pattern name
 *
 * @param pattern_name The name of the pattern
 * @return The corresponding pattern type or -1 if not found
 */
static int
get_pattern_type_from_name(const char *pattern_name)
{
    /* Convert to uppercase for comparison */
    char uppercase_name[128];
    size_t i;

    for (i = 0; i < sizeof(uppercase_name) - 1 && pattern_name[i]; i++) {
        uppercase_name[i] = toupper(pattern_name[i]);
    }
    uppercase_name[i] = '\0';

    /* Map names to types */
    if (strcmp(uppercase_name, "EMPTY_STATE") == 0) {
        return RIFT_BASELINE_PATTERN_EMPTY_STATE;
    } else if (strcmp(uppercase_name, "WHITESPACE") == 0) {
        return RIFT_BASELINE_PATTERN_WHITESPACE;
    } else if (strcmp(uppercase_name, "WORD_BOUNDARY") == 0) {
        return RIFT_BASELINE_PATTERN_WORD_BOUNDARY;
    } else if (strcmp(uppercase_name, "IDENTIFIER") == 0) {
        return RIFT_BASELINE_PATTERN_IDENTIFIER;
    } else if (strcmp(uppercase_name, "NUMERIC") == 0) {
        return RIFT_BASELINE_PATTERN_NUMERIC;
    } else if (strcmp(uppercase_name, "STRING") == 0) {
        return RIFT_BASELINE_PATTERN_STRING;
    } else if (strcmp(uppercase_name, "COMMENT") == 0) {
        return RIFT_BASELINE_PATTERN_COMMENT;
    } else if (strcmp(uppercase_name, "OPERATOR") == 0) {
        return RIFT_BASELINE_PATTERN_OPERATOR;
    } else if (strcmp(uppercase_name, "FUNCTION") == 0) {
        return RIFT_BASELINE_PATTERN_FUNCTION;
    } else if (strcmp(uppercase_name, "STATE_TRANSITION") == 0) {
        return RIFT_BASELINE_PATTERN_STATE_TRANSITION;
    }

    return -1;
}

/**
 * @brief Create a custom pattern from a hybrid of baseline patterns
 *
 * @param template Template string with placeholders in the form {{PATTERN_NAME}}
 * @param flags Additional flags for the custom pattern
 * @param error Pointer to store error information (can be NULL)
 * @return A new pattern based on the template
 */
rift_regex_pattern_t *
rift_pattern_create_custom(const char *template, rift_regex_flags_t flags,
                           rift_regex_error_t *error)
{
    if (!template) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
            snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                     "Invalid template parameter");
        }
        return NULL;
    }

    /* First pass: calculate required buffer size */
    size_t template_len = strlen(template);
    size_t buffer_size = template_len + 1; /* Start with template size + null terminator */
    size_t pos = 0;
    size_t start_pos, end_pos;
    char pattern_name[128];

    while (find_pattern_placeholder(template, pos, &start_pos, &end_pos, pattern_name,
                                    sizeof(pattern_name))) {
        int pattern_type = get_pattern_type_from_name(pattern_name);
        if (pattern_type < 0) {
            if (error) {
                error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
                snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                         "Unknown pattern name: %s", pattern_name);
            }
            return NULL;
        }

        const char *pattern_str = get_pattern_string((rift_baseline_pattern_type_t)pattern_type);
        if (!pattern_str) {
            if (error) {
                error->code = RIFT_REGEX_ERROR_INTERNAL;
                snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                         "Failed to get pattern string for type %d", pattern_type);
            }
            return NULL;
        }

        /* Adjust buffer size: -placeholder_size + pattern_size */
        buffer_size -= (end_pos - start_pos);
        buffer_size += strlen(pattern_str);

        pos = end_pos; /* Continue after this placeholder */
    }

    /* Allocate buffer for custom pattern */
    char *custom_pattern = (char *)rift_malloc(buffer_size);
    if (!custom_pattern) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_MEMORY;
            snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                     "Memory allocation failed");
        }
        return NULL;
    }

    /* Second pass: build custom pattern by replacing placeholders */
    pos = 0;
    size_t dest_pos = 0;

    while (pos < template_len) {
        if (find_pattern_placeholder(template, pos, &start_pos, &end_pos, pattern_name,
                                     sizeof(pattern_name))) {
            /* Copy template content up to placeholder */
            if (start_pos > pos) {
                size_t copy_len = start_pos - pos;
                memcpy(custom_pattern + dest_pos, template + pos, copy_len);
                dest_pos += copy_len;
            }

            /* Replace placeholder with pattern */
            int pattern_type = get_pattern_type_from_name(pattern_name);
            const char *pattern_str =
                get_pattern_string((rift_baseline_pattern_type_t)pattern_type);

            size_t pattern_len = strlen(pattern_str);
            memcpy(custom_pattern + dest_pos, pattern_str, pattern_len);
            dest_pos += pattern_len;

            pos = end_pos; /* Move past the placeholder */
        } else {
            /* Copy remaining template content */
            size_t copy_len = template_len - pos;
            memcpy(custom_pattern + dest_pos, template + pos, copy_len);
            dest_pos += copy_len;
            pos = template_len; /* End loop */
        }
    }

    /* Null terminate */
    custom_pattern[dest_pos] = '\0';

    /* Compile the custom pattern */
    rift_regex_pattern_t *pattern =
        rift_regex_compile(custom_pattern, flags | RIFT_REGEX_FLAG_RIFT_SYNTAX, error);

    /* Free the temporary buffer */
    rift_free(custom_pattern);

    return pattern;
}

/**
 * @brief Create a modifiable copy of a baseline pattern
 *
 * @param pattern_type The pattern type to clone
 * @param flags Additional flags for the cloned pattern
 * @param error Pointer to store error information (can be NULL)
 * @return A new pattern that's a copy of the specified baseline pattern
 */
rift_regex_pattern_t *
rift_pattern_clone_baseline(rift_baseline_pattern_type_t pattern_type, rift_regex_flags_t flags,
                            rift_regex_error_t *error)
{
    /* First try to get a cached pattern */
    rift_regex_pattern_t *source_pattern = rift_baseline_patterns_get(pattern_type, error);
    if (!source_pattern) {
        /* If not cached, compile directly from string */
        const char *pattern_str = get_pattern_string(pattern_type);
        if (!pattern_str) {
            if (error) {
                error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
                snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                         "Invalid pattern type: %d", pattern_type);
            }
            return NULL;
        }

        return rift_regex_compile(pattern_str, flags | RIFT_REGEX_FLAG_RIFT_SYNTAX, error);
    }

    /* Clone the pattern if we need different flags */
    rift_regex_flags_t source_flags = rift_regex_pattern_get_flags(source_pattern);
    if (source_flags != flags) {
        const char *pattern_str = get_pattern_string(pattern_type);
        if (!pattern_str) {
            if (error) {
                error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
                snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                         "Invalid pattern type: %d", pattern_type);
            }
            return NULL;
        }

        return rift_regex_compile(pattern_str, flags | RIFT_REGEX_FLAG_RIFT_SYNTAX, error);
    }

    /* Clone the existing pattern */
    return rift_regex_pattern_clone(source_pattern);
}
