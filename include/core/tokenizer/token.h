/**
 * @file token.h
 * @brief Token structure for the LibRift regex engine
 *
 * This file defines the token structure and associated functions for working
 * with tokens in the LibRift regex engine tokenizer.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#include "core/errors/regex_error.h"
#include "core/parser/validator.h"
#include "core/syntax/lexer.h"
#ifndef LIBRIFT_REGEX_PARSER_TOKEN_H
#define LIBRIFT_REGEX_PARSER_TOKEN_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Token structure for regular expressions
 */
typedef struct {
    rift_regex_token_type_t type; /**< Token type */
    char *value;                  /**< Token value */
    size_t position;              /**< Position in the input */
    size_t start;                 /**< Start position of the token */
    size_t end;                   /**< End position of the token */
} rift_regex_token_t;
/**

/**
 * @brief Create a new token
 *
 * @param type The token type
 * @param value The token value (will be copied, can be NULL)
 * @param position The position in the input string
 * @return A new token
 */
rift_regex_token_t rift_regex_token_create(rift_regex_token_type_t type, const char *value,
                                           size_t position);

/**
 * @brief Create a token with null value
 *
 * @param type The token type
 * @param position The position in the input string
 * @return A new token with NULL value
 */
rift_regex_token_t rift_regex_token_create_simple(rift_regex_token_type_t type, size_t position);

/**
 * @brief Create an error token
 *
 * @param message The error message (will be copied)
 * @param position The position in the input string
 * @return An error token
 */
rift_regex_token_t rift_regex_token_create_error(const char *message, size_t position);

/**
 * @brief Create an end token
 *
 * @param position The position in the input string
 * @return An end token
 */
rift_regex_token_t rift_regex_token_create_end(size_t position);

/**
 * @brief Copy a token
 *
 * @param token The token to copy
 * @return A copy of the token
 */
rift_regex_token_t rift_regex_token_copy(const rift_regex_token_t *token);

/**
 * @brief Free resources associated with a token
 *
 * @param token Pointer to the token to free
 */
void rift_regex_token_free(rift_regex_token_t *token);

/**
 * @brief Free the value of a token without freeing the token itself
 *
 * @param token Pointer to the token
 */
void rift_regex_token_free_value(rift_regex_token_t *token);

/**
 * @brief Check if a token is of a specific type
 *
 * @param token The token to check
 * @param type The type to check against
 * @return true if the token is of the specified type, false otherwise
 */
bool rift_regex_token_is_type(const rift_regex_token_t *token, rift_regex_token_type_t type);

/**
 * @brief Check if a token is an error
 *
 * @param token The token to check
 * @return true if the token is an error, false otherwise
 */
bool rift_regex_token_is_error(const rift_regex_token_t *token);

/**
 * @brief Check if a token is the end token
 *
 * @param token The token to check
 * @return true if the token is the end token, false otherwise
 */
bool rift_regex_token_is_end(const rift_regex_token_t *token);

/**
 * @brief Check if a token has a value
 *
 * @param token The token to check
 * @return true if the token has a value, false otherwise
 */
bool rift_regex_token_has_value(const rift_regex_token_t *token);

/**
 * @brief Get the value of a token
 *
 * @param token The token
 * @return The token value or NULL if the token has no value
 */
const char *rift_regex_token_get_value(const rift_regex_token_t *token);

/**
 * @brief Get the position of a token
 *
 * @param token The token
 * @return The token position
 */
size_t rift_regex_token_get_position(const rift_regex_token_t *token);

/**
 * @brief Get the type of a token
 *
 * @param token The token
 * @return The token type
 */
rift_regex_token_type_t rift_regex_token_get_type(const rift_regex_token_t *token);

/**
 * @brief Create a string representation of a token
 *
 * @param token The token
 * @param buffer Buffer to store the string
 * @param buffer_size Size of the buffer
 * @return true if the token was successfully converted to a string, false otherwise
 */
bool rift_regex_token_to_string(const rift_regex_token_t *token, char *buffer, size_t buffer_size);

/**
 * @brief Check if two tokens are equal
 *
 * @param token1 The first token
 * @param token2 The second token
 * @return true if the tokens are equal, false otherwise
 */
bool rift_regex_token_equals(const rift_regex_token_t *token1, const rift_regex_token_t *token2);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_PARSER_TOKEN_H */