/**
 * @file safe_backtracker_test.c
 * @brief Unit tests for thread-safe backtracker implementation
 *
 * This file contains tests for the thread-safe backtracker functionality
 * in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/errors/regex_error.h"
#include "librift/runtime/backtracker/safe_backtracker.h"

/* Mock state for testing */
typedef struct mock_state {
    int id;
} mock_state_t;

/* Test context */
typedef struct test_context {
    rift_regex_safe_backtracker_t *backtracker;
    rift_regex_error_t error;
    mock_state_t states[10];
} test_context_t;

/* Thread test context */
typedef struct thread_test_context {
    rift_regex_safe_backtracker_t *backtracker;
    int thread_id;
    int operations;
    bool success;
} thread_test_context_t;

/* Setup and teardown */
static test_context_t *
setup_test(void)
{
    test_context_t *ctx = (test_context_t *)malloc(sizeof(test_context_t));
    if (!ctx)
        return NULL;

    memset(ctx, 0, sizeof(test_context_t));

    /* Initialize mock states */
    for (int i = 0; i < 10; i++) {
        ctx->states[i].id = i + 1;
    }

    /* Create a backtracker with reasonable defaults */
    ctx->backtracker = rift_safe_backtracker_create(100, 5);

    return ctx;
}

static void
teardown_test(test_context_t *ctx)
{
    if (!ctx)
        return;

    if (ctx->backtracker) {
        rift_safe_backtracker_free(ctx->backtracker);
    }

    free(ctx);
}

/* Test: creation and initialization */
static bool
test_create_and_init(void)
{
    printf("Running test: create_and_init\n");

    /* Test with valid parameters */
    rift_regex_safe_backtracker_t *bt = rift_safe_backtracker_create(100, 5);
    if (!bt) {
        printf("FAIL: Could not create backtracker with valid params\n");
        return false;
    }

    if (rift_safe_backtracker_get_max_depth(bt) != 100) {
        printf("FAIL: Max depth not set correctly\n");
        rift_safe_backtracker_free(bt);
        return false;
    }

    rift_safe_backtracker_free(bt);

    /* Test with edge case parameters */
    bt = rift_safe_backtracker_create(1, 0);
    if (!bt) {
        printf("FAIL: Could not create backtracker with min params\n");
        return false;
    }

    rift_safe_backtracker_free(bt);

    printf("PASS: create_and_init\n");
    return true;
}

/* Test: get local backtracker */
static bool
test_get_local_backtracker(void)
{
    printf("Running test: get_local_backtracker\n");

    test_context_t *ctx = setup_test();
    if (!ctx)
        return false;

    /* Get local backtracker */
    rift_regex_backtracker_t *local_bt = rift_safe_backtracker_get_local(ctx->backtracker);
    if (!local_bt) {
        printf("FAIL: Could not get local backtracker\n");
        teardown_test(ctx);
        return false;
    }

    /* Verify we can get it again and it's the same instance */
    rift_regex_backtracker_t *second_bt = rift_safe_backtracker_get_local(ctx->backtracker);
    if (local_bt != second_bt) {
        printf("FAIL: Second call to get_local returned different instance\n");
        teardown_test(ctx);
        return false;
    }

    /* Test with NULL parameter */
    rift_regex_backtracker_t *null_bt = rift_safe_backtracker_get_local(NULL);
    if (null_bt != NULL) {
        printf("FAIL: get_local with NULL didn't return NULL\n");
        teardown_test(ctx);
        return false;
    }

    teardown_test(ctx);
    printf("PASS: get_local_backtracker\n");
    return true;
}

