/**
 * @file automaton_observer.h
 * @brief Observer interface for automaton changes in LibRift
 *
 * This header defines the observer interface for components that need
 * to receive notifications about automaton state changes.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#ifndef LIBRIFT_AUTOMATON_OBSERVER_H
#define LIBRIFT_AUTOMATON_OBSERVER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Forward declaration of the automaton observer interface
 */
typedef struct rift_automaton_observer rift_automaton_observer_t;

/**
 * @brief Callback function type for automaton updates
 */
typedef void (*rift_automaton_update_callback_t)(rift_automaton_observer_t *observer,
                                                 void *automaton);

/**
 * @brief Automaton observer interface structure
 */
struct rift_automaton_observer {
    rift_automaton_update_callback_t update; /**< Update callback function */
    void *user_data;                         /**< User data for the observer */
};

/**
 * @brief Creates a new automaton observer
 *
 * @param update_callback The callback function to receive updates
 * @param user_data User data to pass to the callback
 * @return rift_automaton_observer_t* A new observer instance or NULL on failure
 */
rift_automaton_observer_t *
rift_automaton_observer_create(rift_automaton_update_callback_t update_callback, void *user_data);

/**
 * @brief Destroys an automaton observer
 *
 * @param observer The observer to destroy
 */
void rift_automaton_observer_destroy(rift_automaton_observer_t *observer);

/**
 * @brief Notifies an observer of an automaton update
 *
 * @param observer The observer to notify
 * @param automaton The updated automaton
 */
void rift_automaton_observer_notify(rift_automaton_observer_t *observer, void *automaton);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_AUTOMATON_OBSERVER_H */