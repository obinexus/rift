/**
 * @file regex.h
 * @brief Standard regex integration for the LibRift automaton engine
 *
 * This file provides integration with the standard regex library for pattern
 * matching within the automaton component of the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

 #ifndef LIBRIFT_REGEX_AUTOMATON_REGEX_H
 #define LIBRIFT_REGEX_AUTOMATON_REGEX_H
 
 #include <regex.h>
 #include <stdbool.h>
 #include <stddef.h>
 #include <stdint.h>
 #include "core/automaton/automaton.h"
 #include "core/regex/flags.h"
 #include "core/errors/regex_error.h"  /* Include the centralized error definitions */
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Compile a regex pattern
  *
  * @param pattern The pattern string
  * @param cflags Compilation flags
  * @param preg Pointer to a regex_t structure to store the compiled pattern
  * @return 0 on success, non-zero error code on failure
  */
 int rift_regex_compile_posix(const char *pattern, int cflags, regex_t *preg);
 
 /**
  * @brief Execute a regex pattern against a string
  *
  * @param preg Pointer to a compiled regex_t pattern
  * @param string String to match against
  * @param nmatch Maximum number of matches to report
  * @param pmatch Array of regmatch_t structures to store match information
  * @param eflags Execution flags (e.g., REG_NOTBOL, REG_NOTEOL)
  * @return 0 on successful match, REG_NOMATCH if no match, other non-zero on error
  */
 int rift_regex_exec(const regex_t *preg, const char *string, size_t nmatch, regmatch_t *pmatch,
                    int eflags);
 
 /**
  * @brief Free resources associated with a compiled regex pattern
  *
  * @param preg Pointer to the regex_t structure to free
  */
 void rift_regex_free(regex_t *preg);
 
 /**
  * @brief Get error message for a regex error code
  *
  * @param errcode The error code
  * @param preg Pointer to the regex_t structure that generated the error
  * @param errbuf Buffer to store the error message
  * @param errbuf_size Size of the error buffer
  * @return Size of the error message
  */
 size_t rift_regex_error(int errcode, const regex_t *preg, char *errbuf, size_t errbuf_size);
 
 /**
  * @brief Check if a string matches a pattern
  *
  * @param pattern The pattern string
  * @param string The string to match against
  * @param flags Compilation flags
  * @return true if the string matches the pattern, false otherwise
  */
 bool rift_regex_matches_posix(const char *pattern, const char *string, int flags);
 
 /**
  * @brief Get the part of a string that matches a pattern
  *
  * @param pattern The pattern string
  * @param string The string to match against
  * @param start Pointer to store the start index of the match
  * @param end Pointer to store the end index of the match
  * @param flags Compilation flags
  * @return true if a match was found, false otherwise
  */
 bool rift_regex_find_posix(const char *pattern, const char *string, size_t *start, size_t *end,
                      int flags);
 
 /**
  * @brief Replace matches of a pattern in a string
  *
  * @param pattern The compiled pattern
  * @param input The input string to search
  * @param replacement The replacement string
  * @param result Buffer to store the result
  * @param result_size Size of the result buffer
  * @param replacements Pointer to store the number of replacements made
  * @return true if successful, false otherwise
  */
 bool rift_regex_replace(const struct rift_regex_pattern *pattern, const char *input,
                        const char *replacement, char *result, size_t result_size,
                        size_t *replacements);
 
 /**
  * @brief Check if a pattern is valid
  *
  * @param pattern The pattern string
  * @param flags Compilation flags
  * @return true if the pattern is valid, false otherwise
  */
 bool rift_regex_is_valid(const char *pattern, int flags);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* LIBRIFT_REGEX_AUTOMATON_REGEX_H */