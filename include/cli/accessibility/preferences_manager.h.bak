/**
 * @file preferences_manager.h
 * @brief User preferences management for accessibility in LibRift regex engine
 *
 * This file defines the preferences manager component used to store and retrieve
 * user preferences for accessibility settings.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#ifndef LIBRIFT_PREFERENCES_MANAGER_H
#define LIBRIFT_PREFERENCES_MANAGER_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque structure for the preferences manager
 */
typedef struct rift_preferences_manager rift_preferences_manager_t;

/**
 * @brief Preference value types
 */
typedef enum {
    RIFT_PREFERENCE_TYPE_BOOL,
    RIFT_PREFERENCE_TYPE_INT,
    RIFT_PREFERENCE_TYPE_FLOAT,
    RIFT_PREFERENCE_TYPE_STRING,
    RIFT_PREFERENCE_TYPE_POINTER
} rift_preference_type_t;

/**
 * @brief Creates a new preferences manager
 *
 * @return rift_preferences_manager_t* A new preferences manager or NULL on failure
 */
rift_preferences_manager_t *rift_preferences_manager_create(void);

/**
 * @brief Destroys a preferences manager and frees all associated resources
 *
 * @param manager The manager to destroy
 */
void rift_preferences_manager_destroy(rift_preferences_manager_t *manager);

/**
 * @brief Gets a preference value
 *
 * @param manager The manager to query
 * @param key The preference key
 * @param type Pointer to store the value type
 * @return void* The preference value, or NULL if not found
 */
void *rift_preferences_manager_get_preference(const rift_preferences_manager_t *manager,
                                              const char *key, rift_preference_type_t *type);

/**
 * @brief Sets a preference value
 *
 * @param manager The manager to modify
 * @param key The preference key
 * @param value The preference value
 * @param type The value type
 * @return bool True if successful, false otherwise
 */
bool rift_preferences_manager_set_preference(rift_preferences_manager_t *manager, const char *key,
                                             void *value, rift_preference_type_t type);

/**
 * @brief Loads preferences from a file
 *
 * @param manager The manager to modify
 * @param file_path Path to the preferences file
 * @return bool True if successful, false otherwise
 */
bool rift_preferences_manager_load_from_file(rift_preferences_manager_t *manager,
                                             const char *file_path);

/**
 * @brief Saves preferences to a file
 *
 * @param manager The manager to query
 * @param file_path Path to the preferences file
 * @return bool True if successful, false otherwise
 */
bool rift_preferences_manager_save_to_file(const rift_preferences_manager_t *manager,
                                           const char *file_path);

/**
 * @brief Gets a boolean preference
 *
 * @param manager The manager to query
 * @param key The preference key
 * @param default_value The default value to return if the preference is not found
 * @return bool The preference value
 */
bool rift_preferences_manager_get_bool(const rift_preferences_manager_t *manager, const char *key,
                                       bool default_value);

/**
 * @brief Gets an integer preference
 *
 * @param manager The manager to query
 * @param key The preference key
 * @param default_value The default value to return if the preference is not found
 * @return int The preference value
 */
int rift_preferences_manager_get_int(const rift_preferences_manager_t *manager, const char *key,
                                     int default_value);

/**
 * @brief Gets a float preference
 *
 * @param manager The manager to query
 * @param key The preference key
 * @param default_value The default value to return if the preference is not found
 * @return float The preference value
 */
float rift_preferences_manager_get_float(const rift_preferences_manager_t *manager, const char *key,
                                         float default_value);

/**
 * @brief Gets a string preference
 *
 * @param manager The manager to query
 * @param key The preference key
 * @param default_value The default value to return if the preference is not found
 * @return const char* The preference value
 */
const char *rift_preferences_manager_get_string(const rift_preferences_manager_t *manager,
                                                const char *key, const char *default_value);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_PREFERENCES_MANAGER_H */