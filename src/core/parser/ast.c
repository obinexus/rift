/**
 * @file ast.c
 * @brief Implementation of AST functions for the LibRift regex engine
 *
 * This file implements the Abstract Syntax Tree (AST) structure used to
 * represent parsed regular expressions in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/parser/ast.h
#include "librift/parser/ast.h"
/**
tmp/librift_analysis/librift-4933472-W/backup/v1/src/parser/ast.h
tmp/librift_analysis/librift-4933472-W/backup/v2/parser/ast.h
tmp/librift_analysis/librift-4933472-W/src/core/parser/ast.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/parser/ast.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v2/parser/ast.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/parser/ast.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/parser/ast.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v2/parser/ast.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/parser/ast.h"
 * @brief Create a new AST
 *
 * @return A new AST or NULL on failure
 */
rift_regex_ast_t *
rift_regex_ast_create(void)
{
    rift_regex_ast_t *ast = (rift_regex_ast_t *)malloc(sizeof(rift_regex_ast_t));
    if (!ast) {
        return NULL;
    }

    ast->root = NULL;
    ast->flags = RIFT_REGEX_FLAG_NONE;
    ast->group_count = 0;
    ast->is_valid = false;

    return ast;
}

/**
 * @brief Free an AST and all its nodes
 *
 * @param ast The AST to free
 */
void
rift_regex_ast_free(rift_regex_ast_t *ast)
{
    if (!ast) {
        return;
    }

    // Free the root node and all its children
    if (ast->root) {
        free_ast_node(ast->root);
    }

    // Free the AST itself
    free(ast);
}

/**
 * @brief Free an AST (alias for compatibility)
 *
 * @param ast The AST to free
 */
void
free_ast(rift_regex_ast_t *ast)
{
    rift_regex_ast_free(ast);
}

/**
 * @brief Create a new AST node
 *
 * @param type The type of node to create
 * @return A new AST node or NULL on failure
 */
rift_regex_ast_node_t *
rift_regex_ast_create_node(rift_regex_ast_node_type_t type)
{
    return create_ast_node(type);
}

/**
 * @brief Add a child node to a parent node
 *
 * @param parent The parent node
 * @param child The child node to add
 * @return true if successful, false otherwise
 */
bool
rift_regex_ast_add_child(rift_regex_ast_node_t *parent, rift_regex_ast_node_t *child)
{
    return rift_regex_ast_node_add_child(parent, child);
}

/**
 * @brief Set a string value for a node
 *
 * @param node The node
 * @param value The string value to set
 * @return true if successful, false otherwise
 */
bool
rift_regex_ast_set_value(rift_regex_ast_node_t *node, const char *value)
{
    return rift_regex_ast_node_set_value(node, value);
}

/**
 * @brief Get the type of a node
 *
 * @param node The node
 * @return The node type
 */
rift_regex_ast_node_type_t
rift_regex_ast_get_node_type(const rift_regex_ast_node_t *node)
{
    if (!node) {
        return RIFT_REGEX_AST_NODE_ROOT; // Default to root type
    }

    return node->type;
}

/**
 * @brief Get the value of a node
 *
 * @param node The node
 * @return The node value or NULL if not set
 */
const char *
rift_regex_ast_get_node_value(const rift_regex_ast_node_t *node)
{
    return rift_regex_ast_node_get_value(node);
}

/**
 * @brief Get the root node of an AST
 *
 * @param ast The AST
 * @return The root node or NULL if not set
 */
rift_regex_ast_node_t *
rift_regex_ast_get_root(const rift_regex_ast_t *ast)
{
    if (!ast) {
        return NULL;
    }

    return ast->root;
}

/**
 * @brief Set the root node of an AST
 *
 * @param ast The AST
 * @param root The root node to set
 * @return true if successful, false otherwise
 */
bool
rift_regex_ast_set_root(rift_regex_ast_t *ast, rift_regex_ast_node_t *root)
{
    if (!ast) {
        return false;
    }

    ast->root = root;
    return true;
}

/**
 * @brief Get the number of children of a node
 *
 * @param node The node
 * @return The number of children
 */
size_t
rift_regex_ast_get_child_count(const rift_regex_ast_node_t *node)
{
    return rift_regex_ast_node_get_child_count(node);
}

/**
 * @brief Get a child of a node by index
 *
 * @param node The node
 * @param index The index of the child
 * @return The child node or NULL if not found
 */
rift_regex_ast_node_t *
rift_regex_ast_get_child(const rift_regex_ast_node_t *node, size_t index)
{
    return rift_regex_ast_node_get_child(node, index);
}

/**
 * @brief Get the parent of a node
 *
 * @param node The node
 * @return The parent node or NULL if not set
 */
rift_regex_ast_node_t *
rift_regex_ast_get_parent(const rift_regex_ast_node_t *node)
{
    return rift_regex_ast_node_get_parent(node);
}

/**
 * @brief Convert an AST to a string representation
 *
 * @param ast The AST
 * @return A string representation of the AST or NULL on failure. The caller is
 *         responsible for freeing the returned string.
 */
char *
rift_regex_ast_to_string(const rift_regex_ast_t *ast)
{
    if (!ast || !ast->root) {
        return NULL;
    }

    // First line is AST info
    char info[256];
    snprintf(info, sizeof(info), "AST: flags=0x%x, groups=%zu, valid=%s\n", ast->flags,
             ast->group_count, ast->is_valid ? "true" : "false");

    // Get the string representation of the root node
    char *root_str = rift_regex_ast_node_to_string(ast->root, true);
    if (!root_str) {
        return NULL;
    }

    // Combine the info and root string
    size_t info_len = strlen(info);
    size_t root_len = strlen(root_str);
    char *result = (char *)malloc(info_len + root_len + 1);

    if (!result) {
        free(root_str);
        return NULL;
    }

    strcpy(result, info);
    strcat(result, root_str);
    free(root_str);

    return result;
}

