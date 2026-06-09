/**
 * @file syntax_test.c
 * @brief Unit tests for LibRift regex syntax functions
 *
 * This file contains tests for the regex literal syntax functionality
 * in the LibRift language.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/syntax/syntax.h"

/* Test helper macros */
#define TEST_ASSERT(condition, message)                                                            \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            fprintf(stderr, "TEST FAILED: %s\n", message);                                         \
            return false;                                                                          \
        }                                                                                          \
    } while (0)

/* Test function prototypes */
static bool test_regex_literal_create_free(void);
static bool test_regex_literal_compile(void);
static bool test_regex_literal_getters_setters(void);
static bool test_regex_literal_clone(void);
static bool test_regex_parser_extract_pattern(void);
static bool test_regex_syntax_registration(void);
static bool test_regex_parser_is_literal(void);

/**
 * @brief Test creation and freeing of regex literals
 */
static bool
test_regex_literal_create_free(void)
{
    const char *pattern = "a+b*c?";
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_CASE_INSENSITIVE;

    // Test creation with valid parameters
    rift_regex_literal_t *literal = rift_regex_literal_create(pattern, flags);
    TEST_ASSERT(literal != NULL, "Failed to create regex literal");
    TEST_ASSERT(literal->pattern != NULL, "Literal pattern is NULL");
    TEST_ASSERT(strcmp(literal->pattern, pattern) == 0, "Pattern mismatch");
    TEST_ASSERT(literal->flags == flags, "Flags mismatch");
    TEST_ASSERT(literal->compiled_pattern == NULL, "Compiled pattern should be NULL initially");

    // Test freeing
    rift_regex_literal_free(literal);

    // Test creation with NULL pattern
    literal = rift_regex_literal_create(NULL, flags);
    TEST_ASSERT(literal == NULL, "Should return NULL for NULL pattern");

    return true;
}

/**
 * @brief Test regex literal compilation
 */
static bool
test_regex_literal_compile(void)
{
    const char *pattern = "a+b*c?";
    rift_regex_literal_t *literal = rift_regex_literal_create(pattern, 0);
    TEST_ASSERT(literal != NULL, "Failed to create regex literal");

    // Test compilation
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    bool result = rift_regex_literal_compile(literal, &error);
    TEST_ASSERT(result == true, "Compilation should succeed");
    TEST_ASSERT(error.code == RIFT_REGEX_ERROR_NONE, "Error code should be NONE");
    TEST_ASSERT(literal->compiled_pattern != NULL, "Compiled pattern should not be NULL");

    // Test with invalid pattern
    rift_regex_literal_t *invalid_literal = rift_regex_literal_create("a[b", 0);
    result = rift_regex_literal_compile(invalid_literal, &error);
    TEST_ASSERT(result == false, "Compilation of invalid pattern should fail");

    rift_regex_literal_free(literal);
    rift_regex_literal_free(invalid_literal);
    return true;
}

/**
 * @brief Test getters and setters for regex literals
 */
static bool
test_regex_literal_getters_setters(void)
{
    const char *pattern = "a+b*c?";
    rift_regex_literal_t *literal = rift_regex_literal_create(pattern, 0);
    TEST_ASSERT(literal != NULL, "Failed to create regex literal");

    // Compile the pattern first
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    bool result = rift_regex_literal_compile(literal, &error);
    TEST_ASSERT(result == true, "Compilation should succeed");

    // Test getters
    TEST_ASSERT(rift_regex_literal_get_pattern(literal) != NULL,
                "Get pattern should not return NULL");
    TEST_ASSERT(strcmp(rift_regex_literal_get_string(literal), pattern) == 0,
                "Get string mismatch");
    TEST_ASSERT(rift_regex_literal_get_flags(literal) == 0, "Initial flags should be 0");

    // Test flags setter
    rift_regex_flags_t new_flags = RIFT_REGEX_FLAG_CASE_INSENSITIVE | RIFT_REGEX_FLAG_MULTILINE;
    TEST_ASSERT(rift_regex_literal_set_flags(literal, new_flags) == true,
                "Set flags should succeed");
    TEST_ASSERT(rift_regex_literal_get_flags(literal) == new_flags,
                "Get flags mismatch after setting");

    rift_regex_literal_free(literal);
    return true;
}

/**
 * @brief Test cloning of regex literals
 */
