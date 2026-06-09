/**
 * @file state_tests.c
 * @brief Test suite for the state module of LibRift using CTest
 *
 * This file implements comprehensive tests for the state functions
 * in the LibRift regex engine, converted from Check framework to work with CTest.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/automaton/state.h"
#include "librift/automaton/transition.h"

// Test utilities
#define TEST_CASE(name) void test_##name(void)
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
#define ASSERT_NOT_EQUAL_PTR(a, b) ASSERT((a) != (b))
#define ASSERT_EQUAL_STRING(a, b) ASSERT(strcmp((a), (b)) == 0)

// Setup/Teardown
static void
setup()
{
    // Reset state ID counter for consistent testing
    rift_state_reset_id_counter();
}

// Test cases converted from test_state.c

/**
 * Test state creation and basic properties
 */
TEST_CASE(state_create)
{
    // Create non-accepting state
    rift_regex_state_t *non_accepting = rift_state_create(false);
    ASSERT_NOT_NULL(non_accepting);
    ASSERT_FALSE(rift_state_is_accepting(non_accepting));
    ASSERT_EQUAL_INT(rift_state_get_id(non_accepting), 1); // First state has ID 1
    ASSERT_NULL(rift_state_get_pattern(non_accepting));
    ASSERT_EQUAL_INT(rift_state_get_transition_count(non_accepting), 0);

    // Create accepting state
    rift_regex_state_t *accepting = rift_state_create(true);
    ASSERT_NOT_NULL(accepting);
    ASSERT_TRUE(rift_state_is_accepting(accepting));
    ASSERT_EQUAL_INT(rift_state_get_id(accepting), 2); // Second state has ID 2

    // Clean up
    rift_state_free(non_accepting);
    rift_state_free(accepting);
}

/**
 * Test state pattern management
 */
TEST_CASE(state_pattern)
{
    rift_regex_state_t *state = rift_state_create(false);

    // Initially null pattern
    ASSERT_NULL(rift_state_get_pattern(state));

    // Set a pattern
    const char *pattern = "[a-z]+";
    ASSERT_TRUE(rift_state_set_pattern(state, pattern));
    ASSERT_NOT_NULL(rift_state_get_pattern(state));
    ASSERT_EQUAL_STRING(rift_state_get_pattern(state), pattern);

    // Change the pattern
    const char *new_pattern = "[0-9]*";
    ASSERT_TRUE(rift_state_set_pattern(state, new_pattern));
    ASSERT_NOT_NULL(rift_state_get_pattern(state));
    ASSERT_EQUAL_STRING(rift_state_get_pattern(state), new_pattern);

    // Clear the pattern
    ASSERT_TRUE(rift_state_set_pattern(state, NULL));
    ASSERT_NULL(rift_state_get_pattern(state));

    // Clean up
    rift_state_free(state);
}

/**
 * Test state accepting status management
 */
TEST_CASE(state_accepting_status)
{
    // Create non-accepting state and test modification
    rift_regex_state_t *state = rift_state_create(false);
    ASSERT_FALSE(rift_state_is_accepting(state));

    // Change to accepting
    ASSERT_TRUE(rift_state_set_accepting(state, true));
    ASSERT_TRUE(rift_state_is_accepting(state));

    // Change back to non-accepting
    ASSERT_TRUE(rift_state_set_accepting(state, false));
    ASSERT_FALSE(rift_state_is_accepting(state));

    // Clean up
    rift_state_free(state);
}

/**
 * Test state ID management
 */
TEST_CASE(state_id_management)
{
    // Reset ID counter at beginning of test
    rift_state_reset_id_counter();

    // Create multiple states and check sequential IDs
    rift_regex_state_t *state1 = rift_state_create(false);
    rift_regex_state_t *state2 = rift_state_create(false);
    rift_regex_state_t *state3 = rift_state_create(false);

    ASSERT_EQUAL_INT(rift_state_get_id(state1), 1);
    ASSERT_EQUAL_INT(rift_state_get_id(state2), 2);
    ASSERT_EQUAL_INT(rift_state_get_id(state3), 3);

    // Set a specific ID (used for serialization)
    ASSERT_TRUE(rift_state_set_id(state2, 100));
    ASSERT_EQUAL_INT(rift_state_get_id(state2), 100);

    // Create another state and check that the ID counter has been updated
    rift_regex_state_t *state4 = rift_state_create(false);
    ASSERT_EQUAL_INT(rift_state_get_id(state4), 101);

    // Clean up
    rift_state_free(state1);
    rift_state_free(state2);
    rift_state_free(state3);
    rift_state_free(state4);
}

