/**
 * @file ui_adaptation_manager.h
 * @brief UI adaptation for accessibility in LibRift regex engine
 *
 * This file defines the UI adaptation manager component used to adjust
 * the user interface based on accessibility needs.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include "librift/accessibility/config.h"
#ifndef LIBRIFT_UI_ADAPTATION_MANAGER_H
#define LIBRIFT_UI_ADAPTATION_MANAGER_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque structure for the UI adaptation manager
 */
typedef struct rift_ui_adaptation_manager rift_ui_adaptation_manager_t;

/**
 * @brief UI adaptation callback function type
 *
 * @param element_id The ID of the element being adapted
 * @param user_data User data provided when registering the callback
 * @return bool True if the adaptation was successful, false otherwise
 */
typedef bool (*rift_ui_adaptation_callback_t)(const char *element_id, void *user_data);

/**
 * @brief Creates a new UI adaptation manager
 *
 * @return rift_ui_adaptation_manager_t* A new UI adaptation manager or NULL on failure
 */
rift_ui_adaptation_manager_t *rift_ui_adaptation_manager_create(void);

/**
 * @brief Destroys a UI adaptation manager and frees all associated resources
 *
 * @param manager The manager to destroy
 */
void rift_ui_adaptation_manager_destroy(rift_ui_adaptation_manager_t *manager);

/**
 * @brief Applies a UI template
 *
 * @param manager The manager to modify
 * @param template The template to apply
 * @return bool True if successful, false otherwise
 */
bool rift_ui_adaptation_manager_apply_template(rift_ui_adaptation_manager_t *manager,
                                               rift_ui_template_t template);

/**
 * @brief Gets the current UI template
 *
 * @param manager The manager to query
 * @return rift_ui_template_t The current template
 */
rift_ui_template_t
rift_ui_adaptation_manager_get_template(const rift_ui_adaptation_manager_t *manager);

/**
 * @brief Sets a property for a UI element
 *
 * @param manager The manager to modify
 * @param element_id The element ID
 * @param property The property name
 * @param value The property value
 * @return bool True if successful, false otherwise
 */
bool rift_ui_adaptation_manager_set_property(rift_ui_adaptation_manager_t *manager,
                                             const char *element_id, const char *property,
                                             const char *value);

/**
 * @brief Gets a property for a UI element
 *
 * @param manager The manager to query
 * @param element_id The element ID
 * @param property The property name
 * @return const char* The property value, or NULL if not found
 */
const char *rift_ui_adaptation_manager_get_property(const rift_ui_adaptation_manager_t *manager,
                                                    const char *element_id, const char *property);

/**
 * @brief Sets the text scale factor
 *
 * @param manager The manager to modify
 * @param factor The scale factor (1.0 = normal size)
 * @return bool True if successful, false otherwise
 */
bool rift_ui_adaptation_manager_set_text_scale_factor(rift_ui_adaptation_manager_t *manager,
                                                      float factor);

/**
 * @brief Gets the text scale factor
 *
 * @param manager The manager to query
 * @return float The scale factor (1.0 = normal size)
 */
float rift_ui_adaptation_manager_get_text_scale_factor(const rift_ui_adaptation_manager_t *manager);

/**
 * @brief Adapts a UI element using a callback
 *
 * @param manager The manager to use
 * @param element_id The element ID
 * @param callback The adaptation callback
 * @param user_data User data to pass to the callback
 * @return bool True if successful, false otherwise
 */
bool rift_ui_adaptation_manager_adapt_element(rift_ui_adaptation_manager_t *manager,
                                              const char *element_id,
                                              rift_ui_adaptation_callback_t callback,
                                              void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_UI_ADAPTATION_MANAGER_H */