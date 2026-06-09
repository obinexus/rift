/**
 * @file regex_accessibility_bridge.h
 * @brief Accessibility bridge for regex operations in LibRift regex engine
 *
 * This file defines the bridge between the regex engine and accessibility features,
 * enabling accessible pattern matching and visualization.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include "librift/accessibility/features.h"
#include "librift/regex/matcher.h"
#include "librift/visualizer/graph.h"
#ifndef LIBRIFT_REGEX_ACCESSIBILITY_BRIDGE_H
#define LIBRIFT_REGEX_ACCESSIBILITY_BRIDGE_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque structure for the regex accessibility bridge
 */
typedef struct rift_regex_accessibility_bridge rift_regex_accessibility_bridge_t;

/**
 * @brief Creates a new regex accessibility bridge
 *
 * @param engine Regex engine instance
 * @return rift_regex_accessibility_bridge_t* A new regex accessibility bridge or NULL on failure
 */
rift_regex_accessibility_bridge_t *rift_regex_accessibility_bridge_create(void *engine);

/**
 * @brief Destroys a regex accessibility bridge and frees all associated resources
 *
 * @param bridge The bridge to destroy
 */
void rift_regex_accessibility_bridge_destroy(rift_regex_accessibility_bridge_t *bridge);

/**
 * @brief Attaches the bridge to a regex matcher
 *
 * @param bridge The bridge to modify
 * @param matcher The regex matcher to attach to
 * @return bool True if successful, false otherwise
 */
bool rift_regex_accessibility_bridge_attach_to_matcher(rift_regex_accessibility_bridge_t *bridge,
                                                       rift_regex_matcher_t *matcher);

/**
 * @brief Handles accessibility aspects of pattern matching results
 *
 * @param bridge The bridge to use
 * @param result The regex match result
 * @return void* Information about how the result was handled (implementation-specific)
 */
void *
rift_regex_accessibility_bridge_handle_pattern_match(rift_regex_accessibility_bridge_t *bridge,
                                                     rift_regex_match_t *result);

/**
 * @brief Creates an accessible visualization of a regex pattern
 *
 * @param bridge The bridge to use
 * @param pattern The regex pattern to visualize
 * @param graph The graph to populate with the visualization
 * @return bool True if successful, false otherwise
 */
bool
rift_regex_accessibility_bridge_create_automaton_graph(rift_regex_accessibility_bridge_t *bridge,
                                                       const char *pattern, rift_graph_t *graph);

/**
 * @brief Speaks a description of a regex pattern
 *
 * @param bridge The bridge to use
 * @param pattern The regex pattern to describe
 * @return bool True if successful, false otherwise
 */
bool
rift_regex_accessibility_bridge_speak_pattern_description(rift_regex_accessibility_bridge_t *bridge,
                                                          const char *pattern);

/**
 * @brief Gets an accessible description of a regex pattern
 *
 * @param bridge The bridge to use
 * @param pattern The regex pattern to describe
 * @param buffer Buffer to store the description
 * @param buffer_size Size of the buffer
 * @return bool True if successful, false otherwise
 */
bool
rift_regex_accessibility_bridge_get_pattern_description(rift_regex_accessibility_bridge_t *bridge,
                                                        const char *pattern, char *buffer,
                                                        size_t buffer_size);

/**
 * @brief Provides keyboard navigation through a regex pattern
 *
 * @param bridge The bridge to use
 * @param pattern The regex pattern to navigate
 * @param action The navigation action
 * @return bool True if successful, false otherwise
 */
bool rift_regex_accessibility_bridge_navigate_pattern(rift_regex_accessibility_bridge_t *bridge,
                                                      const char *pattern,
                                                      rift_key_action_t action);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_ACCESSIBILITY_BRIDGE_H */