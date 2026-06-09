/**
 * @file test_regex.c
 * @brief Unit tests for the regex module using Check framework
 *
 * This file contains comprehensive tests for the regex functions
 * implemented in the LibRift automaton engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <check.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/errors/regex_error.h"

/* Test basic regex compilation */
START_TEST(test_regex_compile)
{
    regex_t regex;
    int result;

    // Test valid pattern
    result = rift_regex_compile("a+b", 0, &regex);
    ck_assert_int_eq(result, 0);
    rift_regex_free(&regex);

    // Test invalid pattern
    result = rift_regex_compile("[a-z", 0, &regex);
    ck_assert_int_ne(result, 0);

    // Test NULL parameters
    result = rift_regex_compile(NULL, 0, &regex);
    ck_assert_int_ne(result, 0);

    result = rift_regex_compile("a+b", 0, NULL);
    ck_assert_int_ne(result, 0);
}
END_TEST

/* Test regex execution */
START_TEST(test_regex_exec)
{
    regex_t regex;
    int result;
    regmatch_t matches[1];

    // Compile a test pattern
    result = rift_regex_compile("a+b", 0, &regex);
    ck_assert_int_eq(result, 0);

    // Test matching string
    result = rift_regex_exec(&regex, "aaab", 1, matches, 0);
    ck_assert_int_eq(result, 0);
    ck_assert_int_eq(matches[0].rm_so, 0);
    ck_assert_int_eq(matches[0].rm_eo, 4);

    // Test non-matching string
    result = rift_regex_exec(&regex, "ccc", 1, matches, 0);
    ck_assert_int_eq(result, REG_NOMATCH);

    // Test NULL parameters
    result = rift_regex_exec(NULL, "aaab", 1, matches, 0);
    ck_assert_int_eq(result, REG_NOMATCH);

    result = rift_regex_exec(&regex, NULL, 1, matches, 0);
    ck_assert_int_eq(result, REG_NOMATCH);

    rift_regex_free(&regex);
}
END_TEST

/* Test regex error handling */
START_TEST(test_regex_error)
{
    regex_t regex;
    char error_buffer[256];
    size_t len;

    // Create an error condition
    int result = rift_regex_compile("[a-z", 0, &regex);
    ck_assert_int_ne(result, 0);

    len = rift_regex_error(result, &regex, error_buffer, sizeof(error_buffer));
    ck_assert_int_gt(len, 0);
    ck_assert_str_ne(error_buffer, "");

    // Test with NULL buffer
    len = rift_regex_error(result, &regex, NULL, 0);
    ck_assert_int_eq(len, 0);
}
END_TEST

/* Test regex matching convenience function */
START_TEST(test_regex_matches)
{
    // Test basic matching
    ck_assert(rift_regex_matches("a+b", "aaab", 0) == true);
    ck_assert(rift_regex_matches("a+b", "ccc", 0) == false);

    // Test with case insensitivity
    ck_assert(rift_regex_matches("a+b", "AaAB", REG_ICASE) == true);

    // Test NULL parameters
    ck_assert(rift_regex_matches(NULL, "aaab", 0) == false);
    ck_assert(rift_regex_matches("a+b", NULL, 0) == false);
}
END_TEST

/* Test regex find function */
START_TEST(test_regex_find)
{
    size_t start, end;
    bool result;

    // Test basic find
    result = rift_regex_find("a+b", "xxxaaabyyy", &start, &end, 0);
    ck_assert(result == true);
    ck_assert_int_eq(start, 3);
    ck_assert_int_eq(end, 7);

    // Test no match
    result = rift_regex_find("a+b", "xxxyyy", &start, &end, 0);
    ck_assert(result == false);

    // Test NULL parameters
    result = rift_regex_find(NULL, "xxxaaabyyy", &start, &end, 0);
    ck_assert(result == false);

    result = rift_regex_find("a+b", NULL, &start, &end, 0);
    ck_assert(result == false);

    result = rift_regex_find("a+b", "xxxaaabyyy", NULL, &end, 0);
    ck_assert(result == false);

    result = rift_regex_find("a+b", "xxxaaabyyy", &start, NULL, 0);
    ck_assert(result == false);
}
END_TEST

/* Test regex pattern validation */
START_TEST(test_regex_is_valid)
{
    // Test valid patterns
    ck_assert(rift_regex_is_valid("a+b", 0) == true);
    ck_assert(rift_regex_is_valid("[0-9]+", 0) == true);

    // Test invalid patterns
    ck_assert(rift_regex_is_valid("[a-z", 0) == false);
    ck_assert(rift_regex_is_valid("(unclosed", 0) == false);

    // Test NULL parameter
    ck_assert(rift_regex_is_valid(NULL, 0) == false);
}
END_TEST

/* Test regex error init function */
START_TEST(test_regex_error_init)
{
    rift_regex_error_t error;

    // Test initialization
    ck_assert(rift_regex_error_init(&error) == true);
    ck_assert_int_eq(error.code, RIFT_REGEX_ERROR_NONE);
    ck_assert_int_eq(error.position, 0);

    // Test NULL parameter
    ck_assert(rift_regex_error_init(NULL) == false);
}
END_TEST

/* Test regex error string function */
START_TEST(test_regex_error_string)
{
    rift_regex_error_t error;
    const char *error_str;

    // Initialize error structure
    rift_regex_error_init(&error);

    // Test various error codes
    error.code = RIFT_REGEX_ERROR_NONE;
    error_str = rift_regex_get_error_string(error);
    ck_assert_str_eq(error_str, "No error");

    error.code = RIFT_REGEX_ERROR_SYNTAX;
    error_str = rift_regex_get_error_string(error);
    ck_assert_str_eq(error_str, "Syntax error in regular expression pattern");

    error.code = RIFT_REGEX_ERROR_INVALID_ESCAPE;
    error_str = rift_regex_get_error_string(error);
    ck_assert_str_eq(error_str, "Invalid escape sequence in regex pattern");

    // Test unknown error code
    error.code = 9999;
    error_str = rift_regex_get_error_string(error);
    ck_assert_str_eq(error_str, "Unknown regex error");
}
END_TEST

/* Create the test suite */
Suite *
regex_suite(void)
{
    Suite *suite = suite_create("Regex");
    TCase *tc_core = tcase_create("Core");

    /* Add test cases to the test case */
    tcase_add_test(tc_core, test_regex_compile);
    tcase_add_test(tc_core, test_regex_exec);
    tcase_add_test(tc_core, test_regex_error);
    tcase_add_test(tc_core, test_regex_matches);
    tcase_add_test(tc_core, test_regex_find);
    tcase_add_test(tc_core, test_regex_is_valid);
    tcase_add_test(tc_core, test_regex_error_init);
    tcase_add_test(tc_core, test_regex_error_string);

    /* Add the test case to the suite */
    suite_add_tcase(suite, tc_core);

    return suite;
}

/* Main function */
int
main(void)
{
    int number_failed;
    Suite *suite = regex_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}