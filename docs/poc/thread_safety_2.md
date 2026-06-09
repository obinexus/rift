rift_syntax_aware_controller_adjust_limits(
    rift_syntax_aware_controller_t* controller,
    const rift_regex_pattern_t* pattern) {
    
    if (!controller || !pattern) {
        return;
    }
    
    if (!controller->is_r_syntax) {
        // No adjustments needed for standard syntax
        return;
    }
    
    // Analyze R'' syntax pattern for complexity
    float complexity = rift_bailout_calculate_pattern_complexity(
                           pattern, true);
    
    // Store complexity factor for future bailout decisions
    controller->syntax_complexity_factor = complexity;
    
    // Adjust limits based on complexity
    rift_backtracking_controller_t* base = controller->base_controller;
    
    // More complex R'' patterns get stricter limits to prevent excessive backtracking
    if (complexity > 5.0f) {
        // For highly complex patterns, reduce limits substantially
        base->depth_limit = (uint32_t)(base->depth_limit * 0.7f);
        base->time_limit_us = (uint64_t)(base->time_limit_us * 0.7f);
    } else if (complexity > 2.0f) {
        // For moderately complex patterns, reduce limits slightly
        base->depth_limit = (uint32_t)(base->depth_limit * 0.85f);
        base->time_limit_us = (uint64_t)(base->time_limit_us * 0.85f);
    }
    
    // Also register appropriate pattern complexity bailout strategy if not already present
    rift_bailout_strategy_t* complexity_strategy = 
        rift_bailout_pattern_complexity_strategy_create(
            rift_automaton_get_state_count(rift_regex_pattern_get_automaton(pattern)),
            complexity / 2.0f, // Use pre-calculated complexity as branching factor
            rift_regex_ast_count_nodes_of_type(
                rift_regex_pattern_get_ast(pattern), 
                RIFT_REGEX_AST_NODE_QUANTIFIER),
            2.5f // Threshold tuned for R'' syntax
        );
    
    if (complexity_strategy) {
        rift_bailout_strategy_manager_add_strategy(
            base->bailout_manager, complexity_strategy);
    }
}

bool
rift_syntax_aware_controller_register_point(
    rift_syntax_aware_controller_t* controller,
    const rift_backtracking_state_t* state) {
    
    if (!controller || !state) {
        return false;
    }
    
    // Check basic limits first
    rift_backtracking_controller_t* base = controller->base_controller;
    
    // Check depth limit
    if (state->current_depth >= base->depth_limit) {
        return false;
    }
    
    // Check time limit
    if (state->elapsed_time_us >= base->time_limit_us) {
        return false;
    }
    
    // Check transitions limit
    if (state->transition_count >= base->max_transitions) {
        return false;
    }
    
    // Apply R'' syntax specific checks
    if (controller->is_r_syntax) {
        // For R'' syntax, we're more aggressive with bailout strategies
        // to prevent catastrophic backtracking
        
        // Evaluate bailout strategies
        rift_bailout_strategy_t* selected_strategy = 
            rift_bailout_strategy_manager_evaluate(
                base->bailout_manager, state);
        
        if (selected_strategy) {
            // Execute the selected strategy
            int result = rift_bailout_strategy_manager_execute(
                base->bailout_manager, selected_strategy, state);
            
            // Non-zero result means bailout was triggered
            if (result != 0) {
                return false;
            }
        }
    }
    
    // All checks passed
    return true;
}

void
rift_syntax_aware_controller_free(rift_syntax_aware_controller_t* controller) {
    if (!controller) {
        return;
    }
    
    if (controller->base_controller) {
        rift_backtracking_controller_free(controller->base_controller);
    }
    
    free(controller);
}
```

#### 3.4.3 Backtracking Controller Interface

```c
/**
 * @file backtracking_controller.h
 * @brief Core controller for backtracking limits
 */

typedef struct rift_backtracking_controller {
    uint32_t depth_limit;
    uint64_t time_limit_us;
    uint64_t max_transitions;
    rift_execution_time_tracker_t* time_tracker;
    rift_bailout_strategy_manager_t* bailout_manager;
} rift_backtracking_controller_t;

/**
 * @brief Create a new backtracking controller
 * @param config Backtracking limit configuration
 * @return New controller instance
 */
rift_backtracking_controller_t*
rift_backtracking_controller_create(
    const rift_backtrack_limit_config_t* config);

/**
 * @brief Start tracking execution time
 * @param controller The controller to update
 */
void
rift_backtracking_controller_start_timing(
    rift_backtracking_controller_t* controller);

/**
 * @brief Check if any limits are exceeded
 * @param controller The controller to check
 * @param state Current backtracking state
 * @return true if limits are exceeded, false otherwise
 */
bool
rift_backtracking_controller_limits_exceeded(
    const rift_backtracking_controller_t* controller,
    const rift_backtracking_state_t* state);

/**
 * @brief Update backtracking state with current metrics
 * @param controller The controller to use
 * @param state State to update
 * @param current_depth Current backtracking depth
 * @param input_position Current input position
 */
void
rift_backtracking_controller_update_state(
    const rift_backtracking_controller_t* controller,
    rift_backtracking_state_t* state,
    uint32_t current_depth,
    uint32_t input_position);

/**
 * @brief Add a bailout strategy
 * @param controller The controller to modify
 * @param strategy The strategy to add
 * @return true if successful, false otherwise
 */
bool
rift_backtracking_controller_add_strategy(
    rift_backtracking_controller_t* controller,
    rift_bailout_strategy_t* strategy);

/**
 * @brief Free controller resources
 * @param controller The controller to free
 */
void
rift_backtracking_controller_free(
    rift_backtracking_controller_t* controller);
```

### 3.5 Thread-Safe Implementation

#### 3.5.1 Thread-Safe Backtracker

```c
/**
 * @file safe_backtracker.h
 * @brief Enhanced thread-safe backtracker with advanced limit controls
 */

typedef struct rift_regex_safe_backtracker {
    pthread_mutex_t mutex;
    pthread_key_t thread_local_key;
    rift_regex_backtracker_t* global_bt;
    rift_syntax_aware_controller_t* controller;
    uint32_t cleanup_count;
    bool initialized;
} rift_regex_safe_backtracker_t;

/**
 * @brief Thread-local backtracker data
 */
typedef struct rift_thread_local_backtracker {
    rift_regex_backtracker_t* backtracker;
    rift_backtracking_state_t current_state;
} rift_thread_local_backtracker_t;

/**
 * @brief Create a new thread-safe backtracker with advanced controls
 * @param config Backtracking limit configuration
 * @param num_groups Number of capture groups
 * @param is_r_syntax Whether the pattern uses R'' syntax
 * @return New backtracker instance
 */
rift_regex_safe_backtracker_t*
rift_safe_backtracker_create_advanced(
    const rift_backtrack_limit_config_t* config,
    size_t num_groups,
    bool is_r_syntax);

/**
 * @brief Thread-local data cleanup function
 * @param data Thread-local data to clean up
 */
void
rift_thread_local_backtracker_cleanup(void* data);

