/**
core/runtime/context.h"
 * @file context.c
 * @brief Implementation of matching context for the LibRift regex engine
 *
 * This file implements the matching context functionality of the LibRift regex engine,
 * which manages the state during pattern execution with proper thread safety.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/runtime/context.h
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
ontext.h"/a #include "core/config/config.h"
runtime/context.h
tmp/librift_analysis/librift-4933472-W/src/core/runtime/context.h
tmp/librift_analysis/librift-4933472-W/src/runtime/context.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/runtime/context.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/runtime/context.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/runtime/context.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/runtime/context.h"
ontext.h"/a #include "core/config/config.h"


/**
 * @brief Create a new matcher context
 *
 * @param input Input string to match against
 * @param input_length Length of the input string
 * @param max_capture_groups Maximum number of capture groups to support
 * @return Pointer to the new matcher context or NULL on failure
 */
rift_regex_matcher_context_t *
rift_matcher_context_create(const char *input, size_t input_length, size_t max_capture_groups)
{
    // Use strlen if input_length is not specified
    if (input && input_length == (size_t)-1) {
        input_length = strlen(input);
    }

    // Allocate the context structure
    rift_regex_matcher_context_t *context =
        (rift_regex_matcher_context_t *)rift_malloc(sizeof(rift_regex_matcher_context_t));
    if (!context) {
        return NULL;
    }

    // Initialize the context
    context->input = input;
    context->input_length = input && input_length > 0 ? input_length : 0;
    context->current_position = 0;
    context->position = 0; // Keep legacy field in sync
    context->max_capture_groups = max_capture_groups;
    context->capture_groups = NULL;

    // Create the capture groups container if needed
    if (max_capture_groups > 0) {
        context->capture_groups = rift_capture_groups_create(max_capture_groups);
        if (!context->capture_groups) {
            rift_free(context);
            return NULL;
        }
    }

    return context;
}

/**
 * @brief Free resources associated with a matcher context
 *
 * @param context The matcher context to free
 */
void
rift_matcher_context_free(rift_regex_matcher_context_t *context)
{
    if (!context) {
        return;
    }

    // Free capture groups
    if (context->capture_groups) {
        rift_capture_groups_free(context->capture_groups);
    }

    // Free the context itself
    rift_free(context);
}

/**
 * @brief Reset the matcher context to its initial state
 *
 * @param context The matcher context
 * @param pattern Optional pattern reference for specialized reset behavior
 */
void
rift_matcher_context_reset(rift_regex_matcher_context_t *context,
                           const rift_regex_pattern_t *pattern)
{
    if (!context) {
        return;
    }

    // Reset positions
    context->current_position = 0;
    context->position = 0; // Keep legacy field in sync

    // Reset capture groups
    if (context->capture_groups) {
        rift_capture_groups_reset(context->capture_groups);
    }

    // Pattern-specific reset logic could be added here if needed
    (void)pattern; // Unused parameter for now
}

/**
 * @brief Set the input string for the matcher context
 *
 * @param context The matcher context
 * @param input New input string
 * @param input_length Length of the new input string
 * @return true if successful, false otherwise
 */
bool
rift_matcher_context_set_input(rift_regex_matcher_context_t *context, const char *input,
                               size_t input_length)
{
    if (!context) {
        return false;
    }

    // Use strlen if input_length is not specified
    if (input && input_length == (size_t)-1) {
        input_length = strlen(input);
    }

    // Update the context
    context->input = input;
    context->input_length = input && input_length > 0 ? input_length : 0;
    context->current_position = 0;
    context->position = 0; // Keep legacy field in sync

    // Reset capture groups
    if (context->capture_groups) {
        rift_capture_groups_reset(context->capture_groups);
    }

    return true;
}

/**
 * @brief Get the current position in the input string
 *
 * @param context The matcher context
 * @return Current position
 */
size_t
rift_matcher_context_get_position(const rift_regex_matcher_context_t *context)
{
    if (!context) {
        return 0;
    }

    return context->current_position;
}

