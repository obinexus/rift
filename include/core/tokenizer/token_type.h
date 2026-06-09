/**
 * @file token_type.h
 * @brief Token type definitions for the LibRift regex engine
 *
 * This file defines the token types used for tokenizing regular expression patterns
 * in the LibRift regex engine. Separating token types into their own header promotes
 * proper compile-time safety and type checking.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#ifndef LIBRIFT_REGEX_PARSER_TOKEN_TYPE_H
#define LIBRIFT_REGEX_PARSER_TOKEN_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Token types for regular expressions
 */
typedef enum rift_regex_token_type {
    RIFT_REGEX_TOKEN_LITERAL, /**< Literal character */
    RIFT_REGEX_TOKEN_DOT,     /**< Dot (any character) */
    RIFT_REGEX_TOKEN_CARET,   /**< Caret (start anchor) */
    RIFT_REGEX_TOKEN_DOLLAR,  /**< Dollar (end anchor) */

    /* Quantifiers */
    RIFT_REGEX_TOKEN_STAR,     /**< Star (0 or more) */
    RIFT_REGEX_TOKEN_PLUS,     /**< Plus (1 or more) */
    RIFT_REGEX_TOKEN_QUESTION, /**< Question (0 or 1) */

    /* Delimiters */
    RIFT_REGEX_TOKEN_LPAREN,    /**< Left parenthesis */
    RIFT_REGEX_TOKEN_RPAREN,    /**< Right parenthesis */
    RIFT_REGEX_TOKEN_LBRACKET,  /**< Left bracket */
    RIFT_REGEX_TOKEN_RBRACKET,  /**< Right bracket */
    RIFT_REGEX_TOKEN_PIPE,      /**< Pipe (alternation) */
    RIFT_REGEX_TOKEN_LBRACE,    /**< Left brace */
    RIFT_REGEX_TOKEN_RBRACE,    /**< Right brace */
    RIFT_REGEX_TOKEN_COMMA,     /**< Comma */
    RIFT_REGEX_TOKEN_BACKSLASH, /**< Backslash */

    /* Character classes and groups */
    RIFT_REGEX_TOKEN_CHAR_CLASS,    /**< Character class */
    RIFT_REGEX_TOKEN_GROUP_START,   /**< Group specifier (?... */
    RIFT_REGEX_TOKEN_GROUP_END,     /**< Group specifier end */
    RIFT_REGEX_TOKEN_NAMED_GROUP,   /**< Named group specifier */
    RIFT_REGEX_TOKEN_NON_CAPTURING, /**< Non-capturing group */

    /* Lookahead and lookbehind assertions */
    RIFT_REGEX_TOKEN_LOOKAHEAD,           /**< Positive lookahead */
    RIFT_REGEX_TOKEN_NEGATIVE_LOOKAHEAD,  /**< Negative lookahead */
    RIFT_REGEX_TOKEN_LOOKBEHIND,          /**< Positive lookbehind */
    RIFT_REGEX_TOKEN_NEGATIVE_LOOKBEHIND, /**< Negative lookbehind */

    /* Special group types */
    RIFT_REGEX_TOKEN_ATOMIC_GROUP, /**< Atomic group */
    RIFT_REGEX_TOKEN_COMMENT,      /**< Comment */
    RIFT_REGEX_TOKEN_OPTION,       /**< Option */
    /* Rift specific tokens */
    RIFT_REGEX_TOKEN_RIFT_QUOTE_END, /**< End of Rift quote */
    RIFT_REGEX_TOKEN_RIFT_ESCAPE,    /**< Rift escape sequence */

    /* References */
    RIFT_REGEX_TOKEN_BACKREFERENCE,       /**< Backreference */
    RIFT_REGEX_TOKEN_NAMED_BACKREFERENCE, /**< Named backreference */

    RIFT_REGEX_TOKEN_RIFT_PREFIX,      /**< Rift prefix token */
    RIFT_REGEX_TOKEN_RIFT_QUOTE_START, /**< Start of Rift quote */
    /* Escape sequences and boundaries */
    RIFT_REGEX_TOKEN_ESCAPE_SEQUENCE,   /**< Escape sequence */
    RIFT_REGEX_TOKEN_WORD_BOUNDARY,     /**< Word boundary */
    RIFT_REGEX_TOKEN_NOT_WORD_BOUNDARY, /**< Not word boundary */
    RIFT_REGEX_TOKEN_START_OF_INPUT,    /**< Start of input */
    RIFT_REGEX_TOKEN_END_OF_INPUT,      /**< End of input */
    RIFT_REGEX_TOKEN_BACKREF_RESET,     /**< Backref reset \K */

    /* Quantifier token */
    RIFT_REGEX_TOKEN_QUANTIFIER, /**< Quantifier token */

    /* Control tokens */
    RIFT_REGEX_TOKEN_ERROR, /**< Error token */
    RIFT_REGEX_TOKEN_END    /**< End of input */

} rift_regex_token_type_t;

/**
 * @brief Get a string representation of a token type
 *
 * @param type The token type
 * @return A string representing the token type
 */
const char *rift_regex_token_type_to_string(rift_regex_token_type_t type);

/**
 * @brief Check if a token type is a quantifier
 *
 * @param type The token type to check
 * @return true if the token type is a quantifier, false otherwise
 */
bool rift_regex_token_type_is_quantifier(rift_regex_token_type_t type);

/**
 * @brief Check if a token type is a group start
 *
 * @param type The token type to check
 * @return true if the token type is a group start, false otherwise
 */
bool rift_regex_token_type_is_group_start(rift_regex_token_type_t type);

/**
 * @brief Check if a token type is an assertion
 *
 * @param type The token type to check
 * @return true if the token type is an assertion, false otherwise
 */
bool rift_regex_token_type_is_assertion(rift_regex_token_type_t type);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_PARSER_TOKEN_TYPE_H */