/**
 * @file minimizer.h
 * @brief DFA minimization functionality for the LibRift regex engine
 *
 * This file defines the interface for DFA minimization in the LibRift regex engine,
 * implementing Hopcroft's algorithm for state reduction to create smaller,
 * more efficient automata with equivalent language recognition.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "core/automaton/automaton.h"
#include "core/errors/regex_error.h"
#ifndef LIBRIFT_REGEX_AUTOMATON_MINIMIZER_H
#define LIBRIFT_REGEX_AUTOMATON_MINIMIZER_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Minimize a DFA to create an equivalent automaton with fewer states
 *
 * This function applies Hopcroft's algorithm to minimize a deterministic
 * finite automaton (DFA), producing an equivalent automaton with the minimal
 * number of states. Only DFAs can be directly minimized. If an NFA is provided,
 * it will first be converted to a DFA before minimization.
 *
 * @param automaton The automaton to minimize (must be a DFA)
 * @param error Pointer to store error information (can be NULL)
 * @return A new, minimized automaton or NULL on failure
 */
rift_regex_automaton_t *rift_automaton_minimize_dfa(const rift_regex_automaton_t *automaton,
                                                    rift_regex_error_t *error);

/**
 * @brief Validate that a minimized automaton is equivalent to the original
 *
 * This function performs basic validation to check if a minimized automaton
 * is equivalent to the original automaton. It ensures that both automata
 * accept the same language.
 *
 * @param original The original automaton
 * @param minimized The minimized automaton
 * @return 1 if equivalent, 0 if not, negative on error
 */
int rift_automaton_validate_minimization(const rift_regex_automaton_t *original,
                                         const rift_regex_automaton_t *minimized);

/**
 * @brief Optimize transitions in an automaton for better performance
 *
 * This function optimizes the transitions in an automaton to improve
 * matching performance, typically by reordering transitions based on
 * frequency or other heuristics.
 *
 * @param automaton The automaton to optimize
 * @return true if optimization was successful, false otherwise
 */
bool rift_automaton_optimize_transitions(rift_regex_automaton_t *automaton);

/**
 * @brief Remove unreachable states from an automaton
 *
 * This function removes states that cannot be reached from the initial
 * state, reducing the size of the automaton without changing its behavior.
 *
 * @param automaton The automaton to process
 * @return true if successful, false otherwise
 */
bool rift_automaton_remove_unreachable_states(rift_regex_automaton_t *automaton);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_AUTOMATON_MINIMIZER_H */