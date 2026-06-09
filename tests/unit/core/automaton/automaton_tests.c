/**
 * @file automaton_tests.c
 * @brief Test harness for the automaton module of LibRift using ctestr
 *
 * This file implements comprehensive tests for the automaton functions
 * in the LibRift regex engine, ensuring proper functionality of state
 * and transition management, automaton construction, and algorithms
 * like NFA-to-DFA conversion and minimization.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <ctestr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/automaton/automaton.h"
#include "librift/automaton/flags.h"
#include "librift/automaton/state.h"
#include "librift/automaton/transition.h"
#include "librift/errors/regex_error.h"

/* Global variables for test suite */
static rift_regex_automaton_t *test_automaton = NULL;

/* Setup and teardown functions */
static void
setup(void)
{
    /* Reset state ID counter for consistent testing */
    rift_state_reset_id_counter();

    /* Create a fresh automaton for each test */
    test_automaton = rift_automaton_create(RIFT_AUTOMATON_NFA);
}

static void
teardown(void)
{
    if (test_automaton) {
        rift_automaton_free(test_automaton);
        test_automaton = NULL;
    }
}

/**** Basic Automaton Tests ****/

/* Test automaton creation and properties */
CTEST(automaton, create)
{
    /* Test NFA creation */
    rift_regex_automaton_t *nfa = rift_automaton_create(RIFT_AUTOMATON_NFA);
    ASSERT_NOT_NULL(nfa);
    ASSERT_EQUAL(RIFT_AUTOMATON_NFA, rift_automaton_get_type(nfa));
    ASSERT_FALSE(rift_automaton_is_deterministic(nfa));
    ASSERT_EQUAL(0, rift_automaton_get_state_count(nfa));
    ASSERT_NULL(rift_automaton_get_initial_state(nfa));
    rift_automaton_free(nfa);

    /* Test DFA creation */
    rift_regex_automaton_t *dfa = rift_automaton_create(RIFT_AUTOMATON_DFA);
    ASSERT_NOT_NULL(dfa);
    ASSERT_EQUAL(RIFT_AUTOMATON_DFA, rift_automaton_get_type(dfa));
    ASSERT_TRUE(rift_automaton_is_deterministic(dfa));
    ASSERT_EQUAL(0, rift_automaton_get_state_count(dfa));
    ASSERT_NULL(rift_automaton_get_initial_state(dfa));
    rift_automaton_free(dfa);
}

/* Test automaton flag management */
CTEST_SETUP(automaton_flags)
{
    setup();
}

CTEST_TEARDOWN(automaton_flags)
{
    teardown();
}

CTEST(automaton, flags)
{
    /* Set various flags */
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_CASE_INSENSITIVE | RIFT_REGEX_FLAG_MULTILINE;
    ASSERT_TRUE(rift_automaton_set_flags(test_automaton, flags));
    ASSERT_EQUAL(flags, rift_automaton_get_flags(test_automaton));

    /* Change flags */
    flags = RIFT_REGEX_FLAG_DOTALL | RIFT_REGEX_FLAG_EXTENDED;
    ASSERT_TRUE(rift_automaton_set_flags(test_automaton, flags));
    ASSERT_EQUAL(flags, rift_automaton_get_flags(test_automaton));

    /* Clear flags */
    ASSERT_TRUE(rift_automaton_set_flags(test_automaton, RIFT_REGEX_FLAG_NONE));
    ASSERT_EQUAL(RIFT_REGEX_FLAG_NONE, rift_automaton_get_flags(test_automaton));
}

/* Test automaton error handling */
CTEST_SETUP(automaton_error_handling)
{
    setup();
}

CTEST_TEARDOWN(automaton_error_handling)
{
    teardown();
}

CTEST(automaton, error_handling)
{
    rift_regex_error_t error;

    /* Initially, there should be no error */
    ASSERT_FALSE(rift_automaton_get_last_error(test_automaton, &error));

    /* Set up an internal error manually (not ideal, but for testing purposes) */
    test_automaton->last_error.code = RIFT_REGEX_ERROR_INTERNAL;
    strcpy(test_automaton->last_error.message, "Test error message");

    /* Now we should be able to retrieve the error */
    ASSERT_TRUE(rift_automaton_get_last_error(test_automaton, &error));
    ASSERT_EQUAL(RIFT_REGEX_ERROR_INTERNAL, error.code);
    ASSERT_STR_EQUAL("Test error message", error.message);

    /* Clear the error */
    rift_automaton_clear_error(test_automaton);
    ASSERT_FALSE(rift_automaton_get_last_error(test_automaton, &error));
}

