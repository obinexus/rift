/**
 * @file features_utils.h
 * @brief Utility functions for accessibility features
 *
 * This file defines utility functions for working with accessibility features,
 * key events, and speech priorities.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include "librift/accessibility/features.h"
#ifndef LIBRIFT_ACCESSIBILITY_FEATURES_UTILS_H
#define LIBRIFT_ACCESSIBILITY_FEATURES_UTILS_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get a string representation of a feature flag
 *
 * @param feature The feature flag
 * @return const char* String representation of the feature
 */
const char *rift_accessibility_feature_to_string(rift_accessibility_feature_flags_t feature);

/**
 * @brief Create a new key event
 *
 * @param type Event type (press or release)
 * @param keyCode Key code
 * @param ctrl Whether Ctrl is pressed
 * @param alt Whether Alt is pressed
 * @param shift Whether Shift is pressed
 * @param meta Whether Meta is pressed
 * @return rift_key_event_t* The created key event (caller must free)
 */
rift_key_event_t *rift_key_event_create(rift_key_event_type_t type, int keyCode, bool ctrl,
                                        bool alt, bool shift, bool meta);

/**
 * @brief Destroy a key event
 *
 * @param event The event to destroy
 */
void rift_key_event_destroy(rift_key_event_t *event);

/**
 * @brief Get a string representation of a key action
 *
 * @param action The key action
 * @return const char* String representation of the action
 */
const char *rift_key_action_to_string(rift_key_action_t action);

/**
 * @brief Get a string representation of a speech priority
 *
 * @param priority The speech priority
 * @return const char* String representation of the priority
 */
const char *rift_speech_priority_to_string(rift_speech_priority_t priority);

/**
 * @brief Get a string representation of a screen reader type
 *
 * @param type The screen reader type
 * @return const char* String representation of the type
 */
const char *rift_screen_reader_type_to_string(rift_screen_reader_type_t type);

/**
 * @brief Get the key code for a key name
 *
 * @param key_name The name of the key (e.g., "A", "Enter", "F1")
 * @return int The key code, or -1 if unknown
 */
int rift_key_code_from_name(const char *key_name);

/**
 * @brief Get the key name for a key code
 *
 * @param key_code The key code
 * @return const char* The key name, or "Unknown Key" if not recognized
 */
const char *rift_key_name_from_code(int key_code);

/**
 * @brief Parse a key binding string (e.g., "Ctrl+Shift+A")
 *
 * @param binding The key binding string
 * @param event Output for the parsed key event
 * @return bool True if successful, false otherwise
 */
bool rift_parse_key_binding(const char *binding, rift_key_event_t *event);

/**
 * @brief Convert a key event to a key binding string
 *
 * @param event The key event
 * @return char* The key binding string (caller must free)
 */
char *rift_key_binding_from_event(const rift_key_event_t *event);

/**
 * @brief Check if two key events are equivalent
 *
 * @param event1 First key event
 * @param event2 Second key event
 * @return bool True if the events represent the same key combination
 */
bool rift_key_events_equal(const rift_key_event_t *event1, const rift_key_event_t *event2);

/**
 * @brief Combine multiple feature flags
 *
 * @param count Number of feature flags to combine
 * @param ... Feature flags to combine
 * @return rift_accessibility_feature_flags_t Combined feature flags
 */
rift_accessibility_feature_flags_t rift_accessibility_features_combine(int count, ...);

/**
 * @brief Check if all specified features are enabled in a feature set
 *
 * @param features The feature set to check
 * @param required The required features
 * @return bool True if all required features are enabled
 */
bool rift_accessibility_features_contains_all(rift_accessibility_feature_flags_t features,
                                              rift_accessibility_feature_flags_t required);

/**
 * @brief Check if any specified features are enabled in a feature set
 *
 * @param features The feature set to check
 * @param query The features to check for
 * @return bool True if any query features are enabled
 */
bool rift_accessibility_features_contains_any(rift_accessibility_feature_flags_t features,
                                              rift_accessibility_feature_flags_t query);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_ACCESSIBILITY_FEATURES_UTILS_H */