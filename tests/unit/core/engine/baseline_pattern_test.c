/**
 * @file baseline_pattern_tests.c
 * @brief Unit tests for baseline regex pattern utilities
 *
 * Tests initialization, retrieval, and cleanup of baseline regex patterns.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "librift/compiler/compiler.h"
#include "librift/engine/pattern.h"
#include "librift/errors/regex_error.h"
#include "librift/patterns/baseline_patterns.h"
#include "librift/test/unit_test.h"

/* Test initialization and cleanup */
RIFT_TEST(baseline_patterns_init_cleanup)
{
    rift_regex_error_t error = {0};

    /* Test initialization */
    RIFT_ASSERT(rift_baseline_patterns_initialize(&error), "Pattern initialization should succeed");
    RIFT_ASSERT(error.code == 0, "No error should be set during initialization");

    /* Test double initialization (should be safe) */
    RIFT_ASSERT(rift_baseline_patterns_initialize(NULL), "Second initialization should succeed");

    /* Test cleanup */
    rift_baseline_patterns_cleanup();

    /* Test initialization after cleanup */
    RIFT_ASSERT(rift_baseline_patterns_initialize(&error),
                "Re-initialization after cleanup should succeed");
    rift_baseline_patterns_cleanup();
}

/* Test pattern retrieval */
RIFT_TEST(baseline_patterns_get)
{
    rift_regex_error_t error = {0};
    rift_regex_pattern_t *pattern;

    /* Initialize patterns */
    RIFT_ASSERT(rift_baseline_patterns_initialize(&error), "Pattern initialization should succeed");

    /* Test retrieval of each pattern type */
    pattern = rift_baseline_patterns_get(RIFT_BASELINE_PATTERN_EMPTY_STATE, &error);
    RIFT_ASSERT(pattern != NULL, "Empty state pattern should be retrievable");

    pattern = rift_baseline_patterns_get(RIFT_BASELINE_PATTERN_WHITESPACE, &error);
    RIFT_ASSERT(pattern != NULL, "Whitespace pattern should be retrievable");

    pattern = rift_baseline_patterns_get(RIFT_BASELINE_PATTERN_WORD_BOUNDARY, &error);
    RIFT_ASSERT(pattern != NULL, "Word boundary pattern should be retrievable");

    pattern = rift_baseline_patterns_get(RIFT_BASELINE_PATTERN_IDENTIFIER, &error);
    RIFT_ASSERT(pattern != NULL, "Identifier pattern should be retrievable");

    pattern = rift_baseline_patterns_get(RIFT_BASELINE_PATTERN_NUMERIC, &error);
    RIFT_ASSERT(pattern != NULL, "Numeric pattern should be retrievable");

    pattern = rift_baseline_patterns_get(RIFT_BASELINE_PATTERN_STRING, &error);
    RIFT_ASSERT(pattern != NULL, "String pattern should be retrievable");

    pattern = rift_baseline_patterns_get(RIFT_BASELINE_PATTERN_COMMENT, &error);
    RIFT_ASSERT(pattern != NULL, "Comment pattern should be retrievable");

    pattern = rift_baseline_patterns_get(RIFT_BASELINE_PATTERN_OPERATOR, &error);
    RIFT_ASSERT(pattern != NULL, "Operator pattern should be retrievable");

    pattern = rift_baseline_patterns_get(RIFT_BASELINE_PATTERN_FUNCTION, &error);
    RIFT_ASSERT(pattern != NULL, "Function pattern should be retrievable");

    pattern = rift_baseline_patterns_get(RIFT_BASELINE_PATTERN_STATE_TRANSITION, &error);
    RIFT_ASSERT(pattern != NULL, "State transition pattern should be retrievable");

    /* Test invalid pattern type */
    pattern = rift_baseline_patterns_get(999, &error);
    RIFT_ASSERT(pattern == NULL, "Invalid pattern type should return NULL");
    RIFT_ASSERT(error.code != 0, "Error should be set for invalid pattern type");

    /* Cleanup */
    rift_baseline_patterns_cleanup();
}

/* Test pattern matching using compiled patterns */
RIFT_TEST(baseline_patterns_matching)
{
    rift_regex_error_t error = {0};
    rift_regex_pattern_t *pattern;

    /* Initialize patterns */
    RIFT_ASSERT(rift_baseline_patterns_initialize(&error), "Pattern initialization should succeed");

    /* Test identifier pattern with sample input */
    pattern = rift_baseline_patterns_get(RIFT_BASELINE_PATTERN_IDENTIFIER, &error);
    RIFT_ASSERT(rift_regex_match(pattern, "validIdentifier", &error),
                "Should match valid identifier");
    RIFT_ASSERT(!rift_regex_match(pattern, "1invalidIdentifier", &error),
                "Should not match identifier starting with number");

    /* Test numeric pattern */
    pattern = rift_baseline_patterns_get(RIFT_BASELINE_PATTERN_NUMERIC, &error);
    RIFT_ASSERT(rift_regex_match(pattern, "123", &error), "Should match integer");
    RIFT_ASSERT(rift_regex_match(pattern, "123.45", &error), "Should match decimal");
    RIFT_ASSERT(rift_regex_match(pattern, "-123.45e10", &error),
                "Should match scientific notation");
    RIFT_ASSERT(!rift_regex_match(pattern, "abc", &error), "Should not match non-numeric string");

    /* Cleanup */
    rift_baseline_patterns_cleanup();
}

/* Register tests with the test framework */
RIFT_TEST_SUITE(baseline_patterns)
{
    RIFT_TEST_RUN(baseline_patterns_init_cleanup);
    RIFT_TEST_RUN(baseline_patterns_get);
    RIFT_TEST_RUN(baseline_patterns_matching);
}
