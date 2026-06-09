/**
 * @file config_test.c
 * @brief Unit tests for LibRift configuration system
 *
 * This file contains unit tests for the configuration management
 * facilities of the LibRift library.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "librift/core/config/config.h"

/* Test function declarations */
static void test_config_initialization(void);
static void test_config_get_set(void);
static void test_config_reset(void);
static void test_regex_params(void);
static void test_memory_allocator(void);
static void test_config_serialization(void);

/* Custom memory functions for testing */
static void *test_malloc(size_t size);
static void *test_realloc(void *ptr, size_t size);
static void test_free(void *ptr);

/**
 * @brief Main test entry point
 */
int
main(void)
{
    printf("Running configuration tests...\n");

    test_config_initialization();
    test_config_get_set();
    test_config_reset();
    test_regex_params();
    test_memory_allocator();
    test_config_serialization();

    printf("All configuration tests passed!\n");
    return 0;
}

/**
 * @brief Test configuration initialization and cleanup
 */
static void
test_config_initialization(void)
{
    rift_status_t status;

    /* First cleanup in case a previous test left it initialized */
    rift_config_cleanup();

    /* Test initialization */
    status = rift_config_initialize();
    assert(status == RIFT_OK && "Config initialization should succeed");

    /* Test re-initialization (should be safe) */
    status = rift_config_initialize();
    assert(status == RIFT_OK && "Double initialization should be handled gracefully");

    /* Verify we can get the config */
    const rift_config_t *config = rift_config_get();
    assert(config != NULL && "Should get valid config after initialization");

    /* Test cleanup */
    status = rift_config_cleanup();
    assert(status == RIFT_OK && "Config cleanup should succeed");

    /* Test double cleanup */
    status = rift_config_cleanup();
    assert(status == RIFT_OK && "Double cleanup should be handled gracefully");

    printf("✓ Initialization/cleanup tests passed\n");
}

/**
 * @brief Test getting and setting configuration
 */
static void
test_config_get_set(void)
{
    rift_status_t status;

    /* Initialize configuration */
    status = rift_config_initialize();
    assert(status == RIFT_OK);

    /* Get the default configuration */
    const rift_config_t *default_config = rift_config_get();
    assert(default_config != NULL);

    /* Create a modified configuration */
    rift_config_t custom_config = *default_config;

    /* Set the new configuration */
    status = rift_config_set(&custom_config);
    assert(status == RIFT_OK && "Setting config should succeed");

    /* Verify setting NULL fails */
    status = rift_config_set(NULL);
    assert(status != RIFT_OK && "Setting NULL config should fail");

    /* Cleanup */
    status = rift_config_cleanup();
    assert(status == RIFT_OK);

    printf("✓ Get/set tests passed\n");
}

/**
 * @brief Test configuration reset
 */
static void
test_config_reset(void)
{
    rift_status_t status;

    /* Initialize configuration */
    status = rift_config_initialize();
    assert(status == RIFT_OK);

    /* Get default configuration */
    const rift_config_t *default_config = rift_config_get();

    /* Create a modified configuration */
    rift_config_t custom_config = *default_config;

    /* Set the custom configuration */
    status = rift_config_set(&custom_config);
    assert(status == RIFT_OK);

    /* Reset to defaults */
    status = rift_config_reset();
    assert(status == RIFT_OK && "Reset should succeed");

    /* Get the reset configuration */
    const rift_config_t *reset_config = rift_config_get();
    assert(reset_config != NULL);

    /* Cleanup */
    status = rift_config_cleanup();
    assert(status == RIFT_OK);

    printf("✓ Reset tests passed\n");
}

/**
 * @brief Test regex parameter access
 */
static void
test_regex_params(void)
{
    rift_status_t status;

    /* Initialize configuration */
    status = rift_config_initialize();
    assert(status == RIFT_OK);

    /* Test getting regex parameters */
    size_t pattern_length;
    status = rift_config_get_regex_param(RIFT_REGEX_PARAM_MAX_PATTERN_LENGTH, &pattern_length);
    assert(status == RIFT_OK && "Getting regex param should succeed");

    size_t capture_groups;
    status = rift_config_get_regex_param(RIFT_REGEX_PARAM_MAX_CAPTURE_GROUPS, &capture_groups);
    assert(status == RIFT_OK && "Getting capture groups should succeed");

    /* Test setting regex parameters */
    size_t new_pattern_length = 8192;
    status = rift_config_set_regex_param(RIFT_REGEX_PARAM_MAX_PATTERN_LENGTH, &new_pattern_length);
    assert(status == RIFT_OK && "Setting regex param should succeed");

    /* Verify the parameter was updated */
    size_t updated_length;
    status = rift_config_get_regex_param(RIFT_REGEX_PARAM_MAX_PATTERN_LENGTH, &updated_length);
    assert(status == RIFT_OK);
    assert(updated_length == new_pattern_length && "Updated parameter should match what was set");

    /* Test error handling */
    status = rift_config_get_regex_param(RIFT_REGEX_PARAM_MAX_PATTERN_LENGTH, NULL);
    assert(status != RIFT_OK && "NULL output pointer should fail");

    status = rift_config_set_regex_param(RIFT_REGEX_PARAM_MAX_PATTERN_LENGTH, NULL);
    assert(status != RIFT_OK && "NULL value should fail");

    /* Cleanup */
    status = rift_config_cleanup();
    assert(status == RIFT_OK);

    printf("✓ Regex parameter tests passed\n");
}

/**
 * @brief Test memory allocator configuration
 */
static void
test_memory_allocator(void)
{
    rift_status_t status;

    /* Initialize configuration */
    status = rift_config_initialize();
    assert(status == RIFT_OK);

    /* Set custom memory functions */
    status = rift_config_set_memory_allocator(test_malloc, test_realloc, test_free);
    assert(status == RIFT_OK && "Setting memory allocators should succeed");

    /* Test error handling */
    status = rift_config_set_memory_allocator(NULL, test_realloc, test_free);
    assert(status != RIFT_OK && "NULL malloc should fail");

    /* Cleanup */
    status = rift_config_cleanup();
    assert(status == RIFT_OK);

    printf("✓ Memory allocator tests passed\n");
}

/**
 * @brief Test configuration serialization and deserialization
 */
static void
test_config_serialization(void)
{
    rift_status_t status;

    /* Initialize configuration */
    status = rift_config_initialize();
    assert(status == RIFT_OK);

    /* Test JSON serialization */
    char json_buffer[4096];
    status = rift_config_to_json(json_buffer, sizeof(json_buffer));
    assert(status == RIFT_OK && "JSON serialization should succeed");
    assert(strlen(json_buffer) > 0 && "JSON output should not be empty");

    /* Test JSON deserialization */
    status = rift_config_from_json(json_buffer);
    assert(status == RIFT_OK && "JSON deserialization should succeed");

    /* Test error handling */
    status = rift_config_to_json(NULL, sizeof(json_buffer));
    assert(status != RIFT_OK && "NULL buffer should fail");

    status = rift_config_to_json(json_buffer, 0);
    assert(status != RIFT_OK && "Zero buffer size should fail");

    status = rift_config_from_json(NULL);
    assert(status != RIFT_OK && "NULL JSON should fail");

    status = rift_config_from_json("{invalid json}");
    assert(status != RIFT_OK && "Invalid JSON should fail");

    /* Cleanup */
    status = rift_config_cleanup();
    assert(status == RIFT_OK);

    printf("✓ Serialization tests passed\n");
}

/* Custom memory functions for testing */
static void *
test_malloc(size_t size)
{
    return malloc(size);
}

static void *
test_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

static void
test_free(void *ptr)
{
    free(ptr);
}
