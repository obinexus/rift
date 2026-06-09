/**
 * @file text_size_manager.h
 * @brief Text size management for the LibRift SVG renderer
 *
 * This header defines text size management functionality for adjusting
 * text size in SVG elements based on user accessibility preferences.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef LIBRIFT_TEXT_SIZE_MANAGER_H
#define LIBRIFT_TEXT_SIZE_MANAGER_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Forward declaration of the text size manager structure
 */
typedef struct rift_text_size_manager rift_text_size_manager_t;

/**
 * @brief Text size enumeration
 */
typedef enum {
    RIFT_TEXT_SIZE_SMALL = 0,  /**< Small text size */
    RIFT_TEXT_SIZE_MEDIUM,     /**< Medium (default) text size */
    RIFT_TEXT_SIZE_LARGE,      /**< Large text size */
    RIFT_TEXT_SIZE_EXTRA_LARGE /**< Extra large text size */
} rift_text_size_t;

/**
 * @brief Default scale factors for each text size
 */
#define RIFT_TEXT_SIZE_SMALL_SCALE 0.85f
#define RIFT_TEXT_SIZE_MEDIUM_SCALE 1.0f
#define RIFT_TEXT_SIZE_LARGE_SCALE 1.25f
#define RIFT_TEXT_SIZE_EXTRA_LARGE_SCALE 1.5f

/**
 * @brief Creates a new text size manager
 *
 * @return rift_text_size_manager_t* A new text size manager or NULL on failure
 */
rift_text_size_manager_t *rift_text_size_manager_create(void);

/**
 * @brief Destroys a text size manager and frees associated resources
 *
 * @param manager The manager to destroy
 */
void rift_text_size_manager_destroy(rift_text_size_manager_t *manager);

/**
 * @brief Sets the current text size setting
 *
 * @param manager The manager to modify
 * @param size The text size to set
 * @return bool True if successful, false otherwise
 */
bool rift_text_size_manager_set_text_size(rift_text_size_manager_t *manager, rift_text_size_t size);

/**
 * @brief Gets the current text size setting
 *
 * @param manager The manager to query
 * @return rift_text_size_t The current text size
 */
rift_text_size_t rift_text_size_manager_get_text_size(const rift_text_size_manager_t *manager);

/**
 * @brief Gets the scale factor for the current text size
 *
 * @param manager The manager to query
 * @return float The scale factor (1.0 = default size)
 */
float rift_text_size_manager_get_scale_factor(const rift_text_size_manager_t *manager);

/**
 * @brief Sets a custom scale factor for text sizing
 *
 * @param manager The manager to modify
 * @param scale_factor The custom scale factor (must be > 0)
 * @return bool True if successful, false otherwise
 */
bool rift_text_size_manager_set_custom_scale_factor(rift_text_size_manager_t *manager,
                                                    float scale_factor);

/**
 * @brief Scales a font size based on the current text size
 *
 * @param manager The manager to use
 * @param base_size The base font size to scale
 * @return float The scaled font size
 */
float rift_text_size_manager_scale_font_size(const rift_text_size_manager_t *manager,
                                             float base_size);

/**
 * @brief Gets a CSS font size string based on the current text size
 *
 * @param manager The manager to use
 * @param base_size The base font size to scale
 * @param unit The CSS unit to use (e.g., "px", "pt", "em")
 * @return char* The CSS font size string (caller must free) or NULL on failure
 */
char *rift_text_size_manager_get_css_font_size(const rift_text_size_manager_t *manager,
                                               float base_size, const char *unit);

/**
 * @brief Resets the text size to the default setting
 *
 * @param manager The manager to reset
 * @return bool True if successful, false otherwise
 */
bool rift_text_size_manager_reset(rift_text_size_manager_t *manager);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_TEXT_SIZE_MANAGER_H */