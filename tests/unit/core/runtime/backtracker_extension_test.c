/**
 * @file backtracker_extension_test.c
 * @brief Unit tests for backtracker extension functions
 *
 * This file contains unit tests for the backtracker extension functionality
 * in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/runtime/backtracker/backtracker.h"
#include "librift/runtime/backtracker_extension.h"

/**
 * @brief Test the rift_backtracker_get_num_groups function
 */
static void
test_get_num_groups(void)
{
    /* Test with valid backtracker */
    const size_t expected_groups = 5;
    rift_regex_backtracker_t *backtracker = rift_backtracker_create(10, expected_groups);

    assert(backtracker != NULL && "Backtracker creation failed");

    size_t actual_groups = rift_backtracker_get_num_groups(backtracker);
    assert(actual_groups == expected_groups &&
           "rift_backtracker_get_num_groups returned incorrect number of groups");

    rift_backtracker_free(backtracker);

    /* Test with NULL backtracker (should return 0) */
    actual_groups = rift_backtracker_get_num_groups(NULL);
    assert(actual_groups == 0 &&
           "rift_backtracker_get_num_groups should return 0 for NULL backtracker");

    printf("test_get_num_groups: PASSED\n");
}

/**
 * @brief Test edge cases for rift_backtracker_get_num_groups
 */
static void
test_get_num_groups_edge_cases(void)
{
    /* Test with zero groups */
    rift_regex_backtracker_t *backtracker = rift_backtracker_create(10, 0);

    assert(backtracker != NULL && "Backtracker creation failed");

    size_t groups = rift_backtracker_get_num_groups(backtracker);
    assert(groups == 0 && "rift_backtracker_get_num_groups should return 0 for 0 groups");

    rift_backtracker_free(backtracker);

    /* Test with large number of groups */
    const size_t large_groups = 1000;
    backtracker = rift_backtracker_create(10, large_groups);

    assert(backtracker != NULL && "Backtracker creation failed with large groups");

    groups = rift_backtracker_get_num_groups(backtracker);
    assert(groups == large_groups &&
           "rift_backtracker_get_num_groups returned incorrect number for large groups");

    rift_backtracker_free(backtracker);

    printf("test_get_num_groups_edge_cases: PASSED\n");
}

int
main(void)
{
    printf("Running backtracker extension tests...\n");

    test_get_num_groups();
    test_get_num_groups_edge_cases();

    printf("All backtracker extension tests PASSED\n");
    return EXIT_SUCCESS;
}