/* Test: push and pop operations */
static bool
test_push_pop_operations(void)
{
    printf("Running test: push_pop_operations\n");

    test_context_t *ctx = setup_test();
    if (!ctx)
        return false;

    const size_t num_groups = 3;
    size_t group_starts[3] = {0, 5, 10};
    size_t group_ends[3] = {4, 9, 14};
    size_t input_pos = 15;

    /* Push a state */
    bool result =
        rift_safe_backtracker_push(ctx->backtracker, (rift_regex_state_t *)&ctx->states[0],
                                   input_pos, group_starts, group_ends, num_groups, &ctx->error);

    if (!result) {
        printf("FAIL: Could not push state to backtracker\n");
        teardown_test(ctx);
        return false;
    }

    /* Check depth */
    size_t depth = rift_safe_backtracker_get_depth(ctx->backtracker, &ctx->error);
    if (depth != 1) {
        printf("FAIL: Depth should be 1 after push, got %zu\n", depth);
        teardown_test(ctx);
        return false;
    }

    /* Test empty check */
    if (rift_safe_backtracker_is_empty(ctx->backtracker, &ctx->error)) {
        printf("FAIL: Backtracker should not be empty after push\n");
        teardown_test(ctx);
        return false;
    }

    /* Push a second state */
    group_starts[0] = 1;
    group_ends[0] = 5;
    input_pos = 20;

    result =
        rift_safe_backtracker_push(ctx->backtracker, (rift_regex_state_t *)&ctx->states[1],
                                   input_pos, group_starts, group_ends, num_groups, &ctx->error);

    if (!result) {
        printf("FAIL: Could not push second state to backtracker\n");
        teardown_test(ctx);
        return false;
    }

    /* Pop a state */
    rift_regex_state_t *popped_state = NULL;
    size_t popped_input_pos = 0;
    size_t popped_group_starts[3] = {0};
    size_t popped_group_ends[3] = {0};
    size_t popped_num_groups = 0;

    result = rift_safe_backtracker_pop(ctx->backtracker, &popped_state, &popped_input_pos,
                                       popped_group_starts, popped_group_ends, &popped_num_groups,
                                       &ctx->error);

    if (!result) {
        printf("FAIL: Could not pop state from backtracker\n");
        teardown_test(ctx);
        return false;
    }

    /* Verify popped data */
    mock_state_t *mock_state = (mock_state_t *)popped_state;
    if (mock_state->id != 2) { // Second state we pushed had id 2
        printf("FAIL: Popped wrong state, expected id 2, got %d\n", mock_state->id);
        teardown_test(ctx);
        return false;
    }

    if (popped_input_pos != 20) {
        printf("FAIL: Popped wrong input position, expected 20, got %zu\n", popped_input_pos);
        teardown_test(ctx);
        return false;
    }

    if (popped_num_groups != num_groups) {
        printf("FAIL: Popped wrong number of groups, expected %zu, got %zu\n", num_groups,
               popped_num_groups);
        teardown_test(ctx);
        return false;
    }

    teardown_test(ctx);
    printf("PASS: push_pop_operations\n");
    return true;
}

/* Test: reset operations */
static bool
test_reset_operations(void)
{
    printf("Running test: reset_operations\n");

    test_context_t *ctx = setup_test();
    if (!ctx)
        return false;

    const size_t num_groups = 2;
    size_t group_starts[2] = {0, 5};
    size_t group_ends[2] = {4, 9};

    /* Push multiple states */
    for (int i = 0; i < 5; i++) {
        bool result =
            rift_safe_backtracker_push(ctx->backtracker, (rift_regex_state_t *)&ctx->states[i],
                                       i * 5, group_starts, group_ends, num_groups, &ctx->error);

        if (!result) {
            printf("FAIL: Could not push state %d to backtracker\n", i);
            teardown_test(ctx);
            return false;
        }
    }

    /* Verify depth */
    size_t depth = rift_safe_backtracker_get_depth(ctx->backtracker, &ctx->error);
    if (depth != 5) {
        printf("FAIL: Expected depth 5, got %zu\n", depth);
        teardown_test(ctx);
        return false;
    }

    /* Reset the backtracker */
    bool result = rift_safe_backtracker_reset(ctx->backtracker, &ctx->error);
    if (!result) {
        printf("FAIL: Could not reset backtracker\n");
        teardown_test(ctx);
        return false;
    }

    /* Verify it's empty */
    if (!rift_safe_backtracker_is_empty(ctx->backtracker, &ctx->error)) {
        printf("FAIL: Backtracker should be empty after reset\n");
        teardown_test(ctx);
        return false;
    }

    /* Verify depth is 0 */
    depth = rift_safe_backtracker_get_depth(ctx->backtracker, &ctx->error);
    if (depth != 0) {
        printf("FAIL: Depth should be 0 after reset, got %zu\n", depth);
        teardown_test(ctx);
        return false;
    }

    teardown_test(ctx);
    printf("PASS: reset_operations\n");
    return true;
}