/* Test automaton state management */
CTEST_SETUP(automaton_state_management)
{
    setup();
}

CTEST_TEARDOWN(automaton_state_management)
{
    teardown();
}

CTEST(automaton, state_management)
{
    /* Add states */
    rift_regex_state_t *state1 = rift_automaton_create_state(test_automaton, false);
    rift_regex_state_t *state2 = rift_automaton_create_state(test_automaton, true);
    rift_regex_state_t *state3 = rift_automaton_create_state(test_automaton, false);

    ASSERT_NOT_NULL(state1);
    ASSERT_NOT_NULL(state2);
    ASSERT_NOT_NULL(state3);

    /* Check state count */
    ASSERT_EQUAL(3, rift_automaton_get_state_count(test_automaton));

    /* Check state properties */
    ASSERT_FALSE(rift_automaton_is_state_accepting(test_automaton, state1));
    ASSERT_TRUE(rift_automaton_is_state_accepting(test_automaton, state2));

    /* Set initial state */
    ASSERT_TRUE(rift_automaton_set_initial_state(test_automaton, state1));
    ASSERT_EQUAL_PTR(state1, rift_automaton_get_initial_state(test_automaton));

    /* Find state by ID */
    size_t state2_id = rift_automaton_get_state_id(state2);
    ASSERT_EQUAL_PTR(state2, rift_automaton_find_state_by_id(test_automaton, state2_id));

    /* Change state property */
    ASSERT_TRUE(rift_automaton_set_state_accepting(test_automaton, state1, true));
    ASSERT_TRUE(rift_automaton_is_state_accepting(test_automaton, state1));

    /* Set user data */
    int test_data = 42;
    ASSERT_TRUE(rift_automaton_set_state_user_data(state3, &test_data));
    ASSERT_EQUAL_PTR(&test_data, rift_automaton_get_state_user_data(state3));
}

/* Test automaton transition management */
CTEST_SETUP(automaton_transition_management)
{
    setup();
}

CTEST_TEARDOWN(automaton_transition_management)
{
    teardown();
}

CTEST(automaton, transition_management)
{
    /* Create states */
    rift_regex_state_t *state1 = rift_automaton_create_state(test_automaton, false);
    rift_regex_state_t *state2 = rift_automaton_create_state(test_automaton, true);
    rift_regex_state_t *state3 = rift_automaton_create_state(test_automaton, false);

    /* Create transitions */
    ASSERT_TRUE(rift_automaton_add_transition(test_automaton, state1, state2, "a"));
    ASSERT_TRUE(rift_automaton_add_transition(test_automaton, state1, state3, "[0-9]"));
    ASSERT_TRUE(rift_automaton_add_epsilon_transition(test_automaton, state2, state3));

    /* Check transition counts */
    ASSERT_EQUAL(2, rift_automaton_get_state_transition_count(state1));
    ASSERT_EQUAL(1, rift_automaton_get_state_transition_count(state2));
    ASSERT_EQUAL(0, rift_automaton_get_state_transition_count(state3));

    /* Get and check transitions */
    rift_regex_transition_t *t1 = rift_automaton_get_state_transition(state1, 0);
    rift_regex_transition_t *t2 = rift_automaton_get_state_transition(state1, 1);
    rift_regex_transition_t *t3 = rift_automaton_get_state_transition(state2, 0);

    ASSERT_NOT_NULL(t1);
    ASSERT_NOT_NULL(t2);
    ASSERT_NOT_NULL(t3);

    /* Check transition properties */
    ASSERT_STR_EQUAL("a", rift_automaton_transition_get_pattern(t1));
    ASSERT_STR_EQUAL("[0-9]", rift_automaton_transition_get_pattern(t2));
    ASSERT_NULL(rift_automaton_transition_get_pattern(t3)); /* Epsilon transition */

    ASSERT_EQUAL_PTR(state2, rift_automaton_transition_get_target(t1));
    ASSERT_EQUAL_PTR(state3, rift_automaton_transition_get_target(t2));
    ASSERT_EQUAL_PTR(state3, rift_automaton_transition_get_target(t3));

    ASSERT_FALSE(rift_automaton_is_epsilon_transition(t1));
    ASSERT_FALSE(rift_automaton_is_epsilon_transition(t2));
    ASSERT_TRUE(rift_automaton_is_epsilon_transition(t3));

    /* Test transition matching */
    ASSERT_TRUE(rift_automaton_transition_matches(t1, 'a'));
    ASSERT_FALSE(rift_automaton_transition_matches(t1, 'b'));
    ASSERT_TRUE(rift_automaton_transition_matches(t2, '5'));
    ASSERT_FALSE(rift_automaton_transition_matches(t2, 'x'));
    ASSERT_TRUE(rift_automaton_transition_matches(t3, 'x')); /* Epsilon matches any */
}

