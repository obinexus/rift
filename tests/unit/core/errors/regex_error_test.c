/**
 * @file regex_error_test.c
 * @brief Unit tests for the LibRift regex error handling functions
 *
 * This file contains tests that verify the correct functionality
 * of the regex error handling capabilities in LibRift.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Include the testing framework */
#include "ctest.h"

/* Include the LibRift headers */
#include "librift/errors/regex_error.h"

/* Test regex error initialization */
CTEST(regex_error, init)
{
    rift_regex_error_t error;

    /* Test initialization */
    ASSERT_TRUE(rift_regex_error_init(&error));
    ASSERT_EQUAL(RIFT_REGEX_ERROR_NONE, error.code);
    ASSERT_EQUAL(0, error.position);
    ASSERT_STR("", error.message);

    /* Test null parameter */
    ASSERT_FALSE(rift_regex_error_init(NULL));
}

/* Test getting error string */
CTEST(regex_error, get_error_string)
{
    rift_regex_error_t error;

    /* Initialize error structure */
    rift_regex_error_init(&error);

    /* Test various error codes */
    error.code = RIFT_REGEX_ERROR_NONE;
    ASSERT_STR("No error", rift_regex_get_error_string(error));

    error.code = RIFT_REGEX_ERROR_SYNTAX;
    ASSERT_STR("Syntax error in regular expression pattern", rift_regex_get_error_string(error));

    error.code = RIFT_REGEX_ERROR_MEMORY;
    ASSERT_STR("Memory allocation failed", rift_regex_get_error_string(error));

    /* Test unknown error code */
    error.code = 9999;
    ASSERT_STR("Unknown regex error", rift_regex_get_error_string(error));
}

/* Test error to string formatting */
CTEST(regex_error, error_to_string)
{
    rift_regex_error_t error;
    char buffer[256];

    /* Initialize error structure */
    rift_regex_error_init(&error);

    /* Set error values */
    error.code = RIFT_REGEX_ERROR_SYNTAX;
    strcpy(error.message, "Unclosed bracket");
    error.position = 5;

    /* Test with position */
    ASSERT_TRUE(rift_regex_error_to_string(&error, buffer, sizeof(buffer)));
    ASSERT_STR("Error 3 at position 5: Unclosed bracket", buffer);

    /* Test without position */
    error.position = 0;
    ASSERT_TRUE(rift_regex_error_to_string(&error, buffer, sizeof(buffer)));
    ASSERT_STR("Error 3: Unclosed bracket", buffer);

    /* Test with null parameters */
    ASSERT_FALSE(rift_regex_error_to_string(NULL, buffer, sizeof(buffer)));
    ASSERT_FALSE(rift_regex_error_to_string(&error, NULL, sizeof(buffer)));
    ASSERT_FALSE(rift_regex_error_to_string(&error, buffer, 0));
}

/* Test error existence check */
CTEST(regex_error, error_exists)
{
    rift_regex_error_t error;

    /* Initialize error structure */
    rift_regex_error_init(&error);

    /* Test with no error */
    error.code = RIFT_REGEX_ERROR_NONE;
    ASSERT_FALSE(rift_regex_error_exists(&error));

    /* Test with error */
    error.code = RIFT_REGEX_ERROR_SYNTAX;
    ASSERT_TRUE(rift_regex_error_exists(&error));

    /* Test with null parameter */
    ASSERT_FALSE(rift_regex_error_exists(NULL));
}

/* Test error copying */
CTEST(regex_error, error_copy)
{
    rift_regex_error_t source;
    rift_regex_error_t dest;

    /* Initialize source error */
    rift_regex_error_init(&source);
    source.code = RIFT_REGEX_ERROR_SYNTAX;
    strcpy(source.message, "Test error message");
    source.position = 10;

    /* Test copy */
    ASSERT_TRUE(rift_regex_error_copy(&dest, &source));
    ASSERT_EQUAL(source.code, dest.code);
    ASSERT_EQUAL(source.position, dest.position);
    ASSERT_STR(source.message, dest.message);

    /* Test with null parameters */
    ASSERT_FALSE(rift_regex_error_copy(NULL, &source));
    ASSERT_FALSE(rift_regex_error_copy(&dest, NULL));
}

/* Test error clearing */
CTEST(regex_error, clear)
{
    rift_regex_error_t error;

    /* Initialize and set error */
    rift_regex_error_init(&error);
    error.code = RIFT_REGEX_ERROR_SYNTAX;
    strcpy(error.message, "Test error message");
    error.position = 10;

    /* Clear error */
    rift_regex_error_clear(&error);

    /* Verify cleared state */
    ASSERT_EQUAL(RIFT_REGEX_ERROR_NONE, error.code);
    ASSERT_EQUAL(0, error.position);
    ASSERT_STR("", error.message);

    /* Test with null parameter - should not crash */
    rift_regex_error_clear(NULL);
}

/* Test converting system errors to regex errors */
CTEST(regex_error, from_system_error)
{
    rift_regex_error_t error;

    /* Initialize error structure */
    rift_regex_error_init(&error);

    /* Convert ENOMEM to regex error */
    ASSERT_TRUE(rift_regex_error_from_system(ENOMEM, &error));
    ASSERT_EQUAL(RIFT_REGEX_ERROR_MEMORY, error.code);

    /* Convert unknown error */
    ASSERT_TRUE(rift_regex_error_from_system(9999, &error));
    ASSERT_EQUAL(RIFT_REGEX_ERROR_UNKNOWN, error.code);

    /* Test with null parameter */
    ASSERT_FALSE(rift_regex_error_from_system(ENOMEM, NULL));
}