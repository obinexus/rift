/**
 * @file compiler_tests.c
 * @brief Unit tests for the LibRift regex compiler
 *
 * This file contains tests that verify the functionality of the regex compiler
 * component, including pattern compilation, optimization, and error handling.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "librift/automaton/automaton.h"
#include "librift/compiler/compiler.h"
#include "librift/parser/parser.h"

/* Test helper functions */
static void
print_test_result(const char *test_name, bool success)
{
    printf("Test %s: %s\n", test_name, success ? "PASSED" : "FAILED");
}

/**
 * Test basic pattern compilation
 */
static bool
test_compile_basic_pattern()
{
    rift_regex_error_t error = {0};
    rift_regex_automaton_t *automaton = rift_regex_compile_pattern("abc", 0, &error);

    if (!automaton) {
        printf("Failed to compile pattern: %s\n", rift_regex_get_compile_error_message(error));
        return false;
    }

    // Basic validation that automaton was created correctly
    bool valid = (automaton != NULL);

    // Clean up
    rift_regex_automaton_free(automaton);

    return valid;
}

/**
 * Test compilation of patterns with alternation
 */
static bool
test_compile_alternation()
{
    rift_regex_error_t error = {0};
    rift_regex_automaton_t *automaton = rift_regex_compile_pattern("a|b|c", 0, &error);

    if (!automaton) {
        printf("Failed to compile alternation pattern: %s\n",
               rift_regex_get_compile_error_message(error));
        return false;
    }

    bool valid = (automaton != NULL);
    rift_regex_automaton_free(automaton);

    return valid;
}

/**
 * Test compilation of patterns with quantifiers
 */
static bool
test_compile_quantifiers()
{
    rift_regex_error_t error = {0};

    // Test various quantifiers
    const char *patterns[] = {
        "a?",     // Zero or one
        "a+",     // One or more
        "a*",     // Zero or more
        "a{3}",   // Exactly 3
        "a{2,5}", // Between 2 and 5
        "a{2,}"   // At least 2
    };

    bool all_valid = true;

    for (size_t i = 0; i < sizeof(patterns) / sizeof(patterns[0]); i++) {
        rift_regex_automaton_t *automaton = rift_regex_compile_pattern(patterns[i], 0, &error);

        if (!automaton) {
            printf("Failed to compile quantifier pattern '%s': %s\n", patterns[i],
                   rift_regex_get_compile_error_message(error));
            all_valid = false;
            continue;
        }

        rift_regex_automaton_free(automaton);
    }

    return all_valid;
}

/**
 * Test compilation of patterns with character classes
 */
static bool
test_compile_character_classes()
{
    rift_regex_error_t error = {0};

    const char *patterns[] = {
        "[abc]",      // Character class
        "[^abc]",     // Negated character class
        "[a-z]",      // Range
        "[a-zA-Z0-9]" // Multiple ranges
    };

    bool all_valid = true;

    for (size_t i = 0; i < sizeof(patterns) / sizeof(patterns[0]); i++) {
        rift_regex_automaton_t *automaton = rift_regex_compile_pattern(patterns[i], 0, &error);

        if (!automaton) {
            printf("Failed to compile character class pattern '%s': %s\n", patterns[i],
                   rift_regex_get_compile_error_message(error));
            all_valid = false;
            continue;
        }

        rift_regex_automaton_free(automaton);
    }

    return all_valid;
}

/**
 * Test optimization of automaton
 */
static bool
test_optimize_automaton()
{
    rift_regex_error_t error = {0};
    rift_regex_automaton_t *automaton = rift_regex_compile_pattern("a|b|c", 0, &error);

    if (!automaton) {
        printf("Failed to compile pattern for optimization test: %s\n",
               rift_regex_get_compile_error_message(error));
        return false;
    }

    // Test optimization
    bool optimized =
        rift_regex_optimize_automaton(automaton, RIFT_REGEX_FLAG_OPTIMIZE_SPEED, &error);

    if (!optimized) {
        printf("Failed to optimize automaton: %s\n", rift_regex_get_compile_error_message(error));
        rift_regex_automaton_free(automaton);
        return false;
    }

    rift_regex_automaton_free(automaton);
    return true;
}

/**
 * Test error handling with invalid patterns
 */
static bool
test_error_handling()
{
    rift_regex_error_t error = {0};

    // Test invalid patterns
    const char *invalid_patterns[] = {
        "[unclosed",  // Unclosed character class
        "a{invalid}", // Invalid quantifier
        "a(unclosed", // Unclosed group
        "a\\",        // Trailing backslash
        "a**"         // Invalid double quantifier
    };

    bool all_failed = true;

    for (size_t i = 0; i < sizeof(invalid_patterns) / sizeof(invalid_patterns[0]); i++) {
        rift_regex_automaton_t *automaton =
            rift_regex_compile_pattern(invalid_patterns[i], 0, &error);

        if (automaton != NULL) {
            printf("Expected pattern '%s' to fail compilation, but it succeeded\n",
                   invalid_patterns[i]);
            all_failed = false;
            rift_regex_automaton_free(automaton);
        }
        // Reset error for next iteration
        memset(&error, 0, sizeof(error));
    }

    return all_failed;
}

/**
 * Main function to run all tests
 */
int
main()
{
    int passed = 0;
    int total = 0;
    bool result;

    // Test basic pattern compilation
    result = test_compile_basic_pattern();
    print_test_result("Basic Pattern Compilation", result);
    if (result)
        passed++;
    total++;

    // Test alternation
    result = test_compile_alternation();
    print_test_result("Alternation Pattern Compilation", result);
    if (result)
        passed++;
    total++;

    // Test quantifiers
    result = test_compile_quantifiers();
    print_test_result("Quantifier Pattern Compilation", result);
    if (result)
        passed++;
    total++;

    // Test character classes
    result = test_compile_character_classes();
    print_test_result("Character Class Pattern Compilation", result);
    if (result)
        passed++;
    total++;

    // Test optimization
    result = test_optimize_automaton();
    print_test_result("Automaton Optimization", result);
    if (result)
        passed++;
    total++;

    // Test error handling
    result = test_error_handling();
    print_test_result("Error Handling", result);
    if (result)
        passed++;
    total++;

    // Print summary
    printf("\nTest Results: %d/%d passed\n", passed, total);

    return (passed == total) ? 0 : 1;
}