/**
 * Test state user data management
 */
TEST_CASE(state_user_data)
{
    rift_regex_state_t *state = rift_state_create(false);

    // Initially null user data
    ASSERT_NULL(rift_state_get_user_data(state));

    // Set user data
    int test_data = 42;
    ASSERT_TRUE(rift_state_set_user_data(state, &test_data));
    ASSERT_EQUAL_PTR(rift_state_get_user_data(state), &test_data);

    // Change user data
    char *str_data = "test string";
    ASSERT_TRUE(rift_state_set_user_data(state, str_data));
    ASSERT_EQUAL_PTR(rift_state_get_user_data(state), str_data);

    // Clear user data
    ASSERT_TRUE(rift_state_set_user_data(state, NULL));
    ASSERT_NULL(rift_state_get_user_data(state));

    // Clean up
    rift_state_free(state);
}

/**
 * Test state transition addition
 */
TEST_CASE(state_transition_addition)
{
    rift_regex_state_t *from_state = rift_state_create(false);
    rift_regex_state_t *to_state = rift_state_create(true);

    // Initially no transitions
    ASSERT_EQUAL_INT(rift_state_get_transition_count(from_state), 0);

    // Add regular transition
    ASSERT_TRUE(rift_state_add_transition(from_state, to_state, "a"));
    ASSERT_EQUAL_INT(rift_state_get_transition_count(from_state), 1);

    // Add another transition
    ASSERT_TRUE(rift_state_add_transition(from_state, to_state, "[0-9]"));
    ASSERT_EQUAL_INT(rift_state_get_transition_count(from_state), 2);

    // Add epsilon transition
    ASSERT_TRUE(rift_state_add_epsilon_transition(from_state, to_state));
    ASSERT_EQUAL_INT(rift_state_get_transition_count(from_state), 3);

    // Check transition properties
    rift_regex_transition_t *t1 = rift_state_get_transition(from_state, 0);
    rift_regex_transition_t *t2 = rift_state_get_transition(from_state, 1);
    rift_regex_transition_t *t3 = rift_state_get_transition(from_state, 2);

    ASSERT_NOT_NULL(t1);
    ASSERT_NOT_NULL(t2);
    ASSERT_NOT_NULL(t3);

    ASSERT_EQUAL_STRING(rift_transition_get_pattern(t1), "a");
    ASSERT_EQUAL_STRING(rift_transition_get_pattern(t2), "[0-9]");
    ASSERT_NULL(rift_transition_get_pattern(t3)); // Epsilon transition

    ASSERT_EQUAL_PTR(rift_transition_get_target(t1), to_state);
    ASSERT_EQUAL_PTR(rift_transition_get_target(t2), to_state);
    ASSERT_EQUAL_PTR(rift_transition_get_target(t3), to_state);

    // Clean up
    rift_state_free(from_state);
    rift_state_free(to_state);
}

/**
 * Test state transition removal
 */
TEST_CASE(state_transition_removal)
{
    rift_regex_state_t *from_state = rift_state_create(false);
    rift_regex_state_t *to_state = rift_state_create(true);

    // Add multiple transitions
    rift_state_add_transition(from_state, to_state, "a");
    rift_state_add_transition(from_state, to_state, "b");
    rift_state_add_transition(from_state, to_state, "c");
    ASSERT_EQUAL_INT(rift_state_get_transition_count(from_state), 3);

    // Remove middle transition
    ASSERT_TRUE(rift_state_remove_transition(from_state, 1));
    ASSERT_EQUAL_INT(rift_state_get_transition_count(from_state), 2);

    // Check remaining transitions
    rift_regex_transition_t *t1 = rift_state_get_transition(from_state, 0);
    rift_regex_transition_t *t2 = rift_state_get_transition(from_state, 1);

    ASSERT_NOT_NULL(t1);
    ASSERT_NOT_NULL(t2);
    ASSERT_EQUAL_STRING(rift_transition_get_pattern(t1), "a");
    ASSERT_EQUAL_STRING(rift_transition_get_pattern(t2), "c"); // 'b' was removed

    // Remove first transition
    ASSERT_TRUE(rift_state_remove_transition(from_state, 0));
    ASSERT_EQUAL_INT(rift_state_get_transition_count(from_state), 1);

    // Check remaining transition
    rift_regex_transition_t *t3 = rift_state_get_transition(from_state, 0);
    ASSERT_NOT_NULL(t3);
    ASSERT_EQUAL_STRING(rift_transition_get_pattern(t3), "c");

    // Remove last transition
    ASSERT_TRUE(rift_state_remove_transition(from_state, 0));
    ASSERT_EQUAL_INT(rift_state_get_transition_count(from_state), 0);

    // Attempt to remove from empty state
    ASSERT_FALSE(rift_state_remove_transition(from_state, 0));

    // Clean up
    rift_state_free(from_state);
    rift_state_free(to_state);
}

