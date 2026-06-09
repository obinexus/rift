/**
 * @file regex_error_compact_test.c
 * @brief Compact test suite for the regex error handling in LibRift
 *
 * This file implements comprehensive tests for the error handling
 * functions in the LibRift regex engine using a compact test framework.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/automaton/automaton.h"
#include "librift/compiler/compiler.h"
#include "librift/errors/regex_error.h"

// Test utilities
#define TEST_CASE(name) static void test_##name(void)
#define RUN_TEST(name)                                                                             \
    do {                                                                                           \
        printf("Running test: %s\n", #name);                                                       \
        test_##name();                                                                             \
        printf("Test passed: %s\n\n", #name);                                                      \
        test_count++;                                                                              \
    } while (0)

#define ASSERT(expr)                                                                               \
    do {                                                                                           \
        if (!(expr)) {                                                                             \
            fprintf(stderr, "Assertion failed at %s:%d: %s\n", __FILE__, __LINE__, #expr);         \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0)

#define ASSERT_TRUE(expr) ASSERT((expr))
#define ASSERT_FALSE(expr) ASSERT(!(expr))
#define ASSERT_NULL(expr) ASSERT((expr) == NULL)
#define ASSERT_NOT_NULL(expr) ASSERT((expr) != NULL)
#define ASSERT_EQUAL_INT(a, b) ASSERT((a) == (b))
#define ASSERT_EQUAL_PTR(a, b) ASSERT((a) == (b))
#define ASSERT_EQUAL_STRING(a, b) ASSERT(strcmp((a), (b)) == 0)

// Test cases

/**
 * Test error initialization
 */
TEST_CASE(error_initialization)
{
    rift_regex_error_t error;

    // Test initialization
    ASSERT_TRUE(rift_regex_error_init(&error));
    ASSERT_EQUAL_INT(error.code, RIFT_REGEX_ERROR_NONE);
    ASSERT_EQUAL_INT(error.position, 0);
    ASSERT_EQUAL_STRING(error.message, "");

    // Test NULL parameter (should return false)
    ASSERT_FALSE(rift_regex_error_init(NULL));
}

/**
 * Test error message retrieval
 */
TEST_CASE(error_messages)
{
    rift_regex_error_t error;
    const char *error_str;

    // Initialize error structure
    rift_regex_error_init(&error);

    // Test various error codes
    error.code = RIFT_REGEX_ERROR_NONE;
    error_str = rift_regex_get_error_string(error);
    ASSERT_EQUAL_STRING(error_str, "No error");

    error.code = RIFT_REGEX_ERROR_SYNTAX;
    error_str = rift_regex_get_error_string(error);
    ASSERT_EQUAL_STRING(error_str, "Syntax error in regular expression pattern");

    error.code = RIFT_REGEX_ERROR_INVALID_ESCAPE;
    error_str = rift_regex_get_error_string(error);
    ASSERT_EQUAL_STRING(error_str, "Invalid escape sequence in regex pattern");

    error.code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
    error_str = rift_regex_get_error_string(error);
    ASSERT_EQUAL_STRING(error_str, "Invalid parameter provided to function");

    error.code = RIFT_REGEX_ERROR_MEMORY;
    error_str = rift_regex_get_error_string(error);
    ASSERT_EQUAL_STRING(error_str, "Memory allocation failure");

    error.code = RIFT_REGEX_ERROR_INTERNAL;
    error_str = rift_regex_get_error_string(error);
    ASSERT_EQUAL_STRING(error_str, "Internal regex engine error");

    error.code = RIFT_REGEX_ERROR_BUFFER_OVERFLOW;
    error_str = rift_regex_get_error_string(error);
    ASSERT_EQUAL_STRING(error_str, "Buffer overflow error");

    error.code = RIFT_REGEX_ERROR_UNSUPPORTED_FEATURE;
    error_str = rift_regex_get_error_string(error);
    ASSERT_EQUAL_STRING(error_str, "Unsupported regex feature");

    // Test unknown error code
    error.code = 9999;
    error_str = rift_regex_get_error_string(error);
    ASSERT_EQUAL_STRING(error_str, "Unknown regex error");
}

/**
 * Test error in compilation process
 */
