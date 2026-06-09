#ifndef LIBRIFT_CORE_ACCESSIBILITY_COLOR_SCHEME_MANAGER_H
#define LIBRIFT_CORE_ACCESSIBILITY_COLOR_SCHEME_MANAGER_H

#include <stdlib.h>
#include <string.h>
#include "librift/core/error.h"
#include "librift/core/memory.h"


/* Default colors for different roles */
#define DEFAULT_BACKGROUND_COLOR                                                                   \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        255, 255, 255, 255                                                                         \
    } /* White */
#define DEFAULT_FOREGROUND_COLOR                                                                   \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        0, 0, 0, 255                                                                               \
    } /* Black */
#define DEFAULT_PRIMARY_COLOR                                                                      \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        0, 122, 255, 255                                                                           \
    } /* Blue */
#define DEFAULT_SECONDARY_COLOR                                                                    \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        87, 87, 87, 255                                                                            \
    } /* Gray */
#define DEFAULT_ACCENT_COLOR                                                                       \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        255, 149, 0, 255                                                                           \
    } /* Orange */
#define DEFAULT_ERROR_COLOR                                                                        \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        255, 59, 48, 255                                                                           \
    } /* Red */
#define DEFAULT_WARNING_COLOR                                                                      \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        255, 204, 0, 255                                                                           \
    } /* Yellow */
#define DEFAULT_SUCCESS_COLOR                                                                      \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        52, 199, 89, 255                                                                           \
    } /* Green */

/* High contrast colors */
#define HC_BACKGROUND_COLOR                                                                        \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        0, 0, 0, 255                                                                               \
    } /* Black */
#define HC_FOREGROUND_COLOR                                                                        \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        255, 255, 255, 255                                                                         \
    } /* White */
#define HC_PRIMARY_COLOR                                                                           \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        0, 255, 255, 255                                                                           \
    } /* Cyan */
#define HC_SECONDARY_COLOR                                                                         \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        255, 255, 0, 255                                                                           \
    } /* Yellow */
#define HC_ACCENT_COLOR                                                                            \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        255, 0, 255, 255                                                                           \
    } /* Magenta */
#define HC_ERROR_COLOR                                                                             \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        255, 0, 0, 255                                                                             \
    } /* Red */
#define HC_WARNING_COLOR                                                                           \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        255, 255, 0, 255                                                                           \
    } /* Yellow */
#define HC_SUCCESS_COLOR                                                                           \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        0, 255, 0, 255                                                                             \
    } /* Green */

/* Deuteranopia-friendly colors (red-green color blindness) */
#define DEUT_BACKGROUND_COLOR                                                                      \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        255, 255, 255, 255                                                                         \
    } /* White */
#define DEUT_FOREGROUND_COLOR                                                                      \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        0, 0, 0, 255                                                                               \
    } /* Black */
#define DEUT_PRIMARY_COLOR                                                                         \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        0, 80, 255, 255                                                                            \
    } /* Blue */
#define DEUT_SECONDARY_COLOR                                                                       \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        128, 128, 128, 255                                                                         \
    } /* Gray */
#define DEUT_ACCENT_COLOR                                                                          \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        255, 180, 0, 255                                                                           \
    } /* Orange-yellow */
#define DEUT_ERROR_COLOR                                                                           \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        0, 0, 196, 255                                                                             \
    } /* Blue */
#define DEUT_WARNING_COLOR                                                                         \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        255, 215, 0, 255                                                                           \
    } /* Gold */
#define DEUT_SUCCESS_COLOR                                                                         \
    (rift_color_t)                                                                                 \
    {                                                                                              \
        0, 100, 200, 255                                                                           \
    } /* Blue */

/**
 * @brief Structure for the color scheme manager
 */
struct rift_color_scheme_manager {
    rift_color_scheme_t active_scheme;  /**< Active color scheme */
    rift_color_scheme_entry_t *schemes; /**< Array of color schemes */
    size_t scheme_capacity;             /**< Capacity of schemes array */
    bool initialized;                   /**< Initialization flag */
};

/**
 * @brief Initializes the color scheme manager
 *
 * @param manager The manager to initialize
 * @return bool True if successful, false otherwise
 */
bool rift_color_scheme_manager_initialize(rift_color_scheme_manager_t *manager);

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
                                          const rift_color_palette_t *palette);

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
 * @return rift_color_scheme_t The active color scheme
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

#endif /* LIBRIFT_CORE_ACCESSIBILITY_COLOR_SCHEME_MANAGER_H */
