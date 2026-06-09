/**
 * @file ast_node.h
 * @brief Header file for AST node types and operations in the LibRift regex engine
 *
 * This file defines the node type enumerations and core operations for
 * Abstract Syntax Tree (AST) nodes in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "core/automaton/flags.h"
#include "core/errors/regex_error.h"
#ifndef LIBRIFT_REGEX_PARSER_AST_NODE_H
#define LIBRIFT_REGEX_PARSER_AST_NODE_H



/* Forward declaration for state information type */
typedef struct rift_state_info rift_state_info_t;

#ifdef __cplusplus
extern "C" {
#endif
/* Initial capacity for the children array */
#define INITIAL_CHILD_CAPACITY 4
/**
 * @brief Type of AST node
 */
typedef enum rift_regex_ast_node_type {
    RIFT_REGEX_AST_NODE_NONE = 0,            /**< Invalid/uninitialized node */
    RIFT_REGEX_AST_NODE_ALTERNATION,         /**< Alternation (|) */
    RIFT_REGEX_AST_NODE_CONCATENATION,       /**< Concatenation (sequence) */
    RIFT_REGEX_AST_NODE_LITERAL,             /**< Literal character */
    RIFT_REGEX_AST_NODE_DOT,                 /**< Dot (any character) */
    RIFT_REGEX_AST_NODE_CHARACTER_CLASS,     /**< Character class [...] */
    RIFT_REGEX_AST_NODE_GROUP,               /**< Capturing group (...) */
    RIFT_REGEX_AST_NODE_NON_CAPTURING_GROUP, /**< Non-capturing group (?:...) */
    RIFT_REGEX_AST_NODE_NAMED_GROUP,         /**< Named capturing group (?<name>...) */
    RIFT_REGEX_AST_NODE_BACKREFERENCE,       /**< Backreference \1, \2, etc. */
    RIFT_REGEX_AST_NODE_NAMED_BACKREFERENCE, /**< Named backreference \k<name> */
    RIFT_REGEX_AST_NODE_QUANTIFIER,          /**< Quantifier (*, +, ?, {m,n}) */
    RIFT_REGEX_AST_NODE_ANCHOR,              /**< Anchor (^, $, \b, etc.) */
    RIFT_REGEX_AST_NODE_LOOKAHEAD,           /**< Positive lookahead (?=...) */
    RIFT_REGEX_AST_NODE_NEGATIVE_LOOKAHEAD,  /**< Negative lookahead (?!...) */
    RIFT_REGEX_AST_NODE_LOOKBEHIND,          /**< Positive lookbehind (?<=...) */
    RIFT_REGEX_AST_NODE_NEGATIVE_LOOKBEHIND, /**< Negative lookbehind (?<!...) */
    RIFT_REGEX_AST_NODE_ATOMIC_GROUP,        /**< Atomic group (?>...) */
    RIFT_REGEX_AST_NODE_COMMENT,             /**< Comment (?#...) */
    RIFT_REGEX_AST_NODE_CONDITIONAL,         /**< Conditional (?(condition)then|else) */
    RIFT_REGEX_AST_NODE_OPTION,              /**< Option (?i), (?m), etc. */
    RIFT_REGEX_AST_NODE_BACKTRACK_CONTROL,   /**< Backtrack control (*FAIL), etc. */
    RIFT_REGEX_AST_NODE_BACKREF_RESET,       /**< Backref reset \K */
    RIFT_REGEX_AST_NODE_WORD_BOUNDARY,       /**< Word boundary \b */
    RIFT_REGEX_AST_NODE_NOT_WORD_BOUNDARY,   /**< Not word boundary \B */
    RIFT_REGEX_AST_NODE_POSIX_CLASS,         /**< POSIX character class [:alpha:], etc. */
    RIFT_REGEX_AST_NODE_UNICODE_PROPERTY,    /**< Unicode property \p{L}, etc. */
    RIFT_REGEX_AST_NODE_ROOT,                /**< Root node of the AST */
    RIFT_REGEX_AST_NODE_PATTERN,             /**< Pattern node */
    RIFT_REGEX_AST_NODE_SEQUENCE,            /**< Sequence of nodes */
    RIFT_REGEX_AST_NODE_CHAR                 /**< Character node */
} rift_regex_ast_node_type_t;

typedef struct rift_regex_ast_node {
    rift_regex_ast_node_type_t type;       /**< Node type */
    char *value;                           /**< Node value (depends on type) */
    struct rift_regex_ast_node **children; /**< Child nodes */
    size_t num_children;                   /**< Number of children */
    size_t child_capacity;                 /**< Capacity of children array */
    rift_regex_flags_t flags;              /**< Regex flags for this node */
    struct rift_regex_ast_node *parent;    /**< Parent node */
    rift_state_info_t *state_info;         /**< State information for this node */
} rift_regex_ast_node_t;

/**
 * @brief Create a new AST node
 *
 * @param type The type of node to create
 * @return A new AST node or NULL on failure
 */
rift_regex_ast_node_t *rift_regex_ast_node_create(rift_regex_ast_node_type_t type);

/**
 * @brief Free an AST node and its resources (but not its children)
 *
 * @param node The node to free
 */
void rift_regex_ast_node_free(rift_regex_ast_node_t *node);

/**
 * @brief Free an AST node and all its children recursively
 *
 * @param node The node to free
 */
void rift_regex_ast_node_free_recursive(rift_regex_ast_node_t *node);

/**
 * @brief Set a string value for a node
 *
 * @param node The node
 * @param value The string value to set
 * @return true if successful, false otherwise
 */
bool rift_regex_ast_node_set_value(rift_regex_ast_node_t *node, const char *value);

/**
 * @brief Get the value of a node
 *
 * @param node The node
 * @return The node value or NULL if not set
 */
const char *rift_regex_ast_node_get_value(const rift_regex_ast_node_t *node);

/**
 * @brief Add a child node
 *
 * @param parent The parent node
 * @param child The child node to add
 * @return true if successful, false otherwise
 */
bool rift_regex_ast_node_add_child(rift_regex_ast_node_t *parent, rift_regex_ast_node_t *child);

/**
 * @brief Remove a child node (without freeing it)
 *
 * @param parent The parent node
 * @param index The index of the child to remove
 * @return The removed child node or NULL if not found
 */
rift_regex_ast_node_t *rift_regex_ast_node_remove_child(rift_regex_ast_node_t *parent,
                                                        size_t index);

/**
 * @brief Get a child node by index
 *
 * @param node The parent node
 * @param index The index of the child
 * @return The child node or NULL if not found
 */
rift_regex_ast_node_t *rift_regex_ast_node_get_child(const rift_regex_ast_node_t *node,
                                                     size_t index);

/**
 * @brief Get the number of children
 *
 * @param node The node
 * @return The number of children
 */
size_t rift_regex_ast_node_get_child_count(const rift_regex_ast_node_t *node);

/**
 * @brief Get the parent node
 *
 * @param node The node
 * @return The parent node or NULL if it's a root node
 */
rift_regex_ast_node_t *rift_regex_ast_node_get_parent(const rift_regex_ast_node_t *node);

/**
 * @brief Clone a node (shallow copy, without children)
 *
 * @param node The node to clone
 * @return A new node that is a copy of the original, or NULL on failure
 */
rift_regex_ast_node_t *rift_regex_ast_node_clone(const rift_regex_ast_node_t *node);

/**
 * @brief Clone a node and all its children recursively
 *
 * @param node The node to clone
 * @return A new node that is a copy of the original with all children, or NULL on failure
 */
rift_regex_ast_node_t *rift_regex_ast_node_clone_recursive(const rift_regex_ast_node_t *node);

/**
 * @brief Check if a node is of a specific type
 *
 * @param node The node to check
 * @param type The type to check against
 * @return true if the node is of the specified type, false otherwise
 */
bool rift_regex_ast_node_is_type(const rift_regex_ast_node_t *node,
                                 rift_regex_ast_node_type_t type);

/**
 * @brief Get a string representation of a node type
 *
 * @param type The node type
 * @return A string representation of the type
 */
const char *rift_regex_ast_node_type_to_string(rift_regex_ast_node_type_t type);

/**
 * @brief Convert a node to a string representation (for debugging)
 *
 * @param node The node
 * @param include_children Whether to include children in the string representation
 * @return A string representation of the node (must be freed by caller)
 */
char *rift_regex_ast_node_to_string(const rift_regex_ast_node_t *node, bool include_children);

/**
 * @brief Get state information for an AST node
 *
 * @param node The AST node
 * @return Pointer to state information or NULL if not set
 */
rift_state_info_t *rift_regex_ast_node_get_state_info(const rift_regex_ast_node_t *node);

/**
 * @brief Set state information for an AST node
 *
 * @param node The AST node
 * @param state_info The state information to set (will be copied)
 * @param error Pointer to error structure (can be NULL)
 * @return true if successful, false otherwise
 */
bool rift_regex_ast_node_set_state_info(rift_regex_ast_node_t *node,
                                        const rift_state_info_t *state_info,
                                        rift_regex_error_t *error);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LIBRIFT_REGEX_PARSER_AST_NODE_H */