TEST_CASE(compilation_errors)
{
    rift_regex_error_t error;
    rift_regex_error_init(&error);

    // Test invalid syntax
    rift_regex_automaton_t *automaton = rift_regex_compile_pattern("[unclosed", 0, &error);
    ASSERT_NULL(automaton);
    ASSERT_TRUE(error.code != RIFT_REGEX_ERROR_NONE);
    ASSERT_TRUE(error.position > 0);
    ASSERT_TRUE(strlen(error.message) > 0);

    const char *error_msg = rift_regex_get_compile_error_message(error);
    ASSERT_NOT_NULL(error_msg);
    ASSERT_TRUE(strlen(error_msg) > 0);

    // Test invalid character class
    rift_regex_error_init(&error);
    automaton = rift_regex_compile_pattern("[a-]", 0, &error);
    ASSERT_NULL(automaton);
    ASSERT_TRUE(error.code != RIFT_REGEX_ERROR_NONE);

    // Test unbalanced parentheses
    rift_regex_error_init(&error);
    automaton = rift_regex_compile_pattern("a(bc", 0, &error);
    ASSERT_NULL(automaton);
    ASSERT_TRUE(error.code != RIFT_REGEX_ERROR_NONE);

    // Test invalid escape sequence
    rift_regex_error_init(&error);
    automaton = rift_regex_compile_pattern("a\\", 0, &error);
    ASSERT_NULL(automaton);
    ASSERT_TRUE(error.code != RIFT_REGEX_ERROR_NONE);
}

/**
 * Test error propagation in automaton operations
 */
TEST_CASE(error_propagation)
{
    rift_regex_error_t error;
    rift_regex_error_init(&error);

    // Create a valid automaton
    rift_regex_automaton_t *automaton = rift_regex_compile_pattern("abc", 0, &error);
    ASSERT_NOT_NULL(automaton);
    ASSERT_EQUAL_INT(error.code, RIFT_REGEX_ERROR_NONE);

    // Test error setting and retrieval in automaton
    error.code = RIFT_REGEX_ERROR_INTERNAL;
    strcpy(error.message, "Test error message");
    rift_automaton_set_error(automaton, &error);

    rift_regex_error_t retrieved_error;
    ASSERT_TRUE(rift_automaton_get_last_error(automaton, &retrieved_error));
    ASSERT_EQUAL_INT(retrieved_error.code, RIFT_REGEX_ERROR_INTERNAL);
    ASSERT_EQUAL_STRING(retrieved_error.message, "Test error message");

    // Clear the error and verify it's cleared
    rift_automaton_clear_error(automaton);
    ASSERT_FALSE(rift_automaton_get_last_error(automaton, &retrieved_error));

    // Clean up
    rift_regex_automaton_free(automaton);
}

/**
 * Test error fields
 */
TEST_CASE(error_fields)
{
    rift_regex_error_t error;
    rift_regex_error_init(&error);

    // Set error fields directly
    error.code = RIFT_REGEX_ERROR_SYNTAX;
    error.position = 42;
    strcpy(error.message, "Custom error message");

    // Verify field values
    ASSERT_EQUAL_INT(error.code, RIFT_REGEX_ERROR_SYNTAX);
    ASSERT_EQUAL_INT(error.position, 42);
    ASSERT_EQUAL_STRING(error.message, "Custom error message");

    // Get error message
    const char *error_msg = rift_regex_get_error_string(error);
    ASSERT_EQUAL_STRING(error_msg, "Syntax error in regular expression pattern");

    // Reset error
    rift_regex_error_init(&error);
    ASSERT_EQUAL_INT(error.code, RIFT_REGEX_ERROR_NONE);
    ASSERT_EQUAL_INT(error.position, 0);
    ASSERT_EQUAL_STRING(error.message, "");
}

/**
 * Test memory allocation errors
 */
TEST_CASE(memory_errors)
{
    rift_regex_error_t error;

    // This test simulates memory allocation failure by attempting to
    // compile an extremely complex pattern that may cause allocation issues

    // Create a pattern with many nested groups and alternations
    char complex_pattern[10000] = "(";
    for (int i = 0; i < 1000; i++) {
        strcat(complex_pattern, "(a|b|c|d|e|f|g|h|i|j)");
    }
    strcat(complex_pattern, ")");

    rift_regex_error_init(&error);
    rift_regex_automaton_t *automaton = rift_regex_compile_pattern(complex_pattern, 0, &error);

    // If compilation succeeded, clean up
    if (automaton) {
        rift_regex_automaton_free(automaton);
    }

    // We don't assert on the outcome as it depends on available memory,
    // but the function should either return a valid automaton or set an error code
    if (!automaton) {
        ASSERT_TRUE(error.code != RIFT_REGEX_ERROR_NONE);
        ASSERT_TRUE(strlen(error.message) > 0);
    }
}

// Main function to run all tests
int
main(void)
{
    int test_count = 0;

    printf("Starting LibRift regex error handling tests\n\n");

    RUN_TEST(error_initialization);
    RUN_TEST(error_messages);
    RUN_TEST(compilation_errors);
    RUN_TEST(error_propagation);
    RUN_TEST(error_fields);
    RUN_TEST(memory_errors);

    printf("\nAll tests passed successfully!\n");
    printf("Total tests executed: %d\n", test_count);

    return 0;
}