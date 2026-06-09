/**
 * @file excution_tracker_test.c
 * @brief Unit tests for execution time tracker
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "librift/runtime/execution/execution_time_tracker.h"

// Test helper function to simulate work
static void
sleep_ms(unsigned int milliseconds)
{
    usleep(milliseconds * 1000);
}

// Test creation with no time limit
static void
test_create_no_limit(void)
{
    rift_execution_time_tracker_t *tracker = rift_execution_time_tracker_create(0);

    assert(tracker != NULL);
    assert(tracker->start_time == 0);
    assert(tracker->max_duration == 0);
    assert(tracker->time_limit_enabled == false);

    rift_execution_time_tracker_free(tracker);
    printf("PASS: test_create_no_limit\n");
}

// Test creation with time limit
static void
test_create_with_limit(void)
{
    uint32_t duration_ms = 500;
    rift_execution_time_tracker_t *tracker = rift_execution_time_tracker_create(duration_ms);

    assert(tracker != NULL);
    assert(tracker->start_time == 0);
    assert(tracker->max_duration == (uint64_t)duration_ms * 1000);
    assert(tracker->time_limit_enabled == true);

    rift_execution_time_tracker_free(tracker);
    printf("PASS: test_create_with_limit\n");
}

// Test start tracking
static void
test_start_tracking(void)
{
    rift_execution_time_tracker_t *tracker = rift_execution_time_tracker_create(500);

    assert(tracker->start_time == 0);
    rift_execution_time_tracker_start(tracker);
    assert(tracker->start_time > 0);

    rift_execution_time_tracker_free(tracker);
    printf("PASS: test_start_tracking\n");
}

// Test that start doesn't affect disabled tracker
static void
test_start_tracking_disabled(void)
{
    rift_execution_time_tracker_t *tracker = rift_execution_time_tracker_create(0);

    assert(tracker->start_time == 0);
    rift_execution_time_tracker_start(tracker);
    assert(tracker->start_time == 0); // Should remain 0 when disabled

    rift_execution_time_tracker_free(tracker);
    printf("PASS: test_start_tracking_disabled\n");
}

// Test has_exceeded_limit with no time limit
static void
test_has_not_exceeded_when_disabled(void)
{
    rift_execution_time_tracker_t *tracker = rift_execution_time_tracker_create(0);

    rift_execution_time_tracker_start(tracker);
    sleep_ms(10);
    assert(rift_execution_time_tracker_has_exceeded_limit(tracker) == false);

    rift_execution_time_tracker_free(tracker);
    printf("PASS: test_has_not_exceeded_when_disabled\n");
}

// Test has_exceeded_limit when under limit
static void
test_has_not_exceeded_when_under_limit(void)
{
    rift_execution_time_tracker_t *tracker = rift_execution_time_tracker_create(100);

    rift_execution_time_tracker_start(tracker);
    sleep_ms(10);
    assert(rift_execution_time_tracker_has_exceeded_limit(tracker) == false);

    rift_execution_time_tracker_free(tracker);
    printf("PASS: test_has_not_exceeded_when_under_limit\n");
}

// Test has_exceeded_limit when over limit
static void
test_has_exceeded_when_over_limit(void)
{
    rift_execution_time_tracker_t *tracker = rift_execution_time_tracker_create(20);

    rift_execution_time_tracker_start(tracker);
    sleep_ms(30); // Sleep longer than the limit
    assert(rift_execution_time_tracker_has_exceeded_limit(tracker) == true);

    rift_execution_time_tracker_free(tracker);
    printf("PASS: test_has_exceeded_when_over_limit\n");
}

// Test get_elapsed time
static void
test_get_elapsed(void)
{
    rift_execution_time_tracker_t *tracker = rift_execution_time_tracker_create(100);

    rift_execution_time_tracker_start(tracker);
    sleep_ms(10);
    uint64_t elapsed = rift_execution_time_tracker_get_elapsed(tracker);

    // Allow for some imprecision in sleep timing
    assert(elapsed >= 9000); // At least 9ms (in microseconds)

    rift_execution_time_tracker_free(tracker);
    printf("PASS: test_get_elapsed\n");
}

// Test get_elapsed with unstarted tracker
static void
test_get_elapsed_unstarted(void)
{
    rift_execution_time_tracker_t *tracker = rift_execution_time_tracker_create(100);

    assert(rift_execution_time_tracker_get_elapsed(tracker) == 0);

    rift_execution_time_tracker_free(tracker);
    printf("PASS: test_get_elapsed_unstarted\n");
}

// Test set_max_duration
static void
test_set_max_duration(void)
{
    rift_execution_time_tracker_t *tracker = rift_execution_time_tracker_create(100);

    assert(tracker->max_duration == 100000); // Initially 100ms
    rift_execution_time_tracker_set_max_duration(tracker, 200);
    assert(tracker->max_duration == 200000); // Updated to 200ms
    assert(tracker->time_limit_enabled == true);

    // Test disabling by setting to 0
    rift_execution_time_tracker_set_max_duration(tracker, 0);
    assert(tracker->max_duration == 0);
    assert(tracker->time_limit_enabled == false);

    rift_execution_time_tracker_free(tracker);
    printf("PASS: test_set_max_duration\n");
}

// Test null pointer handling
static void
test_null_handling(void)
{
    // These should not crash
    rift_execution_time_tracker_start(NULL);
    assert(rift_execution_time_tracker_has_exceeded_limit(NULL) == false);
    assert(rift_execution_time_tracker_get_elapsed(NULL) == 0);
    rift_execution_time_tracker_set_max_duration(NULL, 100);
    rift_execution_time_tracker_free(NULL);

    printf("PASS: test_null_handling\n");
}

int
main(void)
{
    printf("Starting execution_tracker tests\n");

    // Basic functionality tests
    test_create_no_limit();
    test_create_with_limit();
    test_start_tracking();
    test_start_tracking_disabled();

    // Limit checking tests
    test_has_not_exceeded_when_disabled();
    test_has_not_exceeded_when_under_limit();
    test_has_exceeded_when_over_limit();

    // Time measurement tests
    test_get_elapsed();
    test_get_elapsed_unstarted();

    // Configuration tests
    test_set_max_duration();

    // Edge case tests
    test_null_handling();

    printf("All tests passed!\n");
    return 0;
}