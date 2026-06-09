/**
core/syntax/parser.h
tmp/librift_analysis/librift-4933472-W/backup/v1/src/parser/parser.h
tmp/librift_analysis/librift-4933472-W/backup/v1/src/syntax/parser.h
tmp/librift_analysis/librift-4933472-W/backup/v2/parser/parser.h
tmp/librift_analysis/librift-4933472-W/backup/v2/syntax/parser.h
tmp/librift_analysis/librift-4933472-W/src/core/parser/parser.h
tmp/librift_analysis/librift-4933472-W/src/core/syntax/parser.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/parser/parser.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/syntax/parser.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v2/parser/parser.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v2/syntax/parser.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/parser/parser.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/syntax/parser.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/parser/parser.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/syntax/parser.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v2/parser/parser.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v2/syntax/parser.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/parser/parser.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/syntax/parser.h"
core/parser/parser.h"
 * @file parser.c
 * @brief Implementation of the regex syntax parser for LibRift
 *
 * This file provides the parsing functionality for regular expression patterns
 * with special handling for the extended `r''` syntax in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/parser/parser.h
#include "core/syntax/syntax.h"
#include "librift/syntax/parser.h"

/** @brief Create a new parser
 *
 * @param flags Parsing flags
 * @param rift_flag Whether the -lrift flag is enabled
 * @return A new parser or NULL on failure
 */
rift_regex_parser_t *
rift_regex_parser_create(rift_regex_flags_t flags, bool rift_flag)
{
    rift_regex_parser_t *parser = (rift_regex_parser_t *)malloc(sizeof(rift_regex_parser_t));
    if (!parser) {
        return NULL;
    }

    // Initialize parser structure
    parser->lexer = NULL;
    parser->flags = flags;
    parser->error = (rift_regex_error_t){RIFT_REGEX_ERROR_NONE};
    parser->error_message[0] = '\0';
    parser->rift_syntax_mode = false;
    parser->rift_flag_enabled = rift_flag;

    // Initialize current token
    parser->current_token.type = RIFT_REGEX_TOKEN_END;
    parser->current_token.value = NULL;
    parser->current_token.position = 0;

    return parser;
}

/**
 * @brief Free resources associated with a parser
 *
 * @param parser The parser to free
 */
void
rift_regex_parser_free(rift_regex_parser_t *parser)
{
    if (!parser) {
        return;
    }

    // Free the lexer if it exists
    if (parser->lexer) {
        rift_regex_syntax_lexer_free(parser->lexer);
    }

    // Free the current token value if it exists
    if (parser->current_token.value) {
        free(parser->current_token.value);
    }

    // Free the parser
    free(parser);
}

/**
 * @brief Set an error in the parser
 *
 * @param parser The parser
 * @param code Error code
 * @param message Error message
 */
static void
set_error(rift_regex_parser_t *parser, rift_regex_error_t code, const char *message)
{
    if (!parser) {
        return;
    }

    parser->error = code;
    strncpy(parser->error_message, message, sizeof(parser->error_message) - 1);
    parser->error_message[sizeof(parser->error_message) - 1] = '\0';
}

/**
 * @brief Advance to the next token
 *
 * @param parser The parser
 * @return The next token
 */
static rift_regex_token_t
advance(rift_regex_parser_t *parser)
{
    if (!parser || !parser->lexer) {
        rift_regex_token_t empty_token = {RIFT_REGEX_TOKEN_END, NULL, 0};
        return empty_token;
    }

    // Free the previous token's value if it exists
    if (parser->current_token.value) {
        free(parser->current_token.value);
        parser->current_token.value = NULL;
    }

    // Get the next token
    int token_type = rift_regex_syntax_lexer_next_token(parser->lexer);
    parser->current_token.type = (rift_regex_token_type_t)token_type;
    parser->current_token.value = NULL; // You might need to fetch the value from the lexer
    parser->current_token.position = parser->lexer->position; // Update position

    return parser->current_token;
}

/**
 * @brief Match the current token with an expected type
 *
 * @param parser The parser
 * @param type Expected token type
 * @return true if the token matches, false otherwise
 */
static bool
match(rift_regex_parser_t *parser, rift_regex_token_type_t type)
{
    if (!parser) {
        return false;
    }

    return parser->current_token.type == type;
}

