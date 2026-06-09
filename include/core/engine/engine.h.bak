/**
 * @file engine.h
 * @brief Header file for the LibRift regex engine
 *
 * This file defines the public API for the LibRift regex engine, including
 * pattern compilation, matching, and result handling.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

 #ifndef LIBRIFT_REGEX_ENGINE_ENGINE_H
 #define LIBRIFT_REGEX_ENGINE_ENGINE_H
 
 #include <stdbool.h>
 #include <stddef.h>
 #include <stdlib.h>
 #include <string.h>
 
 #include "core/automaton/automaton.h"
 #include "core/automaton/flags.h"
 #include "core/errors/regex_error.h"
 #include "core/engine/matcher.h"
 #include "core/engine/match_types.h"
 #ifdef __cplusplus
 extern "C" {
 #endif

 /**
  * @brief Compile a regex pattern
  *
  * @param pattern The pattern string
  * @param flags Compilation flags
  * @param error Error information (can be NULL)
  * @return A new compiled pattern or NULL on failure
  */
 rift_regex_pattern_t *rift_regex_compile(const char *pattern, rift_regex_flags_t flags,
                                         rift_regex_error_t *error);
 
 /**
  * @brief Free resources associated with a compiled pattern
  *
  * @param pattern The pattern to free
  */
 void rift_regex_pattern_free(rift_regex_pattern_t *pattern);
 
 /**
  * @brief Find a match in a string
  *
  * @param pattern Compiled pattern
  * @param input Input string to search
  * @param match Match information to fill (can be NULL)
  * @return true if a match was found, false otherwise
  */
 bool rift_regex_find(rift_regex_pattern_t *pattern, const char *input, rift_regex_match_t *match);
 
 /**
  * @brief Find all matches in a string
  *
  * @param pattern Compiled pattern
  * @param input Input string to search
  * @param matches Array to store matches
  * @param max_matches Maximum number of matches to return
  * @param num_matches Actual number of matches found
  * @return true if at least one match was found, false otherwise
  */
 bool rift_regex_find_all(rift_regex_pattern_t *pattern, const char *input,
                          rift_regex_match_t *matches, size_t max_matches, size_t *num_matches);
 
 /**
  * @brief Check if a string matches a pattern
  *
  * @param pattern The pattern string
  * @param input Input string to match
  * @param flags Compilation flags
  * @return true if the string matches, false otherwise
  */
 bool rift_regex_matches(const char *pattern, const char *input, rift_regex_flags_t flags);
 
 /**
  * @brief Replace matches in a string
  *
  * @param pattern Compiled pattern
  * @param input Input string
  * @param replacement Replacement string
  * @param output Output buffer
  * @param output_size Size of output buffer
  * @param num_replacements Number of replacements made
  * @return true if successful, false otherwise
  */
 bool rift_regex_replace_with_pattern(rift_regex_pattern_t *pattern, const char *input,
                              const char *replacement, char *output, size_t output_size,
                              size_t *num_replacements);
 
 /**
  * @brief Split a string using a regex pattern
  *
  * @param pattern Compiled pattern
  * @param input Input string
  * @param parts Array to store parts
  * @param max_parts Maximum number of parts
  * @param num_parts Actual number of parts
  * @return true if successful, false otherwise
  */
 bool rift_regex_split(rift_regex_pattern_t *pattern, const char *input,
                      char **parts, size_t max_parts, size_t *num_parts);
 
 /**
  * @brief Free resources associated with a match result
  *
  * @param match The match result to free
  */
 void rift_regex_match_free(rift_regex_match_t *match);
 
 /**
  * @brief Get the number of capture groups in a compiled pattern
  *
  * @param pattern The compiled pattern
  * @return Number of capture groups
  */
 size_t rift_regex_pattern_get_group_count(const rift_regex_pattern_t *pattern);
 
 /**
  * @brief Get the source string of a compiled pattern
  *
  * @param pattern The compiled pattern
  * @return The source string
  */
 const char *rift_regex_pattern_get_source(const rift_regex_pattern_t *pattern);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* LIBRIFT_REGEX_ENGINE_ENGINE_H */