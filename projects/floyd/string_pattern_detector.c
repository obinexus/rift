/*
 * @file string_pattern_detector.c
 * @brief A program that uses LibRift to detect repeating patterns in strings
 *
 * This implementation uses LibRift's regex engine to solve a variant of
 * Floyd's backtracking problem for string pattern detection.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* LibRift headers */
#include "librift/core/error.h"
#include "librift/regex/automaton/automaton.h"
#include "librift/regex/automaton/flags.h"
#include "librift/regex/automaton/regex.h"
#include "librift/regex/engine/matcher.h"
#include "librift/regex/engine/pattern.h"
#include "librift/regex/errors/regex_error.h"
#include "librift/regex/errors/regex_error_compat.h"

/**
 * @brief Detect if a string contains a repeating pattern
 *
 * @param input The input string to analyze
 * @return true if the string has a repeating pattern, false otherwise
 * @note This function uses LibRift's regex engine for pattern detection
 *
gcc -o string_pattern_detector string_pattern_detector.c -I/path/to/librift/include
-L/path/to/librift/lib -lrift -lm

 *  */
bool
detect_repeating_pattern(const char *input)
{
    if (!input || strlen(input) < 2) {
        return false; // Empty or single character strings can't have repeating patterns
    }

    rift_regex_error_t error = {0}; // Initialize the struct to zero
    bool found_pattern = false;

    // We'll use LibRift's r'' syntax for more readable pattern definition
    // The RIFT_REGEX_FLAG_SYNTAX_RIFT flag enables this extended syntax
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_NONE; // Use a default or valid flag

    // Build a pattern that matches strings with repeating substrings
    // Using LibRift's r'' syntax for better readability
    const char *pattern = "r'(.+)\\1+'";

    // Compile the pattern
    rift_regex_pattern_t *compiled_pattern = rift_regex_compile(pattern, flags, &error);

    if (!compiled_pattern) {
        fprintf(stderr, "Pattern compilation failed: %s\n",
                rift_regex_get_compile_error_message(error));
        return false;
    }

    // Create a matcher for the pattern
    rift_regex_matcher_t *matcher = rift_matcher_create(compiled_pattern, 0);
    if (!matcher) {
        fprintf(stderr, "Failed to create matcher\n");
        rift_regex_pattern_free(compiled_pattern);
        return false;
    }

    // Set the input for the matcher
    if (!rift_matcher_set_input(matcher, input, (size_t)-1)) {
        fprintf(stderr, "Failed to set input for matcher\n");
        rift_matcher_free(matcher);
        rift_regex_pattern_free(compiled_pattern);
        return false;
    }

    // Try to match the entire input against the pattern
    rift_regex_match_t match;
    if (rift_matcher_matches(matcher, &match)) {
        found_pattern = true;

        // We can extract the repeating pattern from the first capture group
        if (match.group_count > 0) {
            printf("Repeating pattern: '%s'\n", match.groups[0].value);
        }
    }

    // Clean up resources
    rift_matcher_free(matcher);
    rift_regex_pattern_free(compiled_pattern);

    return found_pattern;
}

/**
 * @brief An extended version that finds the longest repeating pattern
 *
 * @param input The input string to analyze
 * @return A newly allocated string containing the longest repeating pattern, or NULL if none found
 * @note The caller is responsible for freeing the returned string
 */
