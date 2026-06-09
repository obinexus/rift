/**
 * @file accessibility_manager.h
 * @brief Central accessibility manager for the LibRift regex engine
 *
 * This file defines the main accessibility manager which coordinates
 * all accessibility features in the LibRift engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "librift/accessibility/accessibility_manager.h"
#include "librift/accessibility/color_scheme_manager.h"
#include "librift/accessibility/keyboard_navigation_manager.h"
#include "librift/accessibility/preferences_manager.h"
#include "librift/accessibility/screen_reader_adapter.h"
#include "librift/accessibility/ui_adaptation_manager.h"
#include "librift/core/error.h"
#include "librift/core/memory.h"
#ifndef LIBRIFT_ACCESSIBILITY_MANAGER_H
#define LIBRIFT_ACCESSIBILITY_MANAGER_H

/**
 * @file accessibility_manager.c
 * @brief Implementation of the central accessibility manager for LibRift
 *
 * This file implements the functions declared in accessibility_manager.h,
 * providing a central manager for all accessibility features in LibRift.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */



#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Structure for the accessibility manager
 */
struct rift_accessibility_manager {
    rift_color_scheme_manager_t *color_scheme_manager;
    rift_keyboard_navigation_manager_t *keyboard_manager;
    rift_screen_reader_adapter_t *screen_reader;
    rift_ui_adaptation_manager_t *ui_manager;
    rift_preferences_manager_t *preferences_manager;
    rift_accessibility_feature_flags_t enabled_features;
    bool initialized;
};
/**
 * @file accessibility_manager.h
 * @brief Central accessibility manager for LibRift
 *
 * This file defines the central manager for all accessibility features in LibRift.
 * It provides functionality to enable/disable features and access various
 * accessibility-related components.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#ifndef LIBRIFT_CORE_ACCESSIBILITY_MANAGER_H
#define LIBRIFT_CORE_ACCESSIBILITY_MANAGER_H


#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations of component managers */
struct rift_color_scheme_manager;
struct rift_keyboard_navigation_manager;
struct rift_screen_reader_adapter;
struct rift_ui_adaptation_manager;
struct rift_preferences_manager;
struct rift_accessibility_manager {
    struct rift_color_scheme_manager *color_scheme_manager;
    struct rift_keyboard_navigation_manager *keyboard_manager;
    struct rift_screen_reader_adapter *screen_reader;
    struct rift_ui_adaptation_manager *ui_manager;
    struct rift_preferences_manager *preferences_manager;
    rift_accessibility_feature_flags_t enabled_features;
    bool initialized;
};
/* Typedef for convenience */
typedef struct rift_accessibility_manager rift_accessibility_manager_t;
typedef struct rift_color_scheme_manager rift_color_scheme_manager_t;
typedef struct rift_keyboard_navigation_manager rift_keyboard_navigation_manager_t;
typedef struct rift_screen_reader_adapter rift_screen_reader_adapter_t;
typedef struct rift_ui_adaptation_manager rift_ui_adaptation_manager_t;
typedef struct rift_preferences_manager rift_preferences_manager_t;

/**
 * @brief Accessibility feature flags
 */
typedef enum {
    RIFT_ACCESSIBILITY_FEATURE_NONE = 0,
    RIFT_ACCESSIBILITY_FEATURE_COLOR_SCHEME = (1 << 0),
    RIFT_ACCESSIBILITY_FEATURE_KEYBOARD_NAVIGATION = (1 << 1),
    RIFT_ACCESSIBILITY_FEATURE_SCREEN_READER = (1 << 2),
    RIFT_ACCESSIBILITY_FEATURE_UI_ADAPTATION = (1 << 3),
    /* Add more features as needed */
    RIFT_ACCESSIBILITY_FEATURE_ALL = 0xFFFFFFFF
} rift_accessibility_feature_flags_t;

/**
 * @brief Screen reader type enumeration
 */
typedef enum {
    RIFT_SCREEN_READER_NONE,
    RIFT_SCREEN_READER_SYSTEM,
    RIFT_SCREEN_READER_CUSTOM
    /* Add more screen reader types as needed */
} rift_screen_reader_type_t;

