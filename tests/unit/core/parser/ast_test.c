/**
 * @file ast_test.c
 * @brief Unit tests for the AST (Abstract Syntax Tree) implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CTEST_MAIN
#define CTEST_COLOR_OUTPUT
#include "ctest.h"
#include "librift/parser/ast.h"
#include "librift/parser/ast_node.h"

// Setup and teardown functions run before and after each test
CTEST_SETUP(ast)
{
    // Initialize any resources needed for tests
}

CTEST_TEARDOWN(ast)
{
    // Clean up any resources used in tests
}

// Test AST creation
CTEST(ast, create_ast)
{
    rift_regex_ast_t *ast = rift_regex_ast_create();
    ASSERT_NOT_NULL(ast);
    ASSERT_NULL(ast->root);
    rift_regex_ast_free(ast);
}

// Test setting root node
CTEST(ast, set_root_node)
{
    rift_regex_ast_t *ast = rift_regex_ast_create();
    rift_regex_ast_node_t *root = rift_regex_ast_node_create(RIFT_REGEX_AST_NODE_CONCATENATION);

    ASSERT_TRUE(rift_regex_ast_set_root(ast, root));
    ASSERT_EQUAL_PTR(ast->root, root);

    rift_regex_ast_free(ast); // Should free the root node as well
}

// Test AST building with a simple pattern
CTEST(ast, build_simple_ast)
{
    rift_regex_ast_t *ast = rift_regex_ast_create();

    // Create a simple pattern: "ab"
    rift_regex_ast_node_t *concat = rift_regex_ast_node_create(RIFT_REGEX_AST_NODE_CONCATENATION);
    rift_regex_ast_node_t *char_a = rift_regex_ast_node_create(RIFT_REGEX_AST_NODE_CHAR);
    rift_regex_ast_node_t *char_b = rift_regex_ast_node_create(RIFT_REGEX_AST_NODE_CHAR);

    rift_regex_ast_node_set_value(char_a, "a");
    rift_regex_ast_node_set_value(char_b, "b");

    rift_regex_ast_node_add_child(concat, char_a);
    rift_regex_ast_node_add_child(concat, char_b);

    rift_regex_ast_set_root(ast, concat);

    ASSERT_NOT_NULL(ast->root);
    ASSERT_EQUAL(rift_regex_ast_node_get_child_count(ast->root), 2);

    rift_regex_ast_free(ast);
}

// Test AST cloning
CTEST(ast, clone_ast)
{
    rift_regex_ast_t *ast = rift_regex_ast_create();

    // Create a simple pattern: "a"
    rift_regex_ast_node_t *char_a = rift_regex_ast_node_create(RIFT_REGEX_AST_NODE_CHAR);
    rift_regex_ast_node_set_value(char_a, "a");
    rift_regex_ast_set_root(ast, char_a);

    rift_regex_ast_t *clone = rift_regex_ast_clone(ast);

    ASSERT_NOT_NULL(clone);
    ASSERT_NOT_NULL(clone->root);
    ASSERT_NOT_EQUAL_PTR(clone->root, ast->root);
    ASSERT_TRUE(rift_regex_ast_node_is_type(clone->root, RIFT_REGEX_AST_NODE_CHAR));
    ASSERT_STR(rift_regex_ast_node_get_value(clone->root), "a");

    rift_regex_ast_free(ast);
    rift_regex_ast_free(clone);
}

// Main function that runs all tests
int
main(int argc, const char *argv[])
{
    return ctest_main(argc, argv);
}