/**
 * @brief Get or create thread-local backtracker
 * @param safe_bt The thread-safe backtracker
 * @param error Pointer to store error information
 * @return Thread-local backtracker or NULL on error
 */
rift_thread_local_backtracker_t*
rift_safe_backtracker_get_thread_local(
    rift_regex_safe_backtracker_t* safe_bt,
    rift_regex_error_t* error);

/**
 * @brief Push a new backtrack point with full limit checking
 * @param safe_bt The thread-safe backtracker
 * @param state The current state
 * @param input_position Position in the input string
 * @param group_starts Array of group start positions
 * @param group_ends Array of group end positions
 * @param num_groups Number of groups
 * @param error Pointer to store error information
 * @return true if point was pushed, false if limits exceeded
 */
bool
rift_safe_backtracker_push_with_limits(
    rift_regex_safe_backtracker_t* safe_bt,
    rift_regex_state_t* state,
    size_t input_position,
    const size_t* group_starts,
    const size_t* group_ends,
    size_t num_groups,
    rift_regex_error_t* error);

/**
 * @brief Pop a backtrack point from the thread-local stack
 * @param safe_bt The thread-safe backtracker
 * @param state Pointer to store the state
 * @param input_position Pointer to store the input position
 * @param group_starts Array to store group start positions
 * @param group_ends Array to store group end positions
 * @param num_groups Pointer to store the number of groups
 * @param error Pointer to store error information
 * @return true if a point was popped, false if the stack was empty
 */
bool
rift_safe_backtracker_pop_with_update(
    rift_regex_safe_backtracker_t* safe_bt,
    rift_regex_state_t** state,
    size_t* input_position,
    size_t* group_starts,
    size_t* group_ends,
    size_t* num_groups,
    rift_regex_error_t* error);

/**
 * @brief Free resources associated with a thread-safe backtracker
 * @param safe_bt The thread-safe backtracker to free
 */
void
rift_safe_backtracker_free(rift_regex_safe_backtracker_t* safe_bt);
```

#### 3.5.2 Thread-Safe Backtracker Implementation

```c
#include "safe_backtracker.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Thread-local data cleanup function
 */
void
rift_thread_local_backtracker_cleanup(void* data) {
    if (!data) {
        return;
    }
    
    rift_thread_local_backtracker_t* tl_data = 
        (rift_thread_local_backtracker_t*)data;
    
    if (tl_data->backtracker) {
        rift_backtracker_free(tl_data->backtracker);
    }
    
    free(tl_data);
}

/**
 * @brief Create a new thread-safe backtracker with advanced controls
 */
rift_regex_safe_backtracker_t*
rift_safe_backtracker_create_advanced(
    const rift_backtrack_limit_config_t* config,
    size_t num_groups,
    bool is_r_syntax) {
    
    if (!config) {
        return NULL;
    }
    
    rift_regex_safe_backtracker_t* safe_bt = 
        (rift_regex_safe_backtracker_t*)malloc(sizeof(rift_regex_safe_backtracker_t));
    
    if (!safe_bt) {
        return NULL;
    }
    
    // Initialize mutex
    if (pthread_mutex_init(&safe_bt->mutex, NULL) != 0) {
        free(safe_bt);
        return NULL;
    }
    
    // Create thread-local storage key
    if (pthread_key_create(&safe_bt->thread_local_key, 
                           rift_thread_local_backtracker_cleanup) != 0) {
        pthread_mutex_destroy(&safe_bt->mutex);
        free(safe_bt);
        return NULL;
    }
    
    // Create global backtracker as fallback
    safe_bt->global_bt = rift_backtracker_create(
        config->max_depth, num_groups);
    
    if (!safe_bt->global_bt) {
        pthread_key_delete(safe_bt->thread_local_key);
        pthread_mutex_destroy(&safe_bt->mutex);
        free(safe_bt);
        return NULL;
    }
    
    // Create syntax-aware controller
    safe_bt->controller = rift_syntax_aware_controller_create(
        is_r_syntax, config);
    
    if (!safe_bt->controller) {
        rift_backtracker_free(safe_bt->global_bt);
        pthread_key_delete(safe_bt->thread_local_key);
        pthread_mutex_destroy(&safe_bt->mutex);
        free(safe_bt);
        return NULL;
    }
    
    safe_bt->cleanup_count = 0;
    safe_bt->initialized = true;
    
    return safe_bt;
}

/**
 * @brief Get or create thread-local backtracker
 */
rift_thread_local_backtracker_t*
rift_safe_backtracker_get_thread_local(
    rift_regex_safe_backtracker_t* safe_bt,
    rift_regex_error_t* error) {
    
    if (!safe_bt || !safe_bt->initialized) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
            strncpy(error->message, "Invalid backtracker", 
                    RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
        }
        return NULL;
    }
    
    // Try to get the thread-local data
    rift_thread_local_backtracker_t* tl_data = 
        (rift_thread_local_backtracker_t*)pthread_getspecific(safe_bt->thread_local_key);
    
    if (tl_data) {
        return tl_data;
    }
    
    // If not found, create new thread-local data
    pthread_mutex_lock(&safe_bt->mutex);
    
    // Create thread-local data
    tl_data = (rift_thread_local_backtracker_t*)
              malloc(sizeof(rift_thread_local_backtracker_t));
    
    if (!tl_data) {
        pthread_mutex_unlock(&safe_bt->mutex);
        if (error) {
            error->code = RIFT_REGEX_ERROR_MEMORY;
            strncpy(error->message, "Memory allocation failed", 
                    RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
        }
        return NULL;
    }
    
    // Clone the global backtracker
    tl_data->backtracker = rift_backtracker_clone(safe_bt->global_bt);
    
    if (!tl_data->backtracker) {
        free(tl_data);
        pthread_mutex_unlock(&safe_bt->mutex);
        if (error) {
            error->code = RIFT_REGEX_ERROR_MEMORY;
            strncpy(error->message, "Failed to clone backtracker", 
                    RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
        }
        return NULL;
    }
    
    // Initialize backtracking state
    memset(&tl_data->current_state, 0, sizeof(rift_backtracking_state_t));
    tl_data->current_state.is_r_syntax = safe_bt->controller->is_r_syntax;
    
    // Store in thread-local storage
    if (pthread_setspecific(safe_bt->thread_local_key, tl_data) != 0) {
        rift_backtracker_free(tl_data->backtracker);
        free(tl_data);
        pthread_mutex_unlock(&safe_bt->mutex);
        if (error) {
            error->code = RIFT_REGEX_ERROR_INTERNAL;
            strncpy(error->message, "Failed to set thread-local storage", 
                    RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
        }
        return NULL;
    }
    
    pthread_mutex_unlock(&safe_bt->mutex);
    return tl_data;
}

/**
 * @brief Push a new backtrack point with full limit checking
 */
bool
rift_safe_backtracker_push_with_limits(
    rift_regex_safe_backtracker_t* safe_bt,
    rift_regex_state_t* state,
    size_t input_position,
    const size_t* group_starts,
    const size_t* group_ends,
    size_t num_groups,
    rift_regex_error_t* error) {
    
    if (!safe_bt || !state) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
            strncpy(error->message, "Invalid parameters", 
                    RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
        }
        return false;
    }
    
    // Get thread-local data
    rift_thread_local_backtracker_t* tl_data = 
        rift_safe_backtracker_get_thread_local(safe_bt, error);
    
    if (!tl_data) {
        return false;
    }
    
    // Update backtracking state
    rift_backtracking_state_t* state_data = &tl_data->current_state;
    state_data->current_depth = rift_backtracker_get_depth(tl_data->backtracker);
    state_data->progress_marker = input_position;
    
    // Get elapsed time
    rift_backtracking_controller_t* base = safe_bt->controller->base_controller;
    state_data->elapsed_time_us = rift_execution_time_tracker_get_elapsed(
        base->time_tracker);
    
    // Check limits using controller
    if (!rift_syntax_aware_controller_register_point(
            safe_bt->controller, state_data)) {
        
        if (error) {
            error->code = RIFT_REGEX_ERROR_BACKTRACKING_LIMIT;
            
            if (state_data->elapsed_time_us >= base->time_limit_us) {
                snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                         "Backtracking time limit exceeded (%llu μs)",
                         (unsigned long long)state_data->elapsed_time_us);
            } else if (state_data->current_depth >= base->depth_limit) {
                snprintf(error->message, RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH,
                         "Backtracking depth limit exceeded (%u)",
                         state_data->current_depth);
            } else {
                strncpy(error->message, "Backtracking limit exceeded (bailout strategy)", 
                        RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
            }
        }
        
        return false;
    }
    
    // If all checks pass, perform the actual push operation
    return rift_backtracker_push(tl_data->backtracker, state, input_position, 
                               group_starts, group_ends, num_groups);
}

/**
 * @brief Pop a backtrack point and update state
 */
bool
rift_safe_backtracker_pop_with_update(
    rift_regex_safe_backtracker_t* safe_bt,
    rift_regex_state_t** state,
    size_t* input_position,
    size_t* group_starts,
    size_t* group_ends,
    size_t* num_groups,
    rift_regex_error_t* error) {
    
    if (!safe_bt || !state || !input_position || !num_groups) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
            strncpy(error->message, "Invalid parameters", 
                    RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
        }
        return false;
    }
    
    // Get thread-local data
    rift_thread_local_backtracker_t* tl_data = 
        rift_safe_backtracker_get_thread_local(safe_bt, error);
    
    if (!tl_data) {
        return false;
    }
    
    // Perform the pop operation
    bool result = rift_backtracker_pop(tl_data->backtracker, state, input_position, 
                                     group_starts, group_ends, num_groups);
    
    // Update backtracking state
    if (result) {
        tl_data->current_state.current_depth = 
            rift_backtracker_get_depth(tl_data->backtracker);
        tl_data->current_state.progress_marker = *input_position;
    }
    
    return result;
}

