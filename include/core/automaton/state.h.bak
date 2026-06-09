/**
 * @file state.h
 * @brief State structure and functions for the LibRift regex automaton
 *
 * This file defines the State structure and functions for creating and manipulating
 * automaton states in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <fcntl.h>
#include <regex.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "core/automaton/flags.h"
#include "core/errors/error.h" // For status codes
#include "core/errors/regex_error.h"
#include "core/memory/memory.h" // For memory functions
#ifndef LIBRIFT_REGEX_AUTOMATON_STATE_H
#define LIBRIFT_REGEX_AUTOMATON_STATE_H



#ifdef __cplusplus
extern "C" {
#endif
/* Forward declaration for maximum capture groups with default value */
#ifndef RIFT_MAX_CAPTURE_GROUPS
#define RIFT_MAX_CAPTURE_GROUPS 10 /**< Default maximum number of capture groups */
#endif
/**
 * @brief State flags enumeration
 */
typedef enum rift_state_flag {
    RIFT_STATE_FLAG_NONE = 0,             /**< No flags */
    RIFT_STATE_FLAG_ANCHOR_START = 1,     /**< Start anchor (^) */
    RIFT_STATE_FLAG_ANCHOR_END = 2,       /**< End anchor ($) */
    RIFT_STATE_FLAG_WORD_BOUNDARY = 4,    /**< Word boundary (\b) */
    RIFT_STATE_FLAG_NOT_WORD_BOUNDARY = 8 /**< Not word boundary (\B) */
} rift_state_flag_t;

/* Type alias for the state structure */
typedef struct rift_regex_state rift_regex_state_t;
struct rift_regex_state {
    size_t id;                                  /**< Unique identifier */
    bool is_accepting;                          /**< Whether this is an accepting state */
    char *pattern;                              /**< Pattern associated with this state */
    struct rift_regex_transition **transitions; /**< Array of outgoing transitions */
    size_t num_transitions;                     /**< Number of transitions */
    size_t transition_capacity;                 /**< Capacity of transitions array */
    void *user_data;                            /**< User data associated with the state */

    /* Capturing group information */
    char *group_name;    /**< Name of the capturing group (if any) */
    bool is_group_start; /**< Whether this state marks the start of a group */
    bool is_group_end;   /**< Whether this state marks the end of a group */

    size_t capture_group_ends[RIFT_MAX_CAPTURE_GROUPS]; /**< End positions for capture groups */

    /* Flags for special features */
    rift_state_flag_t flags; /**< Special state flags */
};

/**
 * @brief Set a state flag in the automaton
 *
 * @param state The state to modify
 * @param flag The flag to set
 * @return true if successful, false otherwise
 */
bool rift_automaton_set_state_flag(rift_regex_state_t *state, rift_regex_flags_t flag);

/**
 * @brief Set a state as an accepting state
 * @param state The state to modify
 */

bool rift_automaton_state_set_accepting(rift_regex_state_t *state, bool is_accepting);
/**
 *
 * @brief Create a state object (standalone, not added to an automaton)
 *
 * @param is_accepting Whether this is an accepting state
 * @return The newly created state or NULL on failure
 */
rift_regex_state_t *rift_state_create(bool is_accepting);

/**
 * @brief Create a state in an automaton
 *
 * @param automaton The automaton to add the state to
 * @param is_accepting Whether this is an accepting state
 * @return The newly created state or NULL on failure
 */

/**
 * @brief Free a state and all its resources
 *
 * @param state The state to free
 */
void rift_state_free(rift_regex_state_t *state);

/**
 * @brief Clone a state
 *
 * This function creates a copy of a state without its transitions.
 *
 * @param state The state to clone
 * @return A new state that is a copy of the original, or NULL on failure
 */
rift_regex_state_t *rift_state_clone(const rift_regex_state_t *state);

/**
 * @brief Set the pattern for a state
 *
 * @param state The state
 * @param pattern The pattern to set
 * @return true if successful, false otherwise
 */
bool rift_state_set_pattern(rift_regex_state_t *state, const char *pattern);

/**
 * @brief Get the pattern for a state
 *
 * @param state The state
 * @return The pattern or NULL if not set
 */
const char *rift_state_get_pattern(const rift_regex_state_t *state);

/**
 * @brief Check if a state is accepting
 *
 * @param state The state
 * @return true if the state is accepting, false otherwise
 */
bool rift_state_is_accepting(const rift_regex_state_t *state);

/**
 * @brief Set whether a state is accepting
 *
 * @param state The state
 * @param is_accepting Whether the state should be accepting
 * @return true if successful, false otherwise
 */
bool rift_state_set_accepting(rift_regex_state_t *state, bool is_accepting);

