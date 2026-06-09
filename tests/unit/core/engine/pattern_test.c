/**
 * @file pattern_test.c
 * @brief Unit tests for the pattern functionality in the LibRift regex engine
 *
 * This file contains comprehensive tests for the pattern compilation, manipulation,
 * and querying functions in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <check.h>
#include <stdlib.h>
#include <string.h>

#include "librift/automaton/automaton.h"
#include "librift/engine/pattern.h"
#include "librift/errors/regex_error.h"
#include "librift/parser/ast.h"

// Test fixture setup and teardown
static rift_regex_pattern_t *test_pattern = NULL;
static rift_regex_error_t error;

void
setup(void)
{
    // Initialize error structure
    rift_regex_error_init(&error);
    test_pattern = NULL;
}

void
teardown(void)
{
    if (test_pattern) {
        rift_regex_pattern_free(test_pattern);
        test_pattern = NULL;
    }
}

// Test basic pattern compilation
START_TEST(test_compile_basic)
{
    test_pattern = rift_regex_compile("abc", RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_nonnull(test_pattern);
    ck_assert_uint_eq(error.code, RIFT_REGEX_ERROR_NONE);
    ck_assert_ptr_nonnull(rift_regex_pattern_get_automaton(test_pattern));
    ck_assert_str_eq(rift_regex_pattern_get_source(test_pattern), "abc");
    ck_assert_uint_eq(rift_regex_pattern_get_group_count(test_pattern), 0);
    ck_assert(rift_regex_pattern_is_valid(test_pattern));
    ck_assert(!rift_regex_pattern_is_rift_syntax(test_pattern));
}
END_TEST

// Test pattern with capture groups
START_TEST(test_compile_with_groups)
{
    test_pattern = rift_regex_compile("(abc)(def)", RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_nonnull(test_pattern);
    ck_assert_uint_eq(error.code, RIFT_REGEX_ERROR_NONE);
    ck_assert_uint_eq(rift_regex_pattern_get_group_count(test_pattern), 2);
}
END_TEST

// Test r'' syntax support
START_TEST(test_compile_rift_syntax)
{
    test_pattern = rift_regex_compile("r'abc'", RIFT_REGEX_FLAG_RIFT_SYNTAX, &error);
    ck_assert_ptr_nonnull(test_pattern);
    ck_assert_uint_eq(error.code, RIFT_REGEX_ERROR_NONE);
    ck_assert(rift_regex_pattern_is_rift_syntax(test_pattern));
}
END_TEST

// Test r'' syntax without proper flag
START_TEST(test_compile_rift_syntax_without_flag)
{
    test_pattern = rift_regex_compile("r'abc'", RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_null(test_pattern);
    ck_assert_uint_eq(error.code, RIFT_REGEX_ERROR_UNSUPPORTED_FEATURE);
}
END_TEST

// Test optimization flags
START_TEST(test_compile_with_optimization)
{
    test_pattern = rift_regex_compile("abc|def", RIFT_REGEX_FLAG_OPTIMIZE, &error);
    ck_assert_ptr_nonnull(test_pattern);
    ck_assert_uint_eq(error.code, RIFT_REGEX_ERROR_NONE);
}
END_TEST

// Test invalid regex pattern
START_TEST(test_compile_invalid_pattern)
{
    test_pattern = rift_regex_compile("[abc", RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_null(test_pattern);
    ck_assert_uint_ne(error.code, RIFT_REGEX_ERROR_NONE);
}
END_TEST

// Test null pattern
START_TEST(test_compile_null_pattern)
{
    test_pattern = rift_regex_compile(NULL, RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_null(test_pattern);
    ck_assert_uint_eq(error.code, RIFT_REGEX_ERROR_INVALID_PARAMETER);
}
END_TEST

// Test pattern validation
START_TEST(test_pattern_validate)
{
    bool valid = rift_regex_pattern_validate("abc", RIFT_REGEX_FLAG_NONE, &error);
    ck_assert(valid);
    ck_assert_uint_eq(error.code, RIFT_REGEX_ERROR_NONE);

    valid = rift_regex_pattern_validate("[abc", RIFT_REGEX_FLAG_NONE, &error);
    ck_assert(!valid);
    ck_assert_uint_ne(error.code, RIFT_REGEX_ERROR_NONE);
}
END_TEST

// Test pattern cloning
START_TEST(test_pattern_clone)
{
    test_pattern = rift_regex_compile("abc", RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_nonnull(test_pattern);

    rift_regex_pattern_t *clone = rift_regex_pattern_clone(test_pattern);
    ck_assert_ptr_nonnull(clone);
    ck_assert_str_eq(rift_regex_pattern_get_source(clone), "abc");
    ck_assert_uint_eq(rift_regex_pattern_get_flags(clone),
                      rift_regex_pattern_get_flags(test_pattern));
    ck_assert_uint_eq(rift_regex_pattern_get_group_count(clone),
                      rift_regex_pattern_get_group_count(test_pattern));

    rift_regex_pattern_free(clone);
}
END_TEST

// Test pattern equality
START_TEST(test_pattern_equals)
{
    rift_regex_pattern_t *pattern1 = rift_regex_compile("abc", RIFT_REGEX_FLAG_NONE, &error);
    rift_regex_pattern_t *pattern2 = rift_regex_compile("abc", RIFT_REGEX_FLAG_NONE, &error);
    rift_regex_pattern_t *pattern3 = rift_regex_compile("def", RIFT_REGEX_FLAG_NONE, &error);

    ck_assert(rift_regex_pattern_equals(pattern1, pattern2));
    ck_assert(!rift_regex_pattern_equals(pattern1, pattern3));
    ck_assert(!rift_regex_pattern_equals(pattern1, NULL));
    ck_assert(!rift_regex_pattern_equals(NULL, pattern2));
    ck_assert(rift_regex_pattern_equals(NULL, NULL));

    rift_regex_pattern_free(pattern1);
    rift_regex_pattern_free(pattern2);
    rift_regex_pattern_free(pattern3);
}
END_TEST

// Test pattern to string
START_TEST(test_pattern_to_string)
{
    test_pattern = rift_regex_compile("abc", RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_nonnull(test_pattern);

    char buffer[256];
    bool result = rift_regex_pattern_to_string(test_pattern, buffer, sizeof(buffer));
    ck_assert(result);
    ck_assert_str_ne(buffer, "");
}
END_TEST

// Test pattern split alternation
START_TEST(test_pattern_split_alternation)
{
    test_pattern = rift_regex_compile("a|b|c", RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_nonnull(test_pattern);

    rift_regex_pattern_t *subpatterns[10];
    size_t num_subpatterns = 0;

    bool result =
        rift_regex_pattern_split_alternation(test_pattern, subpatterns, 10, &num_subpatterns);
    ck_assert(result);
    ck_assert_uint_eq(num_subpatterns, 3);

    for (size_t i = 0; i < num_subpatterns; i++) {
        rift_regex_pattern_free(subpatterns[i]);
    }
}
END_TEST

// Test complex pattern
START_TEST(test_complex_pattern)
{
    test_pattern =
        rift_regex_compile("^(https?://)?([a-zA-Z0-9\\-\\.]+)\\.([a-zA-Z]{2,})(:[0-9]+)?(/.*)?$",
                           RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_nonnull(test_pattern);
    ck_assert_uint_eq(error.code, RIFT_REGEX_ERROR_NONE);
    ck_assert_uint_eq(rift_regex_pattern_get_group_count(test_pattern), 5);
}
END_TEST

// Test r'' syntax with double quotes
START_TEST(test_rift_syntax_double_quotes)
{
    test_pattern = rift_regex_compile("r\"abc\"", RIFT_REGEX_FLAG_RIFT_SYNTAX, &error);
    ck_assert_ptr_nonnull(test_pattern);
    ck_assert_uint_eq(error.code, RIFT_REGEX_ERROR_NONE);
    ck_assert(rift_regex_pattern_is_rift_syntax(test_pattern));
}
END_TEST

// Test r'' syntax with special characters
START_TEST(test_rift_syntax_special_chars)
{
    test_pattern = rift_regex_compile("r'\\d+\\s+\\w+'", RIFT_REGEX_FLAG_RIFT_SYNTAX, &error);
    ck_assert_ptr_nonnull(test_pattern);
    ck_assert_uint_eq(error.code, RIFT_REGEX_ERROR_NONE);
}
END_TEST

// Test creating pattern from AST
START_TEST(test_pattern_from_ast)
{
    test_pattern = rift_regex_compile("abc", RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_nonnull(test_pattern);

    const rift_regex_ast_t *ast = rift_regex_pattern_get_ast(test_pattern);
    ck_assert_ptr_nonnull(ast);

    rift_regex_pattern_t *ast_pattern =
        rift_regex_pattern_from_ast(ast, RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_nonnull(ast_pattern);
    ck_assert_uint_eq(error.code, RIFT_REGEX_ERROR_NONE);

    rift_regex_pattern_free(ast_pattern);
}
END_TEST

// Test creating pattern from automaton
START_TEST(test_pattern_from_automaton)
{
    test_pattern = rift_regex_compile("abc", RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_nonnull(test_pattern);

    struct rift_regex_automaton *automaton = rift_regex_pattern_get_automaton(test_pattern);
    ck_assert_ptr_nonnull(automaton);

    rift_regex_pattern_t *auto_pattern =
        rift_regex_pattern_from_automaton(automaton, RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_nonnull(auto_pattern);
    ck_assert_uint_eq(error.code, RIFT_REGEX_ERROR_NONE);

    rift_regex_pattern_free(auto_pattern);
}
END_TEST

// Test compile pattern function
START_TEST(test_compile_pattern)
{
    struct rift_regex_automaton *automaton =
        rift_regex_compile_pattern("abc", RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_nonnull(automaton);
    ck_assert_uint_eq(error.code, RIFT_REGEX_ERROR_NONE);

    // Clean up - we need to free the automaton directly
    rift_automaton_free(automaton);
}
END_TEST

// Test error handling
START_TEST(test_error_handling)
{
    rift_regex_error_init(&error);
    test_pattern = rift_regex_compile("[unclosed", RIFT_REGEX_FLAG_NONE, &error);
    ck_assert_ptr_null(test_pattern);
    ck_assert_uint_ne(error.code, RIFT_REGEX_ERROR_NONE);

    const char *error_msg = rift_regex_get_compile_error_message(error);
    ck_assert_ptr_nonnull(error_msg);
    ck_assert_str_ne(error_msg, "");
}
END_TEST

// Create test suite
Suite *
pattern_suite(void)
{
    Suite *suite = suite_create("Pattern");

    // Core functionality tests
    TCase *tc_core = tcase_create("Core");
    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_compile_basic);
    tcase_add_test(tc_core, test_compile_with_groups);
    tcase_add_test(tc_core, test_compile_invalid_pattern);
    tcase_add_test(tc_core, test_compile_null_pattern);
    tcase_add_test(tc_core, test_pattern_validate);
    suite_add_tcase(suite, tc_core);

    // LibRift syntax tests
    TCase *tc_rift = tcase_create("RiftSyntax");
    tcase_add_checked_fixture(tc_rift, setup, teardown);
    tcase_add_test(tc_rift, test_compile_rift_syntax);
    tcase_add_test(tc_rift, test_compile_rift_syntax_without_flag);
    tcase_add_test(tc_rift, test_rift_syntax_double_quotes);
    tcase_add_test(tc_rift, test_rift_syntax_special_chars);
    suite_add_tcase(suite, tc_rift);

    // Advanced functionality tests
    TCase *tc_advanced = tcase_create("Advanced");
    tcase_add_checked_fixture(tc_advanced, setup, teardown);
    tcase_add_test(tc_advanced, test_compile_with_optimization);
    tcase_add_test(tc_advanced, test_pattern_clone);
    tcase_add_test(tc_advanced, test_pattern_equals);
    tcase_add_test(tc_advanced, test_pattern_to_string);
    tcase_add_test(tc_advanced, test_pattern_split_alternation);
    tcase_add_test(tc_advanced, test_complex_pattern);
    tcase_add_test(tc_advanced, test_pattern_from_ast);
    tcase_add_test(tc_advanced, test_pattern_from_automaton);
    tcase_add_test(tc_advanced, test_compile_pattern);
    tcase_add_test(tc_advanced, test_error_handling);
    suite_add_tcase(suite, tc_advanced);

    return suite;
}

// Main function
int
main(void)
{
    Suite *suite = pattern_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    int failures = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}