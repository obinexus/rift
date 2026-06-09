/**
runtime/backtracker_extension.h
tmp/librift_analysis/librift-4933472-W/src/core/runtime/backtracker_extension.h
tmp/librift_analysis/librift-4933472-W/src/runtime/backtracker_extension.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/runtime/backtracker_extension.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/runtime/backtracker_extension.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/runtime/backtracker_extension.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/runtime/backtracker_extension.h"
core/runtime/backtracker_extension.h"
 #include "librift/runtime/backtracker_extension.h"
 * @file backtracker.c
 * @brief Implementation of backtracking functionality for the LibRift regex engine
 *
 * This file implements the backtracking component of the LibRift regex engine,
 * which is essential for handling complex pattern matching scenarios that
 * require backtracking.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/runtime/backtracker_extension.h
#include <stdlib.h>
#include <string.h>
#include "core/runtime/backtracker.h"
#include "librift/runtime/backtracker.h"

/**
 * @file backtracker_extension.c
 * @brief Implementation of extension functions for the backtracker component
 *
 * This file implements the extension functions defined in backtracker_extension.h
 * for the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

 #include "librift/runtime/backtracker_extension.h"

 /**
  * @brief Get the number of groups in the backtracker
  *
  * @param backtracker The backtracker
  * @return The number of groups or 0 if the backtracker is invalid
  */
 size_t
 rift_backtracker_get_num_groups(const rift_regex_backtracker_t *backtracker)
 {
     if (!backtracker) {
         return 0;
     }
 
     /* Access the internal num_groups field of the backtracker */
     return backtracker->num_groups;
 }
static struct rift_regex_backtracker *
create_backtracker(size_t max_depth, size_t num_groups)
{
    struct rift_regex_backtracker *backtracker = malloc(sizeof(struct rift_regex_backtracker));
    if (!backtracker)
        return NULL;

    /* Initialize backtracker */
    backtracker->max_depth = max_depth;
    backtracker->num_groups = num_groups;

    return backtracker;
}

/**
 * @brief Create a new backtracker
 *
 * @param max_depth Maximum depth for backtracking
 * @param num_groups Number of capturing groups
 * @return A new backtracker or NULL on failure
 */
rift_regex_backtracker_t *
rift_backtracker_create(size_t max_depth, size_t num_groups)
{
    rift_regex_backtracker_t *backtracker =
        (rift_regex_backtracker_t *)malloc(sizeof(rift_regex_backtracker_t));
    if (!backtracker) {
        return NULL;
    }

    backtracker->stack_top = NULL;
    backtracker->current_depth = 0;
    backtracker->max_depth = max_depth;
    backtracker->num_groups = num_groups;

    return backtracker;
}

/**
 * @brief Create a backtrack point
 *
 * @param state The current state
 * @param input_position Position in the input string
 * @param group_starts Array of group start positions
 * @param group_ends Array of group end positions
 * @param num_groups Number of groups
 * @return A new backtrack point or NULL on failure
 */
rift_regex_backtrack_point_t *
rift_backtrack_point_create(rift_regex_state_t *state, size_t input_position,
                            const size_t *group_starts, const size_t *group_ends, size_t num_groups)
{
    if (!state || (!group_starts && num_groups > 0) || (!group_ends && num_groups > 0)) {
        return NULL;
    }

    rift_regex_backtrack_point_t *point =
        (rift_regex_backtrack_point_t *)malloc(sizeof(rift_regex_backtrack_point_t));
    if (!point) {
        return NULL;
    }

    point->state = state;
    point->input_position = input_position;
    point->num_groups = num_groups;
    point->next = NULL;

    // Allocate and copy group information if needed
    if (num_groups > 0) {
        point->group_starts = (size_t *)malloc(sizeof(size_t) * num_groups);
        point->group_ends = (size_t *)malloc(sizeof(size_t) * num_groups);

        if (!point->group_starts || !point->group_ends) {
            if (point->group_starts)
                free(point->group_starts);
            if (point->group_ends)
                free(point->group_ends);
            free(point);
            return NULL;
        }

        memcpy(point->group_starts, group_starts, sizeof(size_t) * num_groups);
        memcpy(point->group_ends, group_ends, sizeof(size_t) * num_groups);
    } else {
        point->group_starts = NULL;
        point->group_ends = NULL;
    }

    return point;
}

