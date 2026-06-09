/**
 * @file memory_test.c
 * @brief Unit tests for LibRift memory management facilities
 *
 * This file contains unit tests for memory allocation, tracking,
 * and management functions in the LibRift library.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <librift/core/regex/memory/memory.h>

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

/* Test basic allocation and free */
static bool
test_basic_allocation(void)
{
    void *ptr = rift_malloc(128);
    TEST_ASSERT("malloc should return non-NULL pointer", ptr != NULL);

    rift_free(ptr);
    return true;
}

/* Test calloc zeroes memory */
static bool
test_calloc(void)
{
    int *numbers = (int *)rift_calloc(10, sizeof(int));
    TEST_ASSERT("calloc should return non-NULL pointer", numbers != NULL);

    for (int i = 0; i < 10; i++) {
        TEST_ASSERT("calloc should zero-initialize memory", numbers[i] == 0);
    }

    rift_free(numbers);
    return true;
}

/* Test realloc functionality */
static bool
test_realloc(void)
{
    char *str = (char *)rift_malloc(16);
    TEST_ASSERT("malloc should return non-NULL pointer", str != NULL);

    strcpy(str, "Hello");

    str = (char *)rift_realloc(str, 32);
    TEST_ASSERT("realloc should return non-NULL pointer", str != NULL);
    TEST_ASSERT("realloc should preserve content", strcmp(str, "Hello") == 0);

    strcat(str, ", World!");
    TEST_ASSERT("expanded memory should be usable", strcmp(str, "Hello, World!") == 0);

    rift_free(str);
    return true;
}

/* Test strdup functionality */
static bool
test_strdup(void)
{
    const char *original = "Test string";
    char *copy = rift_strdup(original);

    TEST_ASSERT("strdup should return non-NULL pointer", copy != NULL);
    TEST_ASSERT("strdup should copy the string content", strcmp(copy, original) == 0);

    rift_free(copy);
    return true;
}

/* Test memory tracking */
static bool
test_memory_tracking(void)
{
    size_t current_usage, peak_usage, total_allocs, total_frees, active_allocs;

    /* Enable tracking and reset stats */
    bool previous = rift_memory_tracking_enable(true);
    rift_memory_tracking_reset();

    /* Get initial stats */
    rift_memory_get_stats(&current_usage, &peak_usage, &total_allocs, &total_frees, &active_allocs);
    TEST_ASSERT("Initial current usage should be 0", current_usage == 0);
    TEST_ASSERT("Initial peak usage should be 0", peak_usage == 0);
    TEST_ASSERT("Initial total allocs should be 0", total_allocs == 0);
    TEST_ASSERT("Initial total frees should be 0", total_frees == 0);
    TEST_ASSERT("Initial active allocs should be 0", active_allocs == 0);

    /* Allocate some memory */
    void *ptr1 = rift_malloc(100);
    void *ptr2 = rift_malloc(200);

    /* Check stats after allocations */
    rift_memory_get_stats(&current_usage, &peak_usage, &total_allocs, &total_frees, &active_allocs);
    TEST_ASSERT("Total allocs should be 2", total_allocs == 2);
    TEST_ASSERT("Active allocs should be 2", active_allocs == 2);
    TEST_ASSERT("Current usage should be tracked", current_usage > 0);
    TEST_ASSERT("Peak usage should match current usage", peak_usage == current_usage);

    /* Free one pointer */
    rift_free(ptr1);

    /* Check stats after partial free */
    rift_memory_get_stats(&current_usage, &peak_usage, &total_allocs, &total_frees, &active_allocs);
    TEST_ASSERT("Total frees should be 1", total_frees == 1);
    TEST_ASSERT("Active allocs should be 1", active_allocs == 1);

    /* Free the other pointer */
    rift_free(ptr2);

    /* Check stats after all frees */
    rift_memory_get_stats(&current_usage, &peak_usage, &total_allocs, &total_frees, &active_allocs);
    TEST_ASSERT("Total frees should be 2", total_frees == 2);
    TEST_ASSERT("Active allocs should be 0", active_allocs == 0);
    TEST_ASSERT("Current usage should be 0", current_usage == 0);

    /* Restore previous tracking state */
    rift_memory_tracking_enable(previous);
    return true;
}

/* Test memory report */
static bool
test_memory_report(void)
{
    char report_buffer[1024];

    /* Enable tracking and reset stats */
    rift_memory_tracking_enable(true);
    rift_memory_tracking_reset();

    /* Allocate some memory */
    void *ptr = rift_malloc(1024);
    TEST_ASSERT("malloc should return non-NULL pointer", ptr != NULL);

    /* Generate report */
    rift_status_t status = rift_memory_report(report_buffer, sizeof(report_buffer));
    TEST_ASSERT("Memory report should succeed", status == 0); /* Assuming 0 is RIFT_OK */
    TEST_ASSERT("Report should not be empty", strlen(report_buffer) > 0);

    /* Cleanup */
    rift_free(ptr);
    rift_memory_tracking_enable(false);
    return true;
}

int
main(void)
{
    printf("Running LibRift memory unit tests...\n");

    RUN_TEST(test_basic_allocation);
    RUN_TEST(test_calloc);
    RUN_TEST(test_realloc);
    RUN_TEST(test_strdup);
    RUN_TEST(test_memory_tracking);
    RUN_TEST(test_memory_report);

    printf("\nTest summary: %d tests, %d passed, %d failed\n", tests_run, tests_run - tests_failed,
           tests_failed);

    return tests_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
