/**
 * @file pattern_types.h
 * @brief Type definitions for baseline regex patterns
 *
 * This file defines the types used to identify and work with the
 * baseline regex patterns in the LibRift engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#ifndef LIBRIFT_CORE_REGEX_PATTERNS_PATTERN_TYPES_H
#define LIBRIFT_CORE_REGEX_PATTERNS_PATTERN_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enum for baseline pattern types
 */
typedef enum {
    RIFT_BASELINE_PATTERN_EMPTY_STATE,     /**< Empty state pattern */
    RIFT_BASELINE_PATTERN_WHITESPACE,      /**< Whitespace pattern */
    RIFT_BASELINE_PATTERN_WORD_BOUNDARY,   /**< Word boundary pattern */
    RIFT_BASELINE_PATTERN_IDENTIFIER,      /**< Identifier pattern */
    RIFT_BASELINE_PATTERN_NUMERIC,         /**< Numeric literal pattern */
    RIFT_BASELINE_PATTERN_STRING,          /**< String literal pattern */
    RIFT_BASELINE_PATTERN_COMMENT,         /**< Comment pattern */
    RIFT_BASELINE_PATTERN_OPERATOR,        /**< Operator pattern */
    RIFT_BASELINE_PATTERN_FUNCTION,        /**< Function pattern */
    RIFT_BASELINE_PATTERN_STATE_TRANSITION /**< State transition pattern */
} rift_baseline_pattern_type_t;

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_CORE_REGEX_PATTERNS_PATTERN_TYPES_H */
