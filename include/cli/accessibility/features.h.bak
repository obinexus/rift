/**
 * @file features.h
 * @brief Accessibility feature definitions for the LibRift regex engine
 *
 * This file defines the feature flags and configuration structures for
 * the accessibility subsystem.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdint.h>
#ifndef LIBRIFT_ACCESSIBILITY_FEATURES_H
#define LIBRIFT_ACCESSIBILITY_FEATURES_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Feature flags for accessibility features
 */
typedef enum {
    RIFT_ACCESSIBILITY_FEATURE_NONE = 0,
    RIFT_ACCESSIBILITY_FEATURE_COLOR_SCHEME = (1 << 0),
    RIFT_ACCESSIBILITY_FEATURE_KEYBOARD_NAVIGATION = (1 << 1),
    RIFT_ACCESSIBILITY_FEATURE_SCREEN_READER = (1 << 2),
    RIFT_ACCESSIBILITY_FEATURE_UI_ADAPTATION = (1 << 3),
    RIFT_ACCESSIBILITY_FEATURE_ALL = 0xFFFFFFFF
} rift_accessibility_feature_flags_t;

/**
 * @brief Shortcut actions for keyboard accessibility
 */
typedef enum {
    RIFT_KEY_ACTION_NAVIGATE_NEXT,
    RIFT_KEY_ACTION_NAVIGATE_PREV,
    RIFT_KEY_ACTION_ACTIVATE,
    RIFT_KEY_ACTION_ESCAPE,
    RIFT_KEY_ACTION_HELP,
    /* Add more key actions as needed */
} rift_key_action_t;

/**
 * @brief Key event types
 */
typedef enum { RIFT_KEY_EVENT_PRESS, RIFT_KEY_EVENT_RELEASE } rift_key_event_type_t;

/**
 * @brief Structure for a key event
 */
typedef struct {
    rift_key_event_type_t type; /**< Event type */
    int keyCode;                /**< Key code */
    bool ctrl;                  /**< Whether Ctrl is pressed */
    bool alt;                   /**< Whether Alt is pressed */
    bool shift;                 /**< Whether Shift is pressed */
    bool meta;                  /**< Whether Meta (Windows/Command) is pressed */
} rift_key_event_t;

/**
 * @brief Screen reader priority
 */
typedef enum {
    RIFT_SPEECH_PRIORITY_LOW,
    RIFT_SPEECH_PRIORITY_NORMAL,
    RIFT_SPEECH_PRIORITY_HIGH,
    RIFT_SPEECH_PRIORITY_INTERRUPT
} rift_speech_priority_t;

/**
 * @brief Screen reader types
 */
typedef enum {
    RIFT_SCREEN_READER_NONE,
    RIFT_SCREEN_READER_SYSTEM_DEFAULT,
    RIFT_SCREEN_READER_INTERNAL,
    RIFT_SCREEN_READER_NVDA,
    RIFT_SCREEN_READER_JAWS,
    RIFT_SCREEN_READER_VOICEOVER,
    RIFT_SCREEN_READER_ORCA
} rift_screen_reader_type_t;

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_ACCESSIBILITY_FEATURES_H */