/* Test: max depth limit */
static bool
test_max_depth_limit(void)
{
    printf("Running test: max_depth_limit\n");

    /* Create backtracker with small max depth */
    rift_regex_safe_backtracker_t *bt = rift_safe_backtracker_create(3, 2);
    if (!bt) {
        printf("FAIL: Could not create backtracker\n");
        return false;
    }

    const size_t num_groups = 2;
    size_t group_starts[2] = {0, 5};
    size_t group_ends[2] = {4, 9};
    mock_state_t states[5];
    rift_regex_error_t error;

    for (int i = 0; i < 5; i++) {
        states[i].id = i + 1;
    }

    /* Push up to max depth */
    for (int i = 0; i < 3; i++) {
        bool result = rift_safe_backtracker_push(bt, (rift_regex_state_t *)&states[i], i * 5,
                                                 group_starts, group_ends, num_groups, &error);

        if (!result) {
            printf("FAIL: Could not push state %d to backtracker\n", i);
            rift_safe_backtracker_free(bt);
            return false;
        }
    }

    /* Try to push beyond max depth */
    bool result = rift_safe_backtracker_push(bt, (rift_regex_state_t *)&states[3], 15, group_starts,
                                             group_ends, num_groups, &error);

    if (result) {
        printf("FAIL: Should not be able to push beyond max depth\n");
        rift_safe_backtracker_free(bt);
        return false;
    }

    /* Change max depth */
    if (!rift_safe_backtracker_set_max_depth(bt, 5)) {
        printf("FAIL: Could not set max depth\n");
        rift_safe_backtracker_free(bt);
        return false;
    }

    /* Verify max depth changed */
    if (rift_safe_backtracker_get_max_depth(bt) != 5) {
        printf("FAIL: Max depth not updated correctly\n");
        rift_safe_backtracker_free(bt);
        return false;
    }

    /* Now we should be able to push more */
    result = rift_safe_backtracker_push(bt, (rift_regex_state_t *)&states[3], 15, group_starts,
                                        group_ends, num_groups, &error);

    if (!result) {
        printf("FAIL: Could not push after increasing max depth\n");
        rift_safe_backtracker_free(bt);
        return false;
    }

    rift_safe_backtracker_free(bt);
    printf("PASS: max_depth_limit\n");
    return true;
}

/* Thread function for multithreaded test */
static void *
thread_push_pop(void *arg)
{
    thread_test_context_t *ctx = (thread_test_context_t *)arg;
    mock_state_t states[10];
    const size_t num_groups = 2;
    size_t group_starts[2];
    size_t group_ends[2];
    rift_regex_error_t error;

    /* Initialize states */
    for (int i = 0; i < 10; i++) {
        states[i].id = ctx->thread_id * 100 + i;
    }

    /* Do a series of push and pop operations */
    for (int i = 0; i < ctx->operations; i++) {
        /* Set unique group positions based on thread ID and iteration */
        group_starts[0] = ctx->thread_id * 100 + i;
        group_starts[1] = ctx->thread_id * 100 + i + 10;
        group_ends[0] = group_starts[0] + 5;
        group_ends[1] = group_starts[1] + 5;

        /* Push a state */
        bool result =
            rift_safe_backtracker_push(ctx->backtracker, (rift_regex_state_t *)&states[i % 10], i,
                                       group_starts, group_ends, num_groups, &error);

        if (!result) {
            ctx->success = false;
            return NULL;
        }

        /* Pop a state half the time */
        if (i % 2 == 0) {
            rift_regex_state_t *popped_state = NULL;
            size_t popped_input_pos = 0;
            size_t popped_group_starts[2] = {0};
            size_t popped_group_ends[2] = {0};
            size_t popped_num_groups = 0;

            result = rift_safe_backtracker_pop(ctx->backtracker, &popped_state, &popped_input_pos,
                                               popped_group_starts, popped_group_ends,
                                               &popped_num_groups, &error);

            if (!result) {
                ctx->success = false;
                return NULL;
            }
        }
    }

    ctx->success = true;
    return NULL;
}

