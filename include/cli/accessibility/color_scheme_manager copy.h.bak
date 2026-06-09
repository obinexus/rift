/**
 * @file color_scheme_manager.h
 * @brief Color scheme management for accessibility in LibRift regex engine
 *
 * This file defines the color scheme manager component used to handle
 * color schemes for accessibility purposes.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "librift/accessibility/config.h"
#ifndef LIBRIFT_COLOR_SCHEME_MANAGER_H
#define LIBRIFT_COLOR_SCHEME_MANAGER_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque structure for the color scheme manager
 */
typedef struct rift_color_scheme_manager rift_color_scheme_manager_t;

/**
 * @brief Color roles for UI elements
 */
typedef enum {
    RIFT_COLOR_ROLE_BACKGROUND,
    RIFT_COLOR_ROLE_FOREGROUND,
    RIFT_COLOR_ROLE_PRIMARY,
    RIFT_COLOR_ROLE_SECONDARY,
    RIFT_COLOR_ROLE_ACCENT,
    RIFT_COLOR_ROLE_ERROR,
    RIFT_COLOR_ROLE_WARNING,
    RIFT_COLOR_ROLE_SUCCESS
} rift_color_role_t;

/**
 * @brief Structure for an RGBA color
 */
typedef struct {
    uint8_t r; /**< Red component (0-255) */
    uint8_t g; /**< Green component (0-255) */
    uint8_t b; /**< Blue component (0-255) */
    uint8_t a; /**< Alpha component (0-255, 255 is fully opaque) */
} rift_color_t;

/**
 * @brief Structure for a color scheme entry
 */
typedef struct {
    rift_color_scheme_t id; /**< Scheme ID */
    struct {
        rift_color_t background; /**< Background color */
        rift_color_t foreground; /**< Foreground color */
        rift_color_t primary;    /**< Primary color */
        rift_color_t secondary;  /**< Secondary color */
        rift_color_t accent;     /**< Accent color */
        rift_color_t error;      /**< Error color */
        rift_color_t warning;    /**< Warning color */
        rift_color_t success;    /**< Success color */
    } palette;                   /**< Color palette */
} rift_color_scheme_entry_t;

/**
 * @brief Creates a new color scheme manager
 *
 * @return rift_color_scheme_manager_t* A new color scheme manager or NULL on failure
 */
rift_color_scheme_manager_t *rift_color_scheme_manager_create(void);

/**
 * @brief Destroys a color scheme manager and frees all associated resources
 *
 * @param manager The manager to destroy
 */
void rift_color_scheme_manager_destroy(rift_color_scheme_manager_t *manager);

/**
 * @brief Adds a color scheme to the manager
 *
 * @param manager The manager to modify
 * @param scheme The scheme ID
 * @param palette The color palette
 * @return bool True if successful, false otherwise
 */
bool rift_color_scheme_manager_add_scheme(rift_color_scheme_manager_t *manager,
                                          rift_color_scheme_t scheme,
                                          const rift_color_scheme_entry_t *palette);

/**
 * @brief Sets the active color scheme
 *
 * @param manager The manager to modify
 * @param scheme The scheme to set as active
 * @return bool True if successful, false otherwise
 */
bool rift_color_scheme_manager_set_active_scheme(rift_color_scheme_manager_t *manager,
                                                 rift_color_scheme_t scheme);

/**
 * @brief Gets the active color scheme
 *
 * @param manager The manager to query
 * @return rift_color_scheme_t The active scheme
 */
rift_color_scheme_t
rift_color_scheme_manager_get_active_scheme(const rift_color_scheme_manager_t *manager);

/**
 * @brief Gets a color for a specific role from a scheme
 *
 * @param manager The manager to query
 * @param role The color role
 * @return rift_color_t The color for the role (default if not found)
 */
rift_color_t rift_color_scheme_manager_get_color(const rift_color_scheme_manager_t *manager,
                                                 rift_color_role_t role);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_COLOR_SCHEME_MANAGER_H */