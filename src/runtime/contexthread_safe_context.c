ontexthread_safe_context.h"/a #include "core/errors/error.h"
runtime/contexthread_safe_context.h
tmp/librift_analysis/librift-4933472-W/src/runtime/contexthread_safe_context.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/runtime/contexthread_safe_context.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/runtime/contexthread_safe_context.h"
ontexthread_safe_context.h"/a #include "core/errors/error.h"
ontexthread_safe_context.h"/a #include "core/errors/error.h"
runtime/contexthread_safe_context.h"
core/runtime/contexthread_safe_context.h"
/**
 * @file thread_safe_context.c
 * @brief Implementation of thread-safe context for the LibRift regex engine
 *
 * This file implements the thread-safe context functions declared in
 * thread_safe_context.h for ensuring thread safety in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/runtime/contexthread_safe_context.h
#include "core/runtime/context.h"
#include "librift/runtime/thread_safe_context.h"
ontexthread_safe_context.h"/a #include "core/errors/error.h"

/**
 * @brief Cleanup function for thread-local matcher context
 *
 * This function is registered with pthread_key_create() to ensure that
 * thread-local resources are cleaned up when a thread exits.
 *
 * @param context The thread-local matcher context to free
 */
static void
thread_local_context_cleanup(void *context)
{
    if (context) {
        rift_matcher_context_free((rift_regex_matcher_context_t *)context);
    }
}

/**
 * @brief Create a new thread-safe context
 *
 * @param automaton The automaton to associate with the context
 * @param max_capture_groups Maximum number of capture groups to support
 * @param max_backtrack_depth Maximum backtracking depth
 * @return Pointer to the new thread-safe context or NULL on failure
 */
rift_regex_thread_safe_context_t *
rift_thread_safe_context_create(rift_regex_automaton_t *automaton, size_t max_capture_groups,
                                uint32_t max_backtrack_depth)
{
    if (!automaton) {
        return NULL;
    }

    rift_regex_thread_safe_context_t *ts_context =
        (rift_regex_thread_safe_context_t *)rift_malloc(sizeof(rift_regex_thread_safe_context_t));
    if (!ts_context) {
        return NULL;
    }

    /* Initialize the mutex */
    if (pthread_mutex_init(&ts_context->mutex, NULL) != 0) {
        rift_free(ts_context);
        return NULL;
    }

    /* Initialize thread-local storage key */
    if (pthread_key_create(&ts_context->thread_local_key, thread_local_context_cleanup) != 0) {
        pthread_mutex_destroy(&ts_context->mutex);
        rift_free(ts_context);
        return NULL;
    }

    /* Initialize the global context as a fallback */
    ts_context->global_ctx = rift_matcher_context_create(NULL, 0, max_capture_groups);
    if (!ts_context->global_ctx) {
        pthread_key_delete(ts_context->thread_local_key);
        pthread_mutex_destroy(&ts_context->mutex);
        rift_free(ts_context);
        return NULL;
    }

    /* Initialize other fields */
    ts_context->automaton = automaton;
    ts_context->ref_count = 1;
    ts_context->initialized = true;
    ts_context->max_backtrack_depth = max_backtrack_depth;
    rift_regex_error_clear(&ts_context->error);

    return ts_context;
}

/**
 * @brief Get the thread-local matcher context
 *
 * @param ts_context The thread-safe context
 * @return Pointer to the thread-local matcher context or NULL on failure
 */
rift_regex_matcher_context_t *
rift_thread_safe_context_get_local(rift_regex_thread_safe_context_t *ts_context)
{
    if (!ts_context || !ts_context->initialized) {
        return NULL;
    }

    /* Try to get the thread-local context */
    rift_regex_matcher_context_t *local_ctx =
        (rift_regex_matcher_context_t *)pthread_getspecific(ts_context->thread_local_key);

    /* If no thread-local context exists, create one */
    if (!local_ctx) {
        /* Lock the mutex to ensure thread safety during creation */
        if (pthread_mutex_lock(&ts_context->mutex) != 0) {
            return NULL;
        }

        /* Create a new thread-local context with the same parameters as the global one */
        local_ctx = rift_matcher_context_create(
            rift_matcher_context_get_input(ts_context->global_ctx),
            rift_matcher_context_get_input_length(ts_context->global_ctx),
            ts_context->global_ctx->max_capture_groups);

        pthread_mutex_unlock(&ts_context->mutex);

        if (!local_ctx) {
            return NULL;
        }

        /* Associate the context with the thread-local storage */
        if (pthread_setspecific(ts_context->thread_local_key, local_ctx) != 0) {
            rift_matcher_context_free(local_ctx);
            return NULL;
        }
    }

    return local_ctx;
}

/**
 * @brief Execute a function with a thread-local matcher context
 *
 * @param ts_context The thread-safe context
 * @param callback Function to call with the thread-local context
 * @param user_data User-defined data to pass to the callback
 * @param error Pointer to store error information (can be NULL)
 * @return The return value from the callback, or false on error
 */
bool
rift_thread_safe_context_execute(rift_regex_thread_safe_context_t *ts_context,
                                 bool (*callback)(rift_regex_matcher_context_t *, void *,
                                                  rift_regex_error_t *),
                                 void *user_data, rift_regex_error_t *error)
{
    if (!ts_context || !callback) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_INVALID_PARAMETER,
                                              "Invalid thread-safe context or callback");
        }
        return false;
    }

    /* Get the thread-local context */
    rift_regex_matcher_context_t *local_ctx = rift_thread_safe_context_get_local(ts_context);
    if (!local_ctx) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_INTERNAL,
                                              "Failed to get thread-local context");
        }
        return false;
    }

    /* Execute the callback with the thread-local context */
    return callback(local_ctx, user_data, error);
}

