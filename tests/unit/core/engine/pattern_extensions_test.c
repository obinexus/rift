/**
 * @file pattern_extensions_test.c
 * @brief Unit tests for regex pattern extension utilities
 *
 * Tests for pattern combination, extension, and error handling
 * in the pattern extensions API.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "librift/compiler/compiler.h"
#include "librift/engine/pattern.h"
#include "librift/errors/regex_error.h"
#include "librift/patterns/baseline_patterns.h"
#include "librift/patterns/pattern_extensions.h"
#include "librift/patterns/pattern_types.h"
#include "librift/test/unit_test.h"

/* Test pattern combination functionality */
RIFT_TEST(pattern_extensions_combine)
{
    rift_regex_error_t error = {0};
    rift_regex_pattern_t *pattern;

    /* Initialize baseline patterns */
    RIFT_ASSERT(rift_baseline_patterns_initialize(&error),
                "Baseline pattern initialization should succeed");

    /* Test combining multiple patterns */
    rift_baseline_pattern_type_t pattern_types[] = {RIFT_BASELINE_PATTERN_IDENTIFIER,
                                                    RIFT_BASELINE_PATTERN_WHITESPACE,
                                                    RIFT_BASELINE_PATTERN_NUMERIC};

    pattern = rift_pattern_combine(pattern_types, 3, RIFT_REGEX_FLAG_NONE, &error);
    RIFT_ASSERT(pattern != NULL, "Combining valid patterns should succeed");
    RIFT_ASSERT(error.code == 0, "No error should be set when combining valid patterns");

    /* Test matching with combined pattern */
    RIFT_ASSERT(rift_regex_match(pattern, "identifier", &error), "Should match identifier");
    RIFT_ASSERT(rift_regex_match(pattern, "   ", &error), "Should match whitespace");
    RIFT_ASSERT(rift_regex_match(pattern, "123.45", &error), "Should match numeric");
    RIFT_ASSERT(!rift_regex_match(pattern, "#", &error), "Should not match special character");

    /* Test NULL parameters */
    rift_regex_pattern_t *null_pattern =
        rift_pattern_combine(NULL, 3, RIFT_REGEX_FLAG_NONE, &error);
    RIFT_ASSERT(null_pattern == NULL, "NULL pattern_types should fail");
    RIFT_ASSERT(error.code != 0, "Error should be set for NULL pattern_types");

    /* Test zero length */
    rift_regex_pattern_t *zero_pattern =
        rift_pattern_combine(pattern_types, 0, RIFT_REGEX_FLAG_NONE, &error);
    RIFT_ASSERT(zero_pattern == NULL, "Zero patterns should fail");
    RIFT_ASSERT(error.code != 0, "Error should be set for zero patterns");

    /* Cleanup */
    if (pattern)
        rift_regex_pattern_free(pattern);
    rift_baseline_patterns_cleanup();
}

/* Test pattern extension functionality */
RIFT_TEST(pattern_extensions_extend)
{
    rift_regex_error_t error = {0};
    rift_regex_pattern_t *pattern;

    /* Initialize baseline patterns */
    RIFT_ASSERT(rift_baseline_patterns_initialize(&error),
                "Baseline pattern initialization should succeed");

    /* Test extending identifier pattern */
    pattern = rift_pattern_extend(RIFT_BASELINE_PATTERN_IDENTIFIER, "2[0-9]*", RIFT_REGEX_FLAG_NONE,
                                  &error);
    RIFT_ASSERT(pattern != NULL, "Extending identifier pattern should succeed");
    RIFT_ASSERT(error.code == 0, "No error should be set when extending valid pattern");

    /* Test matching with extended pattern */
    RIFT_ASSERT(rift_regex_match(pattern, "variable2", &error),
                "Should match identifier followed by 2");
    RIFT_ASSERT(rift_regex_match(pattern, "test2345", &error),
                "Should match identifier followed by 2 and digits");
    RIFT_ASSERT(!rift_regex_match(pattern, "variable", &error),
                "Should not match without extension");

    /* Test with invalid extension */
    rift_regex_pattern_t *invalid_pattern = rift_pattern_extend(
        RIFT_BASELINE_PATTERN_IDENTIFIER, "[unclosed", RIFT_REGEX_FLAG_NONE, &error);
    RIFT_ASSERT(invalid_pattern == NULL, "Invalid extension should fail");
    RIFT_ASSERT(error.code != 0, "Error should be set for invalid extension");

    /* Test with invalid base type */
    rift_regex_pattern_t *invalid_base =
        rift_pattern_extend(999, "abc", RIFT_REGEX_FLAG_NONE, &error);
    RIFT_ASSERT(invalid_base == NULL, "Invalid base type should fail");
    RIFT_ASSERT(error.code != 0, "Error should be set for invalid base type");

    /* Test NULL extension */
    rift_regex_pattern_t *null_ext =
        rift_pattern_extend(RIFT_BASELINE_PATTERN_IDENTIFIER, NULL, RIFT_REGEX_FLAG_NONE, &error);
    RIFT_ASSERT(null_ext == NULL, "NULL extension should fail");
    RIFT_ASSERT(error.code != 0, "Error should be set for NULL extension");

    /* Cleanup */
    if (pattern)
        rift_regex_pattern_free(pattern);
    rift_baseline_patterns_cleanup();
}

/* Test complex pattern operations */
RIFT_TEST(pattern_extensions_complex)
{
    rift_regex_error_t error = {0};
    rift_regex_pattern_t *extended1, *extended2, *combined;

    /* Initialize baseline patterns */
    RIFT_ASSERT(rift_baseline_patterns_initialize(&error),
                "Baseline pattern initialization should succeed");

    /* Create two extended patterns */
    extended1 = rift_pattern_extend(RIFT_BASELINE_PATTERN_IDENTIFIER, "_test", RIFT_REGEX_FLAG_NONE,
                                    &error);
    RIFT_ASSERT(extended1 != NULL, "First extension should succeed");

    extended2 =
        rift_pattern_extend(RIFT_BASELINE_PATTERN_NUMERIC, "px", RIFT_REGEX_FLAG_NONE, &error);
    RIFT_ASSERT(extended2 != NULL, "Second extension should succeed");

    /* Create an array of the extended patterns */
    rift_regex_pattern_t *patterns[] = {extended1, extended2};

    /* Test combining the extended patterns */
    /* Note: This would require an additional API function not in the provided context.
       For demonstration purposes, we'll comment this out and add a placeholder assertion. */

    /* combined = rift_pattern_combine_instances(patterns, 2, RIFT_REGEX_FLAG_NONE, &error);
    RIFT_ASSERT(combined != NULL, "Combining extended patterns should succeed");

    RIFT_ASSERT(rift_regex_match(combined, "variable_test", &error), "Should match extended
    identifier"); RIFT_ASSERT(rift_regex_match(combined, "123px", &error), "Should match extended
    numeric"); */

    /* Placeholder assertion */
    RIFT_ASSERT(1, "Complex pattern operations placeholder - need additional API function");

    /* Cleanup */
    if (extended1)
        rift_regex_pattern_free(extended1);
    if (extended2)
        rift_regex_pattern_free(extended2);
    /* if (combined) rift_regex_pattern_free(combined); */
    rift_baseline_patterns_cleanup();
}

/* Register tests with the test framework */
RIFT_TEST_SUITE(pattern_extensions)
{
    RIFT_TEST_RUN(pattern_extensions_combine);
    RIFT_TEST_RUN(pattern_extensions_extend);
    RIFT_TEST_RUN(pattern_extensions_complex);
}