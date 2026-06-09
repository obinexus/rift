/**
 * @file transition.h
 * @brief Transition functions for the LibRift regex automaton
 *
 * This file defines the Transition structure and functions for creating and manipulating
 * transitions between states in the automaton component of the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <regex.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/automaton/state.h"
#include "core/errors/regex_error.h"
#ifndef LIBRIFT_REGEX_AUTOMATON_TRANSITION_H
#define LIBRIFT_REGEX_AUTOMATON_TRANSITION_H



#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration to break circular dependency */
struct rift_regex_pattern;
typedef struct rift_regex_pattern rift_regex_pattern_t;

/* Type alias for the transition structure */
typedef struct rift_regex_transition rift_regex_transition_t;
struct rift_regex_transition {
    struct rift_regex_state *from_state; /**< Source state */
    struct rift_regex_state *to_state;   /**< Target state */
    char *input_pattern;                 /**< Pattern that triggers this transition */
    bool is_epsilon;                     /**< Whether this is an epsilon transition */
    int priority;                        /**< Priority for deterministic resolution */
    struct rift_regex_pattern *pattern;  /**< Associated regex pattern */
};

/**
 * @brief Create a transition between states
 *
 * @param from_state The source state
 * @param to_state The target state
 * @param input_pattern The pattern that triggers this transition
 * @return The created transition or NULL on failure
 */
rift_regex_transition_t *rift_transition_create(struct rift_regex_state *from_state,
                                                struct rift_regex_state *to_state,
                                                const char *input_pattern);

/**
 * @brief Create an epsilon transition
 *
 * @param from_state The source state
 * @param to_state The target state
 * @return The created transition or NULL on failure
 */
rift_regex_transition_t *rift_transition_create_epsilon(struct rift_regex_state *from_state,
                                                        struct rift_regex_state *to_state);

/**
 * @brief Free a transition
 *
 * @param transition The transition to free
 */
void rift_transition_free(rift_regex_transition_t *transition);

/**
 * @brief Check if a transition is an epsilon transition
 *
 * @param transition The transition to check
 * @return true if the transition is an epsilon transition, false otherwise
 */
bool rift_transition_is_epsilon(const rift_regex_transition_t *transition);

/**
 * @brief Get the pattern for a transition
 *
 * @param transition The transition
 * @return The pattern or NULL for epsilon transitions
 */
const char *rift_transition_get_pattern(const rift_regex_transition_t *transition);

/**
 * @brief Get the target state for a transition
 *
 * @param transition The transition
 * @return The target state or NULL if the transition is invalid
 */
struct rift_regex_state *rift_transition_get_target(const rift_regex_transition_t *transition);

/**
 * @brief Get the source state for a transition
 *
 * @param transition The transition
 * @return The source state or NULL if the transition is invalid
 */
struct rift_regex_state *rift_transition_get_source(const rift_regex_transition_t *transition);

/**
 * @brief Check if a transition matches an input character
 *
 * @param transition The transition to check
 * @param c The character to match against
 * @return true if the transition matches the character, false otherwise
 */
bool rift_transition_matches_char(const rift_regex_transition_t *transition, char c);

/**
 * @brief Check if a transition matches an input string
 *
 * @param transition The transition to check
 * @param input The string to match against
 * @return true if the transition matches the string, false otherwise
 */
bool rift_transition_matches(const rift_regex_transition_t *transition, const char *input);

/**
 * @brief Set the priority of a transition
 *
 * @param transition The transition
 * @param priority The priority value
 * @return true if successful, false otherwise
 */
bool rift_transition_set_priority(rift_regex_transition_t *transition, int priority);

/**
 * @brief Get the priority of a transition
 *
 * @param transition The transition
 * @return The priority value
 */
int rift_transition_get_priority(const rift_regex_transition_t *transition);

/**
 * @brief Clone a transition
 *
 * @param transition The transition to clone
 * @return A new transition that is a copy of the original, or NULL on failure
 */
rift_regex_transition_t *rift_transition_clone(const rift_regex_transition_t *transition);

/**
 * @brief Compare two transitions for equality
 *
 * @param t1 The first transition
 * @param t2 The second transition
 * @return true if the transitions are equal, false otherwise
 */
bool rift_transitions_are_equal(const rift_regex_transition_t *t1,
                                const rift_regex_transition_t *t2);

/**
 * @brief Sort an array of transitions by priority (highest first)
 *
 * @param transitions The array of transitions to sort
 * @param count The number of transitions in the array
 */
void rift_sort_transitions_by_priority(rift_regex_transition_t **transitions, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_AUTOMATON_TRANSITION_H */