/**
 * @brief Consume the current token if it matches the expected type, otherwise set an error
 *
 * @param parser The parser
 * @param type Expected token type
 * @param error_message Error message if the token doesn't match
 * @return true if the token was consumed, false otherwise
 */
static bool
consume(rift_regex_parser_t *parser, rift_regex_token_type_t type, const char *error_message)
{
    if (!parser) {
        return false;
    }

    if (match(parser, type)) {
        advance(parser);
        return true;
    }

    set_error(parser, (rift_regex_error_t){RIFT_REGEX_ERROR_UNEXPECTED_TOKEN}, error_message);
    return false;
}

/**
 * @brief Check if a string is using LibRift r'' syntax
 *
 * Updated to check for 'R' prefix instead of 'r'
 *
 * @param pattern The string to check
 * @return true if the string uses LibRift r'' syntax, false otherwise
 */
bool
rift_regex_is_rift_syntax(const char *pattern)
{
    if (!pattern || strlen(pattern) < 3) {
        return false;
    }

    // Check if the pattern starts with 'R' followed by two single quotes
    return (pattern[0] == 'R' && pattern[1] == '\'' && pattern[2] == '\'');
}

/**
 * @brief Parse a LibRift r'' syntax pattern
 *
 * @param parser The parser
 * @return The AST node for the pattern or NULL on failure
 */
static rift_regex_ast_node_t *
parse_rift_syntax(rift_regex_parser_t *parser)
{
    if (!parser || !parser->lexer) {
        return NULL;
    }

    // Create a new AST node for the pattern
    rift_regex_ast_node_t *pattern_node = rift_regex_ast_node_create(RIFT_REGEX_AST_NODE_PATTERN);
    if (!pattern_node) {
        set_error(parser, (rift_regex_error_t){RIFT_REGEX_ERROR_MEMORY_ALLOCATION},
                  "Failed to allocate AST node for pattern");
        return NULL;
    }

    // Expect the pattern to start with r prefix
    if (!consume(parser, RIFT_REGEX_TOKEN_RIFT_PREFIX, "Expected 'r' prefix for LibRift syntax")) {
        rift_regex_ast_node_free(pattern_node);
        return NULL;
    }

    // Expect the opening quote
    if (!consume(parser, RIFT_REGEX_TOKEN_RIFT_QUOTE_START,
                 "Expected opening quote after 'r' prefix")) {
        rift_regex_ast_node_free(pattern_node);
        return NULL;
    }

    // Parse the pattern content until we reach the closing quote
    while (!match(parser, RIFT_REGEX_TOKEN_RIFT_QUOTE_END) &&
           !match(parser, RIFT_REGEX_TOKEN_END)) {
        // TODO: Parse the actual regex pattern within the quotes
        // For now, we're just skipping to the end quote to demonstrate the r'' syntax handling
        advance(parser);
    }

    // Expect the closing quote
    if (!consume(parser, RIFT_REGEX_TOKEN_RIFT_QUOTE_END,
                 "Expected closing quote for LibRift syntax")) {
        rift_regex_ast_node_free(pattern_node);
        return NULL;
    }

    return pattern_node;
}

/**
 * @brief Parse a traditional regex pattern
 *
 * @param parser The parser
 * @return The AST node for the pattern or NULL on failure
 */
static rift_regex_ast_node_t *
parse_standard_syntax(rift_regex_parser_t *parser)
{
    if (!parser || !parser->lexer) {
        return NULL;
    }

    // Create a new AST node for the pattern
    rift_regex_ast_node_t *pattern_node = rift_regex_ast_node_create(RIFT_REGEX_AST_NODE_PATTERN);
    if (!pattern_node) {
        set_error(parser, (rift_regex_error_t){RIFT_REGEX_ERROR_MEMORY_ALLOCATION},
                  "Failed to allocate AST node for pattern");
        return NULL;
    }

    // TODO: Implement standard regex pattern parsing
    // For now, we'll just skip all tokens until the end
    while (!match(parser, RIFT_REGEX_TOKEN_END)) {
        advance(parser);
    }

    return pattern_node;
}

/**
 * @brief Parse a regex pattern
 *
 * @param parser The parser
 * @param pattern Pattern string
 * @return The AST for the pattern or NULL on failure
 */