/**
 * @brief Set the input string for the thread-safe context
 *
 * @param ts_context The thread-safe context
 * @param input The input string to set
 * @param input_length Length of the input string or (size_t)-1 to use strlen
 * @return true if successful, false otherwise
 */
bool
rift_thread_safe_context_set_input(rift_regex_thread_safe_context_t *ts_context, const char *input,
                                   size_t input_length)
{
    if (!ts_context || !input) {
        return false;
    }

    /* Lock the mutex to ensure thread safety */
    if (pthread_mutex_lock(&ts_context->mutex) != 0) {
        return false;
    }

    /* Calculate input length if not provided */
    if (input_length == (size_t)-1) {
        input_length = strlen(input);
    }

    /* Set the input for the global context */
    bool success = rift_matcher_context_set_input(ts_context->global_ctx, input, input_length);

    /* Unlock the mutex */
    pthread_mutex_unlock(&ts_context->mutex);

    return success;
}

/**
 * @brief Free resources associated with a thread-safe context
 *
 * @param ts_context The thread-safe context to free
 */
void
rift_thread_safe_context_free(rift_regex_thread_safe_context_t *ts_context)
{
    if (!ts_context) {
        return;
    }

    if (ts_context->initialized) {
        /* Clean up thread-local storage key */
        pthread_key_delete(ts_context->thread_local_key);

        /* Clean up mutex */
        pthread_mutex_destroy(&ts_context->mutex);

        /* Free the global context */
        if (ts_context->global_ctx) {
            rift_matcher_context_free(ts_context->global_ctx);
        }
    }

    /* Free the thread-safe context itself */
    rift_free(ts_context);
}

/**
 * @brief Increment the reference count for a thread-safe context
 *
 * @param ts_context The thread-safe context
 * @return The new reference count
 */
size_t
rift_thread_safe_context_ref(rift_regex_thread_safe_context_t *ts_context)
{
    if (!ts_context) {
        return 0;
    }

    /* Lock the mutex to ensure thread safety */
    if (pthread_mutex_lock(&ts_context->mutex) != 0) {
        return 0;
    }

    /* Increment the reference count */
    ts_context->ref_count++;
    size_t new_count = ts_context->ref_count;

    /* Unlock the mutex */
    pthread_mutex_unlock(&ts_context->mutex);

    return new_count;
}

/**
 * @brief Decrement the reference count for a thread-safe context
 *
 * @param ts_context The thread-safe context
 * @return The new reference count or (size_t)-1 if the context was freed
 */
size_t
rift_thread_safe_context_unref(rift_regex_thread_safe_context_t *ts_context)
{
    if (!ts_context) {
        return (size_t)-1;
    }

    /* Lock the mutex to ensure thread safety */
    if (pthread_mutex_lock(&ts_context->mutex) != 0) {
        return (size_t)-1;
    }

    /* Decrement the reference count */
    if (ts_context->ref_count > 0) {
        ts_context->ref_count--;
    }

    size_t new_count = ts_context->ref_count;

    /* Unlock the mutex */
    pthread_mutex_unlock(&ts_context->mutex);

    /* Free the context if the reference count reached 0 */
    if (new_count == 0) {
        rift_thread_safe_context_free(ts_context);
        return (size_t)-1;
    }

    return new_count;
}

/**
 * @brief Reset all thread-local contexts
 *
 * @param ts_context The thread-safe context
 * @return true if successful, false otherwise
 */
bool
rift_thread_safe_context_reset_all(rift_regex_thread_safe_context_t *ts_context)
{
    if (!ts_context) {
        return false;
    }

    /* Lock the mutex to ensure thread safety */
    if (pthread_mutex_lock(&ts_context->mutex) != 0) {
        return false;
    }

    /* Reset the global context */
    rift_matcher_context_reset(ts_context->global_ctx, NULL);

    /* We can't directly reset all thread-local contexts, as we don't have
       access to them. Instead, we'll force threads to recreate their contexts
       by removing the thread-local storage key and creating a new one. */
    pthread_key_delete(ts_context->thread_local_key);
    if (pthread_key_create(&ts_context->thread_local_key, thread_local_context_cleanup) != 0) {
        /* If we failed to recreate the key, mark the context as uninitialized */
        ts_context->initialized = false;
        pthread_mutex_unlock(&ts_context->mutex);
        return false;
    }

    /* Unlock the mutex */
    pthread_mutex_unlock(&ts_context->mutex);

    return true;
}

/**
 * @brief Lock the thread-safe context for exclusive access
 *
 * @param ts_context The thread-safe context
 * @return true if successful, false otherwise
 */
bool
rift_thread_safe_context_lock(rift_regex_thread_safe_context_t *ts_context)
{
    if (!ts_context) {
        return false;
    }

    return pthread_mutex_lock(&ts_context->mutex) == 0;
}

/**
 * @brief Unlock the thread-safe context
 *
 * @param ts_context The thread-safe context
 * @return true if successful, false otherwise
 */
bool
rift_thread_safe_context_unlock(rift_regex_thread_safe_context_t *ts_context)
{
    if (!ts_context) {
        return false;
    }

    return pthread_mutex_unlock(&ts_context->mutex) == 0;
}
