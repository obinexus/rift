/**
core/engine/matcher.h
engine/matcher.h"
 * @file matcher.c
 * @brief Implementation of the pattern matching interface for the LibRift regex engine
 *
 * This file provides the core pattern matching functionality of the LibRift regex engine,
 * handling the execution of regex patterns against input strings with proper backtracking
 * support.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/engine/matcher.h
/**
core/runtime/matcher.h
engine/matcher.h
tmp/librift_analysis/librift-4933472-W/backup/v1/src/engine/matcher.h
tmp/librift_analysis/librift-4933472-W/backup/v2/engine/matcher.h
tmp/librift_analysis/librift-4933472-W/src/core/engine/matcher.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/engine/matcher.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v2/engine/matcher.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/engine/matcher.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/engine/matcher.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/engine/matcher.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v2/engine/matcher.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/engine/matcher.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/engine/matcher.h"
 * @brief Check if the matcher has timed out
 *
 * @param matcher The matcher
 * @return true if timed out, false otherwise
 */
bool check_timeout(rift_regex_matcher_t *matcher);

/**
 * @brief Create a new matcher for a pattern
 *
 * @param pattern The compiled regex pattern
 * @param options Matcher options
 * @return A new matcher or NULL on failure
 */

/**
 * @brief Check if the matcher has timed out
 *
 * @param matcher The matcher
 * @return true if timed out, false otherwise
 */
bool
check_timeout(rift_regex_matcher_t *matcher)
{
    if (!matcher || matcher->timeout_ms == 0) {
        return false;
    }

    clock_t current_time = clock();
    double elapsed_ms = (double)(current_time - matcher->start_time) * 1000.0 / CLOCKS_PER_SEC;

    if (elapsed_ms >= matcher->timeout_ms) {
        matcher->timed_out = true;
        return true;
    }

    return false;
}

/**
 * @brief Create a new matcher for a pattern
 *
 * @param pattern The compiled regex pattern
 * @param options Matcher options
 * @return A new matcher or NULL on failure
 */
rift_regex_matcher_t *
rift_matcher_create(const rift_regex_pattern_t *pattern, rift_matcher_option_t options)
{
    if (!pattern) {
        return NULL;
    }

    // Allocate the matcher structure
    rift_regex_matcher_t *matcher = (rift_regex_matcher_t *)malloc(sizeof(rift_regex_matcher_t));
    if (!matcher) {
        return NULL;
    }

    // Initialize the matcher
    matcher->pattern = pattern;
    matcher->context = NULL; // Will be set when input is provided
    matcher->flags = rift_regex_pattern_get_flags(pattern);
    matcher->options = options;
    matcher->timeout_ms = 0;
    matcher->timed_out = false;
    matcher->start_time = 0;

    // Get the number of capture groups from the pattern
    size_t num_groups = rift_regex_pattern_get_group_count(pattern);

    // Create the backtracker
    // Default max depth is 10000, but this can be changed later

    matcher->backtracker = rift_backtracker_create(10000, num_groups);
    if (!matcher->backtracker) {
        free(matcher);
        return NULL;
    }

    return matcher;
}

// Function moved to bottom of file to avoid duplicate declaration
/**
 * @brief Free resources associated with a matcher
 *
 * @param matcher The matcher to free
 */
void
rift_matcher_free(rift_regex_matcher_t *matcher)
{
    if (!matcher) {
        return;
    }

    // Free the context
    if (matcher->context) {
        rift_matcher_context_free(matcher->context);
    }

    // Free the backtracker
    if (matcher->backtracker) {
        rift_backtracker_free((rift_regex_backtracker_t *)matcher->backtracker);
    }

    // Free the matcher itself
    free(matcher);
}

/**
 * @brief Reset the matcher context to its initial state
 *
 * @param context The matcher context
 */
void
rift_matcher_context_reset(rift_regex_matcher_context_t *context,
                           const rift_regex_pattern_t *pattern)
{
    if (context) {
        rift_matcher_context_set_position(context, 0);
        // Reset capture groups if they exist
        rift_regex_capture_groups_t *groups = rift_matcher_context_get_capture_groups(context);
        if (groups) {
            memset(groups, 0,
                   sizeof(rift_regex_capture_groups_t) *
                       rift_regex_pattern_get_group_count(pattern));
        }
    }
}
/**
 * @brief Reset a matcher to its initial state
 *
 * @param matcher The matcher to reset
 */
