/**
 * @file baseline_patterns.c
 * @brief Implementation of baseline regex pattern utilities
 *
 * This file implements utilities for working with the baseline regex patterns
 * defined in baseline_patterns.h.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/patterns/baseline_patterns.h
#include "core/compiler/compiler.h"
#include "core/engine/pattern.h"
#include "core/errors/error.h"
#include "core/errors/regex_error.h"
#include "librift/compiler/compiler.h"
#include "librift/engine/pattern.h"
#include "librift/errors/regex_error.h"
#include "librift/patterns/baseline_patterns.h"

tmp/librift_analysis/librift-4933472-W/backup/v1/src/patterns/baseline_patterns.h
tmp/librift_analysis/librift-4933472-W/backup/v2/patterns/baseline_patterns.h
tmp/librift_analysis/librift-4933472-W/src/core/patterns/baseline_patterns.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/patterns/baseline_patterns.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v2/patterns/baseline_patterns.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/patterns/baseline_patterns.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/patterns/baseline_patterns.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v2/patterns/baseline_patterns.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/patterns/baseline_patterns.h"

/* Internal pattern cache for precompiled patterns */
static struct {
    rift_regex_pattern_t *empty_state;
    rift_regex_pattern_t *whitespace;
    rift_regex_pattern_t *word_boundary;
    rift_regex_pattern_t *identifier;
    rift_regex_pattern_t *numeric;
    rift_regex_pattern_t *string;
    rift_regex_pattern_t *comment;
    rift_regex_pattern_t *operator;
    rift_regex_pattern_t *function;
    rift_regex_pattern_t *state_transition;
} pattern_cache = {0};

/**
 * @brief Initialize the baseline pattern cache
 *
 * @param error Pointer to store error information (can be NULL)
 * @return true if initialization was successful, false otherwise
 */
bool
rift_baseline_patterns_initialize(rift_regex_error_t *error)
{
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_RIFT_SYNTAX | RIFT_REGEX_FLAG_OPTIMIZE;

    if (!pattern_cache.empty_state) {
        pattern_cache.empty_state = rift_regex_compile(RIFT_PATTERN_EMPTY_STATE, flags, error);
        if (!pattern_cache.empty_state)
            return false;
    }

    if (!pattern_cache.whitespace) {
        pattern_cache.whitespace = rift_regex_compile(RIFT_PATTERN_WHITESPACE, flags, error);
        if (!pattern_cache.whitespace)
            return false;
    }

    if (!pattern_cache.word_boundary) {
        pattern_cache.word_boundary = rift_regex_compile(RIFT_PATTERN_WORD_BOUNDARY, flags, error);
        if (!pattern_cache.word_boundary)
            return false;
    }

    if (!pattern_cache.identifier) {
        pattern_cache.identifier = rift_regex_compile(RIFT_PATTERN_IDENTIFIER, flags, error);
        if (!pattern_cache.identifier)
            return false;
    }

    if (!pattern_cache.numeric) {
        pattern_cache.numeric = rift_regex_compile(RIFT_PATTERN_NUMERIC, flags, error);
        if (!pattern_cache.numeric)
            return false;
    }

    if (!pattern_cache.string) {
        pattern_cache.string = rift_regex_compile(RIFT_PATTERN_STRING, flags, error);
        if (!pattern_cache.string)
            return false;
    }

    if (!pattern_cache.comment) {
        pattern_cache.comment = rift_regex_compile(RIFT_PATTERN_COMMENT, flags, error);
        if (!pattern_cache.comment)
            return false;
    }

    if (!pattern_cache.operator) {
        pattern_cache.operator= rift_regex_compile(RIFT_PATTERN_OPERATOR, flags, error);
        if (!pattern_cache.operator)
            return false;
    }

    if (!pattern_cache.function) {
        pattern_cache.function = rift_regex_compile(RIFT_PATTERN_FUNCTION, flags, error);
        if (!pattern_cache.function)
            return false;
    }

    if (!pattern_cache.state_transition) {
        pattern_cache.state_transition =
            rift_regex_compile(RIFT_PATTERN_STATE_TRANSITION, flags, error);
        if (!pattern_cache.state_transition)
            return false;
    }

    return true;
}

/**
 * @brief Cleanup the baseline pattern cache
 */
void
rift_baseline_patterns_cleanup(void)
{
    if (pattern_cache.empty_state) {
        rift_regex_pattern_free(pattern_cache.empty_state);
        pattern_cache.empty_state = NULL;
    }

    if (pattern_cache.whitespace) {
        rift_regex_pattern_free(pattern_cache.whitespace);
        pattern_cache.whitespace = NULL;
    }

    if (pattern_cache.word_boundary) {
        rift_regex_pattern_free(pattern_cache.word_boundary);
        pattern_cache.word_boundary = NULL;
    }

    if (pattern_cache.identifier) {
        rift_regex_pattern_free(pattern_cache.identifier);
        pattern_cache.identifier = NULL;
    }

    if (pattern_cache.numeric) {
        rift_regex_pattern_free(pattern_cache.numeric);
        pattern_cache.numeric = NULL;
    }

    if (pattern_cache.string) {
        rift_regex_pattern_free(pattern_cache.string);
        pattern_cache.string = NULL;
    }

    if (pattern_cache.comment) {
        rift_regex_pattern_free(pattern_cache.comment);
        pattern_cache.comment = NULL;
    }

    if (pattern_cache.operator) {
        rift_regex_pattern_free(pattern_cache.operator);
        pattern_cache.operator= NULL;
    }

    if (pattern_cache.function) {
        rift_regex_pattern_free(pattern_cache.function);
        pattern_cache.function = NULL;
    }

    if (pattern_cache.state_transition) {
        rift_regex_pattern_free(pattern_cache.state_transition);
        pattern_cache.state_transition = NULL;
    }
}

/**
 * @brief Get a precompiled baseline pattern
 *
 * @param pattern_type The pattern type to retrieve
 * @param error Pointer to store error information (can be NULL)
 * @return The compiled pattern or NULL if not available
 */
rift_regex_pattern_t *
rift_baseline_patterns_get(rift_baseline_pattern_type_t pattern_type, rift_regex_error_t *error)
{
    /* Initialize patterns if needed */
    if (!pattern_cache.empty_state) {
        if (!rift_baseline_patterns_initialize(error)) {
            return NULL;
        }
    }

    /* Return the requested pattern */
    switch (pattern_type) {
    case RIFT_BASELINE_PATTERN_EMPTY_STATE:
        return pattern_cache.empty_state;
    case RIFT_BASELINE_PATTERN_WHITESPACE:
        return pattern_cache.whitespace;
    case RIFT_BASELINE_PATTERN_WORD_BOUNDARY:
        return pattern_cache.word_boundary;
    case RIFT_BASELINE_PATTERN_IDENTIFIER:
        return pattern_cache.identifier;
    case RIFT_BASELINE_PATTERN_NUMERIC:
        return pattern_cache.numeric;
    case RIFT_BASELINE_PATTERN_STRING:
        return pattern_cache.string;
    case RIFT_BASELINE_PATTERN_COMMENT:
        return pattern_cache.comment;
    case RIFT_BASELINE_PATTERN_OPERATOR:
        return pattern_cache.operator;
    case RIFT_BASELINE_PATTERN_FUNCTION:
        return pattern_cache.function;
    case RIFT_BASELINE_PATTERN_STATE_TRANSITION:
        return pattern_cache.state_transition;
    default:
        if (error) {
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
            snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                     "Unknown pattern type: %d", pattern_type);
        }
        return NULL;
    }
}
