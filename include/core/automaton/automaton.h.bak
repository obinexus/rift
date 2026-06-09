/**
 * @file automaton.h
 * @brief Automaton functions for the LibRift regex engine
 *
 * This file defines the Automaton structure and functions for creating and manipulating
 * finite automata in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */
#ifndef LIBRIFT_REGEX_AUTOMATON_AUTOMATON_H
#define LIBRIFT_REGEX_AUTOMATON_AUTOMATON_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "core/automaton/flags.h"
#include "core/automaton/state.h"
#include "core/automaton/transition.h"
#include "core/errors/regex_error.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Automaton type definitions
 */
/**
 * @brief Automaton structure forward declaration
 */
typedef struct rift_regex_automaton rift_regex_automaton_t;

typedef enum rift_automaton_type {
    RIFT_AUTOMATON_INVALID = 0,
    RIFT_AUTOMATON_NFA,
    RIFT_AUTOMATON_DFA
} rift_automaton_type_t;

/**
 * @brief Automaton structure definition
 */
struct rift_regex_automaton {
    rift_regex_state_t **states;           /**< Array of states */
    size_t num_states;                     /**< Number of states */
    size_t state_capacity;                 /**< Capacity of states array */
    rift_regex_state_t *initial_state;     /**< Initial state */
    rift_regex_state_t *current_state;     /**< Current state during matching */
    rift_automaton_type_t type;            /**< Automaton type (NFA or DFA) */
    bool is_deterministic;                 /**< Whether the automaton is deterministic (DFA) */
    rift_regex_flags_t flags;              /**< Automaton flags */
    rift_regex_transition_t **transitions; /**< Array of transitions */
    size_t transition_capacity;            /**< Capacity of transitions array */
    rift_regex_error_t last_error;         /**< Last error that occurred */

    size_t num_transitions; /**< Number of transitions */
};

// Define rift_automaton_t if not already defined
typedef struct {
    // Add fields for the automaton structure
    int state_count;
    bool *accepting_states;
    // Add other necessary fields
} rift_automaton_t;
/**
 * @brief Minimize the given automaton
 *
 * @param automaton Pointer to the automaton to minimize
 * @return true on success, false on failure
 */
// Removed duplicate declaration for rift_automaton_minimize

/**
 * @brief Set a state in the automaton as accepting
 *
 * @param automaton Pointer to the automaton
 * @param state The state to set as accepting
 * @return true on success, false on failure
 */
// Removed duplicate declaration for rift_automaton_set_state_accepting
/**
 * @brief Create a new automaton
 *
 * @param type The type of automaton to create (NFA or DFA)
 * @return A new automaton or NULL on failure
 */
rift_regex_automaton_t *rift_automaton_create(rift_automaton_type_t type);

/**
 * @brief Create a transition between two states in the automaton
 *
 * @param automaton The automaton
 * @param from_state The source state
 * @param to_state The target state
 * @param pattern The transition pattern (NULL for epsilon transition)
 * @return true if the transition was successfully created, false otherwise
 */
bool rift_automaton_create_transition(rift_regex_automaton_t *automaton,
                                      rift_regex_state_t *from_state, rift_regex_state_t *to_state,
                                      const char *pattern);

/**
 * @brief Free resources associated with an automaton
 *
 * @param automaton The automaton to free
 */
void rift_automaton_free(rift_regex_automaton_t *automaton);

/**
 * @brief Get the type of an automaton
 *
 * @param automaton The automaton
 * @return The automaton type
 */
rift_automaton_type_t rift_automaton_get_type(const rift_regex_automaton_t *automaton);

/**
 * @brief Check if an automaton is deterministic
 *
 * @param automaton The automaton
 * @return true if the automaton is a DFA, false otherwise
 */
bool rift_automaton_is_deterministic(const rift_regex_automaton_t *automaton);

/**
 * @brief Set the initial state of an automaton
 *
 * @param automaton The automaton
 * @param state The state to set as initial
 * @return true if successful, false otherwise
 */
bool rift_automaton_set_initial_state(rift_regex_automaton_t *automaton, rift_regex_state_t *state);

/**
 * @brief Get the initial state of an automaton
 *
 * @param automaton The automaton
 * @return The initial state or NULL if not set
 */
rift_regex_state_t *rift_automaton_get_initial_state(const rift_regex_automaton_t *automaton);

/**
 * @brief Get the number of states in an automaton
 *
 * @param automaton The automaton
 * @return The number of states
 */
size_t rift_automaton_get_state_count(const rift_regex_automaton_t *automaton);

/**
 * @brief Get a state by index
 *
 * @param automaton The automaton
 * @param index The index of the state
 * @return The state or NULL if the index is out of bounds
 */
