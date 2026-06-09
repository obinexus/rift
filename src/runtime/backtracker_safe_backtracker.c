runtime/backtracker_safe_backtracker.h"
runtime/backtracker_safe_backtracker.h
tmp/librift_analysis/librift-4933472-W/backup/v1/src/runtime/backtracker_safe_backtracker.h
tmp/librift_analysis/librift-4933472-W/backup/v2/runtime/backtracker_safe_backtracker.h
tmp/librift_analysis/librift-4933472-W/src/runtime/backtracker_safe_backtracker.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/runtime/backtracker_safe_backtracker.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v2/runtime/backtracker_safe_backtracker.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/runtime/backtracker_safe_backtracker.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/runtime/backtracker_safe_backtracker.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v2/runtime/backtracker_safe_backtracker.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/runtime/backtracker_safe_backtracker.h"
core/runtime/backtracker_safe_backtracker.h"
/**
 * @file safe_backtracker.c
 * @brief Implementation of thread-safe backtracker for the LibRift regex engine
 *
 * This file implements the thread-safe backtracker functions declared in
 * safe_backtracker.h for ensuring thread safety and backtracking limits
 * in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/runtime/backtracker_safe_backtracker.h
#include "core/runtime/backtracker.h"
#include "librift/runtime/safe_backtracker.h"

/**
 * @brief Cleanup function for thread-local backtracker
 *
 * This function is registered with pthread_key_create() to ensure that
 * thread-local resources are cleaned up when a thread exits.
 *
 * @param backtracker The thread-local backtracker to free
 */
static void
thread_local_backtracker_cleanup(void *backtracker)
{
    if (backtracker) {
        rift_backtracker_free((rift_regex_backtracker_t *)backtracker);
    }
}

/**
 * @brief Mutex for global backtracker operations
 * Protects operations on the registry of thread-local backtrackers
 */
static pthread_mutex_t global_backtracker_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Registry of active thread-local backtrackers for cleanup
 * This allows controlled cleanup when the library is unloaded
 */
static struct {
    rift_regex_safe_backtracker_t **backtrackers;
    size_t count;
    size_t capacity;
    pthread_mutex_t mutex;
    bool initialized;
} backtracker_registry = {NULL, 0, 0, PTHREAD_MUTEX_INITIALIZER, false};

/**
 * @brief Initialize the backtracker registry
 *
 * @return true if initialization successful, false otherwise
 */
static bool
initialize_backtracker_registry(void)
{
    if (pthread_mutex_lock(&backtracker_registry.mutex) != 0) {
        return false;
    }

    if (!backtracker_registry.initialized) {
        backtracker_registry.capacity = 16;
        backtracker_registry.backtrackers = (rift_regex_safe_backtracker_t **)malloc(
            backtracker_registry.capacity * sizeof(rift_regex_safe_backtracker_t *));

        if (!backtracker_registry.backtrackers) {
            pthread_mutex_unlock(&backtracker_registry.mutex);
            return false;
        }

        backtracker_registry.count = 0;
        backtracker_registry.initialized = true;
    }

    pthread_mutex_unlock(&backtracker_registry.mutex);
    return true;
}

/**
 * @brief Register a safe backtracker in the registry
 *
 * @param backtracker The backtracker to register
 * @return true if registration successful, false otherwise
 */
static bool
register_safe_backtracker(rift_regex_safe_backtracker_t *backtracker)
{
    if (!backtracker || !backtracker_registry.initialized) {
        return false;
    }

    if (pthread_mutex_lock(&backtracker_registry.mutex) != 0) {
        return false;
    }

    // Check if we need to expand the registry
    if (backtracker_registry.count >= backtracker_registry.capacity) {
        size_t new_capacity = backtracker_registry.capacity * 2;
        rift_regex_safe_backtracker_t **new_backtrackers =
            (rift_regex_safe_backtracker_t **)realloc(backtracker_registry.backtrackers,
                                                      new_capacity *
                                                          sizeof(rift_regex_safe_backtracker_t *));

        if (!new_backtrackers) {
            pthread_mutex_unlock(&backtracker_registry.mutex);
            return false;
        }

        backtracker_registry.backtrackers = new_backtrackers;
        backtracker_registry.capacity = new_capacity;
    }

    // Add the backtracker to the registry
    backtracker_registry.backtrackers[backtracker_registry.count++] = backtracker;

    pthread_mutex_unlock(&backtracker_registry.mutex);
    return true;
}

