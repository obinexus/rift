/**
 * @file parser.h
 * @brief Header file for the LibRift regex syntax parser
 *
 * This header defines the public interface for the regex syntax parser,
 * which handles both traditional regex patterns and LibRift's extended r'' syntax.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */
#ifndef LIBRIFT_REGEX_SYNTAX_PARSER_H
#define LIBRIFT_REGEX_SYNTAX_PARSER_H

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "core/automaton/flags.h"
#include "core/errors/regex_error.h"
#include "core/errors/regex_error_compat.h"
#include "core/parser/ast.h"
#include "core/parser/token.h"
#include "core/parser/token_type.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations to break circular dependencies */
struct rift_regex_syntax_lexer;
typedef struct rift_regex_syntax_lexer rift_regex_syntax_lexer_t;

struct rift_regex_tokenizer;
typedef struct rift_regex_tokenizer rift_regex_tokenizer_t;

struct rift_regex_validator;
typedef struct rift_regex_validator rift_regex_validator_t;

struct rift_regex_tokenizer *tokenizer; /**< The tokenizer instance */
struct rift_regex_syntax_lexer *lexer;  /**< The syntax lexer instance */
/**
 * @brief Parser structure
 */
typedef struct rift_regex_parser rift_regex_parser_t;
struct rift_regex_parser {
    rift_regex_tokenizer_t *tokenizer; /**< The tokenizer instance */
    rift_regex_syntax_lexer_t *lexer;  /**< The syntax lexer instance */

    rift_regex_token_t current_token;    /**< Current token being processed */
    rift_regex_ast_node_t *current_node; /**< Current AST node being processed */
    rift_regex_flags_t flags;            /**< Parsing flags */
    rift_regex_error_t error;            /**< Last error code */
    char error_message[256];             /**< Last error message */
    bool rift_syntax_mode;               /**< Whether we're parsing LibRift r'' syntax */
    bool rift_flag_enabled;              /**< Whether the -lrift flag is enabled */
    rift_regex_validator_t *validator;   /**< The validator instance */
    void (*set_error)(struct rift_regex_parser *parser, rift_regex_error_t error,
                      const char *message, ...); /**< Error setting function */
    rift_regex_ast_t *ast;                       /**< The abstract syntax tree being built */
    size_t error_position;                       /**< Position of the last error */
    bool owns_tokenizer;                         /**< Whether parser owns the tokenizer */
    bool owns_validator;                         /**< Whether parser owns the validator */
};

/**
 * @brief Free a parser instance
 *
 * @param parser The parser instance to free
 */
void rift_regex_parser_free(rift_regex_parser_t *parser);

/**
 * @brief Parse a regex pattern into an AST
 *
 * @param parser The parser instance
 * @param pattern The pattern string to parse
 * @return The parsed AST or NULL on failure
 */
rift_regex_ast_t *rift_regex_parser_parse(rift_regex_parser_t *parser, const char *pattern);

/**
 * @brief Get the last error code from the parser
 *
 * @param parser The parser instance
 * @return The last error code
 */
rift_regex_error_t rift_regex_parser_get_error(const rift_regex_parser_t *parser);

/**
 * @brief Get the last error message from the parser
 *
 * @param parser The parser instance
 * @return The last error message
 */
const char *rift_regex_parser_get_error_message(const rift_regex_parser_t *parser);

/**
 * @brief Check if the parser is in LibRift syntax mode
 *
 * @param parser The parser instance
 * @return true if in LibRift syntax mode, false otherwise
 */
bool rift_regex_parser_is_rift_syntax(const rift_regex_parser_t *parser);

/**
 * @brief Set whether the -lrift flag is enabled
 *
 * @param parser The parser instance
 * @param enabled Whether the flag should be enabled
 * @return true if successful, false otherwise
 */
bool rift_regex_parser_set_rift_flag(rift_regex_parser_t *parser, bool enabled);

/**
 * @brief Check if the -lrift flag is enabled
 *
 * @param parser The parser instance
 * @return true if the flag is enabled, false otherwise
 */
bool rift_regex_parser_is_rift_flag_enabled(const rift_regex_parser_t *parser);

/**
 * @brief Check if a pattern string uses the R'' syntax format
 *
 * @param pattern The pattern string to check
 * @return true if the pattern uses R'' syntax, false otherwise
 */
bool rift_regex_is_rift_syntax(const char *pattern);

/**
 * @brief Set parser options
 *
 * @param parser The parser instance
 * @param flags Flags controlling parser behavior
 * @return true if successful, false otherwise
 */
bool rift_regex_parser_set_flags(rift_regex_parser_t *parser, rift_regex_flags_t flags);

/**
 * @brief Get the current parser flags
 *
 * @param parser The parser instance
 * @return The current flags
 */
rift_regex_flags_t rift_regex_parser_get_flags(const rift_regex_parser_t *parser);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_SYNTAX_PARSER_H */