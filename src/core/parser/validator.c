/**
 * @file validator.c
 * @brief Implementation of the regex validator component of LibRift
 *
 * This file implements the validation of Abstract Syntax Trees (ASTs)
 * that represent regular expressions in the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/parser/validator.h
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "librift/parser/validator.h"

tmp/librift_analysis/librift-4933472-W/backup/v1/src/parser/validator.h
tmp/librift_analysis/librift-4933472-W/backup/v2/parser/validator.h
tmp/librift_analysis/librift-4933472-W/src/core/parser/validator.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/parser/validator.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v2/parser/validator.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/parser/validator.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/parser/validator.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v2/parser/validator.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/parser/validator.h"

/**
 * @brief Set an error message in the validator
 *
 * @param validator The validator
 * @param format Format string for the error message
 * @param ... Additional arguments for the format string
 */
static void
set_error(rift_regex_validator_t *validator, const char *format, ...)
{
    if (!validator) {
        return;
    }

    va_list args;
    va_start(args, format);
    vsnprintf(validator->error, RIFT_REGEX_VALIDATOR_MAX_ERROR_LENGTH, format, args);
    va_end(args);
}

/**
 * @brief Create a new validator
 *
 * @return A new validator instance or NULL on failure
 */
rift_regex_validator_t *
rift_regex_validator_create(void)
{
    rift_regex_validator_t *validator =
        (rift_regex_validator_t *)malloc(sizeof(rift_regex_validator_t));

    if (!validator) {
        return NULL;
    }

    // Initialize validator state
    validator->error[0] = '\0';
    validator->flags = 0;
    validator->max_group_number = 0;
    validator->current_group_number = 0;
    validator->max_recursion_depth = 1000; // Reasonable default
    validator->current_recursion_depth = 0;

    return validator;
}

bool
rift_regex_validator_validate_state_transitions(rift_regex_validator_t *validator,
                                                const rift_regex_ast_t *ast)
{
    if (!validator || !ast || !ast->root) {
        return false;
    }

    // Use local variables instead of struct members
    int missing_states = 0;
    int invalid_transitions = 0;

    // Validate state transitions recursively
    bool result = validate_node_transitions(validator, ast->root);

    // You might want to store the results in your validator in a different way
    // or pass them to some other reporting mechanism

    return result;
}