/**
 * @brief Free resources associated with a thread-safe backtracker
 */
void
rift_safe_backtracker_free(rift_regex_safe_backtracker_t* safe_bt) {
    if (!safe_bt) {
        return;
    }
    
    // Clean up resources
    if (safe_bt->controller) {
        rift_syntax_aware_controller_free(safe_bt->controller);
    }
    
    if (safe_bt->global_bt) {
        rift_backtracker_free(safe_bt->global_bt);
    }
    
    if (safe_bt->initialized) {
        pthread_key_delete(safe_bt->thread_local_key);
        pthread_mutex_destroy(&safe_bt->mutex);
    }
    
    free(safe_bt);
}
```

## 4. Integration with Main LibRift Codebase

### 4.1 Changes to Existing Header Files

#### 4.1.1 Updates to backtracker.h

```c
/* Add to backtracker.h */

/**
 * @brief Get the state of a backtracker
 * @param backtracker The backtracker to query
 * @param state Backtracking state to fill
 * @return true if successful, false otherwise
 */
bool
rift_backtracker_get_state(
    const rift_regex_backtracker_t* backtracker,
    rift_backtracking_state_t* state);

/**
 * @brief Set backtracking limit options
 * @param backtracker The backtracker to modify
 * @param config Limit configuration
 * @return true if successful, false otherwise
 */
bool
rift_backtracker_set_limits(
    rift_regex_backtracker_t* backtracker,
    const rift_backtrack_limit_config_t* config);
```

#### 4.1.2 Updates to matcher.h

```c
/* Add to matcher.h */

/**
 * @brief Set pattern-specific backtracking limits
 * @param matcher The matcher to modify
 * @param pattern_id Pattern identifier
 * @param config Limit configuration
 * @return true if successful, false otherwise
 */
bool
rift_regex_matcher_set_pattern_limits(
    rift_regex_matcher_t* matcher,
    uint32_t pattern_id,
    const rift_backtrack_limit_config_t* config);

/**
 * @brief Set match-specific backtracking limits
 * @param matcher The matcher to modify
 * @param match_id Match identifier
 * @param config Limit configuration
 * @return true if successful, false otherwise
 */
bool
rift_regex_matcher_set_match_limits(
    rift_regex_matcher_t* matcher,
    uint32_t match_id,
    const rift_backtrack_limit_config_t* config);

/**
 * @brief Configure default limits for R'' syntax
 * @param matcher The matcher to modify
 * @param max_depth Maximum depth
 * @param max_duration_ms Maximum duration in milliseconds
 * @return true if successful, false otherwise
 */
bool
rift_regex_matcher_configure_r_syntax_limits(
    rift_regex_matcher_t* matcher,
    uint32_t max_depth,
    uint32_t max_duration_ms);
```

### 4.2 Implementation of Integration Points

#### 4.2.1 R'' Syntax Detection in Pattern Compiler

```c
/**
 * @brief Extended regex pattern compilation function with automatic R'' syntax detection
 * 
 * This function detects whether the pattern uses R'' syntax and sets up
 * appropriate backtracking limits and strategies.
 */
