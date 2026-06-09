/**
 * @file lexer.h
 * @brief Header file for the regex syntax lexer component of LibRift
 *
 * This file defines the interface for tokenizing regular expression patterns
 * with support for the extended `r''` syntax in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "core/automaton/automaton.h"
#include "core/automaton/flags.h"
#include "core/automaton/state.h"
#include "core/automaton/transition.h"
#include "core/errors/regex_error.h"
#include "core/parser/ast.h"
#include "core/parser/ast_node.h"
#include "core/parser/parser.h"
#ifndef LIBRIFT_REGEX_SYNTAX_LEXER_H
#define LIBRIFT_REGEX_SYNTAX_LEXER_H



#ifdef __cplusplus
extern "C" {
#endif

/* Maximum length of error messages */
#define RIFT_REGEX_MAX_ERROR_LENGTH 256

/* Token value allocation buffer size */
#define TOKEN_VALUE_BUFFER_SIZE 128
/**
 * @brief Syntax lexer structure
 */
/* Forward declaration of the lexer structure */
typedef struct rift_regex_syntax_lexer rift_regex_syntax_lexer_t;
struct rift_regex_syntax_lexer {
    const char *input;                       /* Input string being lexed */
    size_t position;                         /* Current position in the input */
    size_t length;                           /* Length of the input string */
    bool is_rift_syntax;                     /* Whether the input is using LibRift r'' syntax */
    char quote_char;                         /* Quote character used in LibRift syntax (' or ") */
    rift_regex_token_t current;              /* Current token */
    char error[RIFT_REGEX_MAX_ERROR_LENGTH]; /* Error message buffer */
};

/**
 * Forward declarations for regex syntax scanning functions
 *
 * @brief These functions handle parsing different components of regex patterns
 *
 * scan_escape_sequence - Scans and tokenizes escape sequences in the pattern
 * @param lexer The regex syntax lexer context
 * @param start_pos Starting position in the input
 * @return Token representing the escape sequence
 *
 * scan_character_class - Scans and tokenizes character class expressions
 * @param lexer The regex syntax lexer context
 * @param start_pos Starting position in the input
 * @return Token representing the character class
 *
 * scan_group - Scans and tokenizes grouped subexpressions
 * @param lexer The regex syntax lexer context
 * @param start_pos Starting position in the input
 * @return Token representing the group
 */
/* Forward declarations of  functions */
/* Forward declaration for lexer_advance */
char lexer_advance(rift_regex_syntax_lexer_t *lexer);
char peek(const rift_regex_syntax_lexer_t *lexer);
char peek_next(const rift_regex_syntax_lexer_t *lexer);
rift_regex_token_t create_token(rift_regex_token_type_t type, const char *value, size_t position);
rift_regex_token_t create_char_token(rift_regex_token_type_t type, char c, size_t position);
rift_regex_token_t scan_group(rift_regex_syntax_lexer_t *lexer, size_t start_pos);
rift_regex_token_t scan_character_class(rift_regex_syntax_lexer_t *lexer, size_t start_pos);
rift_regex_token_t scan_quantifier(rift_regex_syntax_lexer_t *lexer, size_t start_pos);
rift_regex_token_t scan_escape_sequence(rift_regex_syntax_lexer_t *lexer, size_t start_pos);

/**
 * @brief Create a new syntax lexer
 *
 * @param input The input string to tokenize
 * @return A new syntax lexer or NULL on failure
 */
rift_regex_syntax_lexer_t *rift_regex_syntax_lexer_create(const char *input);

/**
 * @brief Free resources associated with a syntax lexer
 *
 * @param lexer The syntax lexer to free
 */
void rift_regex_syntax_lexer_free(rift_regex_syntax_lexer_t *lexer);

/**
 * @brief Get the next token from the input
 *
 * @param lexer The syntax lexer
 * @return The next token
 */
rift_regex_token_t rift_regex_syntax_lexer_next_token(rift_regex_syntax_lexer_t *lexer);

/**
 * @brief Peek at the next token without consuming it
 *
 * @param lexer The syntax lexer
 * @return The next token
 */
rift_regex_token_t rift_regex_syntax_lexer_peek_token(rift_regex_syntax_lexer_t *lexer);

/**
 * @brief Set the input string for the lexer
 *
 * @param lexer The syntax lexer
 * @param input The input string to set
 */
void rift_regex_syntax_lexer_set_input(rift_regex_syntax_lexer_t *lexer, const char *input);
/**
 * @brief Get the current position in the input string
 *
 * @param lexer The syntax lexer
 * @return The current position
 */
size_t rift_regex_syntax_lexer_get_position(const rift_regex_syntax_lexer_t *lexer);

/**
 * @brief Get the current character at the input position
 *
 * @param lexer The syntax lexer
 * @return The current character or '\0' if at the end of input
 */
char rift_regex_syntax_lexer_get_current_char(const rift_regex_syntax_lexer_t *lexer);

/**
 * @brief Check if the lexer is at the end of input
 *
 * @param lexer The syntax lexer
 * @return true if at the end of input, false otherwise
 */
bool rift_regex_syntax_lexer_is_at_end(const rift_regex_syntax_lexer_t *lexer);

/**
 * @brief Check if the current input is using the LibRift r'' syntax
 *
 * @param lexer The syntax lexer
 * @return true if the input is using LibRift syntax, false otherwise
 */
bool rift_regex_syntax_lexer_is_rift_syntax(const rift_regex_syntax_lexer_t *lexer);

/**
 * @brief Get the last error message
 *
 * @param lexer The syntax lexer
 * @return The last error message or NULL if no error occurred
 */
const char *rift_regex_syntax_lexer_get_error(const rift_regex_syntax_lexer_t *lexer);

/**
 * @brief Reset the lexer to the beginning of the input
 *
 * @param lexer The syntax lexer
 */
void rift_regex_syntax_lexer_reset(rift_regex_syntax_lexer_t *lexer);
rift_regex_token_t scan_quantifier(rift_regex_syntax_lexer_t *lexer, size_t start_pos);

/**
 * @brief Free resources associated with a token
 *
 * @param token The token to free
 */
void rift_regex_syntax_token_free(rift_regex_token_t *token);

/**
 * @brief Get the name of a token type
 *
 * @param type The token type
 * @return The name of the token type
 */
const char *rift_regex_syntax_token_type_name(rift_regex_token_type_t type);

/**
 * @brief Check if the input string begins with `r''` syntax
 *
 * This is a utility function to quickly check if a pattern string
 * uses the LibRift extended syntax format without creating a lexer.
 *
 * @param input The input string to check
 * @return true if the input uses LibRift syntax, false otherwise
 */
bool rift_regex_is_rift_syntax(const char *input);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_SYNTAX_LEXER_H */