/**
 * @file syntax.h
 * @brief Syntax extension for regex literals in the LibRift language
 *
 * This header defines the interface for handling the dedicated `R''` regex
 * literal syntax in the LibRift language. It provides functions for parsing,
 * validating, and compiling regex literals.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */
#ifndef LIBRIFT_REGEX_SYNTAX_H
#define LIBRIFT_REGEX_SYNTAX_H

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "core/automaton/flags.h"
#include "core/engine/pattern.h"
#include "core/errors/regex_error.h"
#include "core/parser/parser.h"
#include "core/parser/token.h"
#include "core/syntax/lexer.h"
#include "core/syntax/parser.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure representing a regex literal
 */
typedef struct rift_regex_literal {
    char *pattern;                          /**< The regex pattern string */
    rift_regex_flags_t flags;               /**< Compilation flags */
    rift_regex_pattern_t *compiled_pattern; /**< Compiled pattern (may be NULL) */
} rift_regex_literal_t;

/**
 * @brief Create a new regex literal
 *
 * @param pattern The regex pattern string
 * @param flags Compilation flags
 * @return A new regex literal or NULL on failure
 */
rift_regex_literal_t *rift_regex_literal_create(const char *pattern, rift_regex_flags_t flags);

/**
 * @brief Free resources associated with a regex literal
 *
 * @param literal The regex literal to free
 */
void rift_regex_literal_free(rift_regex_literal_t *literal);

/**
 * @brief Compile a regex literal
 *
 * @param literal The regex literal to compile
 * @param error Pointer to store error code (can be NULL)
 * @return true if compilation was successful, false otherwise
 */
bool rift_regex_literal_compile(rift_regex_literal_t *literal, rift_regex_error_t *error);

/**
 * @brief Get the compiled pattern from a regex literal
 *
 * @param literal The regex literal
 * @return The compiled pattern or NULL if not compiled
 */
rift_regex_pattern_t *rift_regex_literal_get_pattern(const rift_regex_literal_t *literal);

/**
 * @brief Get the pattern string from a regex literal
 *
 * @param literal The regex literal
 * @return The pattern string
 */
const char *rift_regex_literal_get_string(const rift_regex_literal_t *literal);

/**
 * @brief Get the flags from a regex literal
 *
 * @param literal The regex literal
 * @return The compilation flags
 */
rift_regex_flags_t rift_regex_literal_get_flags(const rift_regex_literal_t *literal);

/**
 * @brief Set the flags for a regex literal
 *
 * @param literal The regex literal
 * @param flags The compilation flags
 * @return true if successful, false otherwise
 */
bool rift_regex_literal_set_flags(rift_regex_literal_t *literal, rift_regex_flags_t flags);

/**
 * @brief Clone a regex literal
 *
 * @param literal The regex literal to clone
 * @return A new regex literal that is a copy of the original, or NULL on failure
 */
rift_regex_literal_t *rift_regex_literal_clone(const rift_regex_literal_t *literal);

/**
 * @brief Parse a regex literal from source code
 *
 * @param parser The regex parser
 * @param source The source code containing the regex literal
 * @param line_number The line number of the regex literal
 * @param column The column of the regex literal
 * @param error Pointer to store error code (can be NULL)
 * @return The parsed regex literal or NULL on failure
 */
rift_regex_literal_t *rift_regex_parser_parse_literal(rift_regex_parser_t *parser,
                                                      const char *source, size_t line_number,
                                                      size_t column, rift_regex_error_t *error);

/**
 * @brief Check if a source string contains a valid regex literal
 *
 * @param source The source code to check
 * @param start_pos Pointer to store the start position of the literal (can be NULL)
 * @param end_pos Pointer to store the end position of the literal (can be NULL)
 * @return true if a valid regex literal was found, false otherwise
 */
bool rift_regex_parser_is_literal(const char *source, size_t *start_pos, size_t *end_pos);

/**
 * @brief Extract flags from a regex literal
 *
 * @param literal_text The text of the regex literal
 * @param flags Pointer to store the extracted flags
 * @return true if flags were extracted successfully, false otherwise
 */
bool rift_regex_parser_extract_flags(const char *literal_text, rift_regex_flags_t *flags);

/**
 * @brief Extract the pattern from a regex literal
 *
 * @param literal_text The text of the regex literal
 * @param output Buffer to store the extracted pattern
 * @param output_size Size of the output buffer
 * @return true if the pattern was extracted successfully, false otherwise
 */
bool rift_regex_parser_extract_pattern(const char *literal_text, char *output, size_t output_size);

/**
 * @brief Register the regex syntax extension with the LibRift runtime
 *
 * @return true if registration was successful, false otherwise
 */
bool rift_regex_syntax_register(void);

/**
 * @brief Unregister the regex syntax extension from the LibRift runtime
 *
 * @return true if unregistration was successful, false otherwise
 */
bool rift_regex_syntax_unregister(void);

/**
 * @brief Check if the regex syntax extension is registered
 *
 * @return true if the extension is registered, false otherwise
 */
bool rift_regex_syntax_is_registered(void);

/**
 * @brief Generate code for a regex literal
 *
 * @param literal The regex literal
 * @param output Buffer to store the generated code
 * @param output_size Size of the output buffer
 * @return true if code generation was successful, false otherwise
 */
bool rift_regex_syntax_generate_code(const rift_regex_literal_t *literal, char *output,
                                     size_t output_size);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_SYNTAX_H */