rift_regex_automaton_t*
rift_regex_compile_pattern_extended(
    const char *pattern,
    rift_regex_flags_t flags,
    rift_backtrack_limit_registry_t* limit_registry,
    uint32_t pattern_id,
    rift_regex_error_t *error)
{
    if (!pattern) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
            strncpy(error->message, "Invalid pattern parameter", 
                    RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
        }
        return NULL;
    }
    
    // Detect R'' syntax
    bool is_r_syntax = rift_regex_is_rift_syntax(pattern);
    
    // If using R'' syntax, ensure the RIFT_SYNTAX flag is set
    if (is_r_syntax) {
        flags |= RIFT_REGEX_FLAG_RIFT_SYNTAX;
    }
    
    // Parse the pattern into an AST
    rift_regex_ast_t *ast = rift_regex_parse(pattern, flags, error);
    if (!ast) {
        return NULL;
    }
    
    // Compile the AST into an automaton
    rift_regex_automaton_t *automaton = rift_regex_compile_ast(ast, flags, error);
    
    // Free the AST (no longer needed after compilation)
    rift_regex_ast_free(ast);
    
    if (!automaton) {
        return NULL;
    }
    
    // If using R'' syntax and limit registry is provided, register pattern-specific limits
    if (is_r_syntax && limit_registry) {
        // Calculate pattern complexity
        float complexity = rift_bailout_calculate_pattern_complexity(
            rift_regex_pattern_create_from_automaton(automaton, flags),
            true);
        
        // Create appropriate limits based on complexity
        rift_backtrack_limit_config_t* r_syntax_config = NULL;
        
        if (complexity > 5.0f) {
            // Stricter limits for highly complex patterns
            r_syntax_config = rift_backtrack_limit_config_create_pattern(
                pattern_id,
                true, // Override global settings
                800,  // Reduced depth limit
                3000, // Reduced time limit (3 seconds)
                50000 // Reduced transitions
            );
        } else if (complexity > 2.0f) {
            // Moderate limits for medium complexity
            r_syntax_config = rift_backtrack_limit_config_create_pattern(
                pattern_id,
                true, // Override global settings
                1200, // Slightly reduced depth
                4000, // 4 seconds
                75000 // Moderately reduced transitions
            );
        } else {
            // Default limits for simple patterns
            r_syntax_config = rift_backtrack_limit_config_create_pattern(
                pattern_id,
                false, // Use global settings
                0, 0, 0 // These values are ignored when not overriding
            );
        }
        
        // Register the configuration
        if (r_syntax_config) {
            rift_backtrack_limit_registry_register_pattern(
                limit_registry, pattern_id, r_syntax_config);
            
            // Free the local copy
            rift_backtrack_limit_config_free(r_syntax_config);
        }
    }
    
    return automaton;
}
```

#### 4.2.2 Integration with Safe Backtracker

```c
/**
 * @brief Match execution with thread-safe backtracking
 */