/**
 * @brief Free a backtrack point
 *
 * @param point The backtrack point to free
 */
void
rift_backtrack_point_free(rift_regex_backtrack_point_t *point)
{
    if (!point) {
        return;
    }

    if (point->group_starts) {
        free(point->group_starts);
    }

    if (point->group_ends) {
        free(point->group_ends);
    }

    free(point);
}

/**
 * @brief Push a new backtrack point onto the stack
 *
 * @param backtracker The backtracker
 * @param state The current state
 * @param input_position Position in the input string
 * @param group_starts Array of group start positions
 * @param group_ends Array of group end positions
 * @param num_groups Number of groups
 * @return true if the point was pushed successfully, false otherwise
 */
bool
rift_backtracker_push(rift_regex_backtracker_t *backtracker, rift_regex_state_t *state,
                      size_t input_position, const size_t *group_starts, const size_t *group_ends,
                      size_t num_groups)
{
    if (!backtracker || !state) {
        return false;
    }

    // Check if we've reached the maximum depth
    if (backtracker->current_depth >= backtracker->max_depth) {
        return false;
    }

    // Create a new backtrack point
    rift_regex_backtrack_point_t *point =
        rift_backtrack_point_create(state, input_position, group_starts, group_ends, num_groups);

    if (!point) {
        return false;
    }

    // Push the point onto the stack
    point->next = backtracker->stack_top;
    backtracker->stack_top = point;
    backtracker->current_depth++;

    return true;
}

/**
 * @brief Pop a backtrack point from the stack
 *
 * @param backtracker The backtracker
 * @param state Pointer to store the state
 * @param input_position Pointer to store the input position
 * @param group_starts Array to store group start positions
 * @param group_ends Array to store group end positions
 * @param num_groups Pointer to store the number of groups
 * @return true if a point was popped, false if the stack was empty
 */
bool
rift_backtracker_pop(rift_regex_backtracker_t *backtracker, rift_regex_state_t **state,
                     size_t *input_position, size_t *group_starts, size_t *group_ends,
                     size_t *num_groups)
{
    if (!backtracker || !state || !input_position || !num_groups) {
        return false;
    }

    // Check if the stack is empty
    if (!backtracker->stack_top) {
        return false;
    }

    // Get the top point
    rift_regex_backtrack_point_t *point = backtracker->stack_top;

    // Extract information
    *state = point->state;
    *input_position = point->input_position;
    *num_groups = point->num_groups;

    // Copy group information if needed
    if (point->num_groups > 0) {
        if (group_starts && point->group_starts) {
            memcpy(group_starts, point->group_starts, sizeof(size_t) * point->num_groups);
        }

        if (group_ends && point->group_ends) {
            memcpy(group_ends, point->group_ends, sizeof(size_t) * point->num_groups);
        }
    }

    // Pop the point
    backtracker->stack_top = point->next;
    backtracker->current_depth--;

    // Free the point
    rift_backtrack_point_free(point);

    return true;
}

/**
 * @brief Check if the backtracker stack is empty
 *
 * @param backtracker The backtracker
 * @return true if the stack is empty, false otherwise
 */
bool
rift_backtracker_is_empty(const rift_regex_backtracker_t *backtracker)
{
    if (!backtracker) {
        return true;
    }

    return backtracker->stack_top == NULL;
}

/**
 * @brief Reset the backtracker
 *
 * @param backtracker The backtracker to reset
 */
void
rift_backtracker_reset(rift_regex_backtracker_t *backtracker)
{
    if (!backtracker) {
        return;
    }

    // Free all backtrack points
    while (backtracker->stack_top) {
        rift_regex_backtrack_point_t *point = backtracker->stack_top;
        backtracker->stack_top = point->next;
        rift_backtrack_point_free(point);
    }

    backtracker->current_depth = 0;
}

/**
 * @brief Get the current depth of the backtracker stack
 *
 * @param backtracker The backtracker
 * @return The current depth
 */
size_t
rift_backtracker_get_depth(const rift_regex_backtracker_t *backtracker)
{
    if (!backtracker) {
        return 0;
    }

    return backtracker->current_depth;
}

