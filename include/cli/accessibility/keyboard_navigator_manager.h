/**
 * @file keyboard_navigation_manager.h
 * @brief Keyboard navigation management for accessibility in LibRift regex engine
 *
 * This file defines the keyboard navigation manager component used to handle
 * keyboard shortcuts and navigation for accessibility purposes.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include "librift/accessibility/config.h"
#include "librift/accessibility/features.h"
#ifndef LIBRIFT_KEYBOARD_NAVIGATION_MANAGER_H
#define LIBRIFT_KEYBOARD_NAVIGATION_MANAGER_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque structure for the keyboard navigation manager
 */
typedef struct rift_keyboard_navigation_manager rift_keyboard_navigation_manager_t;

/**
 * @brief Callback function type for keyboard shortcuts
 *
 * @param action The action that was triggered
 * @param user_data User data provided when registering the shortcut
 * @return bool True if the shortcut was handled, false otherwise
 */
typedef bool (*rift_shortcut_callback_t)(rift_key_action_t action, void *user_data);

/**
 * @brief Creates a new keyboard navigation manager
 *
 * @return rift_keyboard_navigation_manager_t* A new keyboard navigation manager or NULL on failure
 */
rift_keyboard_navigation_manager_t *rift_keyboard_navigation_manager_create(void);

/**
 * @brief Destroys a keyboard navigation manager and frees all associated resources
 *
 * @param manager The manager to destroy
 */
void rift_keyboard_navigation_manager_destroy(rift_keyboard_navigation_manager_t *manager);

/**
 * @brief Registers a keyboard shortcut
 *
 * @param manager The manager to modify
 * @param action The action to register
 * @param key_binding The key binding string (e.g., "Ctrl+S")
 * @param callback The callback function to execute when shortcut is triggered
 * @param user_data User data to pass to the callback
 * @return bool True if successful, false otherwise
 */
bool rift_keyboard_navigation_manager_register_shortcut(rift_keyboard_navigation_manager_t *manager,
                                                        rift_key_action_t action,
                                                        const char *key_binding,
                                                        rift_shortcut_callback_t callback,
                                                        void *user_data);

/**
 * @brief Processes a key event
 *
 * @param manager The manager to use
 * @param event The key event to process
 * @return bool True if the event was handled, false otherwise
 */
bool rift_keyboard_navigation_manager_process_key_event(rift_keyboard_navigation_manager_t *manager,
                                                        const rift_key_event_t *event);

/**
 * @brief Gets all registered shortcuts
 *
 * @param manager The manager to query
 * @param count Pointer to store the number of shortcuts
 * @return rift_keyboard_shortcut_t* Array of shortcuts (caller must free), or NULL on error
 */
rift_keyboard_shortcut_t *
rift_keyboard_navigation_manager_get_shortcuts(const rift_keyboard_navigation_manager_t *manager,
                                               size_t *count);

/**
 * @brief Clears all registered shortcuts
 *
 * @param manager The manager to modify
 * @return bool True if successful, false otherwise
 */
bool rift_keyboard_navigation_manager_clear_shortcuts(rift_keyboard_navigation_manager_t *manager);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_KEYBOARD_NAVIGATION_MANAGER_H */