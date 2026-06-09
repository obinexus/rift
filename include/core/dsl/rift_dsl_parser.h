/**
 * @file rift_dsl_parser.h
 * @brief Header for the parser of the .rift DSL file format
 *
 * This file declares the public API for the .rift Domain-Specific
 * Language used for regex pattern definition, testing, and serialization.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#ifndef RIFT_DSL_PARSER_H
#define RIFT_DSL_PARSER_H


/**
 * @brief Token types for the .rift DSL lexer
 */
typedef enum {
	RIFT_DSL_TOKEN_UNKNOWN = 0,
	RIFT_DSL_TOKEN_AT_SIGN,
	RIFT_DSL_TOKEN_IDENTIFIER,
	RIFT_DSL_TOKEN_EQUALS,
	RIFT_DSL_TOKEN_STRING,
	RIFT_DSL_TOKEN_LBRACE,
	RIFT_DSL_TOKEN_RBRACE,
	RIFT_DSL_TOKEN_LBRACKET,
	RIFT_DSL_TOKEN_RBRACKET,
	RIFT_DSL_TOKEN_COMMA,
	RIFT_DSL_TOKEN_COMMENT,
	RIFT_DSL_TOKEN_EOF
} rift_dsl_token_type_t;

/**
 * @brief Structure representing a token in the .rift DSL
 */
typedef struct {
	rift_dsl_token_type_t type;
	const char *lexeme;
	size_t lexeme_length;
	size_t line;
	size_t column;
} rift_dsl_token_t;

/**
 * @brief Structure for the .rift DSL lexer
 */
typedef struct {
	const char *source;
	size_t source_length;
	size_t start;
	size_t current;
	size_t line;
	size_t column;
	bool has_error;
	char *error_message;
} rift_dsl_lexer_t;

/**
 * @brief Structure representing a regex pattern in the .rift DSL
 */
typedef struct rift_dsl_pattern_struct {
	struct rift_dsl_pattern_struct *next;
	char *name;
	char *pattern;
	char **flags;
	size_t flag_count;
} rift_dsl_pattern_t;

/**
 * @brief Structure representing a test case in the .rift DSL
 */
typedef struct {
	char *input;
	bool expect_match;
	char **match_groups;
	size_t group_count;
} rift_dsl_test_case_t;

/**
 * @brief Structure representing a list of test cases in the .rift DSL
 */
typedef struct rift_dsl_test_case_list {
	struct rift_dsl_test_case_list *next;
	rift_dsl_test_case_t test_case;
} rift_dsl_test_case_list_t;

/**
 * @brief Structure representing a parsed .rift DSL file
 */
typedef struct {
	rift_dsl_pattern_t *patterns;
	rift_dsl_test_case_list_t *test_cases;
	bool has_error;
	char *error_message;
} rift_dsl_file_t;

/**
 * @brief Parse a .rift DSL source string
 *
 * @param source The source string to parse
 * @return Handle to the parsed DSL file or NULL on error
 */
void *rift_dsl_parse(const char *source);

/**
 * @brief Load and parse a .rift DSL file from disk
 *
 * @param filename The path to the file to load
 * @return Handle to the parsed DSL file or NULL on error
 */
void *rift_dsl_load_file(const char *filename);

/**
 * @brief Free resources associated with a parsed .rift DSL file
 *
 * @param handle Handle returned by rift_dsl_parse or rift_dsl_load_file
 */
void rift_dsl_free(void *handle);

/**
 * @brief Get the error message if parsing failed
 *
 * @param handle Handle returned by rift_dsl_parse or rift_dsl_load_file
 * @return Error message or NULL if no error occurred
 */
const char *rift_dsl_get_error_message(void *handle);

/**
 * @brief Get the number of patterns in the parsed .rift DSL file
 *
 * @param handle Handle returned by rift_dsl_parse or rift_dsl_load_file
 * @return Number of patterns
 */
size_t rift_dsl_get_pattern_count(void *handle);

/**
 * @brief Get a pattern from the parsed .rift DSL file
 *
 * @param handle Handle returned by rift_dsl_parse or rift_dsl_load_file
 * @param index Index of the pattern to get
 * @param name Pointer to store the pattern name
 * @param pattern Pointer to store the pattern string
 * @return true if successful, false otherwise
 */
bool rift_dsl_get_pattern(void *handle, size_t index, const char **name, const char **pattern);

/**
 * @brief Get the flags for a pattern from the parsed .rift DSL file
 *
 * @param handle Handle returned by rift_dsl_parse or rift_dsl_load_file
 * @param index Index of the pattern to get flags for
 * @param flags Pointer to store the array of flags
 * @param count Pointer to store the number of flags
 * @return true if successful, false otherwise
 */
bool rift_dsl_get_pattern_flags(void *handle, size_t index, const char ***flags, size_t *count);

/**
 * @brief Get the number of test cases in the parsed .rift DSL file
 *
 * @param handle Handle returned by rift_dsl_parse or rift_dsl_load_file
 * @return Number of test cases
 */
size_t rift_dsl_get_test_case_count(void *handle);

/**
 * @brief Get a test case from the parsed .rift DSL file
 *
 * @param handle Handle returned by rift_dsl_parse or rift_dsl_load_file
 * @param index Index of the test case to get
 * @param input Pointer to store the test input string
 * @param expect_match Pointer to store whether the test expects a match
 * @return true if successful, false otherwise
 */
bool rift_dsl_get_test_case(void *handle, size_t index, const char **input, bool *expect_match);

/**
 * @brief Get the expected match groups for a test case
 *
 * @param handle Handle returned by rift_dsl_parse or rift_dsl_load_file
 * @param index Index of the test case to get groups for
 * @param groups Pointer to store the array of expected match groups
 * @param count Pointer to store the number of groups
 * @return true if successful, false otherwise
 */
bool rift_dsl_get_test_case_groups(void *handle, size_t index, const char ***groups, size_t *count);

#endif /* RIFT_DSL_PARSER_H */