/* Test automaton state tracking */
CTEST_SETUP(automaton_state_tracking)
{
    setup();
}

CTEST_TEARDOWN(automaton_state_tracking)
{
    teardown();
}

CTEST(automaton, state_tracking)
{
    /* Create states */
    rift_regex_state_t *state1 = rift_automaton_create_state(test_automaton, false);
    rift_regex_state_t *state2 = rift_automaton_create_state(test_automaton, true);
    rift_regex_state_t *state3 = rift_automaton_create_state(test_automaton, false);

    /* Create transitions */
    rift_automaton_add_transition(test_automaton, state1, state2, "a");
    rift_automaton_add_transition(test_automaton, state2, state3, "b");

    /* Set initial state */
    rift_automaton_set_initial_state(test_automaton, state1);
    ASSERT_EQUAL_PTR(state1, rift_automaton_get_current_state(test_automaton));

    /* Check initial acceptance state */
    ASSERT_FALSE(rift_automaton_is_accepting(test_automaton));

    /* Manually set current state to accepting one */
    rift_automaton_set_current_state(test_automaton, state2);
    ASSERT_TRUE(rift_automaton_is_accepting(test_automaton));

    /* Reset automaton */
    rift_automaton_reset(test_automaton);
    ASSERT_EQUAL_PTR(state1, rift_automaton_get_current_state(test_automaton));
}

/* Test automaton clone functionality */
CTEST_SETUP(automaton_clone)
{
    setup();
}

CTEST_TEARDOWN(automaton_clone)
{
    teardown();
}

CTEST(automaton, clone)
{
    /* Create original automaton structure */
    rift_regex_state_t *state1 = rift_automaton_create_state(test_automaton, false);
    rift_regex_state_t *state2 = rift_automaton_create_state(test_automaton, true);
    rift_automaton_add_transition(test_automaton, state1, state2, "a");
    rift_automaton_set_initial_state(test_automaton, state1);
    rift_automaton_set_flags(test_automaton, RIFT_REGEX_FLAG_CASE_INSENSITIVE);

    /* Clone the automaton */
    rift_regex_automaton_t *clone = rift_automaton_clone(test_automaton);
    ASSERT_NOT_NULL(clone);

    /* Check that clone has the same properties but different memory addresses */
    ASSERT_EQUAL(rift_automaton_get_type(test_automaton), rift_automaton_get_type(clone));
    ASSERT_EQUAL(rift_automaton_is_deterministic(test_automaton),
                 rift_automaton_is_deterministic(clone));
    ASSERT_EQUAL(rift_automaton_get_state_count(test_automaton),
                 rift_automaton_get_state_count(clone));
    ASSERT_EQUAL(rift_automaton_get_flags(test_automaton), rift_automaton_get_flags(clone));

    /* Check that state properties are preserved */
    rift_regex_state_t *clone_state1 = rift_automaton_get_initial_state(clone);
    rift_regex_state_t *clone_state2 =
        rift_automaton_transition_get_target(rift_automaton_get_state_transition(clone_state1, 0));

    ASSERT_NOT_NULL(clone_state1);
    ASSERT_NOT_NULL(clone_state2);
    ASSERT_NOT_EQUAL_PTR(state1, clone_state1); /* Different memory addresses */
    ASSERT_NOT_EQUAL_PTR(state2, clone_state2);

    ASSERT_EQUAL(rift_automaton_is_state_accepting(test_automaton, state1),
                 rift_automaton_is_state_accepting(clone, clone_state1));
    ASSERT_EQUAL(rift_automaton_is_state_accepting(test_automaton, state2),
                 rift_automaton_is_state_accepting(clone, clone_state2));

    /* Clean up */
    rift_automaton_free(clone);
}

