#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/regex/runtime/context/thread_safe_context.h"

#define NUM_THREADS 4
#define NUM_ITERATIONS 1000

// Test context instance
static RiftRegexThreadSafeContext *ctx = NULL;

// Test data for thread operations
typedef struct {
    int thread_id;
    int iterations;
    bool success;
} TestData;

// Thread function for concurrent access test
void *
test_concurrent_access(void *arg)
{
    TestData *data = (TestData *)arg;

    for (int i = 0; i < data->iterations; i++) {
        // Perform operations on the thread-safe context
        rift_regex_thread_safe_context_lock(ctx);
        // Simulate some work on the context
        rift_regex_thread_safe_context_unlock(ctx);
    }

    data->success = true;
    return NULL;
}

// Test initialization and destruction
bool
test_init_destroy(void)
{
    RiftRegexThreadSafeContext *local_ctx = rift_regex_thread_safe_context_create();
    if (!local_ctx) {
        printf("FAIL: Context creation failed\n");
        return false;
    }

    rift_regex_thread_safe_context_destroy(local_ctx);
    printf("PASS: Context init and destroy\n");
    return true;
}

// Test concurrent access
bool
test_concurrency(void)
{
    pthread_t threads[NUM_THREADS];
    TestData thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].iterations = NUM_ITERATIONS;
        thread_data[i].success = false;

        if (pthread_create(&threads[i], NULL, test_concurrent_access, &thread_data[i]) != 0) {
            printf("FAIL: Thread creation failed\n");
            return false;
        }
    }

    // Join all threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        if (!thread_data[i].success) {
            printf("FAIL: Thread %d reported failure\n", i);
            return false;
        }
    }

    printf("PASS: Concurrent access test\n");
    return true;
}

// Test state consistency
bool
test_state_consistency(void)
{
    rift_regex_thread_safe_context_reset(ctx);

    // Set some state
    rift_regex_thread_safe_context_lock(ctx);
    bool success = rift_regex_thread_safe_context_set_data(ctx, "test_key", "test_value");
    rift_regex_thread_safe_context_unlock(ctx);

    if (!success) {
        printf("FAIL: Setting context data failed\n");
        return false;
    }

    // Verify state
    rift_regex_thread_safe_context_lock(ctx);
    const char *value = rift_regex_thread_safe_context_get_data(ctx, "test_key");
    rift_regex_thread_safe_context_unlock(ctx);

    if (value == NULL || strcmp(value, "test_value") != 0) {
        printf("FAIL: Context state inconsistent\n");
        return false;
    }

    printf("PASS: State consistency test\n");
    return true;
}

// Setup function - runs before tests
bool
setup(void)
{
    ctx = rift_regex_thread_safe_context_create();
    return ctx != NULL;
}

// Teardown function - runs after tests
void
teardown(void)
{
    if (ctx) {
        rift_regex_thread_safe_context_destroy(ctx);
        ctx = NULL;
    }
}

int
main(void)
{
    int passed = 0;
    int total = 3; // Number of tests

    if (!setup()) {
        printf("Setup failed, aborting tests\n");
        return 1;
    }

    if (test_init_destroy())
        passed++;
    if (test_concurrency())
        passed++;
    if (test_state_consistency())
        passed++;

    teardown();

    printf("\n%d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}