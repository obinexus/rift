/**
 * @file matcher.h
 * @brief Header file for the regex matcher component of LibRift
 *
 * This file defines the matcher interface for executing regular expressions
 * against input strings in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <regex.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "core/automaton/automaton.h"
#include "core/automaton/flags.h"
#include "core/errors/regex_error.h"
#ifndef LIBRIFT_REGEX_ENGINE_MATCHER_H
#define LIBRIFT_REGEX_ENGINE_MATCHER_H


#ifdef __cplusplus
extern "C" {
#endif

/* Implementation details */
struct rift_regex_matcher {
    const char *pattern;                        /**< Regex pattern */
    size_t pattern_length;                      /**< Length of the regex pattern */
    rift_matcher_option_t options;              /**< Matcher options */
    rift_regex_matcher_context_t *context;      /**< Matcher context */
    struct rift_regex_backtracker *backtracker; /**< Backtracker for backtracking state */
    uint32_t flags;                             /**< Flags for regex matching */
    bool timed_out;                             /**< Whether the matcher has timed out */
    clock_t start_time;                         /**< Start time for timeout tracking */
    uint32_t timeout_ms;                        /**< Timeout in milliseconds */
};


/**
 * @brief Create a new matcher for a pattern
 *
 * @param pattern The compiled regex pattern
 * @param options Matcher options
 * @return A new matcher or NULL on failure
 */
rift_regex_matcher_t *rift_matcher_create(const rift_regex_pattern_t *pattern,
                                          rift_matcher_option_t options);

/**
 * @brief Free resources associated with a matcher
 *
 * @param matcher The matcher to free
 */
void rift_matcher_free(rift_regex_matcher_t *matcher);

/**
 * @brief Reset a matcher to its initial state
 * 
 * @param matcher The matcher to reset
 */
void rift_matcher_reset(rift_regex_matcher_t *matcher);

/**
 * @brief Set the input string for a matcher
 *
 * @param matcher The matcher
 * @param input The input string
 * @param length Length of the input string or (size_t)-1 to use strlen
 * @return true if successful, false otherwise
 */
bool rift_matcher_set_input(rift_regex_matcher_t *matcher, const char *input, size_t length);

/**
 * @brief Set pattern-specific backtracking limits
 * @param matcher The matcher to modify
 * @param pattern_id Pattern identifier
 * @param config Limit configuration
 * @return true if successful, false otherwise
 */
bool rift_regex_matcher_set_pattern_limits(rift_regex_matcher_t *matcher, uint32_t pattern_id,
                                           const rift_backtrack_limit_config_t *config);

/**
 * @brief Set match-specific backtracking limits
 * @param matcher The matcher to modify
 * @param match_id Match identifier
 * @param config Limit configuration
 * @return true if successful, false otherwise
 */
bool rift_regex_matcher_set_match_limits(rift_regex_matcher_t *matcher, uint32_t match_id,
                                         const rift_backtrack_limit_config_t *config);

/**
 * @brief Configure default limits for R'' syntax
 * @param matcher The matcher to modify
 * @param max_depth Maximum depth
 * @param max_duration_ms Maximum duration in milliseconds
 * @return true if successful, false otherwise
 */
bool rift_regex_matcher_configure_r_syntax_limits(rift_regex_matcher_t *matcher, uint32_t max_depth,
                                                  uint32_t max_duration_ms);
/**
 * @brief Find the next match in the input string
 *
 * @param matcher The matcher
 * @param match Pointer to store match information (can be NULL)
 * @return true if a match was found, false otherwise
 */
bool rift_matcher_find_next(rift_regex_matcher_t *matcher, rift_regex_match_t *match);

/**
 * @brief Check if the entire input string matches the pattern
 *
 * @param matcher The matcher
 * @param match Pointer to store match information (can be NULL)
 * @return true if the entire string matches, false otherwise
 */
bool rift_matcher_matches(rift_regex_matcher_t *matcher, rift_regex_match_t *match);

/**
 * @brief Find all matches in the input string
 *
 * @param matcher The matcher
 * @param matches Array to store match information
 * @param max_matches Maximum number of matches to find
 * @param num_matches Pointer to store the number of matches found
 * @return true if at least one match was found, false otherwise
 */
bool rift_matcher_find_all(rift_regex_matcher_t *matcher, rift_regex_match_t *matches,
                           size_t max_matches, size_t *num_matches);

