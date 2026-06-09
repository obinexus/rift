/**
 * @file ast.h
 * @brief Header file for the regex Abstract Syntax Tree (AST) component of LibRift
 *
 * This file defines the interface for creating and manipulating ASTs
 * that represent regular expressions in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "core/automaton/flags.h"
#include "core/errors/regex_error.h"
#include "core/parser/ast_node.h"
#ifndef LIBRIFT_REGEX_PARSER_AST_H
#define LIBRIFT_REGEX_PARSER_AST_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AST structure
 */
typedef struct rift_regex_ast {
    rift_regex_ast_node_t *root; /**< Root node of the AST */
    rift_regex_flags_t flags;    /**< Compilation flags used for this AST */
    size_t group_count;          /**< Number of capture groups in the AST */
    bool is_valid;               /**< Whether the AST is structurally valid */
} rift_regex_ast_t;

/**
 * @brief Create a new AST
 *
 * @return A new AST or NULL on failure
 */
rift_regex_ast_t *rift_regex_ast_create(void);

/**
 * @brief Free an AST and all its nodes
 *
 * @param ast The AST to free
 */
void rift_regex_ast_free(rift_regex_ast_t *ast);

/**
/**
 * @brief Create a new AST node
 *
 * @param type The type of node to create
 * @return A new AST node or NULL on failure
 */
rift_regex_ast_node_t *create_ast_node(rift_regex_ast_node_type_t type);
/**
 * @brief Add a child node to a parent node
 *
 * @param parent The parent node
 * @param child The child node to add
 * @return true if successful, false otherwise
 */
bool rift_regex_ast_add_child(rift_regex_ast_node_t *parent, rift_regex_ast_node_t *child);

/**
 * @brief Set a string value for a node
 *
 * @param node The node
 * @param value The string value to set
 * @return true if successful, false otherwise
 */
bool rift_regex_ast_set_value(rift_regex_ast_node_t *node, const char *value);

/**
 * @brief Get the type of a node
 *
 * @param node The node
 * @return The node type
 */
rift_regex_ast_node_type_t rift_regex_ast_get_node_type(const rift_regex_ast_node_t *node);

/**
 * @brief Get the value of a node
 *
 * @param node The node
 * @return The node value or NULL if not set
 */
const char *rift_regex_ast_get_node_value(const rift_regex_ast_node_t *node);

/**
 * @brief Get the root node of an AST
 *
 * @param ast The AST
 * @return The root node or NULL if not set
 */
rift_regex_ast_node_t *rift_regex_ast_get_root(const rift_regex_ast_t *ast);

/**
 * @brief Set the root node of an AST
 *
 * @param ast The AST
 * @param root The root node to set
 * @return true if successful, false otherwise
 */
bool rift_regex_ast_set_root(rift_regex_ast_t *ast, rift_regex_ast_node_t *root);

/**
 * @brief Get the number of children of a node
 *
 * @param node The node
 * @return The number of children
 */
size_t rift_regex_ast_get_child_count(const rift_regex_ast_node_t *node);

/**
 * @brief Get a child of a node by index
 *
 * @param node The node
 * @param index The index of the child
 * @return The child node or NULL if not found
 */
rift_regex_ast_node_t *rift_regex_ast_get_child(const rift_regex_ast_node_t *node, size_t index);

/**
 * @brief Get the parent of a node
 *
 * @param node The node
 * @return The parent node or NULL if not set
 */
rift_regex_ast_node_t *rift_regex_ast_get_parent(const rift_regex_ast_node_t *node);

/**
 * @brief Convert an AST to a string representation
 *
 * @param ast The AST
 * @return A string representation of the AST or NULL on failure. The caller is
 *         responsible for freeing the returned string.
 */
char *rift_regex_ast_to_string(const rift_regex_ast_t *ast);

/**
 * @brief Clone an AST
 *
 * @param ast The AST to clone
 * @return A new AST that is a copy of the original, or NULL on failure
 */
rift_regex_ast_t *rift_regex_ast_clone(const rift_regex_ast_t *ast);

/**
 * @brief Clone a node and all its children
 *
 * @param node The node to clone
 * @return A new node that is a copy of the original, or NULL on failure
 */
rift_regex_ast_node_t *rift_regex_ast_clone_node(const rift_regex_ast_node_t *node);

/**
 * @brief Count the number of capture groups in an AST
 *
 * @param ast The AST
 * @return The number of capture groups
 */
size_t rift_regex_ast_count_groups(const rift_regex_ast_t *ast);

/**
 * @brief Validate an AST
 *
 * @param ast The AST to validate
 * @return true if the AST is valid, false otherwise
 */
bool rift_regex_ast_validate(rift_regex_ast_t *ast);

/**
 * @brief Build an AST from a pattern string
 *
 * @param root The root node to build from
 * @param pattern The pattern string
 * @param flags Compilation flags
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool build_ast_from_pattern(rift_regex_ast_node_t *root, const char *pattern,
                            rift_regex_flags_t flags, rift_regex_error_t *error);

/**
 * @brief Parse a regex pattern into an AST
 *
 * @param pattern The pattern string
 * @param flags Compilation flags
 * @param error Pointer to store error code (can be NULL)
 * @return A new AST representing the pattern or NULL on failure
 */
rift_regex_ast_t *rift_regex_parse(const char *pattern, rift_regex_flags_t flags,
                                   rift_regex_error_t *error);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_REGEX_PARSER_AST_H */