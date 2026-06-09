/**
 * @file transition_tests.c
 * @brief Test suite for the transition module of LibRift using CTest
 *
 * This file implements comprehensive tests for the transition functions
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

// Test state references
static rift_regex_state_t *from_state = NULL;
static rift_regex_state_t *to_state = NULL;

// Setup/Teardown
static void
setup()
{
    // Reset state ID counter for consistent testing
    rift_state_reset_id_counter();

    // Create states for transitions
    from_state = rift_state_create(false);
    to_state = rift_state_create(true);
}

static void
teardown()
{
    // Clean up states
    rift_state_free(from_state);
    rift_state_free(to_state);
    from_state = NULL;
    to_state = NULL;
}

/**
 * Test transition creation and basic properties
 */
TEST_CASE(transition_create)
{
    // Create standard transition
    const char *pattern = "a[bc]*";
    rift_regex_transition_t *transition = rift_transition_create(from_state, to_state, pattern);

    ASSERT_NOT_NULL(transition);
    ASSERT_EQUAL_PTR(rift_transition_get_source(transition), from_state);
    ASSERT_EQUAL_PTR(rift_transition_get_target(transition), to_state);
    ASSERT_NOT_NULL(rift_transition_get_pattern(transition));
    ASSERT_EQUAL_STRING(rift_transition_get_pattern(transition), pattern);
    ASSERT_FALSE(rift_transition_is_epsilon(transition));

    // Free transition
    rift_transition_free(transition);
}

/**
 * Test epsilon transition creation
 */
TEST_CASE(epsilon_transition_create)
{
    // Create epsilon transition
    rift_regex_transition_t *transition = rift_transition_create_epsilon(from_state, to_state);

    ASSERT_NOT_NULL(transition);
    ASSERT_EQUAL_PTR(rift_transition_get_source(transition), from_state);
    ASSERT_EQUAL_PTR(rift_transition_get_target(transition), to_state);
    ASSERT_NULL(rift_transition_get_pattern(transition));
    ASSERT_TRUE(rift_transition_is_epsilon(transition));

    // Free transition
    rift_transition_free(transition);
}

/**
 * Test transition priority management
 */
TEST_CASE(transition_priority)
{
    // Create transition with default priority
    rift_regex_transition_t *transition = rift_transition_create(from_state, to_state, "a");

    // Default priority should be 0
    ASSERT_EQUAL_INT(rift_transition_get_priority(transition), 0);

    // Set priority
    ASSERT_TRUE(rift_transition_set_priority(transition, 10));
    ASSERT_EQUAL_INT(rift_transition_get_priority(transition), 10);

    // Change priority
    ASSERT_TRUE(rift_transition_set_priority(transition, 5));
    ASSERT_EQUAL_INT(rift_transition_get_priority(transition), 5);

    // Free transition
    rift_transition_free(transition);
}

/**
 * Test single character transition matching
 */
TEST_CASE(transition_char_matching)
{
    // Create transitions with different patterns
    rift_regex_transition_t *t_single = rift_transition_create(from_state, to_state, "a");
    rift_regex_transition_t *t_class = rift_transition_create(from_state, to_state, "[0-9]");
    rift_regex_transition_t *t_complex = rift_transition_create(from_state, to_state, "[a-z]{2}");
    rift_regex_transition_t *t_epsilon = rift_transition_create_epsilon(from_state, to_state);

    // Test single character pattern
    ASSERT_TRUE(rift_transition_matches_char(t_single, 'a'));
    ASSERT_FALSE(rift_transition_matches_char(t_single, 'b'));

    // Test character class pattern
    ASSERT_TRUE(rift_transition_matches_char(t_class, '5'));
    ASSERT_FALSE(rift_transition_matches_char(t_class, 'a'));

    // Test complex pattern (this is tricky since a single character can't match a 2-char pattern)
    ASSERT_FALSE(rift_transition_matches_char(t_complex, 'a'));

    // Test epsilon transition (should match anything)
    ASSERT_TRUE(rift_transition_matches_char(t_epsilon, 'a'));
    ASSERT_TRUE(rift_transition_matches_char(t_epsilon, '5'));
    ASSERT_TRUE(rift_transition_matches_char(t_epsilon, '\0'));

    // Free transitions
    rift_transition_free(t_single);
    rift_transition_free(t_class);
    rift_transition_free(t_complex);
    rift_transition_free(t_epsilon);
}

/**
 * Test string transition matching
 */