/**
 * @brief Get the ID of a state
 *
 * @param state The state
 * @return The state ID or 0 if the state is NULL
 */
size_t rift_state_get_id(const rift_regex_state_t *state);

/**
 * @brief Set a specific ID for a state
 *
 * This function should only be used during automaton serialization/deserialization.
 *
 * @param state The state
 * @param id The ID to set
 * @return true if successful, false otherwise
 */
bool rift_state_set_id(rift_regex_state_t *state, size_t id);

/**
 * @brief Set user data for a state
 *
 * @param state The state
 * @param user_data The user data to set
 * @return true if successful, false otherwise
 */
bool rift_state_set_user_data(rift_regex_state_t *state, void *user_data);

/**
 * @brief Get user data from a state
 *
 * @param state The state
 * @return The user data or NULL if not set or the state is NULL
 */
void *rift_state_get_user_data(const rift_regex_state_t *state);

/**
 * @brief Get the number of transitions from a state
 *
 * @param state The state
 * @return The number of transitions or 0 if the state is NULL
 */
size_t rift_state_get_transition_count(const rift_regex_state_t *state);

/**
 * @brief Get a transition from a state by index
 *
 * @param state The state
 * @param index The index of the transition
 * @return The transition or NULL if the index is out of bounds or the state is NULL
 */
struct rift_regex_transition *rift_state_get_transition(const rift_regex_state_t *state,
                                                        size_t index);

/**
 * @brief Add a transition from a state
 *
 * @param from_state The source state
 * @param to_state The target state
 * @param input_pattern The pattern that triggers this transition
 * @return true if successful, false otherwise
 */
bool rift_state_add_transition(rift_regex_state_t *from_state, rift_regex_state_t *to_state,
                               const char *input_pattern);

/**
 * @brief Add an epsilon transition (no input required)
 *
 * @param from_state The source state
 * @param to_state The target state
 * @return true if successful, false otherwise
 */
bool rift_state_add_epsilon_transition(rift_regex_state_t *from_state,
                                       rift_regex_state_t *to_state);

/**
 * @brief Remove a transition from a state
 *
 * @param state The state
 * @param index The index of the transition to remove
 * @return true if the transition was removed successfully, false otherwise
 */
bool rift_state_remove_transition(rift_regex_state_t *state, size_t index);

/**
 * @brief Compare two states for equivalence
 *
 * @param state1 The first state
 * @param state2 The second state
 * @return true if the states are equivalent, false otherwise
 */
bool rift_state_are_equivalent(const rift_regex_state_t *state1, const rift_regex_state_t *state2);

/**
 * @brief Set a state flag
 *
 * @param state The state to modify
 * @param flag The flag to set
 * @return true if successful, false otherwise
 */
bool rift_state_set_flag(rift_regex_state_t *state, rift_state_flag_t flag);

/**
 * @brief Check if a state has a specific flag
 *
 * @param state The state to check
 * @param flag The flag to check for
 * @return true if the state has the flag, false otherwise
 */
bool rift_state_has_flag(const rift_regex_state_t *state, rift_state_flag_t flag);

/**
 * @brief Set a group name for a state
 *
 * @param state The state to modify
 * @param name The group name to set
 * @return true if successful, false otherwise
 */
bool rift_state_set_group_name(rift_regex_state_t *state, const char *name);

/**
 * @brief Get the group name from a state
 *
 * @param state The state
 * @return The group name or NULL if not set
 */
const char *rift_state_get_group_name(const rift_regex_state_t *state);

/**
 * @brief Mark a state as a group start
 *
 * @param state The state to modify
 * @param is_group_start Whether the state is a group start
 * @return true if successful, false otherwise
 */
bool rift_state_set_group_start(rift_regex_state_t *state, bool is_group_start);

/**
 * @brief Check if a state is a group start
 *
 * @param state The state to check
 * @return true if the state is a group start, false otherwise
 */
bool rift_state_is_group_start(const rift_regex_state_t *state);

/**
 * @brief Mark a state as a group end
 *
 * @param state The state to modify
 * @param is_group_end Whether the state is a group end
 * @return true if successful, false otherwise
 */
bool rift_state_set_group_end(rift_regex_state_t *state, bool is_group_end);

/**
 * @brief Check if a state is a group end
 *
 * @param state The state to check
 * @return true if the state is a group end, false otherwise
 */
bool rift_state_is_group_end(const rift_regex_state_t *state);

/**
 * @brief Reset the state ID counter
 *
 * This function should only be used for testing purposes.
 */
void rift_state_reset_id_counter(void);

/**
 * @brief Get the next available state ID
 *
 * @return The next available state ID
 */
size_t rift_state_get_next_id(void);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_AUTOMATON_STATE_H */