/**
 * @brief Unregister a safe backtracker from the registry
 *
 * @param backtracker The backtracker to unregister
 * @return true if unregistration successful, false otherwise
 */
static bool
unregister_safe_backtracker(rift_regex_safe_backtracker_t *backtracker)
{
    if (!backtracker || !backtracker_registry.initialized) {
        return false;
    }

    if (pthread_mutex_lock(&backtracker_registry.mutex) != 0) {
        return false;
    }

    // Find the backtracker in the registry
    for (size_t i = 0; i < backtracker_registry.count; i++) {
        if (backtracker_registry.backtrackers[i] == backtracker) {
            // Remove by shifting remaining elements
            if (i < backtracker_registry.count - 1) {
                memmove(&backtracker_registry.backtrackers[i],
                        &backtracker_registry.backtrackers[i + 1],
                        (backtracker_registry.count - i - 1) *
                            sizeof(rift_regex_safe_backtracker_t *));
            }
            backtracker_registry.count--;
            pthread_mutex_unlock(&backtracker_registry.mutex);
            return true;
        }
    }

    pthread_mutex_unlock(&backtracker_registry.mutex);
    return false;
}
/**
 * @brief Create a new thread-safe backtracker
 *
 * @param max_depth Maximum backtracking depth
 * @param num_groups Number of capture groups
 * @return Pointer to the new thread-safe backtracker or NULL on failure
 */
rift_regex_safe_backtracker_t *
rift_safe_backtracker_create(uint32_t max_depth, size_t num_groups)
{
    rift_regex_safe_backtracker_t *safe_bt =
        (rift_regex_safe_backtracker_t *)rift_malloc(sizeof(rift_regex_safe_backtracker_t));
    if (!safe_bt) {
        return NULL;
    }

    /* Initialize the mutex */
    if (pthread_mutex_init(&safe_bt->mutex, NULL) != 0) {
        rift_free(safe_bt);
        return NULL;
    }

    /* Initialize thread-local storage key */
    if (pthread_key_create(&safe_bt->thread_local_key, thread_local_backtracker_cleanup) != 0) {
        pthread_mutex_destroy(&safe_bt->mutex);
        rift_free(safe_bt);
        return NULL;
    }

    /* Initialize the global backtracker as a fallback */
    safe_bt->global_bt = rift_backtracker_create(max_depth, num_groups);
    if (!safe_bt->global_bt) {
        pthread_key_delete(safe_bt->thread_local_key);
        pthread_mutex_destroy(&safe_bt->mutex);
        rift_free(safe_bt);
        return NULL;
    }

    /* Initialize other fields */
    safe_bt->max_depth = max_depth;
    safe_bt->initialized = true;

    /* Register with the global registry */
    if (!initialize_backtracker_registry() || !register_safe_backtracker(safe_bt)) {
        rift_safe_backtracker_free(safe_bt);
        return NULL;
    }

    return safe_bt;
}

/**
 * @brief Get the thread-local backtracker
 *
 * @param safe_bt The thread-safe backtracker
 * @return Pointer to the thread-local backtracker or NULL on failure
 */
// In safe_backtracker.c
rift_regex_backtracker_t *
rift_safe_backtracker_get_local(rift_regex_safe_backtracker_t *safe_bt)
{
    if (!safe_bt || !safe_bt->initialized) {
        return NULL;
    }

    /* Try to get the thread-local backtracker */
    rift_regex_backtracker_t *local_bt =
        (rift_regex_backtracker_t *)pthread_getspecific(safe_bt->thread_local_key);

    /* If no thread-local backtracker exists, create one */
    if (!local_bt) {
        /* Lock the mutex to ensure thread safety during creation */
        if (pthread_mutex_lock(&safe_bt->mutex) != 0) {
            return NULL;
        }

        /* Double-check after acquiring the lock (double-checked locking pattern) */
        local_bt = (rift_regex_backtracker_t *)pthread_getspecific(safe_bt->thread_local_key);
        if (!local_bt) {
            /* Create a new thread-local backtracker */
            size_t num_groups = rift_backtracker_get_num_groups(safe_bt->global_bt);
            local_bt = rift_backtracker_create(safe_bt->max_depth, num_groups);

            if (local_bt) {
                /* Associate the backtracker with the thread-local storage */
                if (pthread_setspecific(safe_bt->thread_local_key, local_bt) != 0) {
                    rift_backtracker_free(local_bt);
                    local_bt = NULL;
                }
            }
        }

        pthread_mutex_unlock(&safe_bt->mutex);

        /* Fall back to global backtracker if local creation failed */
        if (!local_bt) {
            return safe_bt->global_bt;
        }
    }

    return local_bt;
}