/**
 * @brief Set the current position in the input string
 *
 * @param context The matcher context
 * @param position New position
 * @return true if successful, false if position is out of bounds
 */
bool
rift_matcher_context_set_position(rift_regex_matcher_context_t *context, size_t position)
{
    if (!context) {
        return false;
    }

    // Validate position
    if (position > context->input_length) {
        return false;
    }

    // Update positions (maintain consistency between position fields)
    context->current_position = position;
    context->position = position; // Keep legacy field in sync
    return true;
}

/**
 * @brief Advance the current position by one character
 *
 * @param context The matcher context
 * @return true if successful, false if at end of input
 */
bool
rift_matcher_context_advance(rift_regex_matcher_context_t *context)
{
    if (!context) {
        return false;
    }

    // Check if we're already at the end
    if (context->current_position >= context->input_length) {
        return false;
    }

    // Advance position (maintain consistency between position fields)
    context->current_position++;
    context->position = context->current_position; // Keep legacy field in sync
    return true;
}

/**
 * @brief Create a match result from the current context state
 *
 * @param context The matcher context
 * @param start_pos Start position of the match
 * @param end_pos End position of the match
 * @return Pointer to the new match result or NULL on failure
 */
rift_regex_match_result_t *
rift_matcher_context_create_match_result(const rift_regex_matcher_context_t *context,
                                         size_t start_pos, size_t end_pos)
{
    if (!context || start_pos > end_pos || end_pos > context->input_length || !context->input) {
        return NULL;
    }

    // Allocate the match result structure
    rift_regex_match_result_t *result =
        (rift_regex_match_result_t *)rift_malloc(sizeof(rift_regex_match_result_t));
    if (!result) {
        return NULL;
    }

    // Initialize the result
    result->start_pos = start_pos;
    result->end_pos = end_pos;
    result->groups = NULL;
    result->group_count = 0;
    result->matched_text = NULL;

    // Copy the matched text
    size_t match_length = end_pos - start_pos;
    result->matched_text = (char *)rift_malloc(match_length + 1);
    if (!result->matched_text) {
        rift_free(result);
        return NULL;
    }
    memcpy(result->matched_text, context->input + start_pos, match_length);
    result->matched_text[match_length] = '\0';

    // Copy capture groups data if available
    if (context->capture_groups) {
        size_t num_groups = rift_capture_groups_get_count(context->capture_groups);
        if (num_groups > 0) {
            // Allocate space for group data
            result->groups = rift_malloc(sizeof(struct {
                                             char *name;
                                             char *value;
                                             size_t start;
                                             size_t end;
                                         }) *
                                         num_groups);

            if (!result->groups) {
                rift_free(result->matched_text);
                rift_free(result);
                return NULL;
            }

            result->group_count = num_groups;

            // Copy each group's data
            for (size_t i = 0; i < num_groups; i++) {
                rift_regex_capture_group_t *group =
                    rift_capture_groups_get_by_index(context->capture_groups, i);

                if (group) {
                    size_t group_start = rift_capture_group_get_start(group);
                    size_t group_end = rift_capture_group_get_end(group);

                    // Handle group name
                    const char *name = rift_capture_group_get_name(group);
                    if (name) {
                        result->groups[i].name = rift_strdup(name);
                    } else {
                        result->groups[i].name = NULL;
                    }

                    // Copy group content
                    if (group_start != (size_t)-1 && group_end != (size_t)-1 &&
                        group_end > group_start && group_end <= context->input_length) {

                        size_t value_len = group_end - group_start;
                        result->groups[i].value = rift_malloc(value_len + 1);

                        if (result->groups[i].value) {
                            memcpy(result->groups[i].value, context->input + group_start,
                                   value_len);
                            result->groups[i].value[value_len] = '\0';
                        } else {
                            result->groups[i].value = NULL;
                        }
                    } else {
                        result->groups[i].value = NULL;
                    }

                    result->groups[i].start = group_start;
                    result->groups[i].end = group_end;
                } else {
                    result->groups[i].name = NULL;
                    result->groups[i].value = NULL;
                    result->groups[i].start = (size_t)-1;
                    result->groups[i].end = (size_t)-1;
                }
            }
        }
    }

    return result;
}

