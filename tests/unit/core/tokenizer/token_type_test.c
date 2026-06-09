/**
 * @file token_type_test.c
 * @brief Unit tests for token type functions in LibRift regex engine
 *
 * This file contains tests for token type string conversion and classification
 * functions in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/parser/token_type.h"

/* Simple test framework */
static int tests_run = 0;
static int tests_failed = 0;

#define TEST_ASSERT(message, condition)                                                            \
    do {                                                                                           \
        tests_run++;                                                                               \
        if (!(condition)) {                                                                        \
            fprintf(stderr, "FAILED: %s (line %d)\n", message, __LINE__);                          \
            tests_failed++;                                                                        \
            return false;                                                                          \
        }                                                                                          \
    } while (0)

#define RUN_TEST(test)                                                                             \
    do {                                                                                           \
        printf("Running %s...\n", #test);                                                          \
        if (!test()) {                                                                             \
            fprintf(stderr, "Test %s failed.\n", #test);                                           \
        } else {                                                                                   \
            printf("Test %s passed.\n", #test);                                                    \
        }                                                                                          \
    } while (0)

/* Test token type string conversion */
static bool
test_token_type_to_string(void)
{
    /* Test valid token types */
    TEST_ASSERT("LITERAL token string",
                strcmp(rift_regex_token_type_to_string(RIFT_REGEX_TOKEN_LITERAL), "LITERAL") == 0);
    TEST_ASSERT("DOT token string",
                strcmp(rift_regex_token_type_to_string(RIFT_REGEX_TOKEN_DOT), "DOT") == 0);
    TEST_ASSERT("STAR token string",
                strcmp(rift_regex_token_type_to_string(RIFT_REGEX_TOKEN_STAR), "STAR") == 0);
    TEST_ASSERT("END token string",
                strcmp(rift_regex_token_type_to_string(RIFT_REGEX_TOKEN_END), "END") == 0);

    /* Test invalid token type (out of range) */
    TEST_ASSERT("Invalid token type",
                strcmp(rift_regex_token_type_to_string(9999), "UNKNOWN") == 0);

    return true;
}

/* Test quantifier classification */
static bool
test_token_type_is_quantifier(void)
{
    /* Test known quantifiers */
    TEST_ASSERT("STAR is a quantifier",
                rift_regex_token_type_is_quantifier(RIFT_REGEX_TOKEN_STAR) == true);
    TEST_ASSERT("PLUS is a quantifier",
                rift_regex_token_type_is_quantifier(RIFT_REGEX_TOKEN_PLUS) == true);
    TEST_ASSERT("QUESTION is a quantifier",
                rift_regex_token_type_is_quantifier(RIFT_REGEX_TOKEN_QUESTION) == true);
    TEST_ASSERT("LBRACE is a quantifier",
                rift_regex_token_type_is_quantifier(RIFT_REGEX_TOKEN_LBRACE) == true);

    /* Test non-quantifiers */
    TEST_ASSERT("LITERAL is not a quantifier",
                rift_regex_token_type_is_quantifier(RIFT_REGEX_TOKEN_LITERAL) == false);
    TEST_ASSERT("DOT is not a quantifier",
                rift_regex_token_type_is_quantifier(RIFT_REGEX_TOKEN_DOT) == false);
    TEST_ASSERT("LPAREN is not a quantifier",
                rift_regex_token_type_is_quantifier(RIFT_REGEX_TOKEN_LPAREN) == false);

    return true;
}

/* Test group start classification */
static bool
test_token_type_is_group_start(void)
{
    /* Test known group starts */
    TEST_ASSERT("LPAREN is a group start",
                rift_regex_token_type_is_group_start(RIFT_REGEX_TOKEN_LPAREN) == true);
    TEST_ASSERT("GROUP_START is a group start",
                rift_regex_token_type_is_group_start(RIFT_REGEX_TOKEN_GROUP_START) == true);
    TEST_ASSERT("NAMED_GROUP is a group start",
                rift_regex_token_type_is_group_start(RIFT_REGEX_TOKEN_NAMED_GROUP) == true);
    TEST_ASSERT("NON_CAPTURING is a group start",
                rift_regex_token_type_is_group_start(RIFT_REGEX_TOKEN_NON_CAPTURING) == true);

    /* Test non-group starts */
    TEST_ASSERT("LITERAL is not a group start",
                rift_regex_token_type_is_group_start(RIFT_REGEX_TOKEN_LITERAL) == false);
    TEST_ASSERT("RPAREN is not a group start",
                rift_regex_token_type_is_group_start(RIFT_REGEX_TOKEN_RPAREN) == false);
    TEST_ASSERT("STAR is not a group start",
                rift_regex_token_type_is_group_start(RIFT_REGEX_TOKEN_STAR) == false);

    return true;
}

/* Test assertion classification */
static bool
test_token_type_is_assertion(void)
{
    /* Test known assertions */
    TEST_ASSERT("CARET is an assertion",
                rift_regex_token_type_is_assertion(RIFT_REGEX_TOKEN_CARET) == true);
    TEST_ASSERT("DOLLAR is an assertion",
                rift_regex_token_type_is_assertion(RIFT_REGEX_TOKEN_DOLLAR) == true);
    TEST_ASSERT("WORD_BOUNDARY is an assertion",
                rift_regex_token_type_is_assertion(RIFT_REGEX_TOKEN_WORD_BOUNDARY) == true);
    TEST_ASSERT("LOOKAHEAD is an assertion",
                rift_regex_token_type_is_assertion(RIFT_REGEX_TOKEN_LOOKAHEAD) == true);

    /* Test non-assertions */
    TEST_ASSERT("LITERAL is not an assertion",
                rift_regex_token_type_is_assertion(RIFT_REGEX_TOKEN_LITERAL) == false);
    TEST_ASSERT("STAR is not an assertion",
                rift_regex_token_type_is_assertion(RIFT_REGEX_TOKEN_STAR) == false);
    TEST_ASSERT("LPAREN is not an assertion",
                rift_regex_token_type_is_assertion(RIFT_REGEX_TOKEN_LPAREN) == false);

    return true;
}

int
main(void)
{
    printf("Running LibRift token type unit tests...\n");

    RUN_TEST(test_token_type_to_string);
    RUN_TEST(test_token_type_is_quantifier);
    RUN_TEST(test_token_type_is_group_start);
    RUN_TEST(test_token_type_is_assertion);

    printf("\nTest summary: %d tests, %d passed, %d failed\n", tests_run, tests_run - tests_failed,
           tests_failed);

    return tests_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}