/**
 * @brief Push a new backtrack point onto the backtracker stack
 *
 * @param safe_bt The thread-safe backtracker
 * @param state The current state
 * @param input_position Position in the input string
 * @param group_starts Array of group start positions
 * @param group_ends Array of group end positions
 * @param num_groups Number of groups
 * @param error Pointer to store error information (can be NULL)
 * @return true if the point was pushed successfully, false otherwise
 */
bool
rift_safe_backtracker_push(rift_regex_safe_backtracker_t *safe_bt, rift_regex_state_t *state,
                           size_t input_position, const size_t *group_starts,
                           const size_t *group_ends, size_t num_groups, rift_regex_error_t *error)
{
    if (!safe_bt || !state || !group_starts || !group_ends) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_INVALID_PARAMETER,
                                              "Invalid backtracker parameters");
        }
        return false;
    }

    /* Get the thread-local backtracker */
    rift_regex_backtracker_t *local_bt = rift_safe_backtracker_get_local(safe_bt);
    if (!local_bt) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_INTERNAL,
                                              "Failed to get thread-local backtracker");
        }
        return false;
    }

    /* Check if we've exceeded the maximum backtracking depth */
    size_t current_depth = rift_backtracker_get_depth(local_bt);
    if (current_depth >= safe_bt->max_depth) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_BACKTRACKING_LIMIT,
                                              "Maximum backtracking depth exceeded");
        }
        return false;
    }

    /* Push the backtrack point */
    return rift_backtracker_push(local_bt, state, input_position, group_starts, group_ends,
                                 num_groups);
}

/**
 * @brief Pop a backtrack point from the backtracker stack
 *
 * @param safe_bt The thread-safe backtracker
 * @param state Pointer to store the state
 * @param input_position Pointer to store the input position
 * @param group_starts Array to store group start positions
 * @param group_ends Array to store group end positions
 * @param num_groups Pointer to store the number of groups
 * @param error Pointer to store error information (can be NULL)
 * @return true if a point was popped, false if the stack was empty
 */
bool
rift_safe_backtracker_pop(rift_regex_safe_backtracker_t *safe_bt, rift_regex_state_t **state,
                          size_t *input_position, size_t *group_starts, size_t *group_ends,
                          size_t *num_groups, rift_regex_error_t *error)
{
    if (!safe_bt || !state || !input_position || !group_starts || !group_ends || !num_groups) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_INVALID_PARAMETER,
                                              "Invalid backtracker parameters");
        }
        return false;
    }

    /* Get the thread-local backtracker */
    rift_regex_backtracker_t *local_bt = rift_safe_backtracker_get_local(safe_bt);
    if (!local_bt) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_INTERNAL,
                                              "Failed to get thread-local backtracker");
        }
        return false;
    }

    /* Pop the backtrack point */
    return rift_backtracker_pop(local_bt, state, input_position, group_starts, group_ends,
                                num_groups);
}

/**
 * @brief Check if the backtracker stack is empty
 *
 * @param safe_bt The thread-safe backtracker
 * @param error Pointer to store error information (can be NULL)
 * @return true if the stack is empty, false otherwise
 */
