/**
 * @file context_test.c
 * @brief Unit tests for the LibRift regex context functions
 *
 * This file contains unit tests for the context-related functionality
 * in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdlib.h>
#include <string.h>

#include "ctest.h"
#include "librift/core/regex/pattern.h"
#include "librift/runtime/context.h"

CTEST_DATA(context)
{
    const char *input_string;
    size_t input_length;
    rift_regex_matcher_context_t *context;
};

CTEST_SETUP(context)
{
    data->input_string = "test input string";
    data->input_length = strlen(data->input_string);
    data->context = rift_matcher_context_create(data->input_string, data->input_length, 5);
}

CTEST_TEARDOWN(context)
{
    if (data->context) {
        rift_matcher_context_free(data->context);
        data->context = NULL;
    }
}

// Test context creation
CTEST2(context, test_context_create)
{
    ASSERT_NOT_NULL(data->context);
    ASSERT_STR_EQ(data->input_string, rift_matcher_context_get_input(data->context));
    ASSERT_EQUAL(data->input_length, rift_matcher_context_get_input_length(data->context));
    ASSERT_EQUAL(0, rift_matcher_context_get_position(data->context));
    ASSERT_NOT_NULL(rift_matcher_context_get_capture_groups(data->context));
}

// Test creation with NULL input
CTEST(context, test_create_with_null_input)
{
    rift_regex_matcher_context_t *context = rift_matcher_context_create(NULL, 0, 1);
    ASSERT_NOT_NULL(context);
    ASSERT_NULL(rift_matcher_context_get_input(context));
    ASSERT_EQUAL(0, rift_matcher_context_get_input_length(context));
    rift_matcher_context_free(context);
}

// Test creation with empty input
CTEST(context, test_create_with_empty_input)
{
    const char *empty = "";
    rift_regex_matcher_context_t *context = rift_matcher_context_create(empty, 0, 1);
    ASSERT_NOT_NULL(context);
    ASSERT_STR_EQ("", rift_matcher_context_get_input(context));
    ASSERT_EQUAL(0, rift_matcher_context_get_input_length(context));
    rift_matcher_context_free(context);
}

// Test position manipulation
CTEST2(context, test_position_manipulation)
{
    // Test initial position
    ASSERT_EQUAL(0, rift_matcher_context_get_position(data->context));

    // Test setting position
    ASSERT_TRUE(rift_matcher_context_set_position(data->context, 5));
    ASSERT_EQUAL(5, rift_matcher_context_get_position(data->context));

    // Test advancing position
    ASSERT_TRUE(rift_matcher_context_advance(data->context));
    ASSERT_EQUAL(6, rift_matcher_context_get_position(data->context));

    // Test setting to end
    ASSERT_TRUE(rift_matcher_context_set_position(data->context, data->input_length - 1));
    ASSERT_EQUAL(data->input_length - 1, rift_matcher_context_get_position(data->context));

    // Test advancing to the end
    ASSERT_TRUE(rift_matcher_context_advance(data->context));
    ASSERT_TRUE(rift_matcher_context_is_at_end(data->context));

    // Test advancing beyond end (should fail)
    ASSERT_FALSE(rift_matcher_context_advance(data->context));

    // Test setting beyond end (should fail)
    ASSERT_FALSE(rift_matcher_context_set_position(data->context, data->input_length + 5));
}

// Test get current character
CTEST2(context, test_get_current_char)
{
    ASSERT_EQUAL('t', rift_matcher_context_get_current_char(data->context));

    rift_matcher_context_set_position(data->context, 1);
    ASSERT_EQUAL('e', rift_matcher_context_get_current_char(data->context));

    rift_matcher_context_set_position(data->context, data->input_length - 1);
    ASSERT_EQUAL('g', rift_matcher_context_get_current_char(data->context));
}

// Test setting new input
CTEST2(context, test_set_input)
{
    const char *new_input = "new input";
    size_t new_length = strlen(new_input);

    ASSERT_TRUE(rift_matcher_context_set_input(data->context, new_input, new_length));
    ASSERT_STR_EQ(new_input, rift_matcher_context_get_input(data->context));
    ASSERT_EQUAL(new_length, rift_matcher_context_get_input_length(data->context));
    ASSERT_EQUAL(0, rift_matcher_context_get_position(data->context));
}

// Test match result creation
CTEST2(context, test_match_result_creation)
{
    size_t start_pos = 0;
    size_t end_pos = 4; // Capturing "test"

    rift_regex_match_result_t *result =
        rift_matcher_context_create_match_result(data->context, start_pos, end_pos);

    ASSERT_NOT_NULL(result);
    rift_match_result_free(result);
}

// Test regex context creation and freeing
CTEST2(context, test_regex_context_creation)
{
    rift_regex_backtracker_t *backtracker = NULL; // Mock backtracker
    rift_regex_context_t *regex_context = rift_regex_context_create(data->context, backtracker);

    ASSERT_NOT_NULL(regex_context);
    rift_regex_context_free(regex_context, false);
}

// Test with very long input
CTEST(context, test_long_input)
{
    size_t length = 1000;
    char *long_input = (char *)malloc(length + 1);
    if (!long_input) {
        ASSERT_FAIL("Failed to allocate memory for test");
        return;
    }

    memset(long_input, 'a', length);
    long_input[length] = '\0';

    rift_regex_matcher_context_t *context = rift_matcher_context_create(long_input, length, 5);
    ASSERT_NOT_NULL(context);
    ASSERT_EQUAL(length, rift_matcher_context_get_input_length(context));

    // Test moving to the middle
    ASSERT_TRUE(rift_matcher_context_set_position(context, length / 2));
    ASSERT_EQUAL(length / 2, rift_matcher_context_get_position(context));

    rift_matcher_context_free(context);
    free(long_input);
}

// Main function to run the tests
int
main(int argc, const char *argv[])
{
    return ctest_main(argc, argv);
}