char *
find_longest_repeating_pattern(const char *input)
{
    if (!input || strlen(input) < 2) {
        return NULL;
    }

    rift_regex_error_t error = {0}; // Initialize the error struct to zero
    char *result = NULL;

    // We'll create an automaton directly for more control
    rift_regex_automaton_t *automaton = rift_automaton_create(RIFT_AUTOMATON_NFA);
    if (!automaton) {
        fprintf(stderr, "Failed to create automaton\n");
        return NULL;
    }

    // Create states for our pattern detection
    rift_regex_state_t *initial_state = rift_automaton_create_state(automaton, false);
    rift_regex_state_t *pattern_start_state = rift_automaton_create_state(automaton, false);
    rift_regex_state_t *pattern_end_state = rift_automaton_create_state(automaton, false);
    rift_regex_state_t *repeat_state = rift_automaton_create_state(automaton, false);
    rift_regex_state_t *accept_state = rift_automaton_create_state(automaton, true);

    if (!initial_state || !pattern_start_state || !pattern_end_state || !repeat_state ||
        !accept_state) {
        fprintf(stderr, "Failed to create automaton states\n");
        rift_automaton_free(automaton);
        return NULL;
    }

    // Set the initial state
    rift_automaton_set_initial_state(automaton, initial_state);

    // Create transitions
    // From initial -> pattern start (epsilon)
    rift_automaton_create_epsilon_transition(automaton, initial_state, pattern_start_state);

    // From pattern start -> pattern end (any character, capturing)
    // In a real implementation, we'd set capture group flags here
    rift_automaton_add_transition(pattern_start_state, pattern_end_state, ".", NULL);

    // From pattern end -> pattern end (any character, continue capturing)
    rift_automaton_add_transition(pattern_end_state, pattern_end_state, ".", NULL);

    // From pattern end -> repeat state (epsilon, end of capture)
    rift_automaton_create_epsilon_transition(automaton, pattern_end_state, repeat_state);

    // From repeat state -> accept state (backreference to captured group)
    // This is simplified - in reality we'd use proper backreference semantics
    rift_automaton_add_transition(repeat_state, accept_state, "\\1", NULL);

    // Now we can create a pattern and matcher
    rift_regex_pattern_t *pattern =
        rift_regex_pattern_from_automaton(automaton, RIFT_REGEX_FLAG_NONE, &error);

    if (!pattern) {
        fprintf(stderr, "Failed to create pattern from automaton: %s\n",
                rift_regex_get_compile_error_message(error));
        rift_automaton_free(automaton);
        return NULL;
    }

    // Create a matcher for the pattern
    rift_regex_matcher_t *matcher = rift_matcher_create(pattern, 0);
    if (!matcher) {
        fprintf(stderr, "Failed to create matcher\n");
        rift_regex_pattern_free(pattern);
        rift_automaton_free(automaton);
        return NULL;
    }

    // Set the input for the matcher
    if (!rift_matcher_set_input(matcher, input, (size_t)-1)) {
        fprintf(stderr, "Failed to set input for matcher\n");
        rift_matcher_free(matcher);
        rift_regex_pattern_free(pattern);
        rift_automaton_free(automaton);
        return NULL;
    }

    // Find all matches
    rift_regex_match_t matches[10]; // Arbitrary limit
    size_t num_matches = 0;

    if (rift_matcher_find_all(matcher, matches, 10, &num_matches)) {
        // Find the match with the longest repeating pattern
        size_t longest_length = 0;
        size_t longest_index = 0;

        for (size_t i = 0; i < num_matches; i++) {
            if (matches[i].group_count > 0) {
                size_t pattern_length = strlen(matches[i].groups[0].value);

                if (pattern_length > longest_length) {
                    longest_length = pattern_length;
                    longest_index = i;
                }
            }
        }

        if (longest_length > 0) {
            // Copy the pattern
            result = strdup(matches[longest_index].groups[0].value);
        }
    }

    // Free all resources
    rift_matcher_free(matcher);
    rift_regex_pattern_free(pattern);
    rift_automaton_free(automaton);

    return result;
}

int
main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <string_to_analyze>\n", argv[0]);
        return 1;
    }

    const char *input = argv[1];

    printf("Analyzing string: '%s'\n", input);

    if (detect_repeating_pattern(input)) {
        printf("The string contains a repeating pattern.\n");

        char *longest_pattern = find_longest_repeating_pattern(input);
        if (longest_pattern) {
            printf("Longest repeating pattern: '%s'\n", longest_pattern);
            free(longest_pattern);
        }
    } else {
        printf("No repeating pattern found.\n");
    }

    return 0;
}