void
rift_matcher_reset(rift_regex_matcher_t *matcher)
{
    if (!matcher) {
        return;
    }

    // Reset the context
    if (matcher->context) {
        rift_matcher_context_reset(matcher->context, matcher->pattern);
    }

    // Reset the backtracker
    if (matcher->backtracker) {
        rift_backtracker_reset(matcher->backtracker);
    }

    // Reset timeout status
    matcher->timed_out = false;
}

/**
 * @brief Set the input string for a matcher
 *
 * @param matcher The matcher
 * @param input The input string
 * @param length Length of the input string or (size_t)-1 to use strlen
 * @return true if successful, false otherwise
 */
bool
rift_matcher_set_input(rift_regex_matcher_t *matcher, const char *input, size_t length)
{
    if (!matcher || !input) {
        return false;
    }

    // Use strlen if length is not specified
    if (length == (size_t)-1) {
        length = strlen(input);
    }

    // Get the number of capture groups from the pattern
    size_t num_groups = rift_regex_pattern_get_group_count(matcher->pattern);

    // If we already have a context, update it
    if (matcher->context) {
        return rift_matcher_context_set_input(matcher->context, input, length);
    }

    // Otherwise, create a new context
    matcher->context = rift_matcher_context_create(input, length, num_groups);
    if (!matcher->context) {
        return false;
    }

    return true;
}

/**
 * @brief Process a single character through the automaton
 *
 * @param automaton The automaton
 * @param c The character to process
 * @param matcher The matcher for backtracking
 * @param context The matcher context for capturing
 * @return true if the character was accepted, false otherwise
 */
bool
process_character(rift_regex_automaton_t *automaton, char c, rift_regex_matcher_t *matcher,
                  rift_regex_matcher_context_t *context)
{
    // Get all possible transitions from the current state
    rift_regex_transition_t *transitions[100]; // Arbitrary limit, could be dynamic
    size_t num_transitions = rift_automaton_get_current_transitions(automaton, transitions, 100);

    // Try all epsilon transitions first (without consuming input)
    for (size_t i = 0; i < num_transitions; i++) {
        if (rift_automaton_transition_is_epsilon(transitions[i])) {
            // Save current state for potential backtracking
            rift_regex_state_t *current_state = rift_automaton_get_current_state(automaton);
            size_t current_pos = rift_matcher_context_get_position(context);

            // Follow the epsilon transition
            rift_regex_state_t *target = rift_automaton_transition_get_target(transitions[i]);
            rift_automaton_set_current_state(automaton, target);

            // Recursively process the same character
            if (process_character(automaton, c, matcher, context)) {
                return true;
            }

            // Backtrack
            rift_automaton_set_current_state(automaton, current_state);
            rift_matcher_context_set_position(context, current_pos);
        }
    }

    // Try all non-epsilon transitions
    for (size_t i = 0; i < num_transitions; i++) {
        if (!rift_automaton_transition_is_epsilon(transitions[i])) {
            const char *pattern = rift_automaton_transition_get_pattern(transitions[i]);

            // Check if this transition accepts the character
            // This would require a more complex matching logic in practice
            if (pattern && pattern[0] == c) {
                rift_regex_state_t *target = rift_automaton_transition_get_target(transitions[i]);
                rift_automaton_set_current_state(automaton, target);

                // Consume the character in the context
                rift_matcher_context_advance(context);

                return true;
            }
        }
    }

    return false;
}

/**
 * @brief Execute a match attempt starting from the current position
 *
 * This function attempts to match the pattern starting from the current position
 * in the input string. It returns true if a match is found, false otherwise.
 *
 * @param matcher The matcher
 * @param match Pointer to store match information (can be NULL)
 * @param anchored Whether the match should be anchored at the current position
 * @return true if a match was found, false otherwise
 */