/**
 * Test state cloning
 */
TEST_CASE(state_clone)
{
    // Create and configure original state
    rift_regex_state_t *original = rift_state_create(true);
    rift_state_set_pattern(original, "pattern");

    int user_data = 42;
    rift_state_set_user_data(original, &user_data);

    // Clone the state
    rift_regex_state_t *clone = rift_state_clone(original);
    ASSERT_NOT_NULL(clone);

    // Check that clone has same properties but is a different object
    ASSERT_NOT_EQUAL_PTR(clone, original);
    ASSERT_EQUAL_INT(rift_state_is_accepting(clone), rift_state_is_accepting(original));
    ASSERT_EQUAL_STRING(rift_state_get_pattern(clone), rift_state_get_pattern(original));

    // Note: User data is typically not cloned

    // Clean up
    rift_state_free(original);
    rift_state_free(clone);
}

/**
 * Test state equivalence comparison
 */
TEST_CASE(state_equivalence)
{
    // Create two identical states
    rift_regex_state_t *state1 = rift_state_create(true);
    rift_state_set_pattern(state1, "pattern");
    rift_state_set_group_name(state1, "group1");
    rift_state_set_group_start(state1, true);

    rift_regex_state_t *state2 = rift_state_create(true);
    rift_state_set_pattern(state2, "pattern");
    rift_state_set_group_name(state2, "group1");
    rift_state_set_group_start(state2, true);

    // States should be equivalent
    ASSERT_TRUE(rift_state_are_equivalent(state1, state2));

    // Create a different state
    rift_regex_state_t *state3 = rift_state_create(false);
    rift_state_set_pattern(state3, "pattern");
    rift_state_set_group_name(state3, "group1");
    rift_state_set_group_start(state3, true);

    // States should not be equivalent due to accepting status
    ASSERT_FALSE(rift_state_are_equivalent(state1, state3));

    // Another different state
    rift_regex_state_t *state4 = rift_state_create(true);
    rift_state_set_pattern(state4, "different");
    rift_state_set_group_name(state4, "group1");
    rift_state_set_group_start(state4, true);

    // States should not be equivalent due to pattern
    ASSERT_FALSE(rift_state_are_equivalent(state1, state4));

    // Clean up
    rift_state_free(state1);
    rift_state_free(state2);
    rift_state_free(state3);
    rift_state_free(state4);
}

/**
 * Test state flag operations
 */
TEST_CASE(state_flags)
{
    rift_regex_state_t *state = rift_state_create(false);

    // Initially no flags
    ASSERT_FALSE(rift_state_has_flag(state, RIFT_STATE_FLAG_ANCHOR_START));
    ASSERT_FALSE(rift_state_has_flag(state, RIFT_STATE_FLAG_ANCHOR_END));

    // Set one flag
    ASSERT_TRUE(rift_state_set_flag(state, RIFT_STATE_FLAG_ANCHOR_START));
    ASSERT_TRUE(rift_state_has_flag(state, RIFT_STATE_FLAG_ANCHOR_START));
    ASSERT_FALSE(rift_state_has_flag(state, RIFT_STATE_FLAG_ANCHOR_END));

    // Set another flag
    ASSERT_TRUE(rift_state_set_flag(state, RIFT_STATE_FLAG_ANCHOR_END));
    ASSERT_TRUE(rift_state_has_flag(state, RIFT_STATE_FLAG_ANCHOR_START));
    ASSERT_TRUE(rift_state_has_flag(state, RIFT_STATE_FLAG_ANCHOR_END));

    // Clean up
    rift_state_free(state);
}

