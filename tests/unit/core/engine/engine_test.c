/**
 * @file engine_test.c
 * @brief Unit tests for the LibRift regex engine
 *
 * Tests the functionality of the regex engine component of LibRift,
 * including pattern compilation, matching, and specific syntax features.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the testing framework
#include "ctest.h"

// Include the regex engine headers
#include "librift/engine/engine.h"

// Test basic pattern compilation
CTEST(regex_engine, compile_basic_pattern)
{
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    rift_regex_pattern_t *pattern = rift_regex_compile("abc", RIFT_REGEX_FLAG_NONE, &error);

    ASSERT_NOT_NULL(pattern);
    ASSERT_EQUAL(RIFT_REGEX_ERROR_NONE, error.code);

    rift_regex_pattern_free(pattern);
}

// Test pattern with capture groups
CTEST(regex_engine, pattern_with_groups)
{
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    rift_regex_pattern_t *pattern = rift_regex_compile("(a)(b)(c)", RIFT_REGEX_FLAG_NONE, &error);

    ASSERT_NOT_NULL(pattern);
    ASSERT_EQUAL(RIFT_REGEX_ERROR_NONE, error.code);
    ASSERT_EQUAL(3, rift_regex_pattern_get_group_count(pattern));

    rift_regex_pattern_free(pattern);
}

// Test LibRift syntax with single quotes
CTEST(regex_engine, rift_syntax_single_quotes)
{
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    rift_regex_pattern_t *pattern =
        rift_regex_compile("r'abc'", RIFT_REGEX_FLAG_RIFT_SYNTAX, &error);

    ASSERT_NOT_NULL(pattern);
    ASSERT_EQUAL(RIFT_REGEX_ERROR_NONE, error.code);

    rift_regex_pattern_free(pattern);
}

// Test LibRift syntax with double quotes
CTEST(regex_engine, rift_syntax_double_quotes)
{
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    rift_regex_pattern_t *pattern =
        rift_regex_compile("r\"abc\"", RIFT_REGEX_FLAG_RIFT_SYNTAX, &error);

    ASSERT_NOT_NULL(pattern);
    ASSERT_EQUAL(RIFT_REGEX_ERROR_NONE, error.code);

    rift_regex_pattern_free(pattern);
}

// Test LibRift syntax without appropriate flag
CTEST(regex_engine, rift_syntax_missing_flag)
{
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    rift_regex_pattern_t *pattern = rift_regex_compile("r'abc'", RIFT_REGEX_FLAG_NONE, &error);

    ASSERT_NULL(pattern);
    ASSERT_EQUAL(RIFT_REGEX_ERROR_UNSUPPORTED_FEATURE, error.code);
}

// Test pattern with unterminated LibRift quote
CTEST(regex_engine, rift_syntax_unterminated_quote)
{
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    rift_regex_pattern_t *pattern =
        rift_regex_compile("r'abc", RIFT_REGEX_FLAG_RIFT_SYNTAX, &error);

    ASSERT_NULL(pattern);
    ASSERT_EQUAL(RIFT_REGEX_ERROR_SYNTAX, error.code);
}

// Test complex pattern with quantifiers and character classes
CTEST(regex_engine, complex_pattern)
{
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    rift_regex_pattern_t *pattern =
        rift_regex_compile("[a-z]+[0-9]{2,5}", RIFT_REGEX_FLAG_NONE, &error);

    ASSERT_NOT_NULL(pattern);
    ASSERT_EQUAL(RIFT_REGEX_ERROR_NONE, error.code);

    rift_regex_pattern_free(pattern);
}

// Test pattern with syntax error
CTEST(regex_engine, pattern_syntax_error)
{
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    rift_regex_pattern_t *pattern = rift_regex_compile("[unclosed", RIFT_REGEX_FLAG_NONE, &error);

    ASSERT_NULL(pattern);
    ASSERT_EQUAL(RIFT_REGEX_ERROR_SYNTAX, error.code);
}

// Test pattern flags preservation
CTEST(regex_engine, pattern_flags)
{
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_CASE_INSENSITIVE | RIFT_REGEX_FLAG_DOTALL;
    rift_regex_pattern_t *pattern = rift_regex_compile("abc", flags, &error);

    ASSERT_NOT_NULL(pattern);
    ASSERT_EQUAL(flags, rift_regex_pattern_get_flags(pattern));

    rift_regex_pattern_free(pattern);
}

// Test empty pattern
CTEST(regex_engine, empty_pattern)
{
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    rift_regex_pattern_t *pattern = rift_regex_compile("", RIFT_REGEX_FLAG_NONE, &error);

    ASSERT_NOT_NULL(pattern);
    ASSERT_EQUAL(RIFT_REGEX_ERROR_NONE, error.code);

    rift_regex_pattern_free(pattern);
}

// Test null parameters
CTEST(regex_engine, null_parameters)
{
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    rift_regex_pattern_t *pattern = rift_regex_compile(NULL, RIFT_REGEX_FLAG_NONE, &error);

    ASSERT_NULL(pattern);
    ASSERT_EQUAL(RIFT_REGEX_ERROR_INVALID_PARAMETER, error.code);
}

// Test simple matching functionality
CTEST(regex_engine, simple_matches)
{
    // Test if "abc" matches the pattern "abc"
    ASSERT_TRUE(rift_regex_matches("abc", "abc", RIFT_REGEX_FLAG_NONE));

    // Test if "ABC" doesn't match "abc" without case insensitive flag
    ASSERT_FALSE(rift_regex_matches("abc", "ABC", RIFT_REGEX_FLAG_NONE));

    // Test if "ABC" matches "abc" with case insensitive flag
    ASSERT_TRUE(rift_regex_matches("abc", "ABC", RIFT_REGEX_FLAG_CASE_INSENSITIVE));
}

// Test LibRift extended syntax features
CTEST(regex_engine, rift_extended_syntax)
{
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    rift_regex_pattern_t *pattern =
        rift_regex_compile("r'\\d{3}-\\d{2}-\\d{4}'", // Social security number pattern
                           RIFT_REGEX_FLAG_RIFT_SYNTAX, &error);

    ASSERT_NOT_NULL(pattern);
    ASSERT_EQUAL(RIFT_REGEX_ERROR_NONE, error.code);

    rift_regex_pattern_free(pattern);
}

// Test automaton creation
CTEST(regex_engine, pattern_automaton)
{
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    rift_regex_pattern_t *pattern = rift_regex_compile("abc", RIFT_REGEX_FLAG_NONE, &error);

    ASSERT_NOT_NULL(pattern);
    ASSERT_NOT_NULL(rift_regex_pattern_get_automaton(pattern));

    rift_regex_pattern_free(pattern);
}

// Test error without providing error pointer
CTEST(regex_engine, no_error_pointer)
{
    rift_regex_pattern_t *pattern = rift_regex_compile("[unclosed", RIFT_REGEX_FLAG_NONE, NULL);
    ASSERT_NULL(pattern);
}

int
main(int argc, const char *argv[])
{
    return ctest_main(argc, argv);
}