/**** Advanced Automaton Tests ****/

/* Test NFA to DFA conversion */
CTEST_SETUP(nfa_to_dfa_conversion)
{
    setup();
}

CTEST_TEARDOWN(nfa_to_dfa_conversion)
{
    teardown();
}

CTEST(automaton, nfa_to_dfa_conversion)
{
    /* Create a simple NFA for (a|b)*a */
    rift_regex_state_t *state1 = rift_automaton_create_state(test_automaton, false);
    rift_regex_state_t *state2 = rift_automaton_create_state(test_automaton, false);
    rift_regex_state_t *state3 = rift_automaton_create_state(test_automaton, true);

    rift_automaton_set_initial_state(test_automaton, state1);
    rift_automaton_add_transition(test_automaton, state1, state1, "b");
    rift_automaton_add_transition(test_automaton, state1, state2, "a");
    rift_automaton_add_transition(test_automaton, state2, state1, "b");
    rift_automaton_add_transition(test_automaton, state2, state3, "a");

    /* Convert to DFA */
    rift_regex_error_t error;
    rift_regex_automaton_t *dfa = rift_automaton_nfa_to_dfa(test_automaton, &error);

    ASSERT_NOT_NULL(dfa);
    ASSERT_TRUE(rift_automaton_is_deterministic(dfa));

    /* Validate DFA properties */
    ASSERT_NOT_NULL(rift_automaton_get_initial_state(dfa));

    /* A proper DFA for this regex should have at least 3 states */
    ASSERT_TRUE(rift_automaton_get_state_count(dfa) >= 3);

    /* Clean up */
    rift_automaton_free(dfa);
}

/* Test DFA minimization */
CTEST(automaton, dfa_minimization)
{
    /* Create a DFA with redundant states */
    rift_regex_automaton_t *dfa = rift_automaton_create(RIFT_AUTOMATON_DFA);
    rift_regex_state_t *s1 = rift_automaton_create_state(dfa, false);
    rift_regex_state_t *s2 = rift_automaton_create_state(dfa, false);
    rift_regex_state_t *s3 = rift_automaton_create_state(dfa, true);
    rift_regex_state_t *s4 = rift_automaton_create_state(dfa, true); /* Redundant to s3 */

    rift_automaton_set_initial_state(dfa, s1);

    /* s1 -a-> s2, s1 -b-> s3 */
    rift_automaton_add_transition(dfa, s1, s2, "a");
    rift_automaton_add_transition(dfa, s1, s3, "b");

    /* s2 -a-> s2, s2 -b-> s4 */
    rift_automaton_add_transition(dfa, s2, s2, "a");
    rift_automaton_add_transition(dfa, s2, s4, "b");

    /* s3 -a-> s2, s3 -b-> s3 */
    rift_automaton_add_transition(dfa, s3, s2, "a");
    rift_automaton_add_transition(dfa, s3, s3, "b");

    /* s4 -a-> s2, s4 -b-> s3 - identical to s3's transitions */
    rift_automaton_add_transition(dfa, s4, s2, "a");
    rift_automaton_add_transition(dfa, s4, s3, "b");

    /* Minimize DFA */
    rift_regex_error_t error;
    rift_regex_automaton_t *minimized = rift_automaton_minimize_dfa(dfa, &error);

    ASSERT_NOT_NULL(minimized);

    /* The minimized DFA should have 3 states (s4 should be merged with s3) */
    ASSERT_EQUAL(3, rift_automaton_get_state_count(minimized));

    /* Clean up */
    rift_automaton_free(dfa);
    rift_automaton_free(minimized);
}