bool
execute_match(rift_regex_matcher_t *matcher, rift_regex_match_t *match, bool anchored)
{
    if (!matcher || !matcher->context) {
        return false;
    }

    // Get the automaton from the pattern
    rift_regex_automaton_t *automaton = rift_regex_pattern_get_automaton(matcher->pattern);
    if (!automaton) {
        return false;
    }

    // Reset the backtracker
    rift_backtracker_reset(matcher->backtracker);

    // Reset timeout status
    matcher->timed_out = false;
    matcher->start_time = clock();

    // Save the starting position of the match
    size_t start_pos = rift_matcher_context_get_position(matcher->context);
    size_t input_length = rift_matcher_context_get_input_length(matcher->context);
    const char *input = rift_matcher_context_get_input(matcher->context);

    // Reset the automaton to its initial state
    rift_automaton_reset(automaton);

    // Process the input string through the automaton
    size_t match_end = start_pos;
    bool match_found = false;

    // Only attempt matching if there's input left
    if (start_pos < input_length) {
        size_t pos = start_pos;

        while (pos < input_length) {
            // Check for timeout
            if (check_timeout(matcher)) {
                return false;
            }

            // Try to process the current character
            char current_char = input[pos];
            if (!process_character(automaton, current_char, matcher, matcher->context)) {
                // Character not accepted - try backtracking
                if (anchored || rift_backtracker_is_empty(matcher->backtracker)) {
                    break;
                }

                // Backtrack to a previous state
                rift_regex_state_t *state;
                size_t backtrack_pos;
                size_t group_starts[100]; // Fixed size for simplicity
                size_t group_ends[100];
                size_t num_groups;

                if (!rift_backtracker_pop(matcher->backtracker, &state, &backtrack_pos,
                                          group_starts, group_ends, &num_groups)) {
                    break;
                }

                // Restore the automaton state
                rift_automaton_set_current_state(automaton, state);
                pos = backtrack_pos;

                // Set context position
                rift_matcher_context_set_position(matcher->context, pos);

                // Restore capture groups
                rift_regex_capture_groups_t *groups =
                    rift_matcher_context_get_capture_groups(matcher->context);

                if (groups) {
                    for (size_t i = 0; i < num_groups; i++) {
                        rift_capture_groups_record(groups, i, NULL, group_starts[i], group_ends[i]);
                    }
                }
            } else {
                // Character was processed successfully
                pos++;

                // If the automaton is in an accepting state, we have a match
                if (rift_automaton_is_accepting(automaton)) {
                    match_found = true;
                    match_end = pos;

                    // If we're using greedy matching (default), continue to try to match more
                    if (!(matcher->options & RIFT_MATCHER_OPTION_LAZY)) {
                        // Save this state as a backtrack point
                        size_t num_groups = rift_regex_pattern_get_group_count(matcher->pattern);

                        if (num_groups > 0) {
                            // Get current group positions
                            rift_regex_capture_groups_t *groups =
                                rift_matcher_context_get_capture_groups(matcher->context);

                            size_t current_group_starts[100]; // Fixed size for simplicity
                            size_t current_group_ends[100];

                            for (size_t i = 0; i < num_groups; i++) {
                                rift_regex_capture_groups_t *group =
                                    rift_capture_groups_get_by_index(groups, i);

                                if (group) {
                                    current_group_starts[i] = rift_capture_group_get_start(group);
                                    current_group_ends[i] = rift_capture_group_get_end(group);
                                } else {
                                    current_group_starts[i] = (size_t)-1;
                                    current_group_ends[i] = (size_t)-1;
                                }
                            }

                            // Push the current state as a backtrack point
                            rift_backtracker_push(
                                matcher->backtracker, rift_automaton_get_current_state(automaton),
                                pos, current_group_starts, current_group_ends, num_groups);
                        } else {
                            // No groups to track
                            rift_backtracker_push(matcher->backtracker,
                                                  rift_automaton_get_current_state(automaton), pos,
                                                  NULL, NULL, 0);
                        }
                    } else {
                        // Lazy matching - return the match as soon as we find it
                        break;
                    }
                }

                // Push potential alternative paths for backtracking
                rift_regex_transition_t *transitions[100]; // Arbitrary limit
                size_t num_transitions =
                    rift_automaton_get_current_transitions(automaton, transitions, 100);

                for (size_t i = 1; i < num_transitions; i++) { // Skip the first one
                    // Get the target state of this transition
                    rift_regex_state_t *target_state =
                        rift_automaton_transition_get_target(transitions[i]);

                    // Capture current group state for backtracking
                    size_t num_groups = rift_regex_pattern_get_group_count(matcher->pattern);

                    if (num_groups > 0) {
                        // Get current group positions
                        rift_regex_capture_groups_t *groups =
                            rift_matcher_context_get_capture_groups(matcher->context);

                        size_t current_group_starts[100]; // Fixed size for simplicity
                        size_t current_group_ends[100];

                        for (size_t j = 0; j < num_groups; j++) {
                            rift_regex_capture_groups_t *group =
                                rift_capture_groups_get_by_index(groups, j);

                            if (group) {
                                current_group_starts[j] = rift_capture_group_get_start(group);
                                current_group_ends[j] = rift_capture_group_get_end(group);
                            } else {
                                current_group_starts[j] = (size_t)-1;
                                current_group_ends[j] = (size_t)-1;
                            }
                        }

                        // Push the alternative state as a backtrack point
                        rift_backtracker_push(matcher->backtracker, target_state, pos,
                                              current_group_starts, current_group_ends, num_groups);
                    } else {
                        // No groups to track
                        rift_backtracker_push(matcher->backtracker, target_state, pos, NULL, NULL,
                                              0);
                    }
                }
            }
        }
    }

    // If we found a match, create the match result
    if (match_found && match) {
        rift_regex_match_result_t result =
            *rift_matcher_context_create_match_result(matcher->context, start_pos, match_end);
        match->start_pos = result.start_pos;
        match->end_pos = result.end_pos;
        match->groups = result.groups;
    }

    return match_found;
}

