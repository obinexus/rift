/**
 * @file svg_element.h
 * @brief SVG element representation for the LibRift visualizer
 *
 * This header defines the SVG element structure and associated functions
 * for building and manipulating SVG document elements.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#ifndef LIBRIFT_SVG_ELEMENT_H
#define LIBRIFT_SVG_ELEMENT_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Forward declaration of the SVG element structure
 */
typedef struct rift_svg_element rift_svg_element_t;

/**
 * @brief SVG attribute pair structure
 */
typedef struct {
    char *name;  /**< Attribute name */
    char *value; /**< Attribute value */
} rift_svg_attribute_t;

/**
 * @brief Creates a new SVG element
 *
 * @param name The element name (e.g., "rect", "circle", "svg")
 * @return rift_svg_element_t* A new SVG element or NULL on failure
 */
rift_svg_element_t *rift_svg_element_create(const char *name);

/**
 * @brief Destroys an SVG element and all its children
 *
 * @param element The element to destroy
 */
void rift_svg_element_destroy(rift_svg_element_t *element);

/**
 * @brief Sets an attribute on an SVG element
 *
 * @param element The element to modify
 * @param name The attribute name
 * @param value The attribute value
 * @return bool True if successful, false otherwise
 */
bool rift_svg_element_set_attribute(rift_svg_element_t *element, const char *name,
                                    const char *value);

/**
 * @brief Gets an attribute value from an SVG element
 *
 * @param element The element to query
 * @param name The attribute name
 * @return const char* The attribute value or NULL if not found
 */
const char *rift_svg_element_get_attribute(const rift_svg_element_t *element, const char *name);

/**
 * @brief Adds a child element to an SVG element
 *
 * @param parent The parent element
 * @param child The child element to add
 * @return bool True if successful, false otherwise
 */
bool rift_svg_element_append_child(rift_svg_element_t *parent, rift_svg_element_t *child);

/**
 * @brief Gets the ID of an SVG element
 *
 * @param element The element to query
 * @return const char* The element ID or NULL if not set
 */
const char *rift_svg_element_get_id(const rift_svg_element_t *element);

/**
 * @brief Sets the ID of an SVG element
 *
 * @param element The element to modify
 * @param id The ID to set
 * @return bool True if successful, false otherwise
 */
bool rift_svg_element_set_id(rift_svg_element_t *element, const char *id);

/**
 * @brief Gets a child element by index
 *
 * @param element The parent element
 * @param index The child index
 * @return rift_svg_element_t* The child element or NULL if not found
 */
rift_svg_element_t *rift_svg_element_get_child(const rift_svg_element_t *element, size_t index);

/**
 * @brief Gets the number of children of an SVG element
 *
 * @param element The element to query
 * @return size_t The number of children
 */
size_t rift_svg_element_get_child_count(const rift_svg_element_t *element);

/**
 * @brief Converts an SVG element to a string representation
 *
 * @param element The element to convert
 * @return char* The SVG string (caller must free) or NULL on failure
 */
char *rift_svg_element_to_string(const rift_svg_element_t *element);

/**
 * @brief Sets an accessible label for an SVG element
 *
 * @param element The element to modify
 * @param label The accessible label
 * @return bool True if successful, false otherwise
 */
bool rift_svg_element_set_accessible_label(rift_svg_element_t *element, const char *label);

/**
 * @brief Removes a child element from a parent
 *
 * @param parent The parent element
 * @param child The child element to remove
 * @return bool True if successful, false otherwise
 */
bool rift_svg_element_remove_child(rift_svg_element_t *parent, rift_svg_element_t *child);

/**
 * @brief Creates a deep clone of an SVG element
 *
 * @param element The element to clone
 * @return rift_svg_element_t* The cloned element or NULL on failure
 */
rift_svg_element_t *rift_svg_element_clone(const rift_svg_element_t *element);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_SVG_ELEMENT_H */