/**
 * @brief Get the maximum depth allowed for the backtracker
 *
 * @param backtracker The backtracker
 * @return The maximum allowed depth
 */
size_t
rift_backtracker_get_max_depth(const rift_regex_backtracker_t *backtracker)
{
    if (!backtracker) {
        return 0;
    }

    return backtracker->max_depth;
}

/**
 * @brief Set the maximum depth for the backtracker
 *
 * @param backtracker The backtracker
 * @param max_depth The new maximum depth
 * @return true if successful, false otherwise
 */
bool
rift_backtracker_set_max_depth(rift_regex_backtracker_t *backtracker, size_t max_depth)
{
    if (!backtracker) {
        return false;
    }

    backtracker->max_depth = max_depth;
    return true;
}

/**
 * @brief Peek at the top backtrack point without removing it
 *
 * @param backtracker The backtracker
 * @param state Pointer to store the state
 * @param input_position Pointer to store the input position
 * @param group_starts Array to store group start positions
 * @param group_ends Array to store group end positions
 * @param num_groups Pointer to store the number of groups
 * @return true if a point was peeked, false if the stack was empty
 */
bool
rift_backtracker_peek(const rift_regex_backtracker_t *backtracker, rift_regex_state_t **state,
                      size_t *input_position, size_t *group_starts, size_t *group_ends,
                      size_t *num_groups)
{
    if (!backtracker || !state || !input_position || !num_groups) {
        return false;
    }

    // Check if the stack is empty
    if (!backtracker->stack_top) {
        return false;
    }

    // Get the top point
    rift_regex_backtrack_point_t *point = backtracker->stack_top;

    // Extract information
    *state = point->state;
    *input_position = point->input_position;
    *num_groups = point->num_groups;

    // Copy group information if needed
    if (point->num_groups > 0) {
        if (group_starts && point->group_starts) {
            memcpy(group_starts, point->group_starts, sizeof(size_t) * point->num_groups);
        }

        if (group_ends && point->group_ends) {
            memcpy(group_ends, point->group_ends, sizeof(size_t) * point->num_groups);
        }
    }

    return true;
}

/**
 * @brief Clone a backtracker
 *
 * @param backtracker The backtracker to clone
 * @return A new backtracker that is a copy of the original, or NULL on failure
 */
rift_regex_backtracker_t *
rift_backtracker_clone(const rift_regex_backtracker_t *backtracker)
{
    if (!backtracker) {
        return NULL;
    }

    // Create a new backtracker
    rift_regex_backtracker_t *clone =
        rift_backtracker_create(backtracker->max_depth, backtracker->num_groups);

    if (!clone) {
        return NULL;
    }

    // Clone the stack if not empty
    if (backtracker->stack_top) {
        // Start from the bottom of the stack
        rift_regex_backtrack_point_t **points = NULL;
        size_t count = 0;
        rift_regex_backtrack_point_t *current = backtracker->stack_top;

        // Count the points
        while (current) {
            count++;
            current = current->next;
        }

        // Allocate an array to store the points in reverse order
        points =
            (rift_regex_backtrack_point_t **)malloc(sizeof(rift_regex_backtrack_point_t *) * count);
        if (!points) {
            rift_backtracker_free(clone);
            return NULL;
        }

        // Fill the array
        current = backtracker->stack_top;
        for (size_t i = 0; i < count; i++) {
            points[i] = current;
            current = current->next;
        }

        // Push the points onto the new stack in reverse order
        for (size_t i = count; i > 0; i--) {
            rift_regex_backtrack_point_t *point = points[i - 1];

            if (!rift_backtracker_push(clone, point->state, point->input_position,
                                       point->group_starts, point->group_ends, point->num_groups)) {
                free(points);
                rift_backtracker_free(clone);
                return NULL;
            }
        }

        free(points);
    }

    return clone;
}

/**
 * @brief Free resources associated with a backtracker
 *
 * @param backtracker The backtracker to free
 */
void
rift_backtracker_free(rift_regex_backtracker_t *backtracker)
{
    if (!backtracker) {
        return;
    }

    // Reset the backtracker to free all points
    rift_backtracker_reset(backtracker);

    // Free the backtracker itself
    free(backtracker);
}
