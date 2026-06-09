/**
 * @file svg_automaton_mapper.h
 * @brief Maps between automaton states and SVG elements
 *
 * This header defines the mapping functionality between automaton states/transitions
 * and their corresponding SVG element representations.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "svg_element.h"
#ifndef LIBRIFT_SVG_AUTOMATON_MAPPER_H
#define LIBRIFT_SVG_AUTOMATON_MAPPER_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Forward declaration of the SVG automaton mapper structure
 */
typedef struct rift_svg_automaton_mapper rift_svg_automaton_mapper_t;

/**
 * @brief Creates a new SVG automaton mapper
 *
 * @return rift_svg_automaton_mapper_t* A new mapper instance or NULL on failure
 */
rift_svg_automaton_mapper_t *rift_svg_automaton_mapper_create(void);

/**
 * @brief Destroys an SVG automaton mapper and frees associated resources
 *
 * @param mapper The mapper to destroy
 */
void rift_svg_automaton_mapper_destroy(rift_svg_automaton_mapper_t *mapper);

/**
 * @brief Maps an automaton to SVG elements
 *
 * @param mapper The mapper to use
 * @param automaton The automaton to map
 * @return rift_svg_element_t* The root SVG element containing the mapped automaton
 */
rift_svg_element_t *
rift_svg_automaton_mapper_map_automaton_to_svg(rift_svg_automaton_mapper_t *mapper,
                                               void *automaton);

/**
 * @brief Updates existing SVG elements from an automaton
 *
 * @param mapper The mapper to use
 * @param automaton The updated automaton
 * @return bool True if successful, false otherwise
 */
bool rift_svg_automaton_mapper_update_elements_from_automaton(rift_svg_automaton_mapper_t *mapper,
                                                              void *automaton);

/**
 * @brief Gets the SVG element for a specific automaton state
 *
 * @param mapper The mapper to query
 * @param state The automaton state
 * @return rift_svg_element_t* The corresponding SVG element or NULL if not found
 */
rift_svg_element_t *rift_svg_automaton_mapper_get_state_element(rift_svg_automaton_mapper_t *mapper,
                                                                void *state);

/**
 * @brief Gets the SVG element for a specific automaton transition
 *
 * @param mapper The mapper to query
 * @param transition The automaton transition
 * @return rift_svg_element_t* The corresponding SVG element or NULL if not found
 */
rift_svg_element_t *
rift_svg_automaton_mapper_get_transition_element(rift_svg_automaton_mapper_t *mapper,
                                                 void *transition);

/**
 * @brief Clears all mappings from the mapper
 *
 * @param mapper The mapper to clear
 * @return bool True if successful, false otherwise
 */
bool rift_svg_automaton_mapper_clear_mappings(rift_svg_automaton_mapper_t *mapper);

/**
 * @brief Gets the state from an SVG element
 *
 * @param mapper The mapper to query
 * @param element The SVG element
 * @return void* The corresponding automaton state or NULL if not found
 */
void *rift_svg_automaton_mapper_get_state_from_element(rift_svg_automaton_mapper_t *mapper,
                                                       rift_svg_element_t *element);

/**
 * @brief Gets the transition from an SVG element
 *
 * @param mapper The mapper to query
 * @param element The SVG element
 * @return void* The corresponding automaton transition or NULL if not found
 */
void *rift_svg_automaton_mapper_get_transition_from_element(rift_svg_automaton_mapper_t *mapper,
                                                            rift_svg_element_t *element);

/**
 * @brief Applies a custom layout algorithm to the automaton visualization
 *
 * @param mapper The mapper to use
 * @param layout_algorithm The layout algorithm identifier (0 = default)
 * @return bool True if successful, false otherwise
 */
bool rift_svg_automaton_mapper_apply_layout(rift_svg_automaton_mapper_t *mapper,
                                            int layout_algorithm);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_SVG_AUTOMATON_MAPPER_H */