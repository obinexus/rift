/**
 * @file thread_safe_demo.c
 * @brief Demonstration of thread-safe regex operations with LibRift
 *
 * This file provides a demonstration of using the thread-safe context
 * and backtracker implementations in a multi-threaded environment.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "librift/core/error.h"
#include "librift/regex/automaton/automaton.h"
#include "librift/regex/engine/matcher.h"
#include "librift/regex/engine/pattern.h"
#include "librift/regex/errors/regex_error.h"
#include "librift/regex/runtime/safe_backtracker.h"
#include "librift/regex/runtime/thread_safe_context.h"

/* Number of threads to create */
#define NUM_THREADS 8

/* Maximum backtracking depth */
#define MAX_BACKTRACK_DEPTH 1000

/* Thread argument structure */
typedef struct {
    rift_regex_thread_safe_context_t *context;
    const char *input;
    int thread_id;
    bool success;
} thread_arg_t;

/**
 * @brief Matcher callback function
 *
 * This function is executed with a thread-local matcher context. It
 * handles the actual regex matching operation within the thread.
 *
 * @param context The thread-local matcher context
 * @param user_data User-defined data (thread_arg_t*)
 * @param error Pointer to store error information
 * @return true if matching was successful, false otherwise
 */
static bool
matcher_callback(rift_regex_matcher_context_t *context, void *user_data, rift_regex_error_t *error)
{
    thread_arg_t *arg = (thread_arg_t *)user_data;

    printf("Thread %d: Processing input '%s'\n", arg->thread_id, arg->input);

    /* Set the current position to the start of the input */
    if (!rift_matcher_context_set_position(context, 0)) {
        if (error) {
            rift_regex_error_set_with_message(error, RIFT_REGEX_ERROR_INTERNAL,
                                              "Failed to set position in matcher context");
        }
        return false;
    }

    /* Simulate matching operation by advancing through the input */
    while (!rift_matcher_context_is_at_end(context)) {
        /* Process the current character */
        char c = rift_matcher_context_get_current_char(context);
        printf("Thread %d: Processing character '%c'\n", arg->thread_id, c);

        /* Advance to the next character */
        if (!rift_matcher_context_advance(context)) {
            break;
        }

        /* Simulate some processing time */
        usleep(10000 * (arg->thread_id % 3 + 1));
    }

    printf("Thread %d: Finished processing input\n", arg->thread_id);
    return true;
}

/**
 * @brief Thread function for regex matching
 *
 * @param arg Thread argument (thread_arg_t*)
 * @return NULL
 */
static void *
thread_function(void *arg)
{
    thread_arg_t *thread_arg = (thread_arg_t *)arg;
    rift_regex_error_t error = {0};

    /* Execute the matcher callback with thread-local context */
    thread_arg->success =
        rift_thread_safe_context_execute(thread_arg->context, matcher_callback, thread_arg, &error);

    if (!thread_arg->success) {
        printf("Thread %d: Error: %s\n", thread_arg->thread_id, error.message);
    }

    return NULL;
}

/**
 * @brief Main function
 *
 * @return Exit code
 */
int
main(void)
{
    /* Sample inputs for each thread */
    const char *inputs[NUM_THREADS] = {"Hello, world!",        "LibRift is awesome!",
                                       "Thread safety rocks!", "Regular expressions",
                                       "Backtracking limits",  "No more race conditions",
                                       "Thread-local storage", "Synchronization primitives"};

    /* Create a simple automaton for demonstration */
    rift_regex_automaton_t *automaton = rift_automaton_create(RIFT_AUTOMATON_NFA);
    if (!automaton) {
        fprintf(stderr, "Error: Failed to create automaton\n");
        return 1;
    }

    /* Create a thread-safe context */
    rift_regex_thread_safe_context_t *context =
        rift_thread_safe_context_create(automaton, 10, MAX_BACKTRACK_DEPTH);
    if (!context) {
        fprintf(stderr, "Error: Failed to create thread-safe context\n");
        rift_automaton_free(automaton);
        return 1;
    }

    /* Create thread arguments */
    thread_arg_t thread_args[NUM_THREADS];
    pthread_t threads[NUM_THREADS];

    printf("Starting %d threads with different inputs\n", NUM_THREADS);

    /* Create and start threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].context = context;
        thread_args[i].input = inputs[i];
        thread_args[i].thread_id = i;
        thread_args[i].success = false;

        /* Set input for this thread */
        if (!rift_thread_safe_context_set_input(context, inputs[i], (size_t)-1)) {
            fprintf(stderr, "Error: Failed to set input for thread %d\n", i);
            continue;
        }

        /* Create thread */
        if (pthread_create(&threads[i], NULL, thread_function, &thread_args[i]) != 0) {
            fprintf(stderr, "Error: Failed to create thread %d\n", i);
            continue;
        }
    }

    /* Wait for all threads to complete */
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Print results */
    printf("\nResults:\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        printf("Thread %d: %s\n", i, thread_args[i].success ? "Success" : "Failed");
    }

    /* Clean up */
    rift_thread_safe_context_free(context);
    rift_automaton_free(automaton);

    return 0;
}