/**
 * @brief Replace occurrences of the pattern in the input string
 *
 * @param matcher The matcher
 * @param replacement The replacement string
 * @param output Buffer to store the result
 * @param output_size Size of the output buffer
 * @param num_replacements Pointer to store the number of replacements made
 * @return true if successful, false otherwise
 */
bool rift_matcher_replace(rift_regex_matcher_t *matcher, const char *replacement, char *output,
                          size_t output_size, size_t *num_replacements);

/**
 * @brief Split the input string using the pattern as a delimiter
 *
 * @param matcher The matcher
 * @param parts Array to store the resulting parts
 * @param max_parts Maximum number of parts to return
 * @param num_parts Pointer to store the number of parts
 * @return true if successful, false otherwise
 */
bool rift_matcher_split(rift_regex_matcher_t *matcher, char **parts, size_t max_parts,
                        size_t *num_parts);

/**
 * @brief Set matcher options
 *
 * @param matcher The matcher
 * @param options The options to set
 * @return true if successful, false otherwise
 */
bool rift_matcher_set_options(rift_regex_matcher_t *matcher, rift_matcher_option_t options);

/**
 * @brief Get the current matcher options
 *
 * @param matcher The matcher
 * @return The current options
 */
rift_matcher_option_t rift_matcher_get_options(const rift_regex_matcher_t *matcher);

/**
 * @brief Set the maximum backtracking depth
 *
 * @param matcher The matcher
 * @param max_depth The maximum depth
 * @return true if successful, false otherwise
 */
bool rift_matcher_set_max_backtrack_depth(rift_regex_matcher_t *matcher, size_t max_depth);

/**
 * @brief Get the current backtracking depth
 *
 * @param matcher The matcher
 * @return The current backtracking depth
 */
size_t rift_matcher_get_backtrack_depth(const rift_regex_matcher_t *matcher);

/**
 * @brief Set a timeout for matching operations
 *
 * @param matcher The matcher
 * @param timeout_ms Timeout in milliseconds (0 for no timeout)
 * @return true if successful, false otherwise
 */
bool rift_matcher_set_timeout(rift_regex_matcher_t *matcher, uint32_t timeout_ms);

/**
 * @brief Check if a timeout occurred during the last matching operation
 *
 * @param matcher The matcher
 * @return true if a timeout occurred, false otherwise
 */
bool rift_matcher_timed_out(const rift_regex_matcher_t *matcher);

/**
 * @brief Get the pattern associated with a matcher
 *
 * @param matcher The matcher
 * @return The regex pattern
 */
const rift_regex_pattern_t *rift_matcher_get_pattern(const rift_regex_matcher_t *matcher);

/**
 * @brief Get the input string for a matcher
 *
 * @param matcher The matcher
 * @return The input string
 */
const char *rift_matcher_get_input(const rift_regex_matcher_t *matcher);

/**
 * @brief Get the current position in the input string
 *
 * @param matcher The matcher
 * @return The current position
 */
size_t rift_matcher_get_position(const rift_regex_matcher_t *matcher);

/**
 * @brief Set the current position in the input string
 *
 * @param matcher The matcher
 * @param position The position to set
 * @return true if successful, false otherwise
 */
bool rift_matcher_set_position(rift_regex_matcher_t *matcher, size_t position);

/**
 * @brief Process a single character through the automaton
 *
 * @param automaton The automaton
 * @param c The character to process
 * @param matcher The matcher for backtracking
 * @param context The matcher context for capturing
 * @return true if the character was accepted, false otherwise
 */
bool process_character(rift_regex_automaton_t *automaton, char c, rift_regex_matcher_t *matcher,
                       rift_regex_matcher_context_t *context);

/**
 * @brief Create a matcher from a pattern string
 *
 * This is a convenience function that compiles the pattern and creates a matcher.
 *
 * @param pattern_string The regex pattern string
 * @param flags Compilation flags
 * @param options Matcher options
 * @param error Pointer to store error code (can be NULL)
 * @return A new matcher or NULL on failure
 */
rift_regex_matcher_t *rift_matcher_create_from_string(const char *pattern_string,
                                                      rift_regex_flags_t flags,
                                                      rift_matcher_option_t options,
                                                      rift_regex_error_t *error);

/**
 * @brief Check if a matcher has timed out
 *
 * @param matcher The matcher
 * @return true if timed out, false otherwise
 */
bool check_timeout(rift_regex_matcher_t *matcher);

/**
 * @brief Free resources associated with a match result
 *
 * @param match The match result to free
 */
void rift_match_free(rift_regex_match_t *match);

// Function declarations and other code here

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_ENGINE_MATCHER_H */