/* Test complex automaton construction for a real regex pattern */
CTEST(automaton, complex_automaton_construction)
{
    /* Create an automaton for the regex pattern: "a[bc]+d|ef*g" */
    rift_regex_automaton_t *nfa = rift_automaton_create(RIFT_AUTOMATON_NFA);

    /* States for the first alternative: a[bc]+d */
    rift_regex_state_t *s1 = rift_automaton_create_state(nfa, false); /* Start */
    rift_regex_state_t *s2 = rift_automaton_create_state(nfa, false); /* After 'a' */
    rift_regex_state_t *s3 = rift_automaton_create_state(nfa, false); /* After [bc]+ */
    rift_regex_state_t *s4 = rift_automaton_create_state(nfa, true);  /* After 'd' (accepting) */

    /* States for the second alternative: ef*g */
    rift_regex_state_t *s5 = rift_automaton_create_state(nfa, false); /* After 'e' */
    rift_regex_state_t *s6 = rift_automaton_create_state(nfa, false); /* After 'f*' */
    rift_regex_state_t *s7 = rift_automaton_create_state(nfa, true);  /* After 'g' (accepting) */

    rift_automaton_set_initial_state(nfa, s1);

    /* First alternative: a[bc]+d */
    rift_automaton_add_transition(nfa, s1, s2, "a");
    rift_automaton_add_transition(nfa, s2, s3, "b");
    rift_automaton_add_transition(nfa, s2, s3, "c");
    rift_automaton_add_transition(nfa, s3, s3, "b"); /* Loop for + */
    rift_automaton_add_transition(nfa, s3, s3, "c"); /* Loop for + */
    rift_automaton_add_transition(nfa, s3, s4, "d");

    /* Second alternative: ef*g */
    rift_automaton_add_transition(nfa, s1, s5, "e");
    rift_automaton_add_transition(nfa, s5, s6, "f");
    rift_automaton_add_epsilon_transition(nfa, s5, s6); /* Skip 'f' for * */
    rift_automaton_add_transition(nfa, s6, s6, "f");    /* Loop for * */
    rift_automaton_add_transition(nfa, s6, s7, "g");

    /* Validate automaton structure */
    ASSERT_EQUAL(7, rift_automaton_get_state_count(nfa));
    ASSERT_EQUAL_PTR(s1, rift_automaton_get_initial_state(nfa));

    /* Convert to DFA */
    rift_regex_error_t error;
    rift_regex_automaton_t *dfa = rift_automaton_nfa_to_dfa(nfa, &error);

    ASSERT_NOT_NULL(dfa);
    ASSERT_TRUE(rift_automaton_is_deterministic(dfa));

    /* Minimize DFA */
    rift_regex_automaton_t *minimized = rift_automaton_minimize_dfa(dfa, &error);

    ASSERT_NOT_NULL(minimized);

    /* Clean up */
    rift_automaton_free(dfa);
    rift_automaton_free(minimized);
    rift_automaton_free(nfa);
}

/* Test automaton optimization */
CTEST_SETUP(automaton_optimization)
{
    setup();
}

CTEST_TEARDOWN(automaton_optimization)
{
    teardown();
}

CTEST(automaton, optimization)
{
    /* Create a simple automaton */
    rift_regex_state_t *s1 = rift_automaton_create_state(test_automaton, false);
    rift_regex_state_t *s2 = rift_automaton_create_state(test_automaton, false);
    rift_regex_state_t *s3 = rift_automaton_create_state(test_automaton, true);
    rift_regex_state_t *s4 = rift_automaton_create_state(test_automaton, false); /* Unreachable */

    rift_automaton_set_initial_state(test_automaton, s1);
    rift_automaton_add_transition(test_automaton, s1, s2, "a");
    rift_automaton_add_transition(test_automaton, s2, s3, "b");

    /* Optimize the automaton */
    ASSERT_TRUE(rift_automaton_optimize_transitions(test_automaton));

    /* Note: The actual optimization is limited in this implementation,
     * but we can at least verify that the function doesn't break anything */
    ASSERT_EQUAL(4, rift_automaton_get_state_count(test_automaton));
    ASSERT_EQUAL_PTR(s1, rift_automaton_get_initial_state(test_automaton));
}

/* Main function */
int
main(int argc, const char *argv[])
{
    return ctestr_main(argc, argv);
}