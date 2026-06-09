/**
 * @file ast_node_test.c
 * @brief Unit tests for the AST node implementation
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/parser/ast_node.h"

/* Test fixture setup */
static void
setup(void)
{
    // Initialize any resources needed for tests
}

/* Test fixture teardown */
static void
teardown(void)
{
    // Clean up any resources used in tests
}

/* Test node creation */
static void
test_node_create(void)
{
    rift_regex_ast_node_t *node = rift_regex_ast_node_create(RIFT_REGEX_AST_NODE_CHAR);
    assert(node != NULL);
    assert(node->type == RIFT_REGEX_AST_NODE_CHAR);
    assert(node->value == NULL);
    assert(node->children == NULL);
    assert(node->num_children == 0);
    assert(node->parent == NULL);
    rift_regex_ast_node_free(node);
}

/* Test node value setting */
static void
test_node_set_value(void)
{
    rift_regex_ast_node_t *node = rift_regex_ast_node_create(RIFT_REGEX_AST_NODE_CHAR);
    assert(node != NULL);

    bool result = rift_regex_ast_node_set_value(node, "a");
    assert(result == true);
    assert(strcmp(rift_regex_ast_node_get_value(node), "a") == 0);

    rift_regex_ast_node_free(node);
}

/* Test adding child nodes */
static void
test_node_add_child(void)
{
    rift_regex_ast_node_t *parent = rift_regex_ast_node_create(RIFT_REGEX_AST_NODE_CONCATENATION);
    rift_regex_ast_node_t *child = rift_regex_ast_node_create(RIFT_REGEX_AST_NODE_CHAR);

    assert(parent != NULL);
    assert(child != NULL);

    bool result = rift_regex_ast_node_add_child(parent, child);
    assert(result == true);

    assert(rift_regex_ast_node_get_child_count(parent) == 1);
    assert(rift_regex_ast_node_get_child(parent, 0) == child);
    assert(rift_regex_ast_node_get_parent(child) == parent);

    // Free the parent, which should handle freeing children
    rift_regex_ast_node_free_recursive(parent);
}

/* Test node type checking */
static void
test_node_is_type(void)
{
    rift_regex_ast_node_t *node = rift_regex_ast_node_create(RIFT_REGEX_AST_NODE_ALTERNATION);
    assert(node != NULL);

    assert(rift_regex_ast_node_is_type(node, RIFT_REGEX_AST_NODE_ALTERNATION) == true);
    assert(rift_regex_ast_node_is_type(node, RIFT_REGEX_AST_NODE_CHAR) == false);

    rift_regex_ast_node_free(node);
}

/* Test node cloning */
static void
test_node_clone(void)
{
    rift_regex_ast_node_t *original = rift_regex_ast_node_create(RIFT_REGEX_AST_NODE_CHAR);
    assert(original != NULL);

    rift_regex_ast_node_set_value(original, "a");

    rift_regex_ast_node_t *clone = rift_regex_ast_node_clone(original);
    assert(clone != NULL);
    assert(clone != original);
    assert(clone->type == original->type);
    assert(strcmp(rift_regex_ast_node_get_value(clone), "a") == 0);

    rift_regex_ast_node_free(original);
    rift_regex_ast_node_free(clone);
}

int
main(void)
{
    setup();

    test_node_create();
    test_node_set_value();
    test_node_add_child();
    test_node_is_type();
    test_node_clone();

    teardown();
    printf("All tests passed!\n");
    return 0;
}