/* Test: multithreaded operations */
static bool
test_multithreaded_operations(void)
{
    printf("Running test: multithreaded_operations\n");

    const int num_threads = 4;
    pthread_t threads[num_threads];
    thread_test_context_t contexts[num_threads];

    /* Create a backtracker with enough capacity */
    rift_regex_safe_backtracker_t *bt = rift_safe_backtracker_create(1000, 5);
    if (!bt) {
        printf("FAIL: Could not create backtracker\n");
        return false;
    }

    /* Initialize thread contexts and create threads */
    for (int i = 0; i < num_threads; i++) {
        contexts[i].backtracker = bt;
        contexts[i].thread_id = i + 1;
        contexts[i].operations = 100;
        contexts[i].success = false;

        if (pthread_create(&threads[i], NULL, thread_push_pop, &contexts[i]) != 0) {
            printf("FAIL: Could not create thread %d\n", i);
            rift_safe_backtracker_free(bt);
            return false;
        }
    }

    /* Wait for all threads to finish */
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Check all threads succeeded */
    bool all_succeeded = true;
    for (int i = 0; i < num_threads; i++) {
        if (!contexts[i].success) {
            printf("FAIL: Thread %d reported failure\n", i);
            all_succeeded = false;
        }
    }

    rift_safe_backtracker_free(bt);

    if (!all_succeeded) {
        return false;
    }

    printf("PASS: multithreaded_operations\n");
    return true;
}

/* Error handling test */
static bool
test_error_handling(void)
{
    printf("Running test: error_handling\n");

    test_context_t *ctx = setup_test();
    if (!ctx)
        return false;

    rift_regex_error_t error;

    /* Test with NULL parameters */
    const size_t num_groups = 2;
    size_t group_starts[2] = {0, 5};
    size_t group_ends[2] = {4, 9};

    /* NULL backtracker */
    bool result = rift_safe_backtracker_push(NULL, (rift_regex_state_t *)&ctx->states[0], 0,
                                             group_starts, group_ends, num_groups, &error);

    if (result) {
        printf("FAIL: Push should fail with NULL backtracker\n");
        teardown_test(ctx);
        return false;
    }

    /* NULL state */
    result = rift_safe_backtracker_push(ctx->backtracker, NULL, 0, group_starts, group_ends,
                                        num_groups, &error);

    if (result) {
        printf("FAIL: Push should fail with NULL state\n");
        teardown_test(ctx);
        return false;
    }

    /* NULL group arrays */
    result = rift_safe_backtracker_push(ctx->backtracker, (rift_regex_state_t *)&ctx->states[0], 0,
                                        NULL, group_ends, num_groups, &error);

    if (result) {
        printf("FAIL: Push should fail with NULL group_starts\n");
        teardown_test(ctx);
        return false;
    }

    result = rift_safe_backtracker_push(ctx->backtracker, (rift_regex_state_t *)&ctx->states[0], 0,
                                        group_starts, NULL, num_groups, &error);

    if (result) {
        printf("FAIL: Push should fail with NULL group_ends\n");
        teardown_test(ctx);
        return false;
    }

    teardown_test(ctx);
    printf("PASS: error_handling\n");
    return true;
}

/* Main test runner */
int
main(int argc, char **argv)
{
    printf("===== Running Safe Backtracker Tests =====\n");

    int tests_failed = 0;

    if (!test_create_and_init())
        tests_failed++;
    if (!test_get_local_backtracker())
        tests_failed++;
    if (!test_push_pop_operations())
        tests_failed++;
    if (!test_reset_operations())
        tests_failed++;
    if (!test_max_depth_limit())
        tests_failed++;
    if (!test_multithreaded_operations())
        tests_failed++;
    if (!test_error_handling())
        tests_failed++;

    printf("===== Safe Backtracker Test Results =====\n");
    printf("Tests run: 7\n");
    printf("Tests failed: %d\n", tests_failed);

    return tests_failed > 0 ? 1 : 0;
}