static bool
test_regex_literal_clone(void)
{
    const char *pattern = "a+b*c?";
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_CASE_INSENSITIVE;

    rift_regex_literal_t *original = rift_regex_literal_create(pattern, flags);
    TEST_ASSERT(original != NULL, "Failed to create regex literal");

    // Compile the original
    rift_regex_error_t error = {RIFT_REGEX_ERROR_NONE};
    bool result = rift_regex_literal_compile(original, &error);
    TEST_ASSERT(result == true, "Compilation should succeed");

    // Clone and verify
    rift_regex_literal_t *clone = rift_regex_literal_clone(original);
    TEST_ASSERT(clone != NULL, "Failed to clone regex literal");
    TEST_ASSERT(clone != original, "Clone should be a different object");
    TEST_ASSERT(clone->pattern != NULL, "Clone pattern should not be NULL");
    TEST_ASSERT(strcmp(clone->pattern, original->pattern) == 0, "Clone pattern content mismatch");
    TEST_ASSERT(clone->flags == original->flags, "Clone flags mismatch");

    rift_regex_literal_free(original);
    rift_regex_literal_free(clone);
    return true;
}

/**
 * @brief Test pattern extraction from R'' syntax
 */
static bool
test_regex_parser_extract_pattern(void)
{
    // Test with valid R'' syntax
    const char *literal_text = "R'[abc]+[def]*'";
    char output[256] = {0};

    bool result = rift_regex_parser_extract_pattern(literal_text, output, sizeof(output));
    TEST_ASSERT(result == true, "Pattern extraction should succeed");
    TEST_ASSERT(strcmp(output, "[abc]+[def]*") == 0, "Extracted pattern mismatch");

    // Test with invalid syntax
    const char *invalid_text = "R'incomplete";
    result = rift_regex_parser_extract_pattern(invalid_text, output, sizeof(output));
    TEST_ASSERT(result == false, "Pattern extraction from invalid syntax should fail");

    return true;
}

/**
 * @brief Test syntax registration functions
 */
static bool
test_regex_syntax_registration(void)
{
    // Unregister first to ensure known state
    rift_regex_syntax_unregister();
    TEST_ASSERT(rift_regex_syntax_is_registered() == false, "Initial state should be unregistered");

    // Test registration
    bool result = rift_regex_syntax_register();
    TEST_ASSERT(result == true, "Registration should succeed");
    TEST_ASSERT(rift_regex_syntax_is_registered() == true,
                "Should be registered after registration");

    // Test unregistration
    result = rift_regex_syntax_unregister();
    TEST_ASSERT(result == true, "Unregistration should succeed");
    TEST_ASSERT(rift_regex_syntax_is_registered() == false,
                "Should be unregistered after unregistration");

    return true;
}

/**
 * @brief Test detection of regex literals in source code
 */
static bool
test_regex_parser_is_literal(void)
{
    const char *valid_source = "R'pattern'";
    size_t start_pos = 0;
    size_t end_pos = 0;

    // Test with valid literal
    bool result = rift_regex_parser_is_literal(valid_source, &start_pos, &end_pos);
    TEST_ASSERT(result == true, "Should detect valid literal");
    TEST_ASSERT(start_pos == 0, "Start position should be 0");
    TEST_ASSERT(end_pos == strlen(valid_source), "End position should be at end of string");

    // Test with invalid literal
    const char *invalid_source = "Not-a-literal";
    result = rift_regex_parser_is_literal(invalid_source, &start_pos, &end_pos);
    TEST_ASSERT(result == false, "Should not detect invalid literal");

    // Test with embedded literal
    const char *embedded_source = "prefix R'pattern' suffix";
    result = rift_regex_parser_is_literal(embedded_source, &start_pos, &end_pos);
    TEST_ASSERT(result == true, "Should detect embedded literal");

    return true;
}

/* Run all tests */
static void
run_tests(void)
{
    struct {
        const char *name;
        bool (*func)(void);
    } tests[] = {{"Regex Literal Create/Free", test_regex_literal_create_free},
                 {"Regex Literal Compile", test_regex_literal_compile},
                 {"Regex Literal Getters/Setters", test_regex_literal_getters_setters},
                 {"Regex Literal Clone", test_regex_literal_clone},
                 {"Regex Parser Extract Pattern", test_regex_parser_extract_pattern},
                 {"Regex Syntax Registration", test_regex_syntax_registration},
                 {"Regex Parser Is Literal", test_regex_parser_is_literal},
                 {NULL, NULL}};

    int passed = 0;
    int failed = 0;

    for (int i = 0; tests[i].name != NULL; i++) {
        printf("Running test: %s\n", tests[i].name);

        if (tests[i].func()) {
            printf("  PASSED\n");
            passed++;
        } else {
            printf("  FAILED\n");
            failed++;
        }
    }

    printf("\nTest Summary: %d passed, %d failed\n", passed, failed);
}

/**
 * @brief Main function to run all tests
 */
int
main(void)
{
    printf("Running LibRift Regex Syntax Tests\n");
    run_tests();
    return 0;
}
