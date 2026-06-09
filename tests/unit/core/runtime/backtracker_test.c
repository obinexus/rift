/**
 * @file backtracker_test.c
 * @brief Unit tests for the backtracking component of the LibRift regex engine
 *
 * This file contains test cases for the backtracker functionality, verifying
 * that the backtracker correctly handles state management, depth tracking,
 * and memory operations.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/runtime/backtracker.h"

/* Mock regex state structure for testing */
typedef struct {
    int id;
} mock_regex_state_t;

/* Test backtracker creation and initialization */
void
test_backtracker_create(void)
{
    size_t max_depth = 100;
    size_t num_groups = 5;

    rift_regex_backtracker_t *backtracker = rift_backtracker_create(max_depth, num_groups);
    assert(backtracker != NULL);
    assert(backtracker->stack_top == NULL);
    assert(backtracker->current_depth == 0);
    assert(backtracker->max_depth == max_depth);
    assert(backtracker->num_groups == num_groups);

    rift_backtracker_free(backtracker);
    printf("test_backtracker_create: PASSED\n");
}

/* Test backtracker push and pop operations */
void
test_backtracker_push_pop(void)
{
    rift_regex_backtracker_t *backtracker = rift_backtracker_create(10, 2);
    assert(backtracker != NULL);

    mock_regex_state_t state1 = {.id = 1};
    mock_regex_state_t state2 = {.id = 2};

    // Test initial empty state
    assert(rift_backtracker_is_empty(backtracker));
    assert(rift_backtracker_get_depth(backtracker) == 0);

    // Push first state
    assert(rift_backtracker_push(backtracker, (rift_regex_state_t *)&state1, 2));
    assert(!rift_backtracker_is_empty(backtracker));
    assert(rift_backtracker_get_depth(backtracker) == 1);

    // Push second state
    assert(rift_backtracker_push(backtracker, (rift_regex_state_t *)&state2, 2));
    assert(rift_backtracker_get_depth(backtracker) == 2);

    // Pop and verify states
    rift_regex_state_t *popped_state;
    size_t num_groups;

    assert(rift_backtracker_pop(backtracker, &popped_state, &num_groups));
    assert(((mock_regex_state_t *)popped_state)->id == 2);
    assert(num_groups == 2);
    assert(rift_backtracker_get_depth(backtracker) == 1);

    assert(rift_backtracker_pop(backtracker, &popped_state, &num_groups));
    assert(((mock_regex_state_t *)popped_state)->id == 1);
    assert(num_groups == 2);
    assert(rift_backtracker_get_depth(backtracker) == 0);

    // Should be empty now
    assert(rift_backtracker_is_empty(backtracker));

    rift_backtracker_free(backtracker);
    printf("test_backtracker_push_pop: PASSED\n");
}

/* Test backtracker max depth enforcement */
void
test_backtracker_max_depth(void)
{
    // Create backtracker with small max depth
    rift_regex_backtracker_t *backtracker = rift_backtracker_create(2, 1);
    assert(backtracker != NULL);

    mock_regex_state_t state1 = {.id = 1};
    mock_regex_state_t state2 = {.id = 2};
    mock_regex_state_t state3 = {.id = 3};

    // Push up to max depth
    assert(rift_backtracker_push(backtracker, (rift_regex_state_t *)&state1, 1));
    assert(rift_backtracker_push(backtracker, (rift_regex_state_t *)&state2, 1));

    // Try to push beyond max depth, should fail
    assert(!rift_backtracker_push(backtracker, (rift_regex_state_t *)&state3, 1));
    assert(rift_backtracker_get_depth(backtracker) == 2);

    // Update max depth and try again
    assert(rift_backtracker_set_max_depth(backtracker, 3));
    assert(rift_backtracker_push(backtracker, (rift_regex_state_t *)&state3, 1));
    assert(rift_backtracker_get_depth(backtracker) == 3);

    rift_backtracker_free(backtracker);
    printf("test_backtracker_max_depth: PASSED\n");
}

/* Test backtracker peek functionality */
void
test_backtracker_peek(void)
{
    rift_regex_backtracker_t *backtracker = rift_backtracker_create(5, 1);
    assert(backtracker != NULL);

    mock_regex_state_t state1 = {.id = 123};

    // Peek on empty backtracker should fail
    rift_regex_state_t *peeked_state;
    size_t num_groups;
    assert(!rift_backtracker_peek(backtracker, &peeked_state, &num_groups));

    // Push a state and peek
    assert(rift_backtracker_push(backtracker, (rift_regex_state_t *)&state1, 1));
    assert(rift_backtracker_peek(backtracker, &peeked_state, &num_groups));
    assert(((mock_regex_state_t *)peeked_state)->id == 123);
    assert(num_groups == 1);

    // Ensure stack is unchanged after peek
    assert(rift_backtracker_get_depth(backtracker) == 1);

    rift_backtracker_free(backtracker);
    printf("test_backtracker_peek: PASSED\n");
}

/* Test backtracker reset functionality */
void
test_backtracker_reset(void)
{
    rift_regex_backtracker_t *backtracker = rift_backtracker_create(5, 1);
    assert(backtracker != NULL);

    mock_regex_state_t state = {.id = 1};

    // Push some states
    assert(rift_backtracker_push(backtracker, (rift_regex_state_t *)&state, 1));
    assert(rift_backtracker_push(backtracker, (rift_regex_state_t *)&state, 1));
    assert(rift_backtracker_get_depth(backtracker) == 2);

    // Reset and verify
    rift_backtracker_reset(backtracker);
    assert(rift_backtracker_is_empty(backtracker));
    assert(rift_backtracker_get_depth(backtracker) == 0);

    rift_backtracker_free(backtracker);
    printf("test_backtracker_reset: PASSED\n");
}

/* Test backtracker clone functionality */
void
test_backtracker_clone(void)
{
    rift_regex_backtracker_t *backtracker = rift_backtracker_create(5, 1);
    assert(backtracker != NULL);

    mock_regex_state_t state1 = {.id = 1};
    mock_regex_state_t state2 = {.id = 2};

    // Push some states
    assert(rift_backtracker_push(backtracker, (rift_regex_state_t *)&state1, 1));
    assert(rift_backtracker_push(backtracker, (rift_regex_state_t *)&state2, 1));

    // Clone the backtracker
    rift_regex_backtracker_t *clone = rift_backtracker_clone(backtracker);
    assert(clone != NULL);
    assert(clone->max_depth == backtracker->max_depth);
    assert(clone->num_groups == backtracker->num_groups);
    assert(clone->current_depth == backtracker->current_depth);

    // Verify clone works independently
    rift_regex_state_t *popped_state;
    size_t num_groups;

    assert(rift_backtracker_pop(clone, &popped_state, &num_groups));
    assert(((mock_regex_state_t *)popped_state)->id == 2);
    assert(rift_backtracker_get_depth(clone) == 1);

    // Original should be unaffected
    assert(rift_backtracker_get_depth(backtracker) == 2);

    rift_backtracker_free(backtracker);
    rift_backtracker_free(clone);
    printf("test_backtracker_clone: PASSED\n");
}

int
main(void)
{
    printf("Running backtracker unit tests...\n");

    test_backtracker_create();
    test_backtracker_push_pop();
    test_backtracker_max_depth();
    test_backtracker_peek();
    test_backtracker_reset();
    test_backtracker_clone();

    printf("All tests passed!\n");
    return 0;
}