/**
 * @brief Clone a node and all its children
 *
 * @param node The node to clone
 * @return A new node that is a copy of the original, or NULL on failure
 */
rift_regex_ast_node_t *
rift_regex_ast_clone_node(const rift_regex_ast_node_t *node)
{
    return rift_regex_ast_node_clone_recursive(node);
}

/**
 * @brief Clone an AST
 *
 * @param ast The AST to clone
 * @return A new AST that is a copy of the original, or NULL on failure
 */
rift_regex_ast_t *
rift_regex_ast_clone(const rift_regex_ast_t *ast)
{
    if (!ast) {
        return NULL;
    }

    // Create a new AST
    rift_regex_ast_t *clone = rift_regex_ast_create();
    if (!clone) {
        return NULL;
    }

    // Copy AST properties
    clone->flags = ast->flags;
    clone->group_count = ast->group_count;
    clone->is_valid = ast->is_valid;

    // Clone the root node if present
    if (ast->root) {
        clone->root = rift_regex_ast_clone_node(ast->root);
        if (!clone->root) {
            rift_regex_ast_free(clone);
            return NULL;
        }
    }

    return clone;
}

/**
 * @brief Count the number of capture groups in an AST
 *
 * @param ast The AST
 * @return The number of capture groups
 */
size_t
rift_regex_ast_count_groups(const rift_regex_ast_t *ast)
{
    if (!ast || !ast->root) {
        return 0;
    }

    // In a full implementation, we would traverse the tree and count group nodes
    // For now, we'll just return the stored count
    return ast->group_count;
}

/**
 * @brief Validate an AST
 *
 * @param ast The AST to validate
 * @return true if the AST is valid, false otherwise
 */
bool
rift_regex_ast_validate(rift_regex_ast_t *ast)
{
    if (!ast || !ast->root) {
        return false;
    }

    // In a full implementation, we would perform validation checks
    // For now, we'll just mark it as valid
    ast->is_valid = true;
    return true;
}

/**
 * @brief Build an AST from a pattern string
 *
 * @param root The root node to build from
 * @param pattern The pattern string
 * @param flags Compilation flags
 * @param error Pointer to store error code (can be NULL)
 * @return true if successful, false otherwise
 */
bool
build_ast_from_pattern(rift_regex_ast_node_t *root, const char *pattern, rift_regex_flags_t flags,
                       rift_regex_error_t *error)
{
    if (!root || !pattern) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
            strcpy(error->message, "Invalid parameter: root or pattern is NULL");
        }
        return false;
    }

    // This is a stub implementation - in a real implementation,
    // we would parse the pattern string and build an AST

    // For now, we'll create a simple AST with a literal node
    rift_regex_ast_node_t *literal = rift_regex_ast_create_node(RIFT_REGEX_AST_NODE_LITERAL);
    if (!literal) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_MEMORY_ALLOCATION;
            strcpy(error->message, "Memory allocation failed");
        }
        return false;
    }

    if (!rift_regex_ast_set_value(literal, pattern)) {
        free_ast_node(literal);
        if (error) {
            error->code = RIFT_REGEX_ERROR_MEMORY_ALLOCATION;
            strcpy(error->message, "Memory allocation failed");
        }
        return false;
    }

    if (!rift_regex_ast_add_child(root, literal)) {
        rift_regex_ast_node_free_recursive(literal);
        if (error) {
            error->code = RIFT_REGEX_ERROR_INTERNAL;
            strcpy(error->message, "Internal error while adding child node");
        }
        return false;
    }

    return true;
}

/**
 * @brief Parse a regex pattern into an AST
 *
 * @param pattern The pattern string
 * @param flags Compilation flags
 * @param error Pointer to store error code (can be NULL)
 * @return A new AST representing the pattern or NULL on failure
 */
rift_regex_ast_t *
rift_regex_parse(const char *pattern, rift_regex_flags_t flags, rift_regex_error_t *error)
{
    if (!pattern) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
            strcpy(error->message, "Invalid parameter: pattern is NULL");
        }
        return NULL;
    }

    // Create a new AST
    rift_regex_ast_t *ast = rift_regex_ast_create();
    if (!ast) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_MEMORY_ALLOCATION;
            strcpy(error->message, "Memory allocation failed");
        }
        return NULL;
    }

    // Set the flags
    ast->flags = flags;

    // Create a root node
    rift_regex_ast_node_t *root = rift_regex_ast_create_node(RIFT_REGEX_AST_NODE_ROOT);
    if (!root) {
        rift_regex_ast_free(ast);
        if (error) {
            error->code = RIFT_REGEX_ERROR_MEMORY_ALLOCATION;
            strcpy(error->message, "Memory allocation failed");
        }
        return NULL;
    }

    ast->root = root;

    // Build the AST from the pattern
    if (!build_ast_from_pattern(root, pattern, flags, error)) {
        rift_regex_ast_free(ast);
        return NULL;
    }

    // Count the number of capture groups
    ast->group_count = rift_regex_ast_count_groups(ast);

    // Validate the AST
    ast->is_valid = rift_regex_ast_validate(ast);
    if (!ast->is_valid) {
        rift_regex_ast_free(ast);
        if (error) {
            error->code = RIFT_REGEX_ERROR_SYNTAX;
            strcpy(error->message, "Invalid regex syntax");
        }
        return NULL;
    }

    return ast;
}
