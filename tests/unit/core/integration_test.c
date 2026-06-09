/**
 * @file integration_test.c
 * @brief Unit tests for the regex syntax integration layer
 *
 * This file contains tests for the integration of lexer and parser components
 * in the LibRift regex syntax module.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <ctest.h>
#include <string.h>

#include "librift/syntax/integration.h"

// Test callback for tokenization tests
static bool
test_token_callback(int type, const char *value, size_t position, void *user_data)
{
    int *token_count = (int *)user_data;
    (*token_count)++;
    return true;
}

// Test context creation and initialization
CTEST(regex_syntax, context_create)
{
    rift_regex_syntax_context_t *context = rift_regex_syntax_context_create(0);
    ASSERT_NOT_NULL(context);
    ASSERT_NOT_NULL(context->parser);
    ASSERT_NULL(context->lexer);
    ASSERT_EQUAL(RIFT_REGEX_ERROR_NONE, context->error.code);
    rift_regex_syntax_context_free(context);
}

// Test context creation with specific flags
CTEST(regex_syntax, context_create_with_flags)
{
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_CASE_INSENSITIVE | RIFT_REGEX_FLAG_RIFT_SYNTAX;
    rift_regex_syntax_context_t *context = rift_regex_syntax_context_create(flags);
    ASSERT_NOT_NULL(context);
    ASSERT_EQUAL(flags, context->flags);
    ASSERT_TRUE(rift_regex_syntax_is_rift_mode(context));
    rift_regex_syntax_context_free(context);
}

// Test is_rift_syntax detection
CTEST(regex_syntax, is_rift_syntax)
{
    // Standard regex pattern
    ASSERT_FALSE(rift_regex_syntax_is_rift_syntax("a[b-c]+"));

    // LibRift syntax pattern
    ASSERT_TRUE(rift_regex_syntax_is_rift_syntax("@capture(name, [a-z]+)"));
}

// Test basic pattern compilation
CTEST(regex_syntax, basic_compilation)
{
    rift_regex_syntax_context_t *context = rift_regex_syntax_context_create(0);
    ASSERT_NOT_NULL(context);

    const char *pattern = "a[bc]+d";
    rift_regex_ast_t *ast = rift_regex_syntax_compile(context, pattern);
    ASSERT_NOT_NULL(ast);
    ASSERT_EQUAL(RIFT_REGEX_ERROR_NONE, rift_regex_syntax_get_error(context).code);

    // Free resources
    // Note: In a real implementation, we would need to free the AST
    // But since the actual implementation of ast_free is not provided, we're skipping it
    rift_regex_syntax_context_free(context);
}

// Test invalid pattern compilation
CTEST(regex_syntax, invalid_pattern)
{
    rift_regex_syntax_context_t *context = rift_regex_syntax_context_create(0);
    ASSERT_NOT_NULL(context);

    const char *pattern = "a[bc"; // Unclosed bracket
    rift_regex_ast_t *ast = rift_regex_syntax_compile(context, pattern);
    ASSERT_NULL(ast);
    ASSERT_NOT_EQUAL(RIFT_REGEX_ERROR_NONE, rift_regex_syntax_get_error(context).code);
    ASSERT_NOT_NULL(rift_regex_syntax_get_error_message(context));

    rift_regex_syntax_context_free(context);
}

// Test flag setting and getting
CTEST(regex_syntax, flags_management)
{
    rift_regex_syntax_context_t *context = rift_regex_syntax_context_create(0);
    ASSERT_NOT_NULL(context);

    rift_regex_flags_t flags = RIFT_REGEX_FLAG_CASE_INSENSITIVE | RIFT_REGEX_FLAG_MULTILINE;
    ASSERT_TRUE(rift_regex_syntax_set_flags(context, flags));
    ASSERT_EQUAL(flags, rift_regex_syntax_get_flags(context));

    rift_regex_syntax_context_free(context);
}

// Test pattern validation
CTEST(regex_syntax, pattern_validation)
{
    rift_regex_syntax_context_t *context = rift_regex_syntax_context_create(0);
    ASSERT_NOT_NULL(context);

    // Valid pattern
    ASSERT_TRUE(rift_regex_syntax_validate(context, "a[bc]+d"));

    // Invalid pattern
    ASSERT_FALSE(rift_regex_syntax_validate(context, "a[bc"));

    rift_regex_syntax_context_free(context);
}

// Test balanced check
CTEST(regex_syntax, balanced_check)
{
    // Balanced patterns
    ASSERT_TRUE(rift_regex_syntax_check_balanced("a(b[c])d"));
    ASSERT_TRUE(rift_regex_syntax_check_balanced("{a}[b](c)"));

    // Unbalanced patterns
    ASSERT_FALSE(rift_regex_syntax_check_balanced("a(b[c]"));
    ASSERT_FALSE(rift_regex_syntax_check_balanced("a{b[c])"));
}

// Test tokenization
CTEST(regex_syntax, tokenization)
{
    int token_count = 0;
    ASSERT_TRUE(rift_regex_syntax_tokenize("a[bc]+d", test_token_callback, &token_count));
    ASSERT_TRUE(token_count > 0);
}

// Test diagnostics
CTEST(regex_syntax, diagnostics)
{
    rift_regex_syntax_context_t *context = rift_regex_syntax_context_create(0);
    ASSERT_NOT_NULL(context);

    char diagnostics[256] = {0};
    ASSERT_TRUE(
        rift_regex_syntax_get_diagnostics(context, "a[bc", diagnostics, sizeof(diagnostics)));
    ASSERT_TRUE(strlen(diagnostics) > 0);

    rift_regex_syntax_context_free(context);
}