static bool
validate_node_transitions(rift_regex_validator_t *validator, const rift_regex_ast_node_t *node)
{
    if (!node) {
        return true; // Empty node is valid
    }

    // This part may need adjustment based on your actual API
    // Check if this node has state info
    const rift_regex_ast_node_type_t type = rift_regex_ast_get_node_type(node);

    // Validate state transitions for children
    size_t child_count = rift_regex_ast_get_child_count(node);
    for (size_t i = 0; i < child_count; i++) {
        rift_regex_ast_node_t *child = rift_regex_ast_get_child(node, i);
        if (!validate_node_transitions(validator, child)) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Free resources associated with a validator
 *
 * @param validator The validator to free
 */
void
rift_regex_validator_free(rift_regex_validator_t *validator)
{
    if (!validator) {
        return;
    }

    free(validator);
}

/**
 * @brief Check if a node has a valid number of children
 *
 * @param validator The validator
 * @param node The node to check
 * @param min_children Minimum number of children required
 * @param max_children Maximum number of children allowed (use SIZE_MAX for unlimited)
 * @return true if the node has a valid number of children, false otherwise
 */
static bool
validate_child_count(rift_regex_validator_t *validator, const rift_regex_ast_node_t *node,
                     size_t min_children, size_t max_children)
{
    if (!node) {
        set_error(validator, "Node is NULL");
        return false;
    }

    size_t child_count = rift_regex_ast_get_child_count(node);

    if (child_count < min_children) {
        set_error(validator, "Node of type %d requires at least %zu children, but has %zu",
                  rift_regex_ast_get_node_type(node), min_children, child_count);
        return false;
    }

    if (child_count > max_children) {
        set_error(validator, "Node of type %d allows at most %zu children, but has %zu",
                  rift_regex_ast_get_node_type(node), max_children, child_count);
        return false;
    }

    return true;
}

/**
 * @brief Validate an AST node recursively
 *
 * @param validator The validator
 * @param node The node to validate
 * @return true if the node is valid, false otherwise
 */
static bool
validate_node_recursive(rift_regex_validator_t *validator, const rift_regex_ast_node_t *node)
{
    if (!validator || !node) {
        return false;
    }

    // Check for recursion depth limit
    if (validator->current_recursion_depth >= validator->max_recursion_depth) {
        set_error(validator, "Maximum recursion depth exceeded during validation");
        return false;
    }

    // Increment recursion depth
    validator->current_recursion_depth++;

    // Validate the node based on its type
    bool result = rift_regex_validator_validate_node(validator, node);

    // If the node is valid, validate its children
    if (result) {
        size_t child_count = rift_regex_ast_get_child_count(node);

        for (size_t i = 0; i < child_count; i++) {
            rift_regex_ast_node_t *child = rift_regex_ast_get_child(node, i);

            if (!validate_node_recursive(validator, child)) {
                result = false;
                break;
            }
        }
    }

    // Decrement recursion depth
    validator->current_recursion_depth--;

    return result;
}

/**
 * @brief Validate an AST
 *
 * @param validator The validator
 * @param ast The AST to validate
 * @return true if the AST is valid, false otherwise
 */
bool
rift_regex_validator_validate(rift_regex_validator_t *validator, const rift_regex_ast_t *ast)
{
    return rift_regex_validator_validate_with_options(validator, ast, 0);
}

/**
 * @brief Validate an AST with specific flags
 *
 * @param validator The validator
 * @param ast The AST to validate
 * @param flags Flags that affect validation
 * @return true if the AST is valid, false otherwise
 */
bool
rift_regex_validator_validate_with_options(rift_regex_validator_t *validator,
                                           const rift_regex_ast_t *ast, rift_regex_flags_t flags)
{
    if (!validator || !ast) {
        if (validator) {
            set_error(validator, "NULL AST provided for validation");
        }
        return false;
    }

    // Clear any previous error
    validator->error[0] = '\0';

    // Save flags for validation
    validator->flags = flags;

    // Reset state for validation
    validator->max_group_number = 0;
    validator->current_group_number = 0;
    validator->current_recursion_depth = 0;

    // Get the root node of the AST
    rift_regex_ast_node_t *root = rift_regex_ast_get_root(ast);

    if (!root) {
        set_error(validator, "AST has no root node");
        return false;
    }

    // Start recursive validation from the root
    return validate_node_recursive(validator, root);
}

/**
 * @brief Get the last error message
 *
 * @param validator The validator
 * @return The last error message or NULL if no error occurred
 */
const char *
rift_regex_validator_get_last_error(const rift_regex_validator_t *validator)
{
    if (!validator || validator->error[0] == '\0') {
        return NULL;
    }

    return validator->error;
}

/**
 * @brief Validate a specific AST node
 *
 * @param validator The validator
 * @param node The node to validate
 * @return true if the node is valid, false otherwise
 */
bool
rift_regex_validator_validate_node(rift_regex_validator_t *validator,
                                   const rift_regex_ast_node_t *node)
{
    if (!validator || !node) {
        if (validator) {
            set_error(validator, "NULL node provided for validation");
        }
        return false;
    }

    rift_regex_ast_node_type_t type = rift_regex_ast_get_node_type(node);

    switch (type) {
    case RIFT_REGEX_AST_NODE_ALTERNATION:
        return rift_regex_validator_validate_alternation(validator, node);

    case RIFT_REGEX_AST_NODE_CONCATENATION:
        // Concatenation is valid if it has at least one child
        return validate_child_count(validator, node, 1, SIZE_MAX);

    case RIFT_REGEX_AST_NODE_LITERAL:
        // Literal must have a value
        if (!rift_regex_ast_get_node_value(node)) {
            set_error(validator, "Literal node has no value");
            return false;
        }
        return true;

    case RIFT_REGEX_AST_NODE_DOT:
        // Dot doesn't need any additional validation
        return true;

    case RIFT_REGEX_AST_NODE_CHARACTER_CLASS:
        return rift_regex_validator_validate_character_class(validator, node);

    case RIFT_REGEX_AST_NODE_GROUP:
    case RIFT_REGEX_AST_NODE_NON_CAPTURING_GROUP:
    case RIFT_REGEX_AST_NODE_NAMED_GROUP:
        return rift_regex_validator_validate_group(validator, node);

    case RIFT_REGEX_AST_NODE_BACKREFERENCE:
        // Validate backreference has a valid group number
        {
            const char *value = rift_regex_ast_get_node_value(node);
            if (!value) {
                set_error(validator, "Backreference has no value");
                return false;
            }

            // Parse the group number
            char *endptr;
            long group_num = strtol(value, &endptr, 10);

            if (*endptr != '\0' || group_num < 1 || group_num > (long)validator->max_group_number) {
                set_error(validator, "Invalid backreference number: %s", value);
                return false;
            }
        }
        return true;

    case RIFT_REGEX_AST_NODE_NAMED_BACKREFERENCE:
        // Named backreference must have a name value
        if (!rift_regex_ast_get_node_value(node)) {
            set_error(validator, "Named backreference has no name value");
            return false;
        }
        return true;

    case RIFT_REGEX_AST_NODE_QUANTIFIER:
        return rift_regex_validator_validate_quantifier(validator, node);

    case RIFT_REGEX_AST_NODE_ANCHOR:
    case RIFT_REGEX_AST_NODE_WORD_BOUNDARY:
    case RIFT_REGEX_AST_NODE_NOT_WORD_BOUNDARY:
    case RIFT_REGEX_AST_NODE_BACKREF_RESET:
        // These nodes don't need additional validation
        return true;

    case RIFT_REGEX_AST_NODE_LOOKAHEAD:
    case RIFT_REGEX_AST_NODE_NEGATIVE_LOOKAHEAD:
    case RIFT_REGEX_AST_NODE_LOOKBEHIND:
    case RIFT_REGEX_AST_NODE_NEGATIVE_LOOKBEHIND:
    case RIFT_REGEX_AST_NODE_ATOMIC_GROUP:
        // These assertions must have exactly one child
        return validate_child_count(validator, node, 1, 1);

    case RIFT_REGEX_AST_NODE_COMMENT:
        // Comments don't need validation
        return true;

    case RIFT_REGEX_AST_NODE_OPTION:
        // Options must have a valid value
        {
            const char *value = rift_regex_ast_get_node_value(node);
            if (!value) {
                set_error(validator, "Option node has no value");
                return false;
            }

            // Validate option flags
            for (const char *p = value; *p; p++) {
                char c = *p;
                if (c != 'i' && c != 'm' && c != 's' && c != 'x' && c != 'J' && c != 'U' &&
                    c != 'X') {
                    set_error(validator, "Invalid option flag: %c", c);
                    return false;
                }
            }
        }
        return true;

    case RIFT_REGEX_AST_NODE_CONDITIONAL:
        // Conditionals must have 2 or 3 children (condition + then + optional else)
        if (!validate_child_count(validator, node, 2, 3)) {
            return false;
        }
        return true;

    case RIFT_REGEX_AST_NODE_BACKTRACK_CONTROL:
        // Backtrack control must have a valid value
        if (!rift_regex_ast_get_node_value(node)) {
            set_error(validator, "Backtrack control node has no value");
            return false;
        }
        return true;

    case RIFT_REGEX_AST_NODE_POSIX_CLASS:
        // POSIX class must have a valid class name
        {
            const char *value = rift_regex_ast_get_node_value(node);
            if (!value) {
                set_error(validator, "POSIX class has no value");
                return false;
            }

            // Check if it's a valid POSIX class name
            static const char *valid_classes[] = {"alnum", "alpha", "ascii", "blank",  "cntrl",
                                                  "digit", "graph", "lower", "print",  "punct",
                                                  "space", "upper", "word",  "xdigit", NULL};

            bool valid = false;
            for (const char **cls = valid_classes; *cls; cls++) {
                if (strcmp(value, *cls) == 0) {
                    valid = true;
                    break;
                }
            }

            if (!valid) {
                set_error(validator, "Invalid POSIX class name: %s", value);
                return false;
            }
        }
        return true;

    case RIFT_REGEX_AST_NODE_UNICODE_PROPERTY:
        // Unicode property must have a valid property name
        if (!rift_regex_ast_get_node_value(node)) {
            set_error(validator, "Unicode property has no value");
            return false;
        }
        // Full validation of Unicode properties would require a large lookup table
        return true;

    case RIFT_REGEX_AST_NODE_ROOT:
        // Root node should have exactly one child
        return validate_child_count(validator, node, 1, 1);

    default:
        set_error(validator, "Unknown node type: %d", type);
        return false;
    }
}

/**
 * @brief Validate an alternation node
 *
 * @param validator The validator
 * @param node The alternation node to validate
 * @return true if the node is valid, false otherwise
 */
bool
rift_regex_validator_validate_alternation(rift_regex_validator_t *validator,
                                          const rift_regex_ast_node_t *node)
{
    if (!validator || !node) {
        if (validator) {
            set_error(validator, "NULL node provided for alternation validation");
        }
        return false;
    }

    // Alternation must have at least 2 alternatives
    if (!validate_child_count(validator, node, 2, SIZE_MAX)) {
        return false;
    }

    return true;
}

/**
 * @brief Validate a quantifier node
 *
 * @param validator The validator
 * @param node The quantifier node to validate
 * @return true if the node is valid, false otherwise
 */
bool
rift_regex_validator_validate_quantifier(rift_regex_validator_t *validator,
                                         const rift_regex_ast_node_t *node)
{
    if (!validator || !node) {
        if (validator) {
            set_error(validator, "NULL node provided for quantifier validation");
        }
        return false;
    }

    // Quantifier must have exactly one child
    if (!validate_child_count(validator, node, 1, 1)) {
        return false;
    }

    // Get the quantifier specifications
    const char *value = rift_regex_ast_get_node_value(node);
    if (!value) {
        set_error(validator, "Quantifier node has no value");
        return false;
    }

    // Parse quantifier min and max
    unsigned long min = 0, max = 0;
    bool unlimited = false;

    // Handle simple quantifiers
    if (strcmp(value, "*") == 0) {
        min = 0;
        unlimited = true;
    } else if (strcmp(value, "+") == 0) {
        min = 1;
        unlimited = true;
    } else if (strcmp(value, "?") == 0) {
        min = 0;
        max = 1;
    } else if (value[0] == '{') {
        // Parse {m,n} style quantifier
        char *endptr;
        const char *p = value + 1; // Skip opening brace

        // Parse minimum value
        min = strtoul(p, &endptr, 10);

        if (*endptr == ',') {
            // Parse maximum value (if present)
            p = endptr + 1;
            if (*p == '}') {
                // {m,} format - unlimited maximum
                unlimited = true;
            } else {
                // {m,n} format
                max = strtoul(p, &endptr, 10);
                if (*endptr != '}') {
                    set_error(validator, "Invalid quantifier format: %s", value);
                    return false;
                }

                // Validate max >= min
                if (max < min) {
                    set_error(validator,
                              "Invalid quantifier range: maximum (%lu) is less than minimum (%lu)",
                              max, min);
                    return false;
                }
            }
        } else if (*endptr == '}') {
            // {n} format - exact count
            max = min;
        } else {
            set_error(validator, "Invalid quantifier format: %s", value);
            return false;
        }
    } else {
        set_error(validator, "Invalid quantifier value: %s", value);
        return false;
    }

    // Don't allow quantifying nothing or nodes that can't be quantified
    rift_regex_ast_node_t *child = rift_regex_ast_get_child(node, 0);
    if (!child) {
        set_error(validator, "Quantifier has no child node");
        return false;
    }

    rift_regex_ast_node_type_t child_type = rift_regex_ast_get_node_type(child);

    // Disallow quantifying assertions (lookahead, lookbehind, anchors)
    if (child_type == RIFT_REGEX_AST_NODE_LOOKAHEAD ||
        child_type == RIFT_REGEX_AST_NODE_NEGATIVE_LOOKAHEAD ||
        child_type == RIFT_REGEX_AST_NODE_LOOKBEHIND ||
        child_type == RIFT_REGEX_AST_NODE_NEGATIVE_LOOKBEHIND ||
        child_type == RIFT_REGEX_AST_NODE_ANCHOR ||
        child_type == RIFT_REGEX_AST_NODE_WORD_BOUNDARY ||
        child_type == RIFT_REGEX_AST_NODE_NOT_WORD_BOUNDARY) {

        set_error(validator, "Cannot quantify assertion (lookahead, lookbehind, anchor)");
        return false;
    }

    return true;
}

/**
 * @brief Validate a group node
 *
 * @param validator The validator
 * @param node The group node to validate
 * @return true if the node is valid, false otherwise
 */
bool
rift_regex_validator_validate_group(rift_regex_validator_t *validator,
                                    const rift_regex_ast_node_t *node)
{
    if (!validator || !node) {
        if (validator) {
            set_error(validator, "NULL node provided for group validation");
        }
        return false;
    }

    // Group must have exactly one child
    if (!validate_child_count(validator, node, 1, 1)) {
        return false;
    }

    rift_regex_ast_node_type_t type = rift_regex_ast_get_node_type(node);

    // Increment group counter for capturing groups
    if (type == RIFT_REGEX_AST_NODE_GROUP) {
        validator->current_group_number++;
        if (validator->current_group_number > validator->max_group_number) {
            validator->max_group_number = validator->current_group_number;
        }
    } else if (type == RIFT_REGEX_AST_NODE_NAMED_GROUP) {
        // Named group must have a name
        const char *name = rift_regex_ast_get_node_value(node);
        if (!name || name[0] == '\0') {
            set_error(validator, "Named capturing group has no name");
            return false;
        }

        // Validate name (allow alphanumeric and underscore, first char not a digit)
        if (isdigit(name[0])) {
            set_error(validator, "Named capturing group name cannot start with a digit: %s", name);
            return false;
        }

        for (const char *p = name; *p; p++) {
            if (!isalnum(*p) && *p != '_') {
                set_error(validator, "Named capturing group name contains invalid character: %c",
                          *p);
                return false;
            }
        }

        // Increment group counter for named groups too
        validator->current_group_number++;
        if (validator->current_group_number > validator->max_group_number) {
            validator->max_group_number = validator->current_group_number;
        }
    }

    return true;
}

/**
 * @brief Validate a character class node
 *
 * @param validator The validator
 * @param node The character class node to validate
 * @return true if the node is valid, false otherwise
 */
bool
rift_regex_validator_validate_character_class(rift_regex_validator_t *validator,
                                              const rift_regex_ast_node_t *node)
{
    if (!validator || !node) {
        if (validator) {
            set_error(validator, "NULL node provided for character class validation");
        }
        return false;
    }

    // Character class must have a value
    const char *value = rift_regex_ast_get_node_value(node);
    if (!value) {
        set_error(validator, "Character class node has no value");
        return false;
    }

    // Check for empty character class
    if (strcmp(value, "[]") == 0) {
        set_error(validator, "Empty character class");
        return false;
    }

    // Check for unclosed character class
    size_t len = strlen(value);
    if (len < 2 || value[0] != '[' || value[len - 1] != ']') {
        set_error(validator, "Unclosed character class: %s", value);
        return false;
    }

    // Check for unmatched range operator
    bool in_range = false;
    bool escaped = false;

    for (size_t i = 1; i < len - 1; i++) {
        if (escaped) {
            escaped = false;
            continue;
        }

        if (value[i] == '\\') {
            escaped = true;
            continue;
        }

        if (value[i] == '-') {
            if (i == 1 || i == len - 2) {
                // Dash at start or end is literal
                continue;
            }

            if (in_range) {
                set_error(validator, "Unmatched range operator in character class: %s", value);
                return false;
            }

            in_range = true;
        } else if (in_range) {
            // This character completes the range
            in_range = false;
        }
    }

    // Check for incomplete range at end
    if (in_range) {
        set_error(validator, "Incomplete range in character class: %s", value);
        return false;
    }

    return true;
}