TEST_CASE(transition_string_matching)
{
    // Create transitions with different patterns
    rift_regex_transition_t *t_single = rift_transition_create(from_state, to_state, "a");
    rift_regex_transition_t *t_class = rift_transition_create(from_state, to_state, "[0-9]");
    rift_regex_transition_t *t_complex = rift_transition_create(from_state, to_state, "[a-z]{2}");
    rift_regex_transition_t *t_epsilon = rift_transition_create_epsilon(from_state, to_state);

    // Test single character pattern
    ASSERT_TRUE(rift_transition_matches(t_single, "a"));
    ASSERT_TRUE(rift_transition_matches(t_single, "ab")); // Matches the start
    ASSERT_FALSE(rift_transition_matches(t_single, "b"));

    // Test character class pattern
    ASSERT_TRUE(rift_transition_matches(t_class, "5"));
    ASSERT_TRUE(rift_transition_matches(t_class, "5a"));  // Matches the start
    ASSERT_FALSE(rift_transition_matches(t_class, "a5")); // Does not match the start

    // Test complex pattern
    ASSERT_TRUE(rift_transition_matches(t_complex, "ab"));
    ASSERT_FALSE(rift_transition_matches(t_complex, "a"));   // Not enough characters
    ASSERT_TRUE(rift_transition_matches(t_complex, "abc"));  // Matches the start
    ASSERT_FALSE(rift_transition_matches(t_complex, "1ab")); // Does not match the start

    // Test epsilon transition (should match anything)
    ASSERT_TRUE(rift_transition_matches(t_epsilon, ""));
    ASSERT_TRUE(rift_transition_matches(t_epsilon, "123abc"));
    ASSERT_TRUE(rift_transition_matches(t_epsilon, "a"));

    // Free transitions
    rift_transition_free(t_single);
    rift_transition_free(t_class);
    rift_transition_free(t_complex);
    rift_transition_free(t_epsilon);
}

/**
 * Test transition clone functionality
 */
TEST_CASE(transition_clone)
{
    // Create original transition
    const char *pattern = "a[bc]*";
    rift_regex_transition_t *original = rift_transition_create(from_state, to_state, pattern);
    rift_transition_set_priority(original, 10);

    // Clone the transition
    rift_regex_transition_t *clone = rift_transition_clone(original);
    ASSERT_NOT_NULL(clone);

    // Check that clone has same properties
    ASSERT_EQUAL_PTR(rift_transition_get_source(clone), from_state);
    ASSERT_EQUAL_PTR(rift_transition_get_target(clone), to_state);
    ASSERT_EQUAL_STRING(rift_transition_get_pattern(clone), pattern);
    ASSERT_EQUAL_INT(rift_transition_get_priority(clone), 10);
    ASSERT_FALSE(rift_transition_is_epsilon(clone));

    // Free transitions
    rift_transition_free(original);
    rift_transition_free(clone);
}

/**
 * Test transition equality
 */
TEST_CASE(transitions_are_equal)
{
    // Create identical transitions
    rift_regex_transition_t *t1 = rift_transition_create(from_state, to_state, "a");
    rift_regex_transition_t *t2 = rift_transition_create(from_state, to_state, "a");

    // These should be equal
    ASSERT_TRUE(rift_transitions_are_equal(t1, t2));

    // Create different transitions
    rift_regex_transition_t *t3 = rift_transition_create(from_state, to_state, "b");

    // These should not be equal
    ASSERT_FALSE(rift_transitions_are_equal(t1, t3));

    // Create epsilon transitions
    rift_regex_transition_t *e1 = rift_transition_create_epsilon(from_state, to_state);
    rift_regex_transition_t *e2 = rift_transition_create_epsilon(from_state, to_state);

    // Epsilon transitions should be equal
    ASSERT_TRUE(rift_transitions_are_equal(e1, e2));

    // Epsilon and non-epsilon should not be equal
    ASSERT_FALSE(rift_transitions_are_equal(t1, e1));

    // Free transitions
    rift_transition_free(t1);
    rift_transition_free(t2);
    rift_transition_free(t3);
    rift_transition_free(e1);
    rift_transition_free(e2);
}

/**
 * Test transition priority sorting
 */
TEST_CASE(sort_transitions_by_priority)
{
    // Create several transitions with different priorities
    rift_regex_transition_t *t1 = rift_transition_create(from_state, to_state, "a");
    rift_regex_transition_t *t2 = rift_transition_create(from_state, to_state, "b");
    rift_regex_transition_t *t3 = rift_transition_create(from_state, to_state, "c");

    // Set different priorities
    rift_transition_set_priority(t1, 5);
    rift_transition_set_priority(t2, 10);
    rift_transition_set_priority(t3, 1);

    // Create an array of transitions
    rift_regex_transition_t *transitions[3] = {t1, t2, t3};

    // Sort them by priority
    rift_sort_transitions_by_priority(transitions, 3);

    // Check the sort order (highest priority first)
    ASSERT_EQUAL_PTR(transitions[0], t2); // Priority 10
    ASSERT_EQUAL_PTR(transitions[1], t1); // Priority 5
    ASSERT_EQUAL_PTR(transitions[2], t3); // Priority 1

    // Free transitions
    rift_transition_free(t1);
    rift_transition_free(t2);
    rift_transition_free(t3);
}

/**
 * Main function to run all tests
 */
int
main(void)
{
    printf("Running LibRift transition tests...\n\n");

    int test_count = 0;

    // Set up the environment for the tests
    setup();

    // Run all tests
    RUN_TEST(transition_create);
    RUN_TEST(epsilon_transition_create);
    RUN_TEST(transition_priority);
    RUN_TEST(transition_char_matching);
    RUN_TEST(transition_string_matching);
    RUN_TEST(transition_clone);
    RUN_TEST(transitions_are_equal);
    RUN_TEST(sort_transitions_by_priority);

    // Clean up
    teardown();

    printf("All tests passed! Total tests: %d\n", test_count);
    return EXIT_SUCCESS;
}