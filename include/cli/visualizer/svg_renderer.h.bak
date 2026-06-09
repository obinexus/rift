/**
 * @file svg_renderer.h
 * @brief SVG renderer for the LibRift visualizer module
 *
 * This header defines the SVG renderer interface for visualizing
 * regex automata with accessibility integrations.
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
#include "automaton_observer.h"
#include "real_time_update_manager.h"
#include "svg_automaton_mapper.h"
#include "svg_element.h"
#ifndef LIBRIFT_SVG_RENDERER_H
#define LIBRIFT_SVG_RENDERER_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Forward declaration of the SVG renderer structure
 */
typedef struct rift_svg_renderer rift_svg_renderer_t;

/**
 * @brief Creates a new SVG renderer
 *
 * @return rift_svg_renderer_t* A new SVG renderer instance or NULL on failure
 */
rift_svg_renderer_t *rift_svg_renderer_create(void);

/**
 * @brief Destroys an SVG renderer and frees all associated resources
 *
 * @param renderer The renderer to destroy
 */
void rift_svg_renderer_destroy(rift_svg_renderer_t *renderer);

/**
 * @brief Initializes the SVG renderer with specific dimensions
 *
 * @param renderer The renderer to initialize
 * @param width The width of the rendering area
 * @param height The height of the rendering area
 * @return bool True if initialization was successful, false otherwise
 */
bool rift_svg_renderer_initialize(rift_svg_renderer_t *renderer, int width, int height);

/**
 * @brief Renders an automaton to SVG string
 *
 * @param renderer The renderer to use
 * @param automaton The automaton to render
 * @return char* The rendered SVG as a string (caller must free) or NULL on failure
 */
char *rift_svg_renderer_render(rift_svg_renderer_t *renderer, void *automaton);

/**
 * @brief Exports the current SVG to a file
 *
 * @param renderer The renderer to use
 * @param file_path The file path to export to
 * @return bool True if export was successful, false otherwise
 */
bool rift_svg_renderer_export_to_file(rift_svg_renderer_t *renderer, const char *file_path);

/**
 * @brief Applies accessibility settings to the renderer
 *
 * @param renderer The renderer to modify
 * @param settings The accessibility settings to apply
 * @return void
 */
void rift_svg_renderer_apply_accessibility_settings(rift_svg_renderer_t *renderer, void *settings);

/**
 * @brief Sets the color scheme for the SVG renderer
 *
 * @param renderer The renderer to modify
 * @param scheme The color scheme to apply
 * @return void
 */
void rift_svg_renderer_set_color_scheme(rift_svg_renderer_t *renderer, int scheme);

/**
 * @brief Gets the underlying SVG element from the renderer
 *
 * @param renderer The renderer to query
 * @return rift_svg_element_t* The root SVG element
 */
rift_svg_element_t *rift_svg_renderer_get_root_element(rift_svg_renderer_t *renderer);

/**
 * @brief Updates the renderer in response to automaton changes
 *
 * @param observer The observer (cast to rift_svg_renderer_t*)
 * @param automaton The updated automaton
 * @return void
 */
void rift_svg_renderer_update(rift_automaton_observer_t *observer, void *automaton);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_SVG_RENDERER_H */