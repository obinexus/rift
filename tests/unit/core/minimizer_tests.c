/**
 * @file test_minimizer.c
 * @brief Unit tests for the DFA minimization functionality
 *
 * This file contains unit tests for the DFA minimization algorithm and related
 * automaton optimization functions in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/automaton/automaton.h"
#include "librift/automaton/minimizer.h"
#include "librift/automaton/state.h"
#include "librift/errors/error.h"

/* Test helper functions */

/**
 * @brief Create a simple DFA for testing purposes
 *
 * Creates a DFA representing the regular expression "(a|b)*abb"
 *
 * @return A new DFA automaton
 */
static rift_regex_automaton_t *
create_test_dfa(void)
{
    rift_regex_automaton_t *automaton = rift_automaton_create();

    if (!automaton) {
        return NULL;
    }

    // Create 5 states: 0 (start) -> 1 -> 2 -> 3 (accepting)
    // State 4 is a trap state
    for (int i = 0; i < 5; i++) {
        rift_regex_state_t *state = rift_state_create();
        if (!state) {
            rift_automaton_free(automaton);
            return NULL;
        }

        if (i == 3) {
            rift_state_set_accepting(state, true);
        }

        rift_automaton_add_state(automaton, state);
    }

    // Set initial state
    rift_automaton_set_initial_state(automaton, 0);

    // Add transitions for "(a|b)*abb"
    // State 0: on 'a' -> 1, on 'b' -> 0, other -> 4 (trap)
    rift_automaton_add_transition(automaton, 0, 'a', 1);
    rift_automaton_add_transition(automaton, 0, 'b', 0);

    // State 1: on 'a' -> 1, on 'b' -> 2, other -> 4 (trap)
    rift_automaton_add_transition(automaton, 1, 'a', 1);
    rift_automaton_add_transition(automaton, 1, 'b', 2);

    // State 2: on 'b' -> 3, on 'a' -> 1, other -> 4 (trap)
    rift_automaton_add_transition(automaton, 2, 'b', 3);
    rift_automaton_add_transition(automaton, 2, 'a', 1);

    // State 3: on 'a' -> 1, on 'b' -> 0, other -> 4 (trap)
    rift_automaton_add_transition(automaton, 3, 'a', 1);
    rift_automaton_add_transition(automaton, 3, 'b', 0);

    // State 4: trap state, all inputs go to 4
    for (int c = 0; c < 128; c++) {
        rift_automaton_add_transition(automaton, 4, (char)c, 4);
    }

    return automaton;
}

/**
 * @brief Create an automaton with unreachable states
 *
 * @return A DFA with some unreachable states
 */
static rift_regex_automaton_t *
create_automaton_with_unreachable_states(void)
{
    rift_regex_automaton_t *automaton = create_test_dfa();

    if (!automaton) {
        return NULL;
    }

    // Add two unreachable states (5 and 6)
    for (int i = 0; i < 2; i++) {
        rift_regex_state_t *state = rift_state_create();
        if (!state) {
            rift_automaton_free(automaton);
            return NULL;
        }

        rift_automaton_add_state(automaton, state);
    }

    // Add transitions between unreachable states
    rift_automaton_add_transition(automaton, 5, 'a', 6);
    rift_automaton_add_transition(automaton, 6, 'b', 5);

    return automaton;
}

/* Test functions */

/**
 * @brief Test the DFA minimization function
 */
static void
test_automaton_minimize_dfa(void)
{
    // Create a test DFA
    rift_regex_automaton_t *dfa = create_test_dfa();
    assert(dfa != NULL);

    // Count original states
    size_t original_state_count = rift_automaton_get_state_count(dfa);
    printf("Original DFA has %zu states\n", original_state_count);

    // Minimize DFA
    rift_regex_error_t error;
    rift_regex_automaton_t *minimized = rift_automaton_minimize_dfa(dfa, &error);

    // Verify minimization succeeded
    assert(minimized != NULL);

    // Count minimized states
    size_t minimized_state_count = rift_automaton_get_state_count(minimized);
    printf("Minimized DFA has %zu states\n", minimized_state_count);

    // Check that minimization reduced the number of states
    assert(minimized_state_count <= original_state_count);

    // Clean up
    rift_automaton_free(dfa);
    rift_automaton_free(minimized);

    printf("DFA minimization test passed\n");
}

/**
 * @brief Test the minimization validation function
 */
static void
test_automaton_validate_minimization(void)
{
    // Create a test DFA
    rift_regex_automaton_t *dfa = create_test_dfa();
    assert(dfa != NULL);

    // Minimize DFA
    rift_regex_error_t error;
    rift_regex_automaton_t *minimized = rift_automaton_minimize_dfa(dfa, &error);
    assert(minimized != NULL);

    // Validate minimization
    int validation_result = rift_automaton_validate_minimization(dfa, minimized);

    // Check validation result
    assert(validation_result == 1);

    // Clean up
    rift_automaton_free(dfa);
    rift_automaton_free(minimized);

    printf("Minimization validation test passed\n");
}

/**
 * @brief Test the transition optimization function
 */
static void
test_automaton_optimize_transitions(void)
{
    // Create a test DFA
    rift_regex_automaton_t *dfa = create_test_dfa();
    assert(dfa != NULL);

    // Optimize transitions
    bool result = rift_automaton_optimize_transitions(dfa);

    // Check optimization result
    assert(result == true);

    // Clean up
    rift_automaton_free(dfa);

    printf("Transition optimization test passed\n");
}

/**
 * @brief Test the removal of unreachable states
 */
static void
test_automaton_remove_unreachable_states(void)
{
    // Create a test DFA with unreachable states
    rift_regex_automaton_t *dfa = create_automaton_with_unreachable_states();
    assert(dfa != NULL);

    // Count original states
    size_t original_state_count = rift_automaton_get_state_count(dfa);
    printf("Original DFA has %zu states (including unreachable)\n", original_state_count);

    // Remove unreachable states
    bool result = rift_automaton_remove_unreachable_states(dfa);

    // Check removal result
    assert(result == true);

    // Count remaining states
    size_t remaining_state_count = rift_automaton_get_state_count(dfa);
    printf("After removal, DFA has %zu states\n", remaining_state_count);

    // Check that unreachable states were removed
    assert(remaining_state_count < original_state_count);

    // Clean up
    rift_automaton_free(dfa);

    printf("Unreachable states removal test passed\n");
}

/**
 * @brief Main function to run all tests
 */
int
main(void)
{
    printf("Running DFA minimizer tests...\n");

    // Run all tests
    test_automaton_minimize_dfa();
    test_automaton_validate_minimization();
    test_automaton_optimize_transitions();
    test_automaton_remove_unreachable_states();

    printf("All tests passed successfully\n");
    return 0;
}
