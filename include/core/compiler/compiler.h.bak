/**
 * @file compiler.h
 * @brief Compiler for the LibRift regex engine
 *
 * This file defines the compiler component of the LibRift regex engine,
 * which converts AST representations of patterns into automaton structures.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "core//automaton/automaton.h"
#include "core//automaton/flags.h"
#include "core//automaton/state.h"
#include "core//automaton/transition.h"
#include "core//errors/regex_error.h"
#include "core//parser/ast.h"
#include "core//parser/parser.h"
#ifndef LIBRIFT_REGEX_COMPILER_COMPILER_H
#define LIBRIFT_REGEX_COMPILER_COMPILER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */
#define MAX_STATES 1000
#define RIFT_REGEX_MAX_STATES 1000

/**
 * @brief Helper function to create an epsilon transition in the automaton
 *
 * @param automaton The automaton to modify
 * @param from_state The source state
 * @param to_state The target state
 * @return true if successful, false otherwise
 */
bool rift_automaton_create_epsilon_transition(rift_regex_automaton_t *automaton,
                                              rift_regex_state_t *from_state,
                                              rift_regex_state_t *to_state);

/**
 * @brief Compile an AST into an automaton structure
 *
 * This function traverses the AST and constructs the corresponding NFA,
 * which is then optionally converted to a DFA for optimization.
 *
 * @param ast The Abstract Syntax Tree representing the pattern
 * @param flags Compilation flags
 * @param error Pointer to store error code (can be NULL)
 * @return The compiled automaton or NULL on failure
 */
rift_regex_automaton_t *rift_regex_compile_ast(const rift_regex_ast_t *ast,
                                               rift_regex_flags_t flags, rift_regex_error_t *error);

/**
 * @brief Compile a pattern string into an automaton structure
 *
 * This function parses the pattern string into an AST and then
 * compiles it into an automaton for pattern matching.
 *
 * @param pattern The regex pattern string
 * @param flags Compilation flags
 * @param error Pointer to store error code (can be NULL)
 * @return The compiled automaton or NULL on failure
 */
rift_regex_automaton_t *rift_regex_compile_pattern(const char *pattern, rift_regex_flags_t flags,
                                                   rift_regex_error_t *error);

/**
 * @brief Apply optimizations to an automaton
 *
 * This function applies various optimizations to the automaton to improve
 * matching performance.
 *
 * @param automaton The automaton to optimize
 * @param flags Optimization flags
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool rift_regex_optimize_automaton(rift_regex_automaton_t *automaton, rift_regex_flags_t flags,
                                   rift_regex_error_t *error);

/**
 * @brief Get a string describing a pattern compilation error
 *
 * @param error The error information
 * @return A string describing the error
 */
const char *rift_regex_get_compile_error_message(rift_regex_error_t error);

