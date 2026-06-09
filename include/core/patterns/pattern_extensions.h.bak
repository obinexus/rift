/**
 * @file pattern_extensions.h
 * @brief Utilities for extending baseline regex patterns
 *
 * This file provides utilities for extending the baseline regex patterns
 * to create custom patterns for specific language features.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/engine/pattern.h"
#include "core/errors/regex_error.h"
#include "core/patterns/pattern_types.h"
#ifndef LIBRIFT_CORE_REGEX_PATTERNS_PATTERN_EXTENSIONS_H
#define LIBRIFT_CORE_REGEX_PATTERNS_PATTERN_EXTENSIONS_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Combine multiple baseline patterns into a single pattern
 *
 * @param pattern_types Array of pattern types to combine
 * @param num_patterns Number of patterns in the array
 * @param flags Additional flags for the combined pattern
 * @param error Pointer to store error information (can be NULL)
 * @return A new pattern that matches any of the input patterns
 */
rift_regex_pattern_t *rift_pattern_combine(const rift_baseline_pattern_type_t *pattern_types,
                                           size_t num_patterns, rift_regex_flags_t flags,
                                           rift_regex_error_t *error);

/**
 * @brief Extend a baseline pattern with additional constraints
 *
 * @param base_type The base pattern type to extend
 * @param extension The extension pattern string
 * @param flags Additional flags for the extended pattern
 * @param error Pointer to store error information (can be NULL)
 * @return A new pattern that extends the base pattern
 */
rift_regex_pattern_t *rift_pattern_extend(rift_baseline_pattern_type_t base_type,
                                          const char *extension, rift_regex_flags_t flags,
                                          rift_regex_error_t *error);

/**
 * @brief Create a sequence of patterns
 *
 * @param pattern_types Array of pattern types to sequence
 * @param num_patterns Number of patterns in the array
 * @param flags Additional flags for the sequenced pattern
 * @param error Pointer to store error information (can be NULL)
 * @return A new pattern that matches the sequence of input patterns
 */
rift_regex_pattern_t *rift_pattern_sequence(const rift_baseline_pattern_type_t *pattern_types,
                                            size_t num_patterns, rift_regex_flags_t flags,
                                            rift_regex_error_t *error);

/**
 * @brief Pattern fingerprint for performance analysis and optimization
 *
 * Contains metrics and characteristics of a pattern that can be used
 * for performance tracking, pattern optimization, and caching.
 */

/**
 * @brief Create a fingerprint from a regex pattern
 *
 * @param pattern The pattern to analyze and fingerprint
 * @return A new fingerprint for the given pattern
 */
rift_pattern_fingerprint_t *rift_pattern_create_fingerprint(const rift_regex_pattern_t *pattern);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_CORE_REGEX_PATTERNS_PATTERN_EXTENSIONS_H */
