/**
 * @file config_utils.h
 * @brief Utility functions for accessibility configuration
 *
 * This file defines utility functions for working with accessibility configurations,
 * including serialization, validation, and predefined configuration sets.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include "librift/accessibility/config.h"
#ifndef LIBRIFT_ACCESSIBILITY_CONFIG_UTILS_H
#define LIBRIFT_ACCESSIBILITY_CONFIG_UTILS_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Serialization format for configuration data
 */
typedef enum {
    RIFT_CONFIG_FORMAT_JSON,  /**< JSON format */
    RIFT_CONFIG_FORMAT_INI,   /**< INI format */
    RIFT_CONFIG_FORMAT_BINARY /**< Binary format */
} rift_config_format_t;

/**
 * @brief Get a predefined configuration for high contrast mode
 *
 * @return rift_accessibility_config_t* A new configuration (caller must free)
 */
rift_accessibility_config_t *rift_accessibility_config_high_contrast(void);

/**
 * @brief Get a predefined configuration for screen reader mode
 *
 * @return rift_accessibility_config_t* A new configuration (caller must free)
 */
rift_accessibility_config_t *rift_accessibility_config_screen_reader(void);

/**
 * @brief Get a predefined configuration for keyboard navigation mode
 *
 * @return rift_accessibility_config_t* A new configuration (caller must free)
 */
rift_accessibility_config_t *rift_accessibility_config_keyboard_navigation(void);

/**
 * @brief Get a predefined configuration for large text mode
 *
 * @return rift_accessibility_config_t* A new configuration (caller must free)
 */
rift_accessibility_config_t *rift_accessibility_config_large_text(void);

/**
 * @brief Get a predefined configuration for color blind mode (deuteranopia)
 *
 * @return rift_accessibility_config_t* A new configuration (caller must free)
 */
rift_accessibility_config_t *rift_accessibility_config_deuteranopia(void);

/**
 * @brief Get a predefined configuration for color blind mode (protanopia)
 *
 * @return rift_accessibility_config_t* A new configuration (caller must free)
 */
rift_accessibility_config_t *rift_accessibility_config_protanopia(void);

/**
 * @brief Get a predefined configuration for color blind mode (tritanopia)
 *
 * @return rift_accessibility_config_t* A new configuration (caller must free)
 */
rift_accessibility_config_t *rift_accessibility_config_tritanopia(void);

/**
 * @brief Merge two configurations (second configuration overrides first)
 *
 * @param base Base configuration
 * @param override Configuration with override values
 * @return rift_accessibility_config_t* A new merged configuration (caller must free)
 */
rift_accessibility_config_t *
rift_accessibility_config_merge(const rift_accessibility_config_t *base,
                                const rift_accessibility_config_t *override);

/**
 * @brief Validate a configuration
 *
 * @param config The configuration to validate
 * @param error_message Buffer to store error message
 * @param buffer_size Size of error message buffer
 * @return bool True if configuration is valid, false otherwise
 */
bool rift_accessibility_config_validate(const rift_accessibility_config_t *config,
                                        char *error_message, size_t buffer_size);

/**
 * @brief Serialize a configuration to a string
 *
 * @param config The configuration to serialize
 * @param format The serialization format
 * @return char* The serialized configuration (caller must free), or NULL on error
 */
char *rift_accessibility_config_serialize(const rift_accessibility_config_t *config,
                                          rift_config_format_t format);

/**
 * @brief Deserialize a configuration from a string
 *
 * @param str The string to deserialize
 * @param format The serialization format
 * @return rift_accessibility_config_t* The deserialized configuration (caller must free), or NULL
 * on error
 */
rift_accessibility_config_t *rift_accessibility_config_deserialize(const char *str,
                                                                   rift_config_format_t format);

/**
 * @brief Save a configuration to a file
 *
 * @param config The configuration to save
 * @param file_path The file path
 * @param format The serialization format
 * @return bool True if successful, false otherwise
 */
bool rift_accessibility_config_save_to_file(const rift_accessibility_config_t *config,
                                            const char *file_path, rift_config_format_t format);

/**
 * @brief Load a configuration from a file
 *
 * @param file_path The file path
 * @param format The serialization format
 * @return rift_accessibility_config_t* The loaded configuration (caller must free), or NULL on
 * error
 */
rift_accessibility_config_t *rift_accessibility_config_load_from_file(const char *file_path,
                                                                      rift_config_format_t format);

/**
 * @brief Get a string representation of a configuration (for logging/debugging)
 *
 * @param config The configuration
 * @return char* The string representation (caller must free)
 */
char *rift_accessibility_config_to_string(const rift_accessibility_config_t *config);

/**
 * @brief Apply a configuration profile from a name
 *
 * @param profile_name The profile name (e.g., "high_contrast", "screen_reader")
 * @return rift_accessibility_config_t* The configuration for the profile (caller must free), or
 * NULL if not found
 */
rift_accessibility_config_t *rift_accessibility_config_from_profile(const char *profile_name);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_ACCESSIBILITY_CONFIG_UTILS_H */