rift_regex_ast_t *
rift_regex_parser_parse(rift_regex_parser_t *parser, const char *pattern)
{
    if (!parser || !pattern) {
        if (parser) {
            set_error(parser, (rift_regex_error_t){RIFT_REGEX_ERROR_INVALID_PARAMETER},
                      "Invalid parameters");
        }
        return NULL;
    }

    // Reset parser state
    parser->error = (rift_regex_error_t){RIFT_REGEX_ERROR_NONE};
    parser->error_message[0] = '\0';

    // Free the previous lexer if it exists
    if (parser->lexer) {
        rift_regex_syntax_lexer_free(parser->lexer);
        parser->lexer = NULL;
    }

    // Create a new lexer for the pattern
    parser->lexer = rift_regex_syntax_lexer_create(pattern);
    if (!parser->lexer) {
        set_error(parser, (rift_regex_error_t){RIFT_REGEX_ERROR_MEMORY_ALLOCATION},
                  "Failed to create lexer");
        return NULL;
    }

    // Check if this is LibRift r'' syntax
    bool is_rift_syntax = rift_regex_is_rift_syntax(pattern);
    parser->rift_syntax_mode = is_rift_syntax;

    // If the pattern uses R'' syntax but -lrift flag is not enabled, report an error
    if (is_rift_syntax && !parser->rift_flag_enabled) {
        set_error(parser, (rift_regex_error_t){RIFT_REGEX_ERROR_UNSUPPORTED_FEATURE},
                  "LibRift R'' syntax detected but -lrift flag is not enabled");
        return NULL;
    }

    // Create a new AST
    rift_regex_ast_t *ast = rift_regex_ast_create();
    if (!ast) {
        set_error(parser, (rift_regex_error_t){RIFT_REGEX_ERROR_MEMORY_ALLOCATION},
                  "Failed to create AST");
        return NULL;
    }

    // Get the first token
    advance(parser);

    // Parse the pattern based on syntax mode
    rift_regex_ast_node_t *pattern_node = NULL;
    if (is_rift_syntax) {
        pattern_node = parse_rift_syntax(parser);
    } else {
        pattern_node = parse_standard_syntax(parser);
    }

    // Check if parsing was successful
    if (!pattern_node) {
        rift_regex_ast_free(ast);
        return NULL;
    }

    // Set the root node of the AST
    if (!rift_regex_ast_set_root(ast, pattern_node)) {
        rift_regex_ast_node_free(pattern_node);
        rift_regex_ast_free(ast);
        set_error(parser, (rift_regex_error_t){RIFT_REGEX_ERROR_INTERNAL},
                  "Failed to set AST root");
        return NULL;
    }

    return ast;
}

/**
 * @brief Get the last error code from the parser
 *
 * @param parser The parser
 * @return The last error code
 */
rift_regex_error_t
rift_regex_parser_get_error(const rift_regex_parser_t *parser)
{
    if (!parser) {
        return (rift_regex_error_t){RIFT_REGEX_ERROR_INVALID_PARAMETER};
    }

    return parser->error;
}

/**
 * @brief Get the last error message from the parser
 *
 * @param parser The parser
 * @return The last error message
 */
const char *
rift_regex_parser_get_error_message(const rift_regex_parser_t *parser)
{
    if (!parser) {
        return "Invalid parser";
    }

    return parser->error_message;
}

/**
 * @brief Check if the parser is in LibRift syntax mode
 *
 * @param parser The parser
 * @return true if in LibRift syntax mode, false otherwise
 */
bool
rift_regex_parser_is_rift_syntax(const rift_regex_parser_t *parser)
{
    if (!parser) {
        return false;
    }

    return parser->rift_syntax_mode;
}

/**
 * @brief Set whether the -lrift flag is enabled
 *
 * @param parser The parser
 * @param enabled Whether the -lrift flag is enabled
 * @return true if successful, false otherwise
 */
bool
rift_regex_parser_set_rift_flag(rift_regex_parser_t *parser, bool enabled)
{
    if (!parser) {
        return false;
    }

    parser->rift_flag_enabled = enabled;
    return true;
}

/**
 * @brief Check if the -lrift flag is enabled
 *
 * @param parser The parser
 * @return true if the -lrift flag is enabled, false otherwise
 */
bool
rift_regex_parser_is_rift_flag_enabled(const rift_regex_parser_t *parser)
{
    if (!parser) {
        return false;
    }

    return parser->rift_flag_enabled;
}
