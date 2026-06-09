/**
 * @file rift_syntax_demo.c
 * @brief Demonstration of the LibRift R"" syntax extension
 *
 * This example shows how to use the R"" syntax extension provided by LibRift
 * to create more readable and powerful regular expressions.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "librift/regex/automaton/flags.h"
#include "librift/regex/engine/matcher.h"
#include "librift/regex/engine/pattern.h"
#include "librift/regex/errors/regex_error.h"
#include "librift/regex/syntax/integration.h"

/* Function prototypes */
void demonstrate_standard_regex();
void demonstrate_rift_syntax();
void compare_patterns();
void benchmark_patterns();

int
main()
{
    printf("LibRift R\"\" Syntax Extension Demonstration\n");
    printf("==========================================\n\n");

    /* Show standard regex usage first */
    demonstrate_standard_regex();

    /* Demonstrate the improved R"" syntax */
    demonstrate_rift_syntax();

    /* Compare standard vs. R"" syntax patterns */
    compare_patterns();

    /* Simple benchmark of pattern matching */
    benchmark_patterns();

    return 0;
}

/**
 * @brief Demonstrate standard regex pattern usage
 */
void
demonstrate_standard_regex()
{
    printf("Standard Regex Pattern Usage:\n");
    printf("-----------------------------\n");

    /* Standard regex pattern with double escaping */
    const char *standard_pattern =
        "\\[(\\d{4}-\\d{2}-\\d{2})\\] \\[([A-Za-z0-9_-]+)\\] \\[([A-Z]+)\\]: (.+)";

    /* Sample log entry */
    const char *log_entry =
        "[2025-03-15] [user_auth] [ERROR]: Failed login attempt from IP 192.168.1.100";

    /* Compile the pattern */
    rift_regex_error_t error = {0};
    rift_regex_pattern_t *pattern =
        rift_regex_compile(standard_pattern, RIFT_REGEX_FLAG_NONE, &error);

    if (!pattern) {
        printf("Error compiling pattern: %s\n", error.message);
        return;
    }

    /* Create matcher */
    rift_regex_matcher_t *matcher = rift_matcher_create(pattern, RIFT_MATCHER_OPTION_NONE);
    if (!matcher) {
        printf("Error creating matcher\n");
        rift_regex_pattern_free(pattern);
        return;
    }

    /* Set input */
    if (!rift_matcher_set_input(matcher, log_entry, strlen(log_entry))) {
        printf("Error setting input\n");
        rift_matcher_free(matcher);
        rift_regex_pattern_free(pattern);
        return;
    }

    /* Match the pattern */
    rift_regex_match_t match = {0};
    bool result = rift_matcher_matches(matcher, &match);

    if (result) {
        printf("Match found!\n");
        printf("  Date: %s\n", match.groups[1].value);
        printf("  Module: %s\n", match.groups[2].value);
        printf("  Level: %s\n", match.groups[3].value);
        printf("  Message: %s\n", match.groups[4].value);
    } else {
        printf("No match found.\n");
    }

    /* Clean up */
    rift_match_free(&match);
    rift_matcher_free(matcher);
    rift_regex_pattern_free(pattern);

    printf("\n");
}

/**
 * @brief Demonstrate R"" syntax pattern usage
 */
