#include <stdio.h>
#include <stdlib.h>

#include "librift/regex/automaton/automaton.h"
#include "librift/regex/automaton/flags.h"

int
main()
{
    // The tea starts here - we're creating our automaton from scratch
    rift_regex_automaton_t *automaton = rift_automaton_create(RIFT_AUTOMATON_NFA);
    if (!automaton) {
        fprintf(stderr, "Bestie, we couldn't create the automaton. Big yikes! 😭\n");
        return 1;
    }

    // Let's create our state with that fire r'' pattern
    // This is where the MAGIC happens!
    rift_regex_state_t *state = rift_automaton_create_state(automaton, false);
    if (!state) {
        fprintf(stderr, "State creation failed and I'm literally crying rn 😢\n");
        rift_automaton_free(automaton);
        return 1;
    }

    // The pattern we're using: r'/[^A-Z0-9\b]/'
    // This matches any character that isn't uppercase, digit, or word boundary
    // Set this pattern as the state's definition - THIS IS THE KEY PART!
    rift_regex_state_t *accepting_state = rift_automaton_create_state(automaton, true);
    rift_automaton_add_transition(state, accepting_state, "[^A-Z0-9\\b]", NULL);

    // Set as initial state
    rift_automaton_set_initial_state(automaton, state);

    // Now let's vibe check some input against our automaton
    const char *test_strings[] = {"a", "Z", "5", " ", "\t", "!"};

    printf("✨ Testing our r'/[^A-Z0-9\\b]/' pattern as automaton state ✨\n\n");

    for (int i = 0; i < 6; i++) {
        // Reset the automaton for each test
        rift_automaton_reset(automaton);

        // Process the character
        printf("Testing '%s': ", test_strings[i]);

        char c = test_strings[i][0];
        rift_regex_state_t *current = rift_automaton_get_current_state(automaton);

        // Get transitions from current state
        rift_regex_transition_t *transitions[10];
        size_t num_transitions = rift_automaton_get_current_transitions(automaton, transitions, 10);

        // Try to match with any transition
        bool matched = false;
        for (size_t j = 0; j < num_transitions; j++) {
            if (rift_automaton_transition_matches(transitions[j], &c)) {
                rift_regex_state_t *target = rift_automaton_transition_get_target(transitions[j]);
                rift_automaton_set_current_state(automaton, target);
                matched = true;
                break;
            }
        }

        // Check if we're now in an accepting state
        if (matched && rift_automaton_is_accepting(automaton)) {
            printf(
                "MATCH! This char is NOT a capital letter, digit, or word boundary. Period. 💅\n");
        } else {
            printf(
                "No match. This is either a capital letter, digit, or word boundary. Basic. 🙄\n");
        }
    }

    // Clean up our mess because we're responsible Gen Z devs
    rift_automaton_free(automaton);

    return 0;
}