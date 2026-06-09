/**
 * @file color_scheme_manager.h
 * @brief Color scheme management for the LibRift SVG renderer
 *
 * This header defines color scheme management functionality for
 * applying different color schemes to SVG elements with accessibility
 * considerations.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stdint.h>
#ifndef LIBRIFT_COLOR_SCHEME_MANAGER_H
#define LIBRIFT_COLOR_SCHEME_MANAGER_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Forward declaration of the color scheme manager structure
 */
typedef struct rift_color_scheme_manager rift_color_scheme_manager_t;

/**
 * @brief Color scheme enumeration
 */
typedef enum {
    RIFT_COLOR_SCHEME_DEFAULT = 0,   /**< Default color scheme */
    RIFT_COLOR_SCHEME_HIGH_CONTRAST, /**< High contrast scheme for visual impairments */
    RIFT_COLOR_SCHEME_DEUTERANOPIA,  /**< Optimized for red-green color blindness (deuteranopia) */
    RIFT_COLOR_SCHEME_PROTANOPIA,    /**< Optimized for red-green color blindness (protanopia) */
    RIFT_COLOR_SCHEME_TRITANOPIA,    /**< Optimized for blue-yellow color blindness (tritanopia) */
    RIFT_COLOR_SCHEME_MONOCHROME,    /**< Monochrome color scheme */
    RIFT_COLOR_SCHEME_DARK_MODE,     /**< Dark mode color scheme */
    RIFT_COLOR_SCHEME_CUSTOM         /**< Custom color scheme */
} rift_color_scheme_t;

/**
 * @brief Color role enumeration
 */
typedef enum {
    RIFT_COLOR_ROLE_BACKGROUND = 0, /**< Background color */
    RIFT_COLOR_ROLE_FOREGROUND,     /**< Foreground/text color */
    RIFT_COLOR_ROLE_PRIMARY,        /**< Primary element color */
    RIFT_COLOR_ROLE_SECONDARY,      /**< Secondary element color */
    RIFT_COLOR_ROLE_ACCENT,         /**< Accent color */
    RIFT_COLOR_ROLE_HIGHLIGHT,      /**< Highlight color */
    RIFT_COLOR_ROLE_START_STATE,    /**< Start state color */
    RIFT_COLOR_ROLE_ACCEPT_STATE,   /**< Accept state color */
    RIFT_COLOR_ROLE_NORMAL_STATE,   /**< Normal state color */
    RIFT_COLOR_ROLE_TRANSITION,     /**< Transition color */
    RIFT_COLOR_ROLE_ERROR,          /**< Error color */
    RIFT_COLOR_ROLE_WARNING,        /**< Warning color */
    RIFT_COLOR_ROLE_SUCCESS         /**< Success color */
} rift_color_role_t;

/**
 * @brief RGBA color structure
 */
typedef struct {
    uint8_t r; /**< Red component (0-255) */
    uint8_t g; /**< Green component (0-255) */
    uint8_t b; /**< Blue component (0-255) */
    uint8_t a; /**< Alpha component (0-255) */
} rift_color_t;

/**
 * @brief Color palette structure
 */
typedef struct {
    rift_color_t colors[RIFT_COLOR_ROLE_SUCCESS + 1]; /**< Colors for each role */
} rift_color_palette_t;

/**
 * @brief Creates a new color scheme manager
 *
 * @return rift_color_scheme_manager_t* A new color scheme manager or NULL on failure
 */
rift_color_scheme_manager_t *rift_color_scheme_manager_create(void);

/**
 * @brief Destroys a color scheme manager and frees associated resources
 *
 * @param manager The manager to destroy
 */
void rift_color_scheme_manager_destroy(rift_color_scheme_manager_t *manager);

/**
 * @brief Sets the active color scheme
 *
 * @param manager The manager to modify
 * @param scheme The color scheme to set as active
 * @return bool True if successful, false otherwise
 */
bool rift_color_scheme_manager_set_active_scheme(rift_color_scheme_manager_t *manager,
                                                 rift_color_scheme_t scheme);

/**
 * @brief Gets the active color scheme
 *
 * @param manager The manager to query
 * @return rift_color_scheme_t The active color scheme
 */
rift_color_scheme_t
rift_color_scheme_manager_get_active_scheme(const rift_color_scheme_manager_t *manager);

/**
 * @brief Gets a color for a specific role from a color scheme
 *
 * @param manager The manager to query
 * @param scheme The scheme ID
 * @param role The color role
 * @return rift_color_t The color for the role in the specified scheme
 */
rift_color_t rift_color_scheme_manager_get_color(const rift_color_scheme_manager_t *manager,
                                                 rift_color_scheme_t scheme,
                                                 rift_color_role_t role);

/**
 * @brief Gets a color for a specific role from the active color scheme
 *
 * @param manager The manager to query
 * @param role The color role
 * @return rift_color_t The color for the role in the active scheme
 */
rift_color_t rift_color_scheme_manager_get_active_color(const rift_color_scheme_manager_t *manager,
                                                        rift_color_role_t role);

/**
 * @brief Sets a color in a custom color scheme
 *
 * @param manager The manager to modify
 * @param role The color role
 * @param color The color to set
 * @return bool True if successful, false otherwise
 */
bool rift_color_scheme_manager_set_custom_color(rift_color_scheme_manager_t *manager,
                                                rift_color_role_t role, rift_color_t color);

/**
 * @brief Converts a color to a CSS color string
 *
 * @param color The color to convert
 * @return char* The CSS color string (caller must free) or NULL on failure
 */
char *rift_color_scheme_manager_color_to_css_string(rift_color_t color);

/**
 * @brief Creates a color from RGB values
 *
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 * @return rift_color_t The created color
 */
rift_color_t rift_color_scheme_manager_rgb(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Creates a color from RGBA values
 *
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 * @param a Alpha component (0-255)
 * @return rift_color_t The created color
 */
rift_color_t rift_color_scheme_manager_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_COLOR_SCHEME_MANAGER_H */