rift_regex_match_result_t*
rift_regex_match_with_safe_backtracking(
    rift_regex_matcher_t* matcher,
    const char* input,
    size_t input_length,
    uint32_t match_id,
    rift_regex_error_t* error)
{
    if (!matcher || !input) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
            strncpy(error->message, "Invalid parameters", 
                    RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
        }
        return NULL;
    }
    
    // Use strlen if input_length not specified
    if (input_length == (size_t)-1) {
        input_length = strlen(input);
    }
    
    // Get the pattern
    rift_regex_pattern_t* pattern = rift_regex_matcher_get_pattern(matcher);
    if (!pattern) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
            strncpy(error->message, "No pattern associated with matcher", 
                    RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
        }
        return NULL;
    }
    
    // Get pattern ID
    uint32_t pattern_id = rift_regex_pattern_get_id(pattern);
    
    // Get effective backtracking limits
    rift_backtrack_limit_config_t* effective_config = 
        rift_backtrack_limit_registry_get_effective_config(
            rift_regex_matcher_get_limit_registry(matcher),
            pattern_id,
            match_id
        );
    
    if (!effective_config) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_INTERNAL;
            strncpy(error->message, "Failed to get backtracking limits", 
                    RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
        }
        return NULL;
    }
    
    // Determine if pattern uses R'' syntax
    bool is_r_syntax = rift_regex_pattern_is_rift_syntax(pattern);
    
    // Create thread-safe backtracker
    rift_regex_safe_backtracker_t* safe_bt = 
        rift_safe_backtracker_create_advanced(
            effective_config,
            rift_regex_pattern_get# Thread-Safe Backtracking Management Plan for LibRift

## 1. Introduction

This technical specification details the implementation plan for the enhanced backtracking management system in LibRift, with particular focus on thread safety, configurable limits, and early bailout mechanisms for the R'' syntax extension. The plan addresses the specific requirements identified during code review and aims to resolve potential race conditions and catastrophic backtracking scenarios in multi-threaded environments.

## 2. Architecture Overview

The implementation architecture consists of interrelated modules designed to provide comprehensive backtracking control:

1. **Time-based Limiting System**
2. **Multi-scope Configuration Framework** 
3. **Early Bailout Strategy System**
4. **R'' Syntax Integration Layer**
5. **Thread-Safe Controller Implementation**

### 2.1 Core Components Diagram

![Architecture Diagram](architecture-diagram.png)

### 2.2 Component Interactions

![Sequence Diagram](sequence-diagram.png)

## 3. Implementation Details

### 3.1 Time-Based Limits

#### 3.1.1 ExecutionTimeTracker Implementation

```c
/**
 * @file execution_time_tracker.h
 * @brief Time-based limit tracking for regex backtracking
 */

typedef struct rift_execution_time_tracker {
    uint64_t start_time;       /**< Match start timestamp in microseconds */
    uint64_t max_duration;     /**< Maximum allowed duration in microseconds */
    bool time_limit_enabled;   /**< Whether time limiting is enabled */
} rift_execution_time_tracker_t;

/**
 * @brief Create a new execution time tracker
 * @param max_duration_ms Maximum allowed duration in milliseconds (0 = no limit)
 * @return New tracker instance
 */
rift_execution_time_tracker_t* 
rift_execution_time_tracker_create(uint32_t max_duration_ms);

/**
 * @brief Start tracking execution time
 * @param tracker The tracker to start
 */
void 
rift_execution_time_tracker_start(rift_execution_time_tracker_t* tracker);

/**
 * @brief Check if execution time limit has been exceeded
 * @param tracker The tracker to check
 * @return true if limit exceeded, false otherwise
 */
bool 
rift_execution_time_tracker_has_exceeded_limit(
    const rift_execution_time_tracker_t* tracker);

/**
 * @brief Get elapsed time in microseconds
 * @param tracker The tracker to query
 * @return Elapsed time in microseconds
 */
uint64_t 
rift_execution_time_tracker_get_elapsed(
    const rift_execution_time_tracker_t* tracker);

/**
 * @brief Set maximum duration
 * @param tracker The tracker to modify
 * @param max_duration_ms Maximum duration in milliseconds
 */
void 
rift_execution_time_tracker_set_max_duration(
    rift_execution_time_tracker_t* tracker, 
    uint32_t max_duration_ms);

/**
 * @brief Free tracker resources
 * @param tracker The tracker to free
 */
void 
rift_execution_time_tracker_free(rift_execution_time_tracker_t* tracker);
```

#### 3.1.2 Time Tracking Implementation

```c
#include <sys/time.h>
#include "execution_time_tracker.h"

static uint64_t get_current_time_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec) * 1000000 + (uint64_t)(tv.tv_usec);
}

rift_execution_time_tracker_t* 
rift_execution_time_tracker_create(uint32_t max_duration_ms) {
    rift_execution_time_tracker_t* tracker = 
        (rift_execution_time_tracker_t*)malloc(sizeof(rift_execution_time_tracker_t));
    
    if (tracker) {
        tracker->start_time = 0;
        tracker->max_duration = (uint64_t)max_duration_ms * 1000; // Convert to microseconds
        tracker->time_limit_enabled = (max_duration_ms > 0);
    }
    
    return tracker;
}

void 
rift_execution_time_tracker_start(rift_execution_time_tracker_t* tracker) {
    if (tracker && tracker->time_limit_enabled) {
        tracker->start_time = get_current_time_us();
    }
}

bool 
rift_execution_time_tracker_has_exceeded_limit(
    const rift_execution_time_tracker_t* tracker) {
    
    if (!tracker || !tracker->time_limit_enabled || tracker->start_time == 0) {
        return false;
    }
    
    uint64_t current_time = get_current_time_us();
    uint64_t elapsed = current_time - tracker->start_time;
    
    return elapsed > tracker->max_duration;
}

uint64_t 
rift_execution_time_tracker_get_elapsed(
    const rift_execution_time_tracker_t* tracker) {
    
    if (!tracker || tracker->start_time == 0) {
        return 0;
    }
    
    uint64_t current_time = get_current_time_us();
    return current_time - tracker->start_time;
}

void 
rift_execution_time_tracker_set_max_duration(
    rift_execution_time_tracker_t* tracker, 
    uint32_t max_duration_ms) {
    
    if (tracker) {
        tracker->max_duration = (uint64_t)max_duration_ms * 1000;
        tracker->time_limit_enabled = (max_duration_ms > 0);
    }
}

void 
rift_execution_time_tracker_free(rift_execution_time_tracker_t* tracker) {
    free(tracker);
}
```

### 3.2 Configurable Limit Scopes

#### 3.2.1 Limit Configuration Interface

```c
/**
 * @file backtrack_limit_config.h
 * @brief Configurable backtracking limits at different scopes
 */

typedef enum {
    RIFT_BACKTRACK_SCOPE_GLOBAL,
    RIFT_BACKTRACK_SCOPE_PATTERN,
    RIFT_BACKTRACK_SCOPE_MATCH
} rift_backtrack_limit_scope_t;

typedef struct rift_backtrack_limit_config {
    uint32_t max_depth;           /**< Maximum backtracking depth */
    uint32_t max_duration_ms;     /**< Maximum time in milliseconds */
    uint64_t max_transitions;     /**< Maximum state transitions */
    rift_backtrack_limit_scope_t scope; /**< Configuration scope */
    bool override_parent;         /**< Whether to override parent scope */
} rift_backtrack_limit_config_t;

/**
 * @brief Create limit configuration with default values
 * @param scope The scope of this configuration
 * @return New configuration
 */
rift_backtrack_limit_config_t*
rift_backtrack_limit_config_create(rift_backtrack_limit_scope_t scope);

/**
 * @brief Create global limit configuration
 * @param max_depth Maximum backtracking depth
 * @param max_duration_ms Maximum duration in milliseconds
 * @param max_transitions Maximum state transitions
 * @return New global configuration
 */
rift_backtrack_limit_config_t*
rift_backtrack_limit_config_create_global(
    uint32_t max_depth,
    uint32_t max_duration_ms,
    uint64_t max_transitions);

/**
 * @brief Create pattern-specific limit configuration
 * @param pattern_id Pattern identifier
 * @param override_global Whether to override global settings
 * @param max_depth Maximum backtracking depth
 * @param max_duration_ms Maximum duration in milliseconds
 * @param max_transitions Maximum state transitions
 * @return New pattern configuration
 */
rift_backtrack_limit_config_t*
rift_backtrack_limit_config_create_pattern(
    uint32_t pattern_id,
    bool override_global,
    uint32_t max_depth,
    uint32_t max_duration_ms,
    uint64_t max_transitions);

/**
 * @brief Create match-specific limit configuration
 * @param match_id Match operation identifier
 * @param override_pattern Whether to override pattern settings
 * @param max_depth Maximum backtracking depth
 * @param max_duration_ms Maximum duration in milliseconds
 * @param max_transitions Maximum state transitions
 * @return New match configuration
 */
rift_backtrack_limit_config_t*
rift_backtrack_limit_config_create_match(
    uint32_t match_id,
    bool override_pattern,
    uint32_t max_depth,
    uint32_t max_duration_ms,
    uint64_t max_transitions);

/**
 * @brief Free configuration resources
 * @param config The configuration to free
 */
void
rift_backtrack_limit_config_free(rift_backtrack_limit_config_t* config);
```

#### 3.2.2 Configuration Registry Implementation

```c
/**
 * @file backtrack_limit_registry.h
 * @brief Registry for backtracking limit configurations
 */

typedef struct rift_backtrack_limit_registry rift_backtrack_limit_registry_t;

/**
 * @brief Create a new limit registry
 * @return New registry instance
 */
rift_backtrack_limit_registry_t*
rift_backtrack_limit_registry_create(void);

/**
 * @brief Set global limit configuration
 * @param registry The registry to modify
 * @param config The global configuration
 * @return true if successful, false otherwise
 */
bool
rift_backtrack_limit_registry_set_global(
    rift_backtrack_limit_registry_t* registry,
    const rift_backtrack_limit_config_t* config);

/**
 * @brief Register pattern-specific configuration
 * @param registry The registry to modify
 * @param pattern_id Pattern identifier
 * @param config The pattern configuration
 * @return true if successful, false otherwise
 */
bool
rift_backtrack_limit_registry_register_pattern(
    rift_backtrack_limit_registry_t* registry,
    uint32_t pattern_id,
    const rift_backtrack_limit_config_t* config);

/**
 * @brief Register match-specific configuration
 * @param registry The registry to modify
 * @param match_id Match operation identifier
 * @param config The match configuration
 * @return true if successful, false otherwise
 */
bool
rift_backtrack_limit_registry_register_match(
    rift_backtrack_limit_registry_t* registry,
    uint32_t match_id,
    const rift_backtrack_limit_config_t* config);

/**
 * @brief Get effective configuration for a match operation
 * @param registry The registry to query
 * @param pattern_id Pattern identifier
 * @param match_id Match operation identifier
 * @return Effective configuration considering all scopes and overrides
 */
rift_backtrack_limit_config_t*
rift_backtrack_limit_registry_get_effective_config(
    const rift_backtrack_limit_registry_t* registry,
    uint32_t pattern_id,
    uint32_t match_id);

/**
 * @brief Free registry resources
 * @param registry The registry to free
 */
void
rift_backtrack_limit_registry_free(rift_backtrack_limit_registry_t* registry);
```

#### 3.2.3 Registry Implementation

```c
#include <stdlib.h>
#include <string.h>
#include "backtrack_limit_registry.h"

struct rift_backtrack_limit_registry {
    rift_backtrack_limit_config_t* global_config;
    struct {
        uint32_t pattern_id;
        rift_backtrack_limit_config_t* config;
    }* pattern_configs;
    size_t pattern_config_count;
    size_t pattern_config_capacity;

    struct {
        uint32_t match_id;
        rift_backtrack_limit_config_t* config;
    }* match_configs;
    size_t match_config_count;
    size_t match_config_capacity;
};

rift_backtrack_limit_registry_t*
rift_backtrack_limit_registry_create(void) {
    rift_backtrack_limit_registry_t* registry = 
        (rift_backtrack_limit_registry_t*)malloc(sizeof(rift_backtrack_limit_registry_t));
    
    if (registry) {
        registry->global_config = rift_backtrack_limit_config_create_global(
            1000,   // Default max depth
            5000,   // Default max duration (5 seconds)
            100000  // Default max transitions
        );
        
        registry->pattern_configs = NULL;
        registry->pattern_config_count = 0;
        registry->pattern_config_capacity = 0;
        
        registry->match_configs = NULL;
        registry->match_config_count = 0;
        registry->match_config_capacity = 0;
    }
    
    return registry;
}

bool
rift_backtrack_limit_registry_set_global(
    rift_backtrack_limit_registry_t* registry,
    const rift_backtrack_limit_config_t* config) {
    
    if (!registry || !config || config->scope != RIFT_BACKTRACK_SCOPE_GLOBAL) {
        return false;
    }
    
    if (registry->global_config) {
        rift_backtrack_limit_config_free(registry->global_config);
    }
    
    // Make a copy of the configuration
    registry->global_config = rift_backtrack_limit_config_create_global(
        config->max_depth,
        config->max_duration_ms,
        config->max_transitions
    );
    
    return registry->global_config != NULL;
}

// Implementation of pattern registry functions
bool
rift_backtrack_limit_registry_register_pattern(
    rift_backtrack_limit_registry_t* registry,
    uint32_t pattern_id,
    const rift_backtrack_limit_config_t* config) {
    
    if (!registry || !config || config->scope != RIFT_BACKTRACK_SCOPE_PATTERN) {
        return false;
    }
    
    // Check if we need to expand capacity
    if (registry->pattern_config_count >= registry->pattern_config_capacity) {
        size_t new_capacity = registry->pattern_config_capacity == 0 ? 8 :
                              registry->pattern_config_capacity * 2;
        
        void* new_array = realloc(registry->pattern_configs, 
                                  new_capacity * sizeof(*registry->pattern_configs));
        
        if (!new_array) {
            return false;
        }
        
        registry->pattern_configs = new_array;
        registry->pattern_config_capacity = new_capacity;
    }
    
    // Check if pattern already exists
    for (size_t i = 0; i < registry->pattern_config_count; i++) {
        if (registry->pattern_configs[i].pattern_id == pattern_id) {
            // Replace existing configuration
            rift_backtrack_limit_config_free(registry->pattern_configs[i].config);
            
            registry->pattern_configs[i].config = rift_backtrack_limit_config_create_pattern(
                pattern_id,
                config->override_parent,
                config->max_depth,
                config->max_duration_ms,
                config->max_transitions
            );
            
            return registry->pattern_configs[i].config != NULL;
        }
    }
    
    // Add new pattern configuration
    registry->pattern_configs[registry->pattern_config_count].pattern_id = pattern_id;
    registry->pattern_configs[registry->pattern_config_count].config = 
        rift_backtrack_limit_config_create_pattern(
            pattern_id,
            config->override_parent,
            config->max_depth,
            config->max_duration_ms,
            config->max_transitions
        );
    
    if (!registry->pattern_configs[registry->pattern_config_count].config) {
        return false;
    }
    
    registry->pattern_config_count++;
    return true;
}

// Implementation of match registry functions (similar to pattern)
bool
rift_backtrack_limit_registry_register_match(
    rift_backtrack_limit_registry_t* registry,
    uint32_t match_id,
    const rift_backtrack_limit_config_t* config) {
    // Similar to register_pattern implementation
    // ...
    return true;
}

rift_backtrack_limit_config_t*
rift_backtrack_limit_registry_get_effective_config(
    const rift_backtrack_limit_registry_t* registry,
    uint32_t pattern_id,
    uint32_t match_id) {
    
    if (!registry) {
        return NULL;
    }
    
    // Start with global config as base
    rift_backtrack_limit_config_t* effective_config = 
        rift_backtrack_limit_config_create_global(
            registry->global_config->max_depth,
            registry->global_config->max_duration_ms,
            registry->global_config->max_transitions
        );
    
    if (!effective_config) {
        return NULL;
    }
    
    // Apply pattern-specific overrides if applicable
    for (size_t i = 0; i < registry->pattern_config_count; i++) {
        if (registry->pattern_configs[i].pattern_id == pattern_id) {
            rift_backtrack_limit_config_t* pattern_config = 
                registry->pattern_configs[i].config;
            
            if (pattern_config->override_parent) {
                effective_config->max_depth = pattern_config->max_depth;
                effective_config->max_duration_ms = pattern_config->max_duration_ms;
                effective_config->max_transitions = pattern_config->max_transitions;
            }
            
            break;
        }
    }
    
    // Apply match-specific overrides if applicable
    for (size_t i = 0; i < registry->match_config_count; i++) {
        if (registry->match_configs[i].match_id == match_id) {
            rift_backtrack_limit_config_t* match_config = 
                registry->match_configs[i].config;
            
            if (match_config->override_parent) {
                effective_config->max_depth = match_config->max_depth;
                effective_config->max_duration_ms = match_config->max_duration_ms;
                effective_config->max_transitions = match_config->max_transitions;
            }
            
            break;
        }
    }
    
    return effective_config;
}

void
rift_backtrack_limit_registry_free(rift_backtrack_limit_registry_t* registry) {
    if (!registry) {
        return;
    }
    
    // Free global config
    if (registry->global_config) {
        rift_backtrack_limit_config_free(registry->global_config);
    }
    
    // Free pattern configs
    for (size_t i = 0; i < registry->pattern_config_count; i++) {
        rift_backtrack_limit_config_free(registry->pattern_configs[i].config);
    }
    free(registry->pattern_configs);
    
    // Free match configs
    for (size_t i = 0; i < registry->match_config_count; i++) {
        rift_backtrack_limit_config_free(registry->match_configs[i].config);
    }
    free(registry->match_configs);
    
    // Free registry itself
    free(registry);
}
```

### 3.3 Early Bailout Strategies

#### 3.3.1 Strategy Interface

```c
/**
 * @file bailout_strategy.h
 * @brief Early termination strategies for backtracking operations
 */

typedef struct rift_backtracking_state {
    uint32_t current_depth;
    uint64_t elapsed_time_us;
    uint64_t transition_count;
    uint32_t progress_marker;
    bool is_r_syntax;
    void* pattern_data;  /* Pattern-specific data */
} rift_backtracking_state_t;

typedef struct rift_bailout_strategy rift_bailout_strategy_t;

/**
 * @brief Check if strategy should be activated
 * @param strategy The strategy to check
 * @param state Current backtracking state
 * @return true if strategy should activate, false otherwise
 */
typedef bool (*rift_bailout_should_activate_fn)(
    const rift_bailout_strategy_t* strategy,
    const rift_backtracking_state_t* state);

/**
 * @brief Execute bailout strategy
 * @param strategy The strategy to execute
 * @param state Current backtracking state
 * @return Result code from strategy execution
 */
typedef int (*rift_bailout_execute_fn)(
    rift_bailout_strategy_t* strategy,
    const rift_backtracking_state_t* state);

/**
 * @brief Get strategy priority
 * @param strategy The strategy to query
 * @return Priority value (higher = higher priority)
 */
typedef int (*rift_bailout_get_priority_fn)(
    const rift_bailout_strategy_t* strategy);

struct rift_bailout_strategy {
    const char* name;
    rift_bailout_should_activate_fn should_activate;
    rift_bailout_execute_fn execute;
    rift_bailout_get_priority_fn get_priority;
    void* strategy_data;
};

/**
 * @brief Create a new bailout strategy
 * @param name Strategy name
 * @param should_activate Function to check activation conditions
 * @param execute Function to execute the strategy
 * @param get_priority Function to determine strategy priority
 * @param strategy_data Strategy-specific data
 * @return New strategy instance
 */
rift_bailout_strategy_t*
rift_bailout_strategy_create(
    const char* name,
    rift_bailout_should_activate_fn should_activate,
    rift_bailout_execute_fn execute,
    rift_bailout_get_priority_fn get_priority,
    void* strategy_data);

/**
 * @brief Free strategy resources
 * @param strategy The strategy to free
 */
void
rift_bailout_strategy_free(rift_bailout_strategy_t* strategy);
```

#### 3.3.1.1 Strategy Manager Implementation

```c
/**
 * @file bailout_strategy_manager.h
 * @brief Manager for multiple bailout strategies
 */

typedef struct rift_bailout_strategy_manager {
    rift_bailout_strategy_t** strategies;
    size_t strategy_count;
    size_t strategy_capacity;
} rift_bailout_strategy_manager_t;

/**
 * @brief Create a new bailout strategy manager
 * @return New manager instance
 */
rift_bailout_strategy_manager_t*
rift_bailout_strategy_manager_create(void);

/**
 * @brief Add a strategy to the manager
 * @param manager The manager to modify
 * @param strategy The strategy to add
 * @return true if successful, false otherwise
 */
bool
rift_bailout_strategy_manager_add_strategy(
    rift_bailout_strategy_manager_t* manager,
    rift_bailout_strategy_t* strategy);

/**
 * @brief Evaluate all strategies and select the highest priority active one
 * @param manager The manager to query
 * @param state Current backtracking state
 * @return The selected strategy or NULL if none are active
 */
rift_bailout_strategy_t*
rift_bailout_strategy_manager_evaluate(
    const rift_bailout_strategy_manager_t* manager,
    const rift_backtracking_state_t* state);

/**
 * @brief Execute the selected strategy
 * @param manager The manager to use
 * @param strategy The strategy to execute
 * @param state Current backtracking state
 * @return Result code from strategy execution
 */
int
rift_bailout_strategy_manager_execute(
    rift_bailout_strategy_manager_t* manager,
    rift_bailout_strategy_t* strategy,
    const rift_backtracking_state_t* state);

/**
 * @brief Free manager resources
 * @param manager The manager to free
 * @param free_strategies Whether to also free contained strategies
 */
void
rift_bailout_strategy_manager_free(
    rift_bailout_strategy_manager_t* manager,
    bool free_strategies);
```

```c
#include "bailout_strategy_manager.h"
#include <stdlib.h>

rift_bailout_strategy_manager_t*
rift_bailout_strategy_manager_create(void) {
    rift_bailout_strategy_manager_t* manager = 
        (rift_bailout_strategy_manager_t*)malloc(sizeof(rift_bailout_strategy_manager_t));
    
    if (!manager) {
        return NULL;
    }
    
    manager->strategies = NULL;
    manager->strategy_count = 0;
    manager->strategy_capacity = 0;
    
    return manager;
}

bool
rift_bailout_strategy_manager_add_strategy(
    rift_bailout_strategy_manager_t* manager,
    rift_bailout_strategy_t* strategy) {
    
    if (!manager || !strategy) {
        return false;
    }
    
    // Check if we need to expand capacity
    if (manager->strategy_count >= manager->strategy_capacity) {
        size_t new_capacity = manager->strategy_capacity == 0 ? 4 :
                               manager->strategy_capacity * 2;
        
        void* new_array = realloc(manager->strategies, 
                                 new_capacity * sizeof(rift_bailout_strategy_t*));
        
        if (!new_array) {
            return false;
        }
        
        manager->strategies = new_array;
        manager->strategy_capacity = new_capacity;
    }
    
    // Add the strategy
    manager->strategies[manager->strategy_count++] = strategy;
    return true;
}

rift_bailout_strategy_t*
rift_bailout_strategy_manager_evaluate(
    const rift_bailout_strategy_manager_t* manager,
    const rift_backtracking_state_t* state) {
    
    if (!manager || !state) {
        return NULL;
    }
    
    rift_bailout_strategy_t* selected_strategy = NULL;
    int highest_priority = -1;
    
    // Evaluate each strategy
    for (size_t i = 0; i < manager->strategy_count; i++) {
        rift_bailout_strategy_t* strategy = manager->strategies[i];
        
        // Skip invalid strategies
        if (!strategy || !strategy->should_activate) {
            continue;
        }
        
        // Check if strategy should activate
        if (strategy->should_activate(strategy, state)) {
            // Get priority
            int priority = strategy->get_priority ? 
                           strategy->get_priority(strategy) : 0;
            
            // Select highest priority
            if (priority > highest_priority) {
                highest_priority = priority;
                selected_strategy = strategy;
            }
        }
    }
    
    return selected_strategy;
}

int
rift_bailout_strategy_manager_execute(
    rift_bailout_strategy_manager_t* manager,
    rift_bailout_strategy_t* strategy,
    const rift_backtracking_state_t* state) {
    
    if (!manager || !strategy || !state || !strategy->execute) {
        return -1;
    }
    
    return strategy->execute(strategy, state);
}

void
rift_bailout_strategy_manager_free(
    rift_bailout_strategy_manager_t* manager,
    bool free_strategies) {
    
    if (!manager) {
        return;
    }
    
    if (free_strategies && manager->strategies) {
        for (size_t i = 0; i < manager->strategy_count; i++) {
            rift_bailout_strategy_free(manager->strategies[i]);
        }
    }
    
    free(manager->strategies);
    free(manager);
}

#### 3.3.2 Pattern Complexity Strategy

```c
/**
 * @file pattern_complexity_strategy.h
 * @brief Bailout strategy based on pattern complexity
 */

typedef struct rift_pattern_complexity_data {
    uint32_t state_count;
    float branching_factor;
    uint32_t repetition_count;
    float complexity_threshold;
} rift_pattern_complexity_data_t;

/**
 * @brief Create a pattern complexity bailout strategy
 * @param state_count Number of states in the pattern
 * @param branching_factor Average number of transitions per state
 * @param repetition_count Number of quantifiers/repetitions
 * @param complexity_threshold Threshold value for early bailout
 * @return New strategy instance
 */
rift_bailout_strategy_t*
rift_bailout_pattern_complexity_strategy_create(
    uint32_t state_count,
    float branching_factor,
    uint32_t repetition_count,
    float complexity_threshold);

/**
 * @brief Calculate complexity score for a pattern
 * @param pattern The regex pattern to analyze
 * @param is_r_syntax Whether pattern uses R'' syntax
 * @return Complexity score (higher = more complex)
 */
float
rift_bailout_calculate_pattern_complexity(
    const rift_regex_pattern_t* pattern,
    bool is_r_syntax);
```

#### 3.3.3 Pattern Complexity Implementation

```c
#include "pattern_complexity_strategy.h"
#include "bailout_strategy.h"

static bool
pattern_complexity_should_activate(
    const rift_bailout_strategy_t* strategy,
    const rift_backtracking_state_t* state) {
    
    if (!strategy || !state || !strategy->strategy_data) {
        return false;
    }
    
    rift_pattern_complexity_data_t* data = 
        (rift_pattern_complexity_data_t*)strategy->strategy_data;
    
    // Calculate complexity score
    float complexity_score = 
        (data->state_count * data->branching_factor) + 
        (data->repetition_count * data->repetition_count);
    
    // Adjust for R'' syntax if needed
    if (state->is_r_syntax) {
        complexity_score *= 1.5f;  // R'' syntax can be more complex
    }
    
    // Factor in current depth - more likely to bail as we go deeper
    float depth_factor = (float)state->current_depth / 100.0f;
    complexity_score *= (1.0f + depth_factor);
    
    // Compare against threshold
    return complexity_score > data->complexity_threshold;
}

static int
pattern_complexity_execute(
    rift_bailout_strategy_t* strategy,
    const rift_backtracking_state_t* state) {
    
    // This strategy simply prevents further backtracking
    // Return code 1 indicates bailout was executed
    return 1;
}

static int
pattern_complexity_get_priority(
    const rift_bailout_strategy_t* strategy) {
    
    // This strategy has medium priority
    return 50;
}

rift_bailout_strategy_t*
rift_bailout_pattern_complexity_strategy_create(
    uint32_t state_count,
    float branching_factor,
    uint32_t repetition_count,
    float complexity_threshold) {
    
    rift_pattern_complexity_data_t* data = 
        (rift_pattern_complexity_data_t*)malloc(sizeof(rift_pattern_complexity_data_t));
    
    if (!data) {
        return NULL;
    }
    
    data->state_count = state_count;
    data->branching_factor = branching_factor;
    data->repetition_count = repetition_count;
    data->complexity_threshold = complexity_threshold;
    
    rift_bailout_strategy_t* strategy = rift_bailout_strategy_create(
        "Pattern Complexity Strategy",
        pattern_complexity_should_activate,
        pattern_complexity_execute,
        pattern_complexity_get_priority,
        data
    );
    
    if (!strategy) {
        free(data);
        return NULL;
    }
    
    return strategy;
}

float
rift_bailout_calculate_pattern_complexity(
    const rift_regex_pattern_t* pattern,
    bool is_r_syntax) {
    
    if (!pattern) {
        return 0.0f;
    }
    
    // Extract automaton from pattern
    rift_regex_automaton_t* automaton = rift_regex_pattern_get_automaton(pattern);
    if (!automaton) {
        return 0.0f;
    }
    
    // Get automaton statistics
    uint32_t state_count = rift_automaton_get_state_count(automaton);
    uint32_t transition_count = rift_automaton_get_transition_count(automaton);
    
    // Calculate branching factor
    float branching_factor = state_count > 0 ? 
                            (float)transition_count / (float)state_count : 0.0f;
    
    // Count repetition nodes from AST
    uint32_t repetition_count = 0;
    const rift_regex_ast_t* ast = rift_regex_pattern_get_ast(pattern);
    if (ast) {
        repetition_count = rift_regex_ast_count_nodes_of_type(
            ast, RIFT_REGEX_AST_NODE_QUANTIFIER);
    }
    
    // Calculate base complexity
    float complexity = (state_count * branching_factor) + 
                       (repetition_count * repetition_count);
    
    // Apply R'' syntax factor if applicable
    if (is_r_syntax) {
        complexity *= 1.5f;
    }
    
    return complexity;
}
```

#### 3.3.4 Progress Tracking Strategy

```c
/**
 * @file progress_tracking_strategy.h
 * @brief Bailout strategy based on match progress
 */

typedef struct rift_progress_tracking_data {
    float progress_threshold;
    uint32_t last_progress_point;
    uint32_t stagnation_count;
    uint32_t max_stagnation_allowed;
} rift_progress_tracking_data_t;

/**
 * @brief Create a progress tracking bailout strategy
 * @param progress_threshold Minimum required progress ratio
 * @param max_stagnation_allowed Maximum allowed periods without progress
 * @return New strategy instance
 */
rift_bailout_strategy_t*
rift_bailout_progress_tracking_strategy_create(
    float progress_threshold,
    uint32_t max_stagnation_allowed);

/**
 * @brief Update progress marker
 * @param strategy The strategy to update
 * @param current_position Current position in input
 * @return true if progress was made, false otherwise
 */
bool
rift_bailout_progress_update_marker(
    rift_bailout_strategy_t* strategy,
    uint32_t current_position);
```

#### 3.3.5 Progress Tracking Implementation

```c
#include "progress_tracking_strategy.h"
#include "bailout_strategy.h"

static bool
progress_tracking_should_activate(
    const rift_bailout_strategy_t* strategy,
    const rift_backtracking_state_t* state) {
    
    if (!strategy || !state || !strategy->strategy_data) {
        return false;
    }
    
    rift_progress_tracking_data_t* data = 
        (rift_progress_tracking_data_t*)strategy->strategy_data;
    
    // If we've made no progress but backtracked several times,
    // that's a sign of ineffective matching
    if (state->progress_marker <= data->last_progress_point) {
        return data->stagnation_count >= data->max_stagnation_allowed;
    }
    
    // We've made progress, so reset stagnation counter
    data->last_progress_point = state->progress_marker;
    data->stagnation_count = 0;
    return false;
}

static int
progress_tracking_execute(
    rift_bailout_strategy_t* strategy,
    const rift_backtracking_state_t* state) {
    
    // This strategy simply prevents further backtracking
    // Return code 1 indicates bailout was executed
    return 1;
}

static int
progress_tracking_get_priority(
    const rift_bailout_strategy_t* strategy) {
    
    // This strategy has high priority
    return 80;
}

rift_bailout_strategy_t*
rift_bailout_progress_tracking_strategy_create(
    float progress_threshold,
    uint32_t max_stagnation_allowed) {
    
    rift_progress_tracking_data_t* data = 
        (rift_progress_tracking_data_t*)malloc(sizeof(rift_progress_tracking_data_t));
    
    if (!data) {
        return NULL;
    }
    
    data->progress_threshold = progress_threshold;
    data->last_progress_point = 0;
    data->stagnation_count = 0;
    data->max_stagnation_allowed = max_stagnation_allowed;
    
    rift_bailout_strategy_t* strategy = rift_bailout_strategy_create(
        "Progress Tracking Strategy",
        progress_tracking_should_activate,
        progress_tracking_execute,
        progress_tracking_get_priority,
        data
    );
    
    if (!strategy) {
        free(data);
        return NULL;
    }
    
    return strategy;
}

bool
rift_bailout_progress_update_marker(
    rift_bailout_strategy_t* strategy,
    uint32_t current_position) {
    
    if (!strategy || !strategy->strategy_data) {
        return false;
    }
    
    rift_progress_tracking_data_t* data = 
        (rift_progress_tracking_data_t*)strategy->strategy_data;
    
    if (current_position > data->last_progress_point) {
        data->last_progress_point = current_position;
        data->stagnation_count = 0;
        return true;
    } else {
        data->stagnation_count++;
        return false;
    }
}