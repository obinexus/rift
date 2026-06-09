/**
 * @file validator_test.c
 * @brief Test file for the regex validator component of LibRift
 *
 * This file contains unit tests for the regex validator component
 * of the LibRift regex engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "librift/parser/ast.h"
#include "librift/parser/validator.h"

// Mock functions for testing
static rift_regex_ast_t *
create_mock_ast(void)
{
    rift_regex_ast_t *ast = malloc(sizeof(rift_regex_ast_t));
    ast->root = malloc(sizeof(rift_regex_ast_node_t));
    ast->root->type = RIFT_REGEX_AST_NODE_LITERAL;
    ast->root->data.literal.value = 'a';
    ast->root->children = NULL;
    ast->root->num_children = 0;
    return ast;
}

static rift_regex_ast_t *
create_invalid_ast(void)
{
    rift_regex_ast_t *ast = malloc(sizeof(rift_regex_ast_t));
    ast->root = NULL; // Invalid AST with no root node
    return ast;
}

static void
free_mock_ast(rift_regex_ast_t *ast)
{
    if (ast) {
        if (ast->root) {
            free(ast->root);
        }
        free(ast);
    }
}

// Test cases
void
test_validator_create(void)
{
    rift_regex_validator_t *validator = rift_regex_validator_create();
    assert(validator != NULL);
    assert(validator->error[0] == '\0');
    assert(validator->flags == 0);
    assert(validator->max_recursion_depth == 1000);
    rift_regex_validator_free(validator);
    printf("test_validator_create: PASSED\n");
}

void
test_validator_validate(void)
{
    rift_regex_validator_t *validator = rift_regex_validator_create();
    rift_regex_ast_t *ast = create_mock_ast();

    // Test basic validation
    bool result = rift_regex_validator_validate(validator, ast);
    assert(result == true);

    // Test with NULL parameters
    result = rift_regex_validator_validate(NULL, ast);
    assert(result == false);

    result = rift_regex_validator_validate(validator, NULL);
    assert(result == false);

    // Test with invalid AST
    rift_regex_ast_t *invalid_ast = create_invalid_ast();
    result = rift_regex_validator_validate(validator, invalid_ast);
    assert(result == false);

    free_mock_ast(ast);
    free_mock_ast(invalid_ast);
    rift_regex_validator_free(validator);
    printf("test_validator_validate: PASSED\n");
}

void
test_validator_error_handling(void)
{
    rift_regex_validator_t *validator = rift_regex_validator_create();
    rift_regex_ast_t *invalid_ast = create_invalid_ast();

    // Force an error
    bool result = rift_regex_validator_validate(validator, invalid_ast);
    assert(result == false);

    // Check if error message is set
    const char *error = rift_regex_validator_get_last_error(validator);
    assert(error != NULL);
    assert(strlen(error) > 0);

    free_mock_ast(invalid_ast);
    rift_regex_validator_free(validator);
    printf("test_validator_error_handling: PASSED\n");
}

int
main(void)
{
    printf("Running validator tests...\n");

    test_validator_create();
    test_validator_validate();
    test_validator_error_handling();

    printf("All validator tests passed!\n");
    return 0;
}