/**
 * Test state group management
 */
TEST_CASE(state_group_management)
{
    rift_regex_state_t *state = rift_state_create(false);

    // Initially not related to groups
    ASSERT_NULL(rift_state_get_group_name(state));
    ASSERT_FALSE(rift_state_is_group_start(state));
    ASSERT_FALSE(rift_state_is_group_end(state));

    // Set group name
    ASSERT_TRUE(rift_state_set_group_name(state, "capture1"));
    ASSERT_NOT_NULL(rift_state_get_group_name(state));
    ASSERT_EQUAL_STRING(rift_state_get_group_name(state), "capture1");

    // Set as group start
    ASSERT_TRUE(rift_state_set_group_start(state, true));
    ASSERT_TRUE(rift_state_is_group_start(state));

    // Set as group end
    ASSERT_TRUE(rift_state_set_group_end(state, true));
    ASSERT_TRUE(rift_state_is_group_end(state));

    // Change group status
    ASSERT_TRUE(rift_state_set_group_start(state, false));
    ASSERT_FALSE(rift_state_is_group_start(state));
    ASSERT_TRUE(rift_state_is_group_end(state));

    // Clear group name
    ASSERT_TRUE(rift_state_set_group_name(state, NULL));
    ASSERT_NULL(rift_state_get_group_name(state));

    // Clean up
    rift_state_free(state);
}

/**
 * Test state info creation and management
 */
TEST_CASE(state_info)
{
    // Create state info
    rift_state_info_t *info = rift_state_info_create(1, 2, "a.*b");
    ASSERT_NOT_NULL(info);

    // Check properties
    ASSERT_EQUAL_INT(info->entry_state_id, 1);
    ASSERT_EQUAL_INT(info->exit_state_id, 2);
    ASSERT_NOT_NULL(info->transition_condition);
    ASSERT_EQUAL_STRING(info->transition_condition, "a.*b");
    ASSERT_FALSE(info->is_accepting);
    ASSERT_NULL(info->automaton_state_ref);

    // Free state info
    rift_state_info_free(info);

    // Create state info without condition
    rift_state_info_t *info2 = rift_state_info_create(3, 4, NULL);
    ASSERT_NOT_NULL(info2);
    ASSERT_EQUAL_INT(info2->entry_state_id, 3);
    ASSERT_EQUAL_INT(info2->exit_state_id, 4);
    ASSERT_NULL(info2->transition_condition);

    // Free state info
    rift_state_info_free(info2);
}

/**
 * Test next state ID retrieval
 */
TEST_CASE(next_state_id)
{
    // Reset state ID counter
    rift_state_reset_id_counter();

    // First state ID should be 1
    ASSERT_EQUAL_INT(rift_state_get_next_id(), 1);

    // Create a state which increments the counter
    rift_regex_state_t *state = rift_state_create(false);
    ASSERT_EQUAL_INT(rift_state_get_id(state), 1);

    // Next state ID should now be 2
    ASSERT_EQUAL_INT(rift_state_get_next_id(), 2);

    // Create another state
    rift_regex_state_t *state2 = rift_state_create(false);
    ASSERT_EQUAL_INT(rift_state_get_id(state2), 2);

    // Next state ID should now be 3
    ASSERT_EQUAL_INT(rift_state_get_next_id(), 3);

    // Clean up
    rift_state_free(state);
    rift_state_free(state2);
}

/**
 * Main function to run all tests
 */
int
main(void)
{
    printf("Running LibRift state tests...\n\n");

    int test_count = 0;

    // Set up the environment for the tests
    setup();

    // Run all tests
    RUN_TEST(state_create);
    RUN_TEST(state_pattern);
    RUN_TEST(state_accepting_status);
    RUN_TEST(state_id_management);
    RUN_TEST(state_user_data);
    RUN_TEST(state_transition_addition);
    RUN_TEST(state_transition_removal);
    RUN_TEST(state_clone);
    RUN_TEST(state_equivalence);
    RUN_TEST(state_flags);
    RUN_TEST(state_group_management);
    RUN_TEST(state_info);
    RUN_TEST(next_state_id);

    printf("All tests passed! Total tests: %d\n", test_count);
    return EXIT_SUCCESS;
}