rift_regex_state_t *rift_automaton_get_state_by_index(const rift_regex_automaton_t *automaton,
                                                      size_t index);

/**
 * @brief Find a state by ID
 *
 * @param automaton The automaton
 * @param id The ID of the state to find
 * @return The state or NULL if not found
 */
rift_regex_state_t *rift_automaton_find_state_by_id(const rift_regex_automaton_t *automaton,
                                                    size_t id);

/**
 * @brief Convert an NFA to a DFA
 *
 * This function uses the subset construction algorithm to convert an NFA to an equivalent DFA.
 *
 * @param nfa The NFA to convert
 * @param error Pointer to store error code (can be NULL)
 * @return The equivalent DFA or NULL on failure
 */
rift_regex_automaton_t *rift_automaton_nfa_to_dfa(const rift_regex_automaton_t *nfa,
                                                  rift_regex_error_t *error);

/**
 * @brief Add a transition between two states in the automaton
 *
 * @param automaton The automaton
 * @param from_state The source state
 * @param to_state The target state
 * @param pattern The pattern for the transition
 * @return true if successful, false otherwise
 */
bool rift_automaton_add_transition(rift_regex_automaton_t *automaton,
                                   rift_regex_state_t *from_state, rift_regex_state_t *to_state,
                                   const char *pattern);

/**
 * @brief Add an epsilon transition between two states in the automaton
 *
 * @param automaton The automaton
 * @param from_state The source state
 * @param to_state The target state
 * @return true if successful, false otherwise
 */
bool rift_automaton_add_epsilon_transition(rift_regex_automaton_t *automaton,
                                           rift_regex_state_t *from_state,
                                           rift_regex_state_t *to_state);

/**
 * @brief Create an epsilon transition between two states
 *
 * @param automaton The automaton
 * @param from_state The source state
 * @param to_state The target state
 * @return true if successful, false otherwise
 */
bool rift_automaton_create_epsilon_transition(rift_regex_automaton_t *automaton,
                                              rift_regex_state_t *from_state,
                                              rift_regex_state_t *to_state);

bool rift_automaton_optimize_transitions(rift_regex_automaton_t *automaton);

/**
 * @brief Check if a transition matches a character
 *
 * @param transition The transition to check
 * @param c The character to check
 * @return true if the transition matches the character, false otherwise
 */
bool rift_automaton_transition_matches(rift_regex_transition_t *transition, char c);

void rift_automaton_reset(rift_regex_automaton_t *automaton);

/**
 * @brief Get the current transitions from the current state of an automaton
 *
 * @param automaton The automaton
 * @param transitions Array to store the transitions
 * @param max_transitions Maximum number of transitions to store
 * @return The number of transitions stored
 */

/**
 * @brief Get the current state of an automaton
 * @param automaton The automaton
 * @return The current state or NULL if not set
 */
rift_regex_state_t *rift_automaton_get_current_state(const rift_regex_automaton_t *automaton);

rift_regex_automaton_t *rift_automaton_clone(const rift_regex_automaton_t *automaton);

/**
 * @brief Create an epsilon transition between two states
 *
 * @param automaton The automaton
 * @param from_state The source state
 * @param to_state The target state
 * @return true if successful, false otherwise
 */

#ifndef AUTOMATON_IMPL
/* Implementation is defined in automaton.c, so just declare it here */
extern bool rift_automaton_create_epsilon_transition(rift_regex_automaton_t *automaton,
                                                     rift_regex_state_t *from_state,
                                                     rift_regex_state_t *to_state);
#else
/* Implement the function only if explicitly requested by defining AUTOMATON_IMPL */
bool
rift_automaton_create_epsilon_transition(rift_regex_automaton_t *automaton,
                                         rift_regex_state_t *from_state,
                                         rift_regex_state_t *to_state)
{
    if (!automaton || !from_state || !to_state) {
        return false;
    }

    /* Create a special transition with NULL pattern to represent epsilon */
    return rift_automaton_add_transition(automaton, from_state, to_state, NULL);
}
#endif

/**
 * @brief Check if a transition matches a character
 *
 * @param transition The transition to check
 * @param c The character to check
 * @return true if the transition matches the character, false otherwise
 */
bool rift_automaton_transition_matches(rift_regex_transition_t *transition, char c);

void rift_automaton_reset(rift_regex_automaton_t *automaton);

/**
 * @brief Get the current transitions from the current state of an automaton
 *
 * @param automaton The automaton
 * @param transitions Array to store the transitions
 * @param max_transitions Maximum number of transitions to store
 * @return The number of transitions stored
 */
