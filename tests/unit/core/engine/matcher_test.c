/**
 * @file matcher_test.c
 * @brief Unit tests for the regex matcher component of LibRift
 *
 * This file contains tests for the matcher interface for the LibRift regex engine,
 * validating the pattern matching functionality against various input strings.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/compiler/compiler.h"
#include "librift/engine/matcher.h"

// Test status tracking
static int tests_run = 0;
static int tests_failed = 0;

// Helper macros for testing
#define TEST(name) static void test_##name(void)
#define RUN_TEST(name)                                                                             \
    do {                                                                                           \
        printf("Running test: %s\n", #name);                                                       \
        test_##name();                                                                             \
        tests_run++;                                                                               \
        printf("Test %s passed\n", #name);                                                         \
    } while (0)

#define ASSERT(condition, message)                                                                 \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            printf("ASSERTION FAILED: %s\n", message);                                             \
            tests_failed++;                                                                        \
            return;                                                                                \
        }                                                                                          \
    } while (0)

// Helper function to create a pattern for testing
static rift_regex_pattern_t *
create_test_pattern(const char *pattern_str)
{
    rift_regex_error_t error;
    rift_regex_compiler_t *compiler = rift_regex_compiler_create(RIFT_REGEX_DEFAULT);
    assert(compiler != NULL);

    rift_regex_pattern_t *pattern =
        rift_regex_compiler_compile(compiler, pattern_str, strlen(pattern_str), &error);
    rift_regex_compiler_free(compiler);
    return pattern;
}

// Test matcher creation and destruction
TEST(matcher_create_free)
{
    rift_regex_pattern_t *pattern = create_test_pattern("abc");
    ASSERT(pattern != NULL, "Failed to create pattern");

    rift_regex_matcher_t *matcher = rift_matcher_create(pattern, RIFT_MATCHER_DEFAULT);
    ASSERT(matcher != NULL, "Failed to create matcher");

    rift_matcher_free(matcher);
    // No assertion needed - just testing it doesn't crash

    // Also test create_from_string
    rift_regex_error_t error;
    matcher =
        rift_matcher_create_from_string("abc", RIFT_REGEX_DEFAULT, RIFT_MATCHER_DEFAULT, &error);
    ASSERT(matcher != NULL, "Failed to create matcher from string");

    rift_matcher_free(matcher);
}

// Test basic matching functionality
TEST(matcher_basic_matching)
{
    rift_regex_error_t error;
    rift_regex_matcher_t *matcher =
        rift_matcher_create_from_string("abc", RIFT_REGEX_DEFAULT, RIFT_MATCHER_DEFAULT, &error);
    ASSERT(matcher != NULL, "Failed to create matcher");

    ASSERT(rift_matcher_set_input(matcher, "abcdef", 6), "Failed to set input");

    rift_regex_match_t match;
    ASSERT(rift_matcher_matches(matcher, &match), "Pattern should match");
    ASSERT(match.start == 0 && match.end == 3, "Incorrect match position");

    // Reset and test find
    rift_matcher_reset(matcher);
    ASSERT(rift_matcher_find_next(matcher, &match), "Failed to find pattern");
    ASSERT(match.start == 0 && match.end == 3, "Incorrect find position");

    rift_matcher_free(matcher);
}

// Test matching with groups
TEST(matcher_groups)
{
    rift_regex_error_t error;
    rift_regex_matcher_t *matcher = rift_matcher_create_from_string("(a)(b)(c)", RIFT_REGEX_DEFAULT,
                                                                    RIFT_MATCHER_DEFAULT, &error);
    ASSERT(matcher != NULL, "Failed to create matcher");

    ASSERT(rift_matcher_set_input(matcher, "abc", 3), "Failed to set input");

    rift_regex_match_t match;
    ASSERT(rift_matcher_matches(matcher, &match), "Pattern should match");
    ASSERT(match.group_count == 3, "Should have 3 captured groups");

    // Verify group contents if the match API supports it
    // This would depend on how groups are implemented in your regex engine

    rift_matcher_free(matcher);
}

// Test matcher find_all functionality
TEST(matcher_find_all)
{
    rift_regex_error_t error;
    rift_regex_matcher_t *matcher =
        rift_matcher_create_from_string("a+", RIFT_REGEX_DEFAULT, RIFT_MATCHER_DEFAULT, &error);
    ASSERT(matcher != NULL, "Failed to create matcher");

    ASSERT(rift_matcher_set_input(matcher, "aaa bbb aaa", 11), "Failed to set input");

    rift_regex_match_t matches[10];
    size_t num_matches = 0;

    ASSERT(rift_matcher_find_all(matcher, matches, 10, &num_matches), "Find all failed");
    ASSERT(num_matches == 2, "Should find 2 matches");

    ASSERT(matches[0].start == 0 && matches[0].end == 3, "First match incorrect");
    ASSERT(matches[1].start == 8 && matches[1].end == 11, "Second match incorrect");

    rift_matcher_free(matcher);
}

// Test replacement functionality
TEST(matcher_replace)
{
    rift_regex_error_t error;
    rift_regex_matcher_t *matcher =
        rift_matcher_create_from_string("a+", RIFT_REGEX_DEFAULT, RIFT_MATCHER_DEFAULT, &error);
    ASSERT(matcher != NULL, "Failed to create matcher");

    ASSERT(rift_matcher_set_input(matcher, "aaa bbb aaa", 11), "Failed to set input");

    char output[20];
    size_t num_replacements = 0;

    ASSERT(rift_matcher_replace(matcher, "X", output, sizeof(output), &num_replacements),
           "Replace failed");
    ASSERT(num_replacements == 2, "Should have 2 replacements");
    ASSERT(strcmp(output, "X bbb X") == 0, "Incorrect replacement result");

    rift_matcher_free(matcher);
}

// Test split functionality
TEST(matcher_split)
{
    rift_regex_error_t error;
    rift_regex_matcher_t *matcher =
        rift_matcher_create_from_string(",", RIFT_REGEX_DEFAULT, RIFT_MATCHER_DEFAULT, &error);
    ASSERT(matcher != NULL, "Failed to create matcher");

    ASSERT(rift_matcher_set_input(matcher, "one,two,three", 13), "Failed to set input");

    char *parts[5];
    size_t num_parts = 0;

    ASSERT(rift_matcher_split(matcher, parts, 5, &num_parts), "Split failed");
    ASSERT(num_parts == 3, "Should have 3 parts");

    ASSERT(strcmp(parts[0], "one") == 0, "First part incorrect");
    ASSERT(strcmp(parts[1], "two") == 0, "Second part incorrect");
    ASSERT(strcmp(parts[2], "three") == 0, "Third part incorrect");

    // Free the allocated parts
    for (size_t i = 0; i < num_parts; i++) {
        free(parts[i]);
    }

    rift_matcher_free(matcher);
}

// Test timeout functionality
TEST(matcher_timeout)
{
    rift_regex_error_t error;
    // Create a potentially expensive regex (depends on implementation)
    rift_regex_matcher_t *matcher =
        rift_matcher_create_from_string("(a+)+b", RIFT_REGEX_DEFAULT, RIFT_MATCHER_DEFAULT, &error);
    ASSERT(matcher != NULL, "Failed to create matcher");

    // Set a very short timeout
    ASSERT(rift_matcher_set_timeout(matcher, 1), "Failed to set timeout");

    // Input designed to cause backtracking
    ASSERT(rift_matcher_set_input(matcher, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaac", 34),
           "Failed to set input");

    rift_regex_match_t match;
    rift_matcher_matches(matcher, &match); // Expect this to timeout

    // Check if timeout was detected
    ASSERT(rift_matcher_timed_out(matcher), "Timeout should have occurred");

    rift_matcher_free(matcher);
}

// Test backtracking depth
TEST(matcher_backtrack_depth)
{
    rift_regex_error_t error;
    rift_regex_matcher_t *matcher =
        rift_matcher_create_from_string("(a+)+b", RIFT_REGEX_DEFAULT, RIFT_MATCHER_DEFAULT, &error);
    ASSERT(matcher != NULL, "Failed to create matcher");

    // Set max backtracking depth
    ASSERT(rift_matcher_set_max_backtrack_depth(matcher, 100), "Failed to set max depth");
    ASSERT(rift_matcher_get_backtrack_depth(matcher) == 0, "Initial depth should be 0");

    // Input designed to cause backtracking
    ASSERT(rift_matcher_set_input(matcher, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaac", 34),
           "Failed to set input");

    rift_regex_match_t match;
    rift_matcher_matches(matcher, &match); // This should hit the backtracking limit

    // Check if depth was tracked
    ASSERT(rift_matcher_get_backtrack_depth(matcher) > 0, "Backtracking should have occurred");

    rift_matcher_free(matcher);
}

// Test position getting and setting
TEST(matcher_position)
{
    rift_regex_error_t error;
    rift_regex_matcher_t *matcher =
        rift_matcher_create_from_string("abc", RIFT_REGEX_DEFAULT, RIFT_MATCHER_DEFAULT, &error);
    ASSERT(matcher != NULL, "Failed to create matcher");

    ASSERT(rift_matcher_set_input(matcher, "xyzabcdef", 9), "Failed to set input");
    ASSERT(rift_matcher_get_position(matcher) == 0, "Initial position should be 0");

    // Set position manually
    ASSERT(rift_matcher_set_position(matcher, 3), "Failed to set position");
    ASSERT(rift_matcher_get_position(matcher) == 3, "Position should be 3");

    // Match from current position
    rift_regex_match_t match;
    ASSERT(rift_matcher_matches(matcher, &match), "Pattern should match at position 3");

    rift_matcher_free(matcher);
}

// Main test runner
int
main(void)
{
    printf("Starting LibRift regex matcher tests\n");

    RUN_TEST(matcher_create_free);
    RUN_TEST(matcher_basic_matching);
    RUN_TEST(matcher_groups);
    RUN_TEST(matcher_find_all);
    RUN_TEST(matcher_replace);
    RUN_TEST(matcher_split);
    RUN_TEST(matcher_timeout);
    RUN_TEST(matcher_backtrack_depth);
    RUN_TEST(matcher_position);

    printf("\nTest summary: %d tests run, %d failed\n", tests_run, tests_failed);

    return tests_failed ? 1 : 0;
}