/**
 * @brief Keyboard shortcut structure
 */
typedef struct {
    int key_code;
    int modifiers;
    void (*callback)(void *);
    void *user_data;
} rift_keyboard_shortcut_t;

/**
 * @brief Accessibility configuration structure
 */
typedef struct {
    rift_accessibility_feature_flags_t enabledFeatures;
    rift_screen_reader_type_t screenReaderType;
    const char *colorSchemeId;
    const char *uiTemplateId;
    rift_keyboard_shortcut_t *keyboardShortcuts;
    size_t keyboardShortcutCount;
    /* Add more configuration options as needed */
} rift_accessibility_config_t;

/**
 * @brief Structure for the accessibility manager
 */
struct rift_accessibility_manager {
    rift_color_scheme_manager_t *color_scheme_manager;    /**< Color scheme manager */
    rift_keyboard_navigation_manager_t *keyboard_manager; /**< Keyboard navigation manager */
    rift_screen_reader_adapter_t *screen_reader;          /**< Screen reader adapter */
    rift_ui_adaptation_manager_t *ui_manager;             /**< UI adaptation manager */
    rift_preferences_manager_t *preferences_manager;      /**< Preferences manager */
    rift_accessibility_feature_flags_t enabled_features;  /**< Enabled features */
    bool initialized;                                     /**< Initialization flag */
};

/**
 * @brief Creates a new accessibility manager
 *
 * @return rift_accessibility_manager_t* A new accessibility manager or NULL on failure
 */
rift_accessibility_manager_t *rift_accessibility_manager_create(void);

/**
 * @brief Destroys an accessibility manager and frees all associated resources
 *
 * @param manager The manager to destroy
 */
void rift_accessibility_manager_destroy(rift_accessibility_manager_t *manager);

/**
 * @brief Initializes the accessibility manager with a configuration
 *
 * @param manager The manager to initialize
 * @param config The configuration to use, or NULL for default configuration
 * @return bool True if successful, false otherwise
 */
bool rift_accessibility_manager_initialize(rift_accessibility_manager_t *manager,
                                           const rift_accessibility_config_t *config);

/**
 * @brief Enables specified accessibility features
 *
 * @param manager The accessibility manager
 * @param features Bitmask of features to enable
 * @return bool True if successful, false otherwise
 */
bool rift_accessibility_manager_enable_features(rift_accessibility_manager_t *manager,
                                                rift_accessibility_feature_flags_t features);

/**
 * @brief Disables specified accessibility features
 *
 * @param manager The accessibility manager
 * @param features Bitmask of features to disable
 * @return bool True if successful, false otherwise
 */
bool rift_accessibility_manager_disable_features(rift_accessibility_manager_t *manager,
                                                 rift_accessibility_feature_flags_t features);

/**
 * @brief Checks if specified accessibility features are enabled
 *
 * @param manager The accessibility manager
 * @param features Bitmask of features to check
 * @return bool True if all specified features are enabled, false otherwise
 */
bool rift_accessibility_manager_features_enabled(rift_accessibility_manager_t *manager,
                                                 rift_accessibility_feature_flags_t features);

/**
 * @brief Gets the color scheme manager
 *
 * @param manager The accessibility manager
 * @return struct rift_color_scheme_manager* The color scheme manager
 */
struct rift_color_scheme_manager *
rift_accessibility_manager_get_color_scheme_manager(rift_accessibility_manager_t *manager);

/**
 * @brief Gets the keyboard navigation manager
 *
 * @param manager The accessibility manager
 * @return struct rift_keyboard_navigation_manager* The keyboard navigation manager
 */
struct rift_keyboard_navigation_manager *
rift_accessibility_manager_get_keyboard_manager(rift_accessibility_manager_t *manager);

/**
 * @brief Gets the screen reader adapter
 *
 * @param manager The accessibility manager
 * @return struct rift_screen_reader_adapter* The screen reader adapter
 */
struct rift_screen_reader_adapter *
rift_accessibility_manager_get_screen_reader(rift_accessibility_manager_t *manager);

/**
 * @brief Gets the UI adaptation manager
 *
 * @param manager The accessibility manager
 * @return struct rift_ui_adaptation_manager* The UI adaptation manager
 */