size_t rift_automaton_get_current_transitions(rift_regex_automaton_t *automaton,
                                              rift_regex_transition_t **transitions,
                                              size_t max_transitions);

/**
 * @brief Get the current state of an automaton
 *
 * @param automaton The automaton
 * @return The current state or NULL if not set
 */
rift_regex_state_t *rift_automaton_get_current_state(const rift_regex_automaton_t *automaton);

/**
 * @brief Set the current state of an automaton
 *
 * @param automaton The automaton
 * @param state The state to set as current
 * @return true if successful, false otherwise
 */
bool rift_automaton_set_current_state(rift_regex_automaton_t *automaton, rift_regex_state_t *state);

/**
 * @brief Check if the automaton is in an accepting state
 *
 * @param automaton The automaton
 * @return true if the current state is accepting, false otherwise
 */
bool rift_automaton_is_accepting(const rift_regex_automaton_t *automaton);

/**
 * @brief Set the flags for an automaton
 *
 * @param automaton The automaton
 * @param flags The flags to set
 * @return true if successful, false otherwise
 */
bool rift_automaton_set_flags(rift_regex_automaton_t *automaton, rift_regex_flags_t flags);

/**
 * @brief Get the flags of an automaton
 *
 * @param automaton The automaton
 * @return The automaton flags
 */
rift_regex_flags_t rift_automaton_get_flags(const rift_regex_automaton_t *automaton);

/**
 * @brief Set user data for a state
 *
 * @param state The state
 * @param user_data The user data to set
 * @return true if successful, false otherwise
 */
bool rift_automaton_set_state_user_data(rift_regex_state_t *state, void *user_data);

/**
 * @brief Get user data from a state
 *
 * @param state The state
 * @return The user data or NULL if not set
 */
void *rift_automaton_get_state_user_data(const rift_regex_state_t *state);

/**
 * @brief Check if a state is accepting
 *
 * @param automaton The automaton containing the state
 * @param state The state to check
 * @return true if the state is accepting, false otherwise
 */
bool rift_automaton_is_state_accepting(const rift_regex_automaton_t *automaton,
                                       const rift_regex_state_t *state);

/**
 * @brief Get a state's ID
 *
 * @param state The state
 * @return The state ID
 */
size_t rift_automaton_get_state_id(const rift_regex_state_t *state);

/**
 * @brief Get the number of transitions from a state
 *
 * @param state The state
 * @return The number of transitions
 */
size_t rift_automaton_get_state_transition_count(const rift_regex_state_t *state);

/**
 * @brief Get a specific transition from a state
 *
 * @param state The state
 * @param index The index of the transition
 * @return The transition or NULL if the index is out of bounds
 */
rift_regex_transition_t *rift_automaton_get_state_transition(const rift_regex_state_t *state,
                                                             size_t index);

/**
 * @brief Check if a transition is an epsilon transition
 *
 * @param transition The transition to check
 * @return true if the transition is an epsilon transition, false otherwise
 */
bool rift_automaton_is_epsilon_transition(const rift_regex_transition_t *transition);

/**
 * @brief Get the last error from an automaton
 *
 * @param automaton The automaton
 * @param error Pointer to store the error
 * @return true if an error was found, false otherwise
 */
bool rift_automaton_get_last_error(const rift_regex_automaton_t *automaton,
                                   rift_regex_error_t *error);

/**
 * @brief Clear the last error from an automaton
 *
 * @param automaton The automaton
 */
void rift_automaton_clear_error(rift_regex_automaton_t *automaton);

/**
 * @brief Minimize a DFA using Hopcroft's algorithm
 *
 * @param dfa The DFA to minimize
 * @param error Pointer to store error code (can be NULL)
 * @return The minimized DFA or NULL on failure
 */
rift_regex_automaton_t *rift_automaton_minimize_dfa(const rift_regex_automaton_t *dfa,
                                                    rift_regex_error_t *error);

/**
 * @brief Optimize an automaton by minimizing its states and transitions
 *
 * @param automaton The automaton to optimize
 * @return true if successful, false otherwise
 */
bool rift_automaton_optimize(rift_regex_automaton_t *automaton);
/**
 * @brief Minimize an automaton
 *
 * @param automaton The automaton to minimize
 * @return true if successful, false otherwise
 */
bool rift_automaton_minimize(rift_regex_automaton_t *automaton);

/**
 * @brief Set a state as accepting or not
 *
 * @param state The state to modify
 * @param is_accepting Whether the state should be accepting
 * @return true if successful, false otherwise
 */
bool rift_automaton_set_state_accepting(rift_regex_automaton_t *automaton,
                                        rift_regex_state_t *state, bool is_accepting);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_AUTOMATON_AUTOMATON_H */