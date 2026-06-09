/**
 * @file groups_test.c
 * @brief Unit tests for the LibRift regex engine capture groups functionality
 *
 * This file contains unit tests for the capture group handling functions
 * in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/runtime/groups/groups.h"

// Test utility function to verify capture group properties
static void
verify_group(rift_regex_capture_group_t *group, size_t expected_index, const char *expected_name,
             size_t expected_start, size_t expected_end)
{
    assert(group != NULL);
    assert(group->index == expected_index);

    if (expected_name == NULL) {
        assert(group->name == NULL);
    } else {
        assert(group->name != NULL);
        assert(strcmp(group->name, expected_name) == 0);
    }

    assert(group->start_pos == expected_start);
    assert(group->end_pos == expected_end);
}

// Test rift_capture_group_create and rift_capture_group_free
static void
test_capture_group_create_free(void)
{
    printf("Running test_capture_group_create_free...\n");

    // Test with unnamed group
    rift_regex_capture_group_t *group1 = rift_capture_group_create(1, NULL, 5, 10);
    verify_group(group1, 1, NULL, 5, 10);

    // Test with named group
    rift_regex_capture_group_t *group2 = rift_capture_group_create(2, "test_group", 15, 20);
    verify_group(group2, 2, "test_group", 15, 20);

    // Free groups
    rift_capture_group_free(group1);
    rift_capture_group_free(group2);

    // Test free with NULL (should not crash)
    rift_capture_group_free(NULL);

    printf("test_capture_group_create_free passed.\n");
}

// Test rift_capture_group_get_start and rift_capture_group_get_end
static void
test_capture_group_positions(void)
{
    printf("Running test_capture_group_positions...\n");

    rift_regex_capture_group_t *group = rift_capture_group_create(1, "test", 5, 10);

    // Test get start position
    assert(rift_capture_group_get_start(group) == 5);

    // Test get end position
    assert(rift_capture_group_get_end(group) == 10);

    // Test with NULL group
    assert(rift_capture_group_get_start(NULL) == (size_t)-1);
    assert(rift_capture_group_get_end(NULL) == (size_t)-1);

    rift_capture_group_free(group);

    printf("test_capture_group_positions passed.\n");
}

// Test rift_capture_group_get_content
static void
test_capture_group_get_content(void)
{
    printf("Running test_capture_group_get_content...\n");

    const char *input = "Hello world! This is a test.";
    rift_regex_capture_group_t *group = rift_capture_group_create(1, "test", 6, 11);

    char output[20];

    // Test normal case (should extract "world")
    assert(rift_capture_group_get_content(group, input, output, sizeof(output)) == true);
    assert(strcmp(output, "world") == 0);

    // Test with small buffer
    assert(rift_capture_group_get_content(group, input, output, 3) == false);

    // Test with NULL parameters
    assert(rift_capture_group_get_content(NULL, input, output, sizeof(output)) == false);
    assert(rift_capture_group_get_content(group, NULL, output, sizeof(output)) == false);
    assert(rift_capture_group_get_content(group, input, NULL, sizeof(output)) == false);
    assert(rift_capture_group_get_content(group, input, output, 0) == false);

    rift_capture_group_free(group);

    printf("test_capture_group_get_content passed.\n");
}

// Test rift_capture_group_get_name
static void
test_capture_group_get_name(void)
{
    printf("Running test_capture_group_get_name...\n");

    // Test with named group
    rift_regex_capture_group_t *named_group = rift_capture_group_create(1, "test_name", 5, 10);
    assert(strcmp(rift_capture_group_get_name(named_group), "test_name") == 0);

    // Test with unnamed group
    rift_regex_capture_group_t *unnamed_group = rift_capture_group_create(2, NULL, 15, 20);
    assert(rift_capture_group_get_name(unnamed_group) == NULL);

    // Test with NULL group
    assert(rift_capture_group_get_name(NULL) == NULL);

    rift_capture_group_free(named_group);
    rift_capture_group_free(unnamed_group);

    printf("test_capture_group_get_name passed.\n");
}

// Test rift_capture_groups_create and rift_capture_groups_free
static void
test_capture_groups_create_free(void)
{
    printf("Running test_capture_groups_create_free...\n");

    rift_regex_capture_groups_t *groups = rift_capture_groups_create(10);
    assert(groups != NULL);
    assert(groups->capacity == 10);
    assert(groups->count == 0);
    assert(groups->groups != NULL);

    rift_capture_groups_free(groups);

    // Test with edge case
    rift_regex_capture_groups_t *zero_groups = rift_capture_groups_create(0);
    assert(zero_groups != NULL);
    assert(zero_groups->capacity == 0);
    rift_capture_groups_free(zero_groups);

    // Test free with NULL (should not crash)
    rift_capture_groups_free(NULL);

    printf("test_capture_groups_create_free passed.\n");
}

// Test rift_capture_groups_record and rift_capture_groups_get_by_index/name
static void
test_capture_groups_record_and_get(void)
{
    printf("Running test_capture_groups_record_and_get...\n");

    rift_regex_capture_groups_t *groups = rift_capture_groups_create(5);

    // Record some groups
    assert(rift_capture_groups_record(groups, 0, NULL, 0, 5) == true);
    assert(rift_capture_groups_record(groups, 1, "first", 6, 10) == true);
    assert(rift_capture_groups_record(groups, 2, "second", 11, 16) == true);

    // Test count
    assert(rift_capture_groups_get_count(groups) == 3);

    // Test get by index
    rift_regex_capture_group_t *group0 = rift_capture_groups_get_by_index(groups, 0);
    verify_group(group0, 0, NULL, 0, 5);

    rift_regex_capture_group_t *group1 = rift_capture_groups_get_by_index(groups, 1);
    verify_group(group1, 1, "first", 6, 10);

    // Test get by name
    rift_regex_capture_group_t *group_first = rift_capture_groups_get_by_name(groups, "first");
    verify_group(group_first, 1, "first", 6, 10);

    rift_regex_capture_group_t *group_second = rift_capture_groups_get_by_name(groups, "second");
    verify_group(group_second, 2, "second", 11, 16);

    // Test get non-existent group
    assert(rift_capture_groups_get_by_index(groups, 5) == NULL);
    assert(rift_capture_groups_get_by_name(groups, "nonexistent") == NULL);

    rift_capture_groups_free(groups);

    printf("test_capture_groups_record_and_get passed.\n");
}

// Test rift_capture_groups_reset
static void
test_capture_groups_reset(void)
{
    printf("Running test_capture_groups_reset...\n");

    rift_regex_capture_groups_t *groups = rift_capture_groups_create(5);

    // Add groups
    rift_capture_groups_record(groups, 0, NULL, 0, 5);
    rift_capture_groups_record(groups, 1, "test", 6, 10);

    assert(rift_capture_groups_get_count(groups) == 2);

    // Reset groups
    rift_capture_groups_reset(groups);

    // Check that groups are cleared
    assert(rift_capture_groups_get_count(groups) == 0);
    assert(rift_capture_groups_get_by_index(groups, 0) == NULL);
    assert(rift_capture_groups_get_by_name(groups, "test") == NULL);

    // Test with NULL
    rift_capture_groups_reset(NULL); // Should not crash

    rift_capture_groups_free(groups);

    printf("test_capture_groups_reset passed.\n");
}

// Test rift_capture_groups_clone
static void
test_capture_groups_clone(void)
{
    printf("Running test_capture_groups_clone...\n");

    rift_regex_capture_groups_t *original = rift_capture_groups_create(5);

    // Add groups
    rift_capture_groups_record(original, 0, NULL, 0, 5);
    rift_capture_groups_record(original, 1, "test", 6, 10);

    // Clone groups
    rift_regex_capture_groups_t *clone = rift_capture_groups_clone(original);

    // Verify clone has same properties
    assert(clone != NULL);
    assert(clone->capacity == original->capacity);
    assert(clone->count == original->count);

    // Verify groups in clone
    rift_regex_capture_group_t *group0 = rift_capture_groups_get_by_index(clone, 0);
    verify_group(group0, 0, NULL, 0, 5);

    rift_regex_capture_group_t *group1 = rift_capture_groups_get_by_index(clone, 1);
    verify_group(group1, 1, "test", 6, 10);

    // Test clone with NULL
    assert(rift_capture_groups_clone(NULL) == NULL);

    rift_capture_groups_free(original);
    rift_capture_groups_free(clone);

    printf("test_capture_groups_clone passed.\n");
}

// Main test function
int
main(void)
{
    printf("Starting regex groups unit tests\n");

    test_capture_group_create_free();
    test_capture_group_positions();
    test_capture_group_get_content();
    test_capture_group_get_name();
    test_capture_groups_create_free();
    test_capture_groups_record_and_get();
    test_capture_groups_reset();
    test_capture_groups_clone();

    printf("All regex groups unit tests passed!\n");
    return 0;
}