/**
 * @brief Free resources associated with a match result
 *
 * @param result The match result to free
 */
void
rift_match_result_free(rift_regex_match_result_t *result)
{
    if (!result) {
        return;
    }

    // Free matched text
    if (result->matched_text) {
        rift_free(result->matched_text);
    }

    // Free groups
    if (result->groups) {
        for (size_t i = 0; i < result->group_count; i++) {
            if (result->groups[i].name) {
                rift_free(result->groups[i].name);
            }
            if (result->groups[i].value) {
                rift_free(result->groups[i].value);
            }
        }
        rift_free(result->groups);
    }

    // Free the result structure
    rift_free(result);
}

/**
 * @brief Get the input string from the matcher context
 *
 * @param context The matcher context
 * @return Pointer to the input string
 */
const char *
rift_matcher_context_get_input(const rift_regex_matcher_context_t *context)
{
    if (!context) {
        return NULL;
    }

    return context->input;
}

/**
 * @brief Get the input length from the matcher context
 *
 * @param context The matcher context
 * @return Length of the input string
 */
size_t
rift_matcher_context_get_input_length(const rift_regex_matcher_context_t *context)
{
    if (!context) {
        return 0;
    }

    return context->input_length;
}

/**
 * @brief Get the capture groups container from the matcher context
 *
 * @param context The matcher context
 * @return Pointer to the capture groups container
 */
rift_regex_capture_groups_t *
rift_matcher_context_get_capture_groups(const rift_regex_matcher_context_t *context)
{
    if (!context) {
        return NULL;
    }

    return context->capture_groups;
}

/**
 * @brief Get the current character at the input position
 *
 * @param context The matcher context
 * @return Current character or '\0' if at end of input
 */
char
rift_matcher_context_get_current_char(const rift_regex_matcher_context_t *context)
{
    if (!context || !context->input || context->current_position >= context->input_length) {
        return '\0';
    }

    return context->input[context->current_position];
}

/**
 * @brief Check if the matcher context is at the end of input
 *
 * @param context The matcher context
 * @return true if at end of input, false otherwise
 */
bool
rift_matcher_context_is_at_end(const rift_regex_matcher_context_t *context)
{
    if (!context) {
        return true;
    }

    return context->current_position >= context->input_length;
}

/**
 * @brief Create a new regex context wrapper
 *
 * @param matcher_context The matcher context to wrap
 * @param backtracker The backtracker to associate with this context
 * @return Pointer to the new regex context or NULL on failure
 */
rift_regex_context_t *
rift_regex_context_create(rift_regex_matcher_context_t *matcher_context,
                          rift_regex_backtracker_t *backtracker)
{
    if (!matcher_context) {
        return NULL;
    }

    rift_regex_context_t *context =
        (rift_regex_context_t *)rift_malloc(sizeof(rift_regex_context_t));
    if (!context) {
        return NULL;
    }

    // Initialize the context
    context->matcher_context = matcher_context;
    context->backtracker = backtracker;
    context->groups = matcher_context->capture_groups;
    context->match_result = NULL;

    // Initialize input references
    context->input = matcher_context->input;
    context->input_length = matcher_context->input_length;
    context->current_position = matcher_context->current_position;

    // Initialize error state
    context->error.code = RIFT_REGEX_ERROR_NONE;
    context->error.message[0] = '\0';
    context->error.position = 0;

    return context;
}

/**
 * @brief Free resources associated with a regex context
 *
 * @param context The regex context to free
 * @param free_matcher_context Whether to also free the associated matcher context
 */
void
rift_regex_context_free(rift_regex_context_t *context, bool free_matcher_context)
{
    if (!context) {
        return;
    }

    // Free match result if it exists
    if (context->match_result) {
        rift_match_result_free(context->match_result);
    }

    // Free matcher context if requested
    if (free_matcher_context && context->matcher_context) {
        rift_matcher_context_free(context->matcher_context);
    }

    // Free the context itself
    rift_free(context);
}