/**
 * @brief Build automaton from AST (internal function)
 *
 * @param automaton The automaton to build
 * @param ast The AST to process
 * @param flags Compilation flags
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool build_automaton_from_ast(rift_regex_automaton_t *automaton, const rift_regex_ast_t *ast,
                              rift_regex_flags_t flags, rift_regex_error_t *error);

/**
 * @brief Convert an AST node to an NFA (internal function)
 *
 * @param automaton The automaton to build
 * @param node The AST node to process
 * @param start_state Pointer to store the start state
 * @param end_state Pointer to store the end state
 * @param flags Compilation flags
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool convert_node_to_nfa(rift_regex_automaton_t *automaton, const rift_regex_ast_node_t *node,
                         rift_regex_state_t **start_state, rift_regex_state_t **end_state,
                         rift_regex_flags_t flags, rift_regex_error_t *error);

/**
 * @brief Handle an alternation node (internal function)
 *
 * @param automaton The automaton to build
 * @param node The alternation node to process
 * @param start_state Pointer to store the start state
 * @param end_state Pointer to store the end state
 * @param flags Compilation flags
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool handle_alternation(rift_regex_automaton_t *automaton, const rift_regex_ast_node_t *node,
                        rift_regex_state_t **start_state, rift_regex_state_t **end_state,
                        rift_regex_flags_t flags, rift_regex_error_t *error);

/**
 * @brief Handle a concatenation node (internal function)
 *
 * @param automaton The automaton to build
 * @param node The concatenation node to process
 * @param start_state Pointer to store the start state
 * @param end_state Pointer to store the end state
 * @param flags Compilation flags
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool handle_concatenation(rift_regex_automaton_t *automaton, const rift_regex_ast_node_t *node,
                          rift_regex_state_t **start_state, rift_regex_state_t **end_state,
                          rift_regex_flags_t flags, rift_regex_error_t *error);

/**
 * @brief Handle a quantifier node (internal function)
 *
 * @param automaton The automaton to build
 * @param node The quantifier node to process
 * @param start_state Pointer to store the start state
 * @param end_state Pointer to store the end state
 * @param flags Compilation flags
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool handle_quantifier(rift_regex_automaton_t *automaton, const rift_regex_ast_node_t *node,
                       rift_regex_state_t **start_state, rift_regex_state_t **end_state,
                       rift_regex_flags_t flags, rift_regex_error_t *error);

// Function prototype for parse_quantifier_values
bool parse_quantifier_values(const char *pattern, size_t *min, size_t *max, bool *is_greedy);

/**
 * @brief Handle a character class node (internal function)
 *
 * @param automaton The automaton to build
 * @param node The character class node to process
 * @param start_state Pointer to store the start state
 * @param end_state Pointer to store the end state
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool handle_character_class(rift_regex_automaton_t *automaton, const rift_regex_ast_node_t *node,
                            rift_regex_state_t **start_state, rift_regex_state_t **end_state,
                            rift_regex_error_t *error);

/**
 * @brief Handle a literal node (internal function)
 *
 * @param automaton The automaton to build
 * @param node The literal node to process
 * @param start_state Pointer to store the start state
 * @param end_state Pointer to store the end state
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool handle_literal(rift_regex_automaton_t *automaton, const rift_regex_ast_node_t *node,
                    rift_regex_state_t **start_state, rift_regex_state_t **end_state,
                    rift_regex_error_t *error);

/**
 * @brief Handle a group node (internal function)
 *
 * @param automaton The automaton to build
 * @param node The group node to process
 * @param start_state Pointer to store the start state
 * @param end_state Pointer to store the end state
 * @param flags Compilation flags
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool handle_group(rift_regex_automaton_t *automaton, const rift_regex_ast_node_t *node,
                  rift_regex_state_t **start_state, rift_regex_state_t **end_state,
                  rift_regex_flags_t flags, rift_regex_error_t *error);

/**
 * @brief Handle an anchor node (internal function)
 *
 * @param automaton The automaton to build
 * @param node The anchor node to process
 * @param start_state Pointer to store the start state
 * @param end_state Pointer to store the end state
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool handle_anchor(rift_regex_automaton_t *automaton, const rift_regex_ast_node_t *node,
                   rift_regex_state_t **start_state, rift_regex_state_t **end_state,
                   rift_regex_error_t *error);

/**
 * @brief Handle a backreference node (internal function)
 *
 * @param automaton The automaton to build
 * @param node The backreference node to process
 * @param start_state Pointer to store the start state
 * @param end_state Pointer to store the end state
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool handle_backreference(rift_regex_automaton_t *automaton, const rift_regex_ast_node_t *node,
                          rift_regex_state_t **start_state, rift_regex_state_t **end_state,
                          rift_regex_error_t *error);

/**
 * @brief Handle a boundary node (internal function)
 *
 * @param automaton The automaton to build
 * @param node The boundary node to process
 * @param start_state Pointer to store the start state
 * @param end_state Pointer to store the end state
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool handle_boundary(rift_regex_automaton_t *automaton, const rift_regex_ast_node_t *node,
                     rift_regex_state_t **start_state, rift_regex_state_t **end_state,
                     rift_regex_error_t *error);

/**
 * @brief Handle a character node (internal function)
 *
 * @param automaton The automaton to build
 * @param node The character node to process
 * @param start_state Pointer to store the start state
 * @param end_state Pointer to store the end state
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool handle_character(rift_regex_automaton_t *automaton, const rift_regex_ast_node_t *node,
                      rift_regex_state_t **start_state, rift_regex_state_t **end_state,
                      rift_regex_error_t *error);

#ifdef __cplusplus
}
#endif

#endif // LIBRIFT_REGEX_COMPILER_COMPILER_H