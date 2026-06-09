/**
 * @file pattern.h
 * @brief Header file for the pattern compilation in the LibRift regex engine
 *
 * This header defines the pattern compilation and management interface for the LibRift regex
 * engine, supporting both standard regex syntax and the dedicated `r''` syntax.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

 #ifndef LIBRIFT_REGEX_ENGINE_PATTERN_H
 #define LIBRIFT_REGEX_ENGINE_PATTERN_H
 
 #include <stdbool.h>
 #include <stddef.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 #include "core/automaton/automaton.h"
 #include "core/automaton/flags.h"
 #include "core/errors/regex_error.h"
 #include "core/engine/engine.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief Structure representing a compiled regex pattern
  */
 struct rift_regex_pattern {
     char *source;                           /**< Original pattern string */
     rift_regex_flags_t flags;               /**< Compilation flags */
     struct rift_regex_automaton *automaton; /**< Compiled automaton */
     struct rift_regex_ast *ast;             /**< Abstract Syntax Tree */
     size_t group_count;                     /**< Number of capture groups */
     bool is_rift_syntax;                    /**< Whether the pattern uses r'' syntax */
     char error_message[256];                /**< Last error message */
     bool is_valid;                          /**< Whether the pattern is valid */
 };
 
 /**
  * @brief Pattern fingerprint for caching and optimization
  */
 typedef struct rift_pattern_fingerprint {
     float complexity;            /**< Computed complexity score */
     uint32_t state_count;        /**< Number of automaton states */
     uint32_t transition_count;   /**< Number of transitions */
     float branching_factor;      /**< Average branching factor */
     uint32_t max_nesting;        /**< Maximum nesting level */
     uint32_t alternation_count;  /**< Number of alternation operators */
     uint32_t quantifier_count;   /**< Number of quantifiers */
     uint32_t capture_group_count; /**< Number of capture groups */
     uint64_t hash;               /**< Pattern hash for quick lookup */
 } rift_pattern_fingerprint_t;
 
 /**
  * @brief Create a new regex pattern from a string
  *
  * @param pattern The pattern string
  * @param flags Compilation flags
  * @param error Pointer to store error code (can be NULL)
  * @return A new pattern or NULL on failure
  */
 rift_regex_pattern_t *rift_regex_compile(const char *pattern, rift_regex_flags_t flags,
                                          rift_regex_error_t *error);
 
 /**
  * @brief Free resources associated with a pattern
  *
  * @param pattern The pattern to free
  */
 void rift_regex_pattern_free(rift_regex_pattern_t *pattern);
 
 /**
  * @brief Get the flags used to compile the pattern
  *
  * @param pattern The pattern
  * @return The compilation flags
  */
 rift_regex_flags_t rift_regex_pattern_get_flags(const rift_regex_pattern_t *pattern);
 
 /**
  * @brief Get the number of capture groups in the pattern
  *
  * @param pattern The pattern
  * @return The number of groups
  */
 size_t rift_regex_pattern_get_group_count(const rift_regex_pattern_t *pattern);
 
 /**
  * @brief Get the compiled automaton from the pattern
  *
  * @param pattern The pattern
  * @return The automaton or NULL if not compiled
  */
 struct rift_regex_automaton *rift_regex_pattern_get_automaton(const rift_regex_pattern_t *pattern);
 
 /**
  * @brief Get the original source string of the pattern
  *
  * @param pattern The pattern
  * @return The source string or NULL if not available
  */
 const char *rift_regex_pattern_get_source(const rift_regex_pattern_t *pattern);
 
 /**
  * @brief Check if the pattern uses LibRift r'' syntax
  *
  * @param pattern The pattern
  * @return true if it uses r'' syntax, false otherwise
  */
 bool rift_regex_pattern_is_rift_syntax(const rift_regex_pattern_t *pattern);
 
 /**
  * @brief Get the AST for the pattern
  *
  * @param pattern The pattern
  * @return The AST or NULL if not available
  */
 const struct rift_regex_ast *rift_regex_pattern_get_ast(const rift_regex_pattern_t *pattern);
 
 /**
  * @brief Create a duplicate of a pattern
  *
  * @param pattern The pattern to clone
  * @return A new pattern that is a copy of the original, or NULL on failure
  */
 rift_regex_pattern_t *rift_regex_pattern_clone(const rift_regex_pattern_t *pattern);
 
 /**
  * @brief Get the last error message from a pattern compilation
  *
  * @param pattern The pattern
  * @return The error message or NULL if no error occurred
  */
 const char *rift_regex_pattern_get_error(const rift_regex_pattern_t *pattern);
 
 /**
  * @brief Check if a pattern is valid for use
  *
  * @param pattern The pattern to check
  * @return true if the pattern is valid, false otherwise
  */
 bool rift_regex_pattern_is_valid(const rift_regex_pattern_t *pattern);
 
 /**
  * @brief Get a string representation of a pattern for debugging
  *
  * @param pattern The pattern
  * @param buffer Buffer to store the string
  * @param buffer_size Size of the buffer
  * @return true if successful, false otherwise
  */
 bool rift_regex_pattern_to_string(const rift_regex_pattern_t *pattern, char *buffer,
                                   size_t buffer_size);
 
 /**
  * @brief Serialize a pattern to a binary format for storage
  *
  * @param pattern The pattern to serialize
  * @param data Pointer to store the serialized data
  * @param size Pointer to store the size of the serialized data
  * @return true if successful, false otherwise
  */
 bool rift_regex_pattern_serialize(const rift_regex_pattern_t *pattern, unsigned char **data,
                                   size_t *size);
 
 /**
  * @brief Deserialize a pattern from a binary format
  *
  * @param data The serialized data
  * @param size Size of the serialized data
  * @param error Pointer to store error code (can be NULL)
  * @return A new pattern or NULL on failure
  */
 rift_regex_pattern_t *rift_regex_pattern_deserialize(const unsigned char *data, size_t size,
                                                      rift_regex_error_t *error);
 
 /**
  * @brief Create a pattern from an automaton
  *
  * This allows for direct construction of patterns from a custom automaton.
  *
  * @param automaton The automaton to use
  * @param flags Compilation flags
  * @param error Pointer to store error code (can be NULL)
  * @return A new pattern or NULL on failure
  */
 rift_regex_pattern_t *rift_regex_pattern_create_from_automaton(
     const struct rift_regex_automaton *automaton,
     rift_regex_flags_t flags, rift_regex_error_t *error);
 
 /**
  * @brief Get a fingerprint of the pattern for caching and optimization
  *
  * @param pattern The pattern
  * @param include_transitions Whether to include transition info (expensive)
  * @return Pattern fingerprint 
  */
 rift_pattern_fingerprint_t rift_regex_pattern_get_fingerprint(
     const rift_regex_pattern_t *pattern, 
     bool include_transitions);
 
 /**
  * @brief Compile a pattern string into an automaton directly
  *
  * This is a convenience function that combines parsing and compilation.
  *
  * @param pattern The pattern string
  * @param flags Compilation flags
  * @param error Pointer to store error code (can be NULL)
  * @return The compiled automaton or NULL on failure
  */
 struct rift_regex_automaton *rift_regex_compile_pattern(const char *pattern,
                                                         rift_regex_flags_t flags,
                                                         rift_regex_error_t *error);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* LIBRIFT_REGEX_ENGINE_PATTERN_H */