bool
rift_safe_backtracker_is_empty(rift_regex_safe_backtracker_t *safe_bt, rift_regex_error_t *error)
{
    if (!safe_bt) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_INVALID_PARAMETER,
                                              "Invalid backtracker parameter");
        }
        return true; /* Consider NULL backtracker as empty */
    }

    /* Get the thread-local backtracker */
    rift_regex_backtracker_t *local_bt = rift_safe_backtracker_get_local(safe_bt);
    if (!local_bt) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_INTERNAL,
                                              "Failed to get thread-local backtracker");
        }
        return true; /* Consider failure to get backtracker as empty */
    }

    /* Check if the backtracker is empty */
    return rift_backtracker_is_empty(local_bt);
}

/**
 * @brief Reset the backtracker
 *
 * @param safe_bt The thread-safe backtracker
 * @param error Pointer to store error information (can be NULL)
 * @return true if successful, false otherwise
 */
bool
rift_safe_backtracker_reset(rift_regex_safe_backtracker_t *safe_bt, rift_regex_error_t *error)
{
    if (!safe_bt) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_INVALID_PARAMETER,
                                              "Invalid backtracker parameter");
        }
        return false;
    }

    /* Get the thread-local backtracker */
    rift_regex_backtracker_t *local_bt = rift_safe_backtracker_get_local(safe_bt);
    if (!local_bt) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_INTERNAL,
                                              "Failed to get thread-local backtracker");
        }
        return false;
    }

    /* Reset the backtracker */
    rift_backtracker_reset(local_bt);
    return true;
}

/**
 * @brief Get the current depth of the backtracker stack
 *
 * @param safe_bt The thread-safe backtracker
 * @param error Pointer to store error information (can be NULL)
 * @return The current depth or (size_t)-1 on error
 */
size_t
rift_safe_backtracker_get_depth(rift_regex_safe_backtracker_t *safe_bt, rift_regex_error_t *error)
{
    if (!safe_bt) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_INVALID_PARAMETER,
                                              "Invalid backtracker parameter");
        }
        return (size_t)-1;
    }

    /* Get the thread-local backtracker */
    rift_regex_backtracker_t *local_bt = rift_safe_backtracker_get_local(safe_bt);
    if (!local_bt) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_INTERNAL,
                                              "Failed to get thread-local backtracker");
        }
        return (size_t)-1;
    }

    /* Get the current depth */
    return rift_backtracker_get_depth(local_bt);
}

/**
 * @brief Get the maximum depth allowed for the backtracker
 *
 * @param safe_bt The thread-safe backtracker
 * @return The maximum allowed depth
 */
uint32_t
rift_safe_backtracker_get_max_depth(const rift_regex_safe_backtracker_t *safe_bt)
{
    if (!safe_bt) {
        return 0; /* Return 0 for NULL backtracker */
    }

    return safe_bt->max_depth;
}
/**
 * @brief Set the maximum depth for the backtracker
 *
 * @param safe_bt The thread-safe backtracker
 * @param max_depth The new maximum depth
 * @return true if successful, false otherwise
 */
bool
rift_safe_backtracker_set_max_depth(rift_regex_safe_backtracker_t *safe_bt, uint32_t max_depth)
{
    if (!safe_bt) {
        return false;
    }

    /* Lock the mutex to ensure thread safety */
    if (pthread_mutex_lock(&safe_bt->mutex) != 0) {
        return false;
    }

    /* Set the maximum depth */
    safe_bt->max_depth = max_depth;

    /* Set the maximum depth for the global backtracker */
    rift_backtracker_set_max_depth(safe_bt->global_bt, max_depth);

    /* Unlock the mutex */
    pthread_mutex_unlock(&safe_bt->mutex);

    return true;
}

/**
 * @brief Free resources associated with a thread-safe backtracker
 *
 * @param safe_bt The thread-safe backtracker to free
 */
void
rift_safe_backtracker_free(rift_regex_safe_backtracker_t *safe_bt)
{
    if (!safe_bt) {
        return;
    }

    if (safe_bt->initialized) {
        /* Clean up thread-local storage key */
        pthread_key_delete(safe_bt->thread_local_key);

        /* Clean up mutex */
        pthread_mutex_destroy(&safe_bt->mutex);

        /* Free the global backtracker */
        if (safe_bt->global_bt) {
            rift_backtracker_free(safe_bt->global_bt);
        }
    }

    /* Free the thread-safe backtracker itself */
    rift_free(safe_bt);
}
