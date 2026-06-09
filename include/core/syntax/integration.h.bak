/**
 * @file integration.h
 * @brief Interface for the LibRift regex syntax integration layer
 *
 * This file defines the public API for the integration layer between the lexer and parser
 * components of the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "core/automaton/flags.h"
#include "core/errors/regex_error.h"
#include "core/parser/ast.h"
#include "core/parser/parser.h"
#ifndef LIBRIFT_REGEX_SYNTAX_INTEGRATION_H
#define LIBRIFT_REGEX_SYNTAX_INTEGRATION_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum length of error messages
 */
#define RIFT_REGEX_MAX_ERROR_LENGTH 256

/**
 * @brief Structure for the regex syntax processing context
 */
typedef struct rift_regex_syntax_context {
    struct rift_regex_syntax_lexer *lexer;
    struct rift_regex_parser *parser;
    rift_regex_flags_t flags;
    rift_regex_error_t error;

    char error_message[RIFT_REGEX_MAX_ERROR_LENGTH];
    bool owns_lexer;
    bool owns_parser;
    bool rift_syntax_enabled;
} rift_regex_syntax_context_t;

/**
 * @brief Callback function type for token processing
 *
 * @param type The token type
 * @param value The token value (can be NULL)
 * @param position The position of the token in the input
 * @param user_data User data passed to the tokenize function
 * @return true to continue processing, false to stop
 */
typedef bool (*rift_regex_token_callback_t)(int type, const char *value, size_t position,
                                            void *user_data);

/**
 * @brief Create a new regex syntax processing context
 *
 * @param flags Compilation flags
 * @return A new context or NULL on failure
 */
rift_regex_syntax_context_t *rift_regex_syntax_context_create(rift_regex_flags_t flags);

/**
 * @brief Free resources associated with a syntax context
 *
 * @param context The context to free
 */
void rift_regex_syntax_context_free(rift_regex_syntax_context_t *context);

/**
 * @brief Check if a pattern uses the LibRift r'' syntax
 *
 * @param pattern The pattern string
 * @return true if the pattern uses LibRift r'' syntax, false otherwise
 */
bool rift_regex_syntax_is_rift_syntax(const char *pattern);

/**
 * @brief Enable or disable LibRift R'' syntax mode
 *
 * @param context The syntax context
 * @param enable true to enable, false to disable
 */
void rift_regex_syntax_set_rift_mode(rift_regex_syntax_context_t *context, bool enable);
/**
 * @brief Compile a regex pattern into an AST
 *
 * @param context The syntax context
 * @param pattern The pattern string
 * @return The AST for the pattern or NULL on failure
 */
rift_regex_ast_t *rift_regex_syntax_compile(rift_regex_syntax_context_t *context,
                                            const char *pattern);

/**
 * @brief Get the last error code from the syntax context
 *
 * @param context The syntax context
 * @return The last error code
 */
rift_regex_error_t rift_regex_syntax_get_error(const rift_regex_syntax_context_t *context);

/**
 * @brief Get the last error message from the syntax context
 *
 * @param context The syntax context
 * @return The last error message
 */
const char *rift_regex_syntax_get_error_message(const rift_regex_syntax_context_t *context);

/**
 * @brief Check if the syntax mode is LibRift r'' syntax
 *
 * @param context The syntax context
 * @return true if in LibRift syntax mode, false otherwise
 */
bool rift_regex_syntax_is_rift_mode(const rift_regex_syntax_context_t *context);

/**
 * @brief Set compilation flags for the syntax context
 *
 * @param context The syntax context
 * @param flags The new compilation flags
 * @return true if successful, false otherwise
 */
bool rift_regex_syntax_set_flags(rift_regex_syntax_context_t *context, rift_regex_flags_t flags);

/**
 * @brief Get the current compilation flags from the syntax context
 *
 * @param context The syntax context
 * @return The current compilation flags
 */
rift_regex_flags_t rift_regex_syntax_get_flags(const rift_regex_syntax_context_t *context);

/**
 * @brief Validate a regex pattern without fully compiling it
 *
 * This function checks if a pattern is syntactically valid without creating
 * a full AST, which can be more efficient for simple validation.
 *
 * @param context The syntax context
 * @param pattern The pattern string
 * @return true if the pattern is valid, false otherwise
 */
bool rift_regex_syntax_validate(rift_regex_syntax_context_t *context, const char *pattern);

/**
 * @brief Get diagnostic information about a pattern
 *
 * This function provides detailed information about the structure of a pattern,
 * which can be useful for debugging and development.
 *
 * @param context The syntax context
 * @param pattern The pattern string
 * @param diagnostics Buffer to store diagnostic information
 * @param buffer_size Size of the diagnostics buffer
 * @return true if successful, false otherwise
 */
bool rift_regex_syntax_get_diagnostics(rift_regex_syntax_context_t *context, const char *pattern,
                                       char *diagnostics, size_t buffer_size);

/**
 * @brief Check if a pattern has balanced parentheses and brackets
 *
 * This utility function checks for basic structural validity by ensuring
 * that parentheses, brackets, and braces are properly balanced.
 *
 * @param pattern The pattern string
 * @return true if balanced, false otherwise
 */
bool rift_regex_syntax_check_balanced(const char *pattern);

/**
 * @brief Utility function to extract token information
 *
 * This function takes a pattern and extracts detailed token information
 * without building a full AST.
 *
 * @param pattern The pattern string
 * @param callback Function to call for each token
 * @param user_data User data to pass to the callback
 * @return true if successful, false otherwise
 */
bool rift_regex_syntax_tokenize(const char *pattern, rift_regex_token_callback_t callback,
                                void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_SYNTAX_INTEGRATION_H */