void
demonstrate_rift_syntax()
{
    printf("LibRift R\"\" Syntax Usage:\n");
    printf("-------------------------\n");

    /* R"" syntax pattern - notice the improved readability with no escaping needed */
    const char *rift_pattern = R"(\[(\d{4}-\d{2}-\d{2})\] \[([A-Za-z0-9_-]+)\] \[([A-Z]+)\]: (.+))";

    /* Sample log entry */
    const char *log_entry =
        "[2025-03-15] [user_auth] [ERROR]: Failed login attempt from IP 192.168.1.100";

    /* Compile the pattern with RIFT_SYNTAX flag */
    rift_regex_error_t error = {0};
    rift_regex_pattern_t *pattern =
        rift_regex_compile(rift_pattern, RIFT_REGEX_FLAG_RIFT_SYNTAX, &error);

    if (!pattern) {
        printf("Error compiling pattern: %s\n", error.message);
        return;
    }

    /* Create matcher */
    rift_regex_matcher_t *matcher = rift_matcher_create(pattern, RIFT_MATCHER_OPTION_NONE);
    if (!matcher) {
        printf("Error creating matcher\n");
        rift_regex_pattern_free(pattern);
        return;
    }

    /* Set input */
    if (!rift_matcher_set_input(matcher, log_entry, strlen(log_entry))) {
        printf("Error setting input\n");
        rift_matcher_free(matcher);
        rift_regex_pattern_free(pattern);
        return;
    }

    /* Match the pattern */
    rift_regex_match_t match = {0};
    bool result = rift_matcher_matches(matcher, &match);

    if (result) {
        printf("Match found!\n");
        printf("  Date: %s\n", match.groups[1].value);
        printf("  Module: %s\n", match.groups[2].value);
        printf("  Level: %s\n", match.groups[3].value);
        printf("  Message: %s\n", match.groups[4].value);
    } else {
        printf("No match found.\n");
    }

    /* Clean up */
    rift_match_free(&match);
    rift_matcher_free(matcher);
    rift_regex_pattern_free(pattern);

    printf("\n");
}

/**
 * @brief Compare standard and R"" syntax patterns
 */
void
compare_patterns()
{
    printf("Pattern Comparison - Standard vs. R\"\" Syntax:\n");
    printf("--------------------------------------------\n");

    /* Standard regex pattern - complex example with many escapes */
    const char *standard_pattern =
        "\\bhttps?:\\/\\/[-A-Za-z0-9+&@#\\/%?=~_|!:,.;]*[-A-Za-z0-9+&@#\\/%=~_|]";

    /* R"" syntax pattern - much cleaner and more readable */
    const char *rift_pattern =
        R"(\bhttps?:\/\/[-A-Za-z0-9+&@#\/%?=~_|!:,.;]*[-A-Za-z0-9+&@#\/%=~_|])";

    printf("Standard pattern:\n  %s\n\n", standard_pattern);
    printf("R\"\" syntax pattern:\n  %s\n\n", rift_pattern);

    /* Demonstrate pattern equality but improved readability */
    rift_regex_error_t error1 = {0};
    rift_regex_error_t error2 = {0};

    rift_regex_pattern_t *pattern1 =
        rift_regex_compile(standard_pattern, RIFT_REGEX_FLAG_NONE, &error1);
    rift_regex_pattern_t *pattern2 =
        rift_regex_compile(rift_pattern, RIFT_REGEX_FLAG_RIFT_SYNTAX, &error2);

    if (!pattern1 || !pattern2) {
        printf("Error compiling patterns.\n");
        if (pattern1)
            rift_regex_pattern_free(pattern1);
        if (pattern2)
            rift_regex_pattern_free(pattern2);
        return;
    }

    /* Sample URLs to test */
    const char *urls[] = {"https://librift.org", "http://github.com/librift/examples",
                          "https://librift.org/docs?version=1.0.0", "Not a URL at all",
                          "https://example.com/path/file.html#section"};

    printf("Testing both patterns against sample URLs:\n");

    for (int i = 0; i < 5; i++) {
        /* Test with standard pattern */
        rift_regex_matcher_t *matcher1 = rift_matcher_create(pattern1, RIFT_MATCHER_OPTION_NONE);
        rift_matcher_set_input(matcher1, urls[i], strlen(urls[i]));
        bool result1 = rift_matcher_find_next(matcher1, NULL);

        /* Test with R"" syntax pattern */
        rift_regex_matcher_t *matcher2 = rift_matcher_create(pattern2, RIFT_MATCHER_OPTION_NONE);
        rift_matcher_set_input(matcher2, urls[i], strlen(urls[i]));
        bool result2 = rift_matcher_find_next(matcher2, NULL);

        printf("  '%s': %s (standard), %s (R\"\" syntax)\n", urls[i],
               result1 ? "Match" : "No match", result2 ? "Match" : "No match");

        rift_matcher_free(matcher1);
        rift_matcher_free(matcher2);
    }

    /* Clean up */
    rift_regex_pattern_free(pattern1);
    rift_regex_pattern_free(pattern2);

    printf("\n");
}