struct rift_ui_adaptation_manager *
rift_accessibility_manager_get_ui_manager(rift_accessibility_manager_t *manager);

/**
 * @brief Gets the preferences manager
 *
 * @param manager The accessibility manager
 * @return struct rift_preferences_manager* The preferences manager
 */
struct rift_preferences_manager *
rift_accessibility_manager_get_preferences_manager(rift_accessibility_manager_t *manager);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_CORE_ACCESSIBILITY_MANAGER_H */

/**
 * @brief Opaque structure for the accessibility manager
 */
typedef struct rift_accessibility_manager rift_accessibility_manager_t;

/**
 * @brief Creates a new accessibility manager
 *
 * @return rift_accessibility_manager_t* A new accessibility manager or NULL on failure
 */
rift_accessibility_manager_t *rift_accessibility_manager_create(void);

/**
 * @brief Destroys an accessibility manager and frees all associated resources
 *
 * @param manager The manager to destroy
 */
void rift_accessibility_manager_destroy(rift_accessibility_manager_t *manager);

/**
 * @brief Initializes the accessibility manager with a configuration
 *
 * @param manager The manager to initialize
 * @param config The configuration to use, or NULL for default configuration
 * @return bool True if successful, false otherwise
 */
bool rift_accessibility_manager_initialize(rift_accessibility_manager_t *manager,
                                           const rift_accessibility_config_t *config);

/**
 * @brief Enables specified accessibility features
 *
 * @param manager The accessibility manager
 * @param features Bitmask of features to enable
 * @return bool True if successful, false otherwise
 */
bool rift_accessibility_manager_enable_features(rift_accessibility_manager_t *manager,
                                                rift_accessibility_feature_flags_t features);

/**
 * @brief Disables specified accessibility features
 *
 * @param manager The accessibility manager
 * @param features Bitmask of features to disable
 * @return bool True if successful, false otherwise
 */
bool rift_accessibility_manager_disable_features(rift_accessibility_manager_t *manager,
                                                 rift_accessibility_feature_flags_t features);

/**
 * @brief Checks if specific accessibility features are enabled
 *
 * @param manager The accessibility manager
 * @param features The features to check
 * @return bool True if all specified features are enabled, false otherwise
 */
bool rift_accessibility_manager_features_enabled(rift_accessibility_manager_t *manager,
                                                 rift_accessibility_feature_flags_t features);

/**
 * @brief Gets the color scheme manager from the accessibility manager
 *
 * @param manager The accessibility manager
 * @return struct rift_color_scheme_manager* The color scheme manager, or NULL if not available
 */
struct rift_color_scheme_manager *
rift_accessibility_manager_get_color_scheme_manager(rift_accessibility_manager_t *manager);

/**
 * @brief Gets the keyboard navigation manager from the accessibility manager
 *
 * @param manager The accessibility manager
 * @return struct rift_keyboard_navigation_manager* The keyboard navigation manager, or NULL if not
 * available
 */
struct rift_keyboard_navigation_manager *
rift_accessibility_manager_get_keyboard_manager(rift_accessibility_manager_t *manager);

/**
 * @brief Gets the screen reader adapter from the accessibility manager
 *
 * @param manager The accessibility manager
 * @return struct rift_screen_reader_adapter* The screen reader adapter, or NULL if not available
 */
struct rift_screen_reader_adapter *
rift_accessibility_manager_get_screen_reader(rift_accessibility_manager_t *manager);

/**
 * @brief Gets the UI adaptation manager from the accessibility manager
 *
 * @param manager The accessibility manager
 * @return struct rift_ui_adaptation_manager* The UI adaptation manager, or NULL if not available
 */
struct rift_ui_adaptation_manager *
rift_accessibility_manager_get_ui_manager(rift_accessibility_manager_t *manager);

/**
 * @brief Gets the preferences manager from the accessibility manager
 *
 * @param manager The accessibility manager
 * @return struct rift_preferences_manager* The preferences manager, or NULL if not available
 */
struct rift_preferences_manager *
rift_accessibility_manager_get_preferences_manager(rift_accessibility_manager_t *manager);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_ACCESSIBILITY_MANAGER_H */