/**
 * @brief Find the next match in the input string
 *
 * @param matcher The matcher
 * @param match Pointer to store match information (can be NULL)
 * @return true if a match was found, false otherwise
 */
bool
rift_matcher_find_next(rift_regex_matcher_t *matcher, rift_regex_match_t *match)
{
    if (!matcher || !matcher->context) {
        return false;
    }

    size_t input_length = rift_matcher_context_get_input_length(matcher->context);
    size_t start_pos = rift_matcher_context_get_position(matcher->context);

    // Try to find a match starting from the current position
    if (execute_match(matcher, match, false)) {
        return true;
    }

    // If we didn't find a match, and we're not anchored to the start, try from the next position
    if (!(matcher->options & RIFT_MATCHER_OPTION_ANCHOR_START)) {
        // Start from the current position + 1, unless we're at the end
        if (start_pos < input_length) {
            start_pos++;
        } else {
            return false; // No more input to search
        }

        // Set the new start position
        rift_matcher_context_set_position(matcher->context, start_pos);

        // Try to find a match from each position
        while (start_pos < input_length) {
            if (execute_match(matcher, match, false)) {
                return true;
            }

            // Move to the next position
            start_pos++;
            rift_matcher_context_set_position(matcher->context, start_pos);

            // Check for timeout
            if (check_timeout(matcher)) {
                return false;
            }
        }
    }

    return false;
}

/**
 * @brief Check if the entire input string matches the pattern
 *
 * @param matcher The matcher
 * @param match Pointer to store match information (can be NULL)
 * @return true if the entire string matches, false otherwise
 */
bool
rift_matcher_matches(rift_regex_matcher_t *matcher, rift_regex_match_t *match)
{
    if (!matcher || !matcher->context) {
        return false;
    }

    // Reset the matcher to the beginning of the input
    rift_matcher_context_set_position(matcher->context, 0);

    // Try to find a match starting from the beginning
    rift_regex_match_t local_match;
    if (!execute_match(matcher, &local_match, true)) {
        return false;
    }

    // Check if the match covers the entire input
    size_t input_length = rift_matcher_context_get_input_length(matcher->context);

    if (local_match.end_pos == input_length) {
        if (match) {
            *match = local_match;
        }
        return true;
    }

    return false;
}