/**
 * @brief Benchmark standard vs R"" syntax patterns
 */
void
benchmark_patterns()
{
    printf("Performance Benchmark:\n");
    printf("---------------------\n");

    /* More complex patterns for benchmarking */

    /* Standard regex pattern - email validation with many escapes */
    const char *standard_pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";

    /* R"" syntax pattern - same pattern but cleaner */
    const char *rift_pattern = R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)";

    /* Compile both patterns */
    rift_regex_error_t error1 = {0};
    rift_regex_error_t error2 = {0};

    rift_regex_pattern_t *pattern1 =
        rift_regex_compile(standard_pattern, RIFT_REGEX_FLAG_NONE, &error1);
    rift_regex_pattern_t *pattern2 =
        rift_regex_compile(rift_pattern, RIFT_REGEX_FLAG_RIFT_SYNTAX, &error2);

    if (!pattern1 || !pattern2) {
        printf("Error compiling patterns for benchmark.\n");
        if (pattern1)
            rift_regex_pattern_free(pattern1);
        if (pattern2)
            rift_regex_pattern_free(pattern2);
        return;
    }

    /* Sample emails to test */
    const char *emails[] = {
        "user@example.com", "support@librift.org", "john.doe123@company-name.co.uk",
        "invalid-email",    "another.invalid@",    "test.user@subdomain.example.com"};

    const int num_emails = 6;
    const int num_iterations = 10000;

    /* Benchmark standard pattern */
    clock_t start_time = clock();

    for (int iter = 0; iter < num_iterations; iter++) {
        for (int i = 0; i < num_emails; i++) {
            rift_regex_matcher_t *matcher = rift_matcher_create(pattern1, RIFT_MATCHER_OPTION_NONE);
            rift_matcher_set_input(matcher, emails[i], strlen(emails[i]));
            rift_matcher_matches(matcher, NULL);
            rift_matcher_free(matcher);
        }
    }

    clock_t end_time = clock();
    double standard_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    /* Benchmark R"" syntax pattern */
    start_time = clock();

    for (int iter = 0; iter < num_iterations; iter++) {
        for (int i = 0; i < num_emails; i++) {
            rift_regex_matcher_t *matcher = rift_matcher_create(pattern2, RIFT_MATCHER_OPTION_NONE);
            rift_matcher_set_input(matcher, emails[i], strlen(emails[i]));
            rift_matcher_matches(matcher, NULL);
            rift_matcher_free(matcher);
        }
    }

    end_time = clock();
    double rift_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    /* Print benchmark results */
    printf("Performed %d iterations over %d test strings:\n", num_iterations, num_emails);
    printf("  Standard pattern: %.4f seconds\n", standard_time);
    printf("  R\"\" syntax pattern: %.4f seconds\n", rift_time);

    double percentage = (standard_time - rift_time) / standard_time * 100.0;
    if (rift_time < standard_time) {
        printf("  R\"\" syntax is %.2f%% faster!\n", percentage);
    } else if (rift_time > standard_time) {
        printf("  R\"\" syntax is %.2f%% slower.\n", -percentage);
    } else {
        printf("  Performance is identical.\n");
    }

    /* Note on implementation */
    printf("\nNOTE: This benchmark primarily demonstrates ease of use and maintainability.\n");
    printf("Once compiled, both patterns use the same underlying automaton, so \n");
    printf("performance differences are minimal. The R\"\" syntax is primarily a \n");
    printf("usability and readability improvement for developers.\n");

    /* Clean up */
    rift_regex_pattern_free(pattern1);
    rift_regex_pattern_free(pattern2);
}