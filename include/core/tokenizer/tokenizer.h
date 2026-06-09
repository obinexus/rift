/**
 * @file tokenizer.h
 * @brief Header file for the regex tokenizer component of LibRift
 *
 * This file defines the interface for tokenizing regular expression patterns
 * in the LibRift regex engine, leveraging the token_type.h and token.h interfaces
 * for enhanced compile-time safety.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <core/errors/regex_error.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifndef LIBRIFT_REGEX_PARSER_TOKENIZER_H
#define LIBRIFT_REGEX_PARSER_TOKENIZER_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Forward declaration of the tokenizer structure
 */
typedef struct rift_regex_tokenizer rift_regex_tokenizer_t;

/**
 * @brief Create a new tokenizer
 *
 * @param input The input string to tokenize
 * @return A new tokenizer or NULL on failure
 */
rift_regex_tokenizer_t *rift_regex_tokenizer_create(const char *input);

/**
 * @brief Create a new tokenizer with a specific length
 *
 * @param input The input string to tokenize
 * @param length The length of the input string or (size_t)-1 for strlen
 * @return A new tokenizer or NULL on failure
 */
rift_regex_tokenizer_t *rift_regex_tokenizer_create_with_length(const char *input, size_t length);

/**
 * @brief Free resources associated with a tokenizer
 *
 * @param tokenizer The tokenizer to free
 */
void rift_regex_tokenizer_free(rift_regex_tokenizer_t *tokenizer);

/**
 * @brief Get the next token from the input
 *
 * @param tokenizer The tokenizer
 * @return The next token
 */
rift_regex_token_t rift_regex_tokenizer_next_token(rift_regex_tokenizer_t *tokenizer);

/**
 * @brief Peek at the next token without consuming it
 *
 * @param tokenizer The tokenizer
 * @return The next token
 */
rift_regex_token_t rift_regex_tokenizer_peek_token(rift_regex_tokenizer_t *tokenizer);

/**
 * @brief Get the current position in the input string
 *
 * @param tokenizer The tokenizer
 * @return The current position
 */
size_t rift_regex_tokenizer_get_position(const rift_regex_tokenizer_t *tokenizer);

/**
 * @brief Get the current character at the input position
 *
 * @param tokenizer The tokenizer
 * @return The current character or '\0' if at the end of input
 */
char rift_regex_tokenizer_get_current_char(const rift_regex_tokenizer_t *tokenizer);

/**
 * @brief Check if the tokenizer is at the end of input
 *
 * @param tokenizer The tokenizer
 * @return true if at the end of input, false otherwise
 */
bool rift_regex_tokenizer_is_at_end(const rift_regex_tokenizer_t *tokenizer);

/**
 * @brief Get the last error message
 *
 * @param tokenizer The tokenizer
 * @return The last error message or NULL if no error occurred
 */
const char *rift_regex_tokenizer_get_error(const rift_regex_tokenizer_t *tokenizer);

/**
 * @brief Set the error message
 *
 * @param tokenizer The tokenizer
 * @param format The error message format
 * @param ... Additional arguments for the format string
 * @return true if the error was set, false otherwise
 */
bool rift_regex_tokenizer_set_error(rift_regex_tokenizer_t *tokenizer, const char *format, ...);

/**
 * @brief Reset the tokenizer to the beginning of the input
 *
 * @param tokenizer The tokenizer
 */
void rift_regex_tokenizer_reset(rift_regex_tokenizer_t *tokenizer);

/**
 * @brief Get the input string
 *
 * @param tokenizer The tokenizer
 * @return The input string
 */
const char *rift_regex_tokenizer_get_input(const rift_regex_tokenizer_t *tokenizer);

/**
 * @brief Get the input length
 *
 * @param tokenizer The tokenizer
 * @return The input length
 */
size_t rift_regex_tokenizer_get_input_length(const rift_regex_tokenizer_t *tokenizer);

/**
 * @brief Advance the position by one character
 *
 * @param tokenizer The tokenizer
 * @return The character that was at the position before advancing
 */
char rift_regex_tokenizer_advance(rift_regex_tokenizer_t *tokenizer);

/**
 * @brief Look ahead in the input without advancing the position
 *
 * @param tokenizer The tokenizer
 * @param offset The offset from the current position
 * @return The character at the specified offset or '\0' if out of bounds
 */
char rift_regex_tokenizer_peek(const rift_regex_tokenizer_t *tokenizer, size_t offset);

/**
 * @brief Match the current character and advance if it matches
 *
 * @param tokenizer The tokenizer
 * @param expected The expected character
 * @return true if the character matched and the position was advanced, false otherwise
 */
bool rift_regex_tokenizer_match(rift_regex_tokenizer_t *tokenizer, char expected);

/**
 * @brief Skip whitespace characters
 *
 * @param tokenizer The tokenizer
 * @return The number of whitespace characters skipped
 */
size_t rift_regex_tokenizer_skip_whitespace(rift_regex_tokenizer_t *tokenizer);

/**
 * @brief Create an iterator to tokenize an input string
 *
 * This function allows for iterator-like processing of tokens.
 *
 * @param input The input string to tokenize
 * @return A new tokenizer or NULL on failure
 */
rift_regex_tokenizer_t *rift_regex_tokenizer_iterator_create(const char *input);

/**
 * @brief Get the next token using the iterator pattern
 *
 * @param tokenizer The tokenizer iterator
 * @param token Pointer to store the next token
 * @return true if a token was retrieved, false if at the end or an error occurred
 */
bool rift_regex_tokenizer_iterator_next(rift_regex_tokenizer_t *tokenizer,
                                        rift_regex_token_t *token);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_PARSER_TOKENIZER_H */