/**
 * @brief Find all matches in the input string
 *
 * @param matcher The matcher
 * @param matches Array to store match information
 * @param max_matches Maximum number of matches to find
 * @param num_matches Pointer to store the number of matches found
 * @return true if at least one match was found, false otherwise
 */
bool
rift_matcher_find_all(rift_regex_matcher_t *matcher, rift_regex_match_t *matches,
                      size_t max_matches, size_t *num_matches)
{
    if (!matcher || !matches || !num_matches || max_matches == 0) {
        return false;
    }

    // Reset the matcher to the beginning of the input
    rift_matcher_context_set_position(matcher->context, 0);

    // Initialize the match count
    *num_matches = 0;

    // Find all matches, up to max_matches
    while (*num_matches < max_matches) {
        // Check for timeout
        if (check_timeout(matcher)) {
            return *num_matches > 0;
        }

        // Try to find the next match
        if (!rift_matcher_find_next(matcher, &matches[*num_matches])) {
            break;
        }

        // Increment the match count
        (*num_matches)++;

        // Set the next start position to avoid overlapping matches
        // To avoid infinite loops, we advance at least one character
        size_t next_pos = matches[*num_matches - 1].end_pos;
        if (next_pos <= rift_matcher_context_get_position(matcher->context)) {
            next_pos = rift_matcher_context_get_position(matcher->context) + 1;
        }

        // If we've reached the end of the input, we're done
        if (next_pos >= rift_matcher_context_get_input_length(matcher->context)) {
            break;
        }

        rift_matcher_context_set_position(matcher->context, next_pos);
    }

    return *num_matches > 0;
}

/**
 * @brief Replace occurrences of the pattern in the input string
 *
 * @param matcher The matcher
 * @param replacement The replacement string
 * @param output Buffer to store the result
 * @param output_size Size of the output buffer
 * @param num_replacements Pointer to store the number of replacements made
 * @return true if successful, false otherwise
 */
bool
rift_matcher_replace(rift_regex_matcher_t *matcher, const char *replacement, char *output,
                     size_t output_size, size_t *num_replacements)
{
    if (!matcher || !replacement || !output || output_size == 0 || !num_replacements) {
        return false;
    }

    // Reset the matcher to the beginning of the input
    rift_matcher_context_set_position(matcher->context, 0);

    // Get the input string
    const char *input = rift_matcher_context_get_input(matcher->context);
    size_t input_length = rift_matcher_context_get_input_length(matcher->context);

    // Initialize counters
    *num_replacements = 0;
    size_t output_pos = 0;
    size_t last_copy_pos = 0;

    // Find all matches and replace them
    rift_regex_match_t match;
    while (rift_matcher_find_next(matcher, &match)) {
        // Check for timeout
        if (check_timeout(matcher)) {
            break;
        }

        // Copy the text between the last match and this match
        size_t to_copy = match.start_pos - last_copy_pos;
        if (output_pos + to_copy >= output_size) {
            // Not enough space
            return false;
        }
        memcpy(output + output_pos, input + last_copy_pos, to_copy);
        output_pos += to_copy;

        // Copy the replacement text
        size_t replacement_len = strlen(replacement);
        if (output_pos + replacement_len >= output_size) {
            // Not enough space
            return false;
        }
        memcpy(output + output_pos, replacement, replacement_len);
        output_pos += replacement_len;

        // Update counters
        (*num_replacements)++;
        last_copy_pos = match.end_pos;

        // Set the next start position
        // To avoid infinite loops, we advance at least one character
        size_t next_pos = match.end_pos;
        if (next_pos <= rift_matcher_context_get_position(matcher->context)) {
            next_pos = rift_matcher_context_get_position(matcher->context) + 1;
        }

        // If we've reached the end of the input, we're done
        if (next_pos >= input_length) {
            break;
        }

        rift_matcher_context_set_position(matcher->context, next_pos);
    }

    // Copy any remaining text
    size_t to_copy = input_length - last_copy_pos;
    if (output_pos + to_copy >= output_size) {
        // Not enough space
        return false;
    }
    memcpy(output + output_pos, input + last_copy_pos, to_copy);
    output_pos += to_copy;

    // Null-terminate the output
    if (output_pos >= output_size) {
        output[output_size - 1] = '\0';
    } else {
        output[output_pos] = '\0';
    }

    return true;
}

/**
 * @brief Split the input string using the pattern as a delimiter
 *
 * @param matcher The matcher
 * @param parts Array to store the resulting parts
 * @param max_parts Maximum number of parts to return
 * @param num_parts Pointer to store the number of parts
 * @return true if successful, false otherwise
 */
bool
rift_matcher_split(rift_regex_matcher_t *matcher, char **parts, size_t max_parts, size_t *num_parts)
{
    if (!matcher || !parts || max_parts == 0 || !num_parts) {
        return false;
    }

    // Reset the matcher to the beginning of the input
    rift_matcher_context_set_position(matcher->context, 0);

    // Get the input string
    const char *input = rift_matcher_context_get_input(matcher->context);
    size_t input_length = rift_matcher_context_get_input_length(matcher->context);

    // Initialize counters
    *num_parts = 0;
    size_t last_end_pos = 0;

    // Find all matches and split the string
    rift_regex_match_t match;
    while (*num_parts < max_parts - 1 && rift_matcher_find_next(matcher, &match)) {
        // Check for timeout
        if (check_timeout(matcher)) {
            break;
        }

        // Create a part from the text between the last match and this match
        size_t part_length = match.start_pos - last_end_pos;
        parts[*num_parts] = (char *)malloc(part_length + 1);
        if (!parts[*num_parts]) {
            // Cleanup allocated parts
            for (size_t i = 0; i < *num_parts; i++) {
                free(parts[i]);
            }
            return false;
        }

        memcpy(parts[*num_parts], input + last_end_pos, part_length);
        parts[*num_parts][part_length] = '\0';

        (*num_parts)++;
        last_end_pos = match.end_pos;

        // Set the next start position
        // To avoid infinite loops, we advance at least one character
        size_t next_pos = match.end_pos;
        if (next_pos <= rift_matcher_context_get_position(matcher->context)) {
            next_pos = rift_matcher_context_get_position(matcher->context) + 1;
        }

        // If we've reached the end of the input, we're done
        if (next_pos >= input_length) {
            break;
        }

        rift_matcher_context_set_position(matcher->context, next_pos);
    }

    // Add the last part if there's room and it's not empty
    if (*num_parts < max_parts && last_end_pos < input_length) {
        size_t part_length = input_length - last_end_pos;
        parts[*num_parts] = (char *)malloc(part_length + 1);
        if (!parts[*num_parts]) {
            // Cleanup allocated parts
            for (size_t i = 0; i < *num_parts; i++) {
                free(parts[i]);
            }
            return false;
        }

        memcpy(parts[*num_parts], input + last_end_pos, part_length);
        parts[*num_parts][part_length] = '\0';

        (*num_parts)++;
    }

    return *num_parts > 0;
}

/**
 * @brief Set matcher options
 *
 * @param matcher The matcher
 * @param options The options to set
 * @return true if successful, false otherwise
 */
bool
rift_matcher_set_options(rift_regex_matcher_t *matcher, rift_matcher_option_t options)
{
    if (!matcher) {
        return false;
    }

    matcher->options = options;
    return true;
}

/**
 * @brief Set the maximum backtracking depth
 *
 * @param matcher The matcher
 * @param max_depth The maximum depth
 * @return true if successful, false otherwise
 */
bool
rift_matcher_set_max_backtrack_depth(rift_regex_matcher_t *matcher, size_t max_depth)
{
    if (!matcher || !matcher->backtracker) {
        return false;
    }

    return rift_backtracker_set_max_depth(matcher->backtracker, max_depth);
}

/**
 * @brief Get the current backtracking depth
 *
 * @param matcher The matcher
 * @return The current backtracking depth
 */
size_t
rift_matcher_get_backtrack_depth(const rift_regex_matcher_t *matcher)
{
    if (!matcher || !matcher->backtracker) {
        return 0;
    }

    return rift_backtracker_get_depth(matcher->backtracker);
}

/**
 * @brief Set a timeout for matching operations
 *
 * @param matcher The matcher
 * @param timeout_ms Timeout in milliseconds (0 for no timeout)
 * @return true if successful, false otherwise
 */
bool
rift_matcher_set_timeout(rift_regex_matcher_t *matcher, uint32_t timeout_ms)
{
    if (!matcher) {
        return false;
    }

    matcher->timeout_ms = timeout_ms;
    return true;
}

/**
 * @brief Check if a timeout occurred during the last matching operation
 *
 * @param matcher The matcher
 * @return true if a timeout occurred, false otherwise
 */
bool
rift_matcher_timed_out(const rift_regex_matcher_t *matcher)
{
    if (!matcher) {
        return false;
    }

    return matcher->timed_out;
}

/**
 * @brief Get the pattern associated with a matcher
 *
 * @param matcher The matcher
 * @return The regex pattern
 */
const rift_regex_pattern_t *
rift_matcher_get_pattern(const rift_regex_matcher_t *matcher)
{
    if (!matcher) {
        return NULL;
    }

    return matcher->pattern;
}

/**
 * @brief Get the input string for a matcher
 *
 * @param matcher The matcher
 * @return The input string
 */
const char *
rift_matcher_get_input(const rift_regex_matcher_t *matcher)
{
    if (!matcher || !matcher->context) {
        return NULL;
    }

    return rift_matcher_context_get_input(matcher->context);
}

/**
 * @brief Get the current position in the input string
 *
 * @param matcher The matcher
 * @return The current position
 */
size_t
rift_matcher_get_position(const rift_regex_matcher_t *matcher)
{
    if (!matcher || !matcher->context) {
        return 0;
    }

    return rift_matcher_context_get_position(matcher->context);
}

/**
 * @brief Set the current position in the input string
 *
 * @param matcher The matcher
 * @param position The position to set
 * @return true if successful, false otherwise
 */
bool
rift_matcher_set_position(rift_regex_matcher_t *matcher, size_t position)
{
    if (!matcher || !matcher->context) {
        return false;
    }

    return rift_matcher_context_set_position(matcher->context, position);
}

/**
 * @brief Create a matcher from a pattern string
 *
 * This is a convenience function that compiles the pattern and creates a matcher.
 *
 * @param pattern_string The regex pattern string
 * @param flags Compilation flags
 * @param options Matcher options
 * @param error Pointer to store error code (can be NULL)
 * @return A new matcher or NULL on failure
 */
rift_regex_matcher_t *
rift_matcher_create_from_string(const char *pattern_string, rift_regex_flags_t flags,
                                rift_matcher_option_t options, rift_regex_error_t *error)
{
    if (!pattern_string) {
        if (error) {
            char *msg_copy;
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
            const char *msg = "Pattern string cannot be NULL";
            size_t len = strlen(msg) + 1;
            msg_copy = malloc(len);
            if (msg_copy) {
                strncpy(error->message, msg, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
                error->message[RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1] = '\0';
                free(msg_copy);
            }
        }
        return NULL;
    }

    // Compile the pattern
    rift_regex_pattern_t *pattern = rift_regex_compile_pattern(pattern_string, flags, error);
    if (!pattern) {
        return NULL;
    }

    // Create the matcher
    rift_regex_matcher_t *matcher = rift_matcher_create(pattern, options);
    if (!matcher) {
        // Free the pattern since we own it
        rift_regex_pattern_free(pattern);

        if (error) {
            error->code = RIFT_REGEX_ERROR_MEMORY_ALLOCATION;
        }

        return NULL;
    }

    return matcher;
}

void
rift_match_free(rift_regex_match_t *match)
{
    if (!match) {
        return;
    }

    if (match->matched_text) {
        free(match->matched_text);
    }

    if (match->groups) {
        for (size_t i = 0; i < match->group_count; i++) {
            if (match->groups[i].name) {
                free(match->groups[i].name);
            }
            if (match->groups[i].value) {
                free(match->groups[i].value);
            }
        }
        free(match->groups);
    }

    free(match);
}
