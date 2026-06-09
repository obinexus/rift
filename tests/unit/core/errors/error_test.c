/**
 * @file error_test.c
 * @brief Unit tests for the LibRift error handling functionality
 *
 * This file contains unit tests for the error handling functionality
 * provided by the LibRift library.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "librift/errors/error.h"

// Test callback function to verify callback functionality
static rift_error_info_t callback_info;
static int callback_called = 0;

static void
test_error_callback(const rift_error_info_t *info)
{
    callback_called = 1;
    callback_info = *info;
}

// Test: Setting and getting errors
static void
test_error_set_get(void)
{
    const char *test_message = "Test error message";

    // Set an error
    rift_error_set(RIFT_ERROR_INVALID_PARAMETER, __LINE__, __FILE__, test_message);

    // Get the error info
    const rift_error_info_t *info = rift_error_get();

    // Verify error info
    assert(info != NULL);
    assert(info->status == RIFT_ERROR_INVALID_PARAMETER);
    assert(strstr(info->message, test_message) != NULL);
    assert(strcmp(info->file, __FILE__) == 0);

    // Clear the error
    rift_error_clear();

    // Verify error was cleared
    info = rift_error_get();
    assert(info != NULL);
    assert(info->status == RIFT_OK);

    printf("test_error_set_get: PASSED\n");
}

// Test: Error callback registration and calling
static void
test_error_callback_func(void)
{
    // Reset callback variable
    callback_called = 0;

    // Set the callback
    rift_error_callback_t prev_callback = rift_error_set_callback(test_error_callback);
    assert(prev_callback == NULL); // First time setting, should be NULL

    // Set an error which should trigger the callback
    rift_error_set(RIFT_ERROR_MEMORY_ALLOCATION, __LINE__, __FILE__, "Memory allocation test");

    // Verify callback was called
    assert(callback_called == 1);
    assert(callback_info.status == RIFT_ERROR_MEMORY_ALLOCATION);

    // Reset the callback
    prev_callback = rift_error_set_callback(NULL);
    assert(prev_callback == test_error_callback);

    printf("test_error_callback_func: PASSED\n");
}

// Test: Error status string
static void
test_error_status_string(void)
{
    const char *ok_str = rift_error_status_string(RIFT_OK);
    assert(ok_str != NULL);
    assert(strcmp(ok_str, "No error") == 0);

    const char *mem_err_str = rift_error_status_string(RIFT_ERROR_MEMORY_ALLOCATION);
    assert(mem_err_str != NULL);
    assert(strcmp(mem_err_str, "Memory allocation failed") == 0);

    printf("test_error_status_string: PASSED\n");
}

// Test: Error formatting
static void
test_error_format(void)
{
    rift_error_info_t test_info = {.status = RIFT_ERROR_SYNTAX,
                                   .line = 123,
                                   .file = "test_file.c",
                                   .message = "Test syntax error"};

    char buffer[256] = {0};
    rift_status_t status = rift_error_format(&test_info, buffer, sizeof(buffer));

    // Verify format operation succeeded
    assert(status == RIFT_OK);

    // Verify buffer contains expected elements
    assert(strstr(buffer, "RIFT_ERROR_SYNTAX") != NULL);
    assert(strstr(buffer, "test_file.c") != NULL);
    assert(strstr(buffer, "123") != NULL);
    assert(strstr(buffer, "Test syntax error") != NULL);

    // Test buffer overflow handling
    char small_buffer[10] = {0};
    status = rift_error_format(&test_info, small_buffer, sizeof(small_buffer));

    // Should return buffer overflow error
    assert(status == RIFT_ERROR_BUFFER_OVERFLOW);

    printf("test_error_format: PASSED\n");
}

// Test: Error status checking
static void
test_error_is_error(void)
{
    assert(rift_error_is_error(RIFT_OK) == false);
    assert(rift_error_is_error(RIFT_ERROR_UNKNOWN) == true);
    assert(rift_error_is_error(RIFT_ERROR_INVALID_PARAMETER) == true);

    printf("test_error_is_error: PASSED\n");
}

// Test: Current error formatting
static void
test_error_current_format(void)
{
    // Set an error
    rift_error_set(RIFT_ERROR_IO, __LINE__, __FILE__, "I/O test error");

    char buffer[256] = {0};
    rift_status_t status = rift_error_current_format(buffer, sizeof(buffer));

    // Verify format operation succeeded
    assert(status == RIFT_OK);

    // Verify buffer contains expected elements
    assert(strstr(buffer, "RIFT_ERROR_IO") != NULL);
    assert(strstr(buffer, __FILE__) != NULL);
    assert(strstr(buffer, "I/O test error") != NULL);

    // Clear error state
    rift_error_clear();

    printf("test_error_current_format: PASSED\n");
}

// Test: Error logging
static void
test_error_log(void)
{
    // For this test, we just verify the function doesn't crash
    // Since we cannot easily check stdout/stderr in this test setup
    rift_status_t status = rift_error_log(RIFT_ERROR_FORMAT, __LINE__, __FILE__,
                                          "Test log message %s", "with parameter");

    // Should return the same status that was passed in
    assert(status == RIFT_ERROR_FORMAT);

    // Verify that the current error is set
    const rift_error_info_t *info = rift_error_get();
    assert(info->status == RIFT_ERROR_FORMAT);

    // Clear error state
    rift_error_clear();

    printf("test_error_log: PASSED\n");
}

int
main(void)
{
    // Run all tests
    test_error_set_get();
    test_error_callback_func();
    test_error_status_string();
    test_error_format();
    test_error_is_error();
    test_error_current_format();
    test_error_log();

    printf("\nAll error handling tests passed!\n");
    return 0;
}
