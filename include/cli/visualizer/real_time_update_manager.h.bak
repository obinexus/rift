/**
 * @file real_time_update_manager.h
 * @brief Real-time update manager for the LibRift SVG renderer
 *
 * This header defines the real-time update management functionality
 * for efficient processing of SVG updates in response to automaton changes.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stdint.h>
#include "svg_automaton_mapper.h"
#ifndef LIBRIFT_REAL_TIME_UPDATE_MANAGER_H
#define LIBRIFT_REAL_TIME_UPDATE_MANAGER_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Forward declaration of the real-time update manager structure
 */
typedef struct rift_real_time_update_manager rift_real_time_update_manager_t;

/**
 * @brief Update type enumeration
 */
typedef enum {
    RIFT_UPDATE_TYPE_STATE_ADDED,         /**< A state was added */
    RIFT_UPDATE_TYPE_STATE_REMOVED,       /**< A state was removed */
    RIFT_UPDATE_TYPE_STATE_MODIFIED,      /**< A state was modified */
    RIFT_UPDATE_TYPE_TRANSITION_ADDED,    /**< A transition was added */
    RIFT_UPDATE_TYPE_TRANSITION_REMOVED,  /**< A transition was removed */
    RIFT_UPDATE_TYPE_TRANSITION_MODIFIED, /**< A transition was modified */
    RIFT_UPDATE_TYPE_LAYOUT_CHANGED       /**< The overall layout changed */
} rift_update_type_t;

/**
 * @brief Update entry structure
 */
typedef struct {
    rift_update_type_t type; /**< The update type */
    void *element;           /**< The automaton element being updated */
    uint64_t timestamp;      /**< The update timestamp */
    int priority;            /**< The update priority (higher values = higher priority) */
} rift_update_entry_t;

/**
 * @brief Creates a new real-time update manager
 *
 * @param mapper The automaton mapper to use
 * @return rift_real_time_update_manager_t* A new update manager or NULL on failure
 */
rift_real_time_update_manager_t *
rift_real_time_update_manager_create(rift_svg_automaton_mapper_t *mapper);

/**
 * @brief Destroys a real-time update manager and frees associated resources
 *
 * @param manager The manager to destroy
 */
void rift_real_time_update_manager_destroy(rift_real_time_update_manager_t *manager);

/**
 * @brief Schedules an update for processing
 *
 * @param manager The manager to use
 * @param update The update to schedule
 * @return bool True if scheduled successfully, false otherwise
 */
bool rift_real_time_update_manager_schedule_update(rift_real_time_update_manager_t *manager,
                                                   rift_update_entry_t update);

/**
 * @brief Processes all pending updates
 *
 * @param manager The manager to use
 * @return int The number of updates processed
 */
int rift_real_time_update_manager_process_updates(rift_real_time_update_manager_t *manager);

/**
 * @brief Sets the throttling interval for updates
 *
 * @param manager The manager to modify
 * @param interval_ms The throttling interval in milliseconds
 * @return bool True if successful, false otherwise
 */
bool rift_real_time_update_manager_throttle_updates(rift_real_time_update_manager_t *manager,
                                                    uint32_t interval_ms);

/**
 * @brief Gets the current timestamp in milliseconds
 *
 * @return uint64_t The current timestamp
 */
uint64_t rift_real_time_update_manager_get_timestamp(void);

/**
 * @brief Creates a new update entry
 *
 * @param type The update type
 * @param element The automaton element
 * @param priority The update priority
 * @return rift_update_entry_t The created update entry
 */
rift_update_entry_t rift_real_time_update_manager_create_update(rift_update_type_t type,
                                                                void *element, int priority);

/**
 * @brief Checks if the manager has pending updates
 *
 * @param manager The manager to query
 * @return bool True if there are pending updates, false otherwise
 */
bool rift_real_time_update_manager_has_pending_updates(rift_real_time_update_manager_t *manager);

/**
 * @brief Gets the number of pending updates
 *
 * @param manager The manager to query
 * @return size_t The number of pending updates
 */
size_t
rift_real_time_update_manager_get_pending_update_count(rift_real_time_update_manager_t *manager);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REAL_TIME_UPDATE_MANAGER_H */