#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Include the library headers
#include "core/regex/parser/token.h"

// Define the ctest macros
#define CTEST_MAIN
#define CTEST_COLOR_OUTPUT
#include "ctest.h"

// Test creation of different token types
CTEST(token_suite, create_char_token)
{
    token_t *token = token_create(TOKEN_TYPE_CHAR, 'a');
    ASSERT_NOT_NULL(token);
    ASSERT_EQUAL(TOKEN_TYPE_CHAR, token->type);
    ASSERT_EQUAL('a', token->value);
    token_destroy(token);
}

CTEST(token_suite, create_special_tokens)
{
    token_t *star = token_create(TOKEN_TYPE_STAR, 0);
    ASSERT_NOT_NULL(star);
    ASSERT_EQUAL(TOKEN_TYPE_STAR, star->type);
    token_destroy(star);

    token_t *plus = token_create(TOKEN_TYPE_PLUS, 0);
    ASSERT_NOT_NULL(plus);
    ASSERT_EQUAL(TOKEN_TYPE_PLUS, plus->type);
    token_destroy(plus);
}

CTEST(token_suite, token_equals)
{
    token_t *token1 = token_create(TOKEN_TYPE_CHAR, 'a');
    token_t *token2 = token_create(TOKEN_TYPE_CHAR, 'a');
    token_t *token3 = token_create(TOKEN_TYPE_CHAR, 'b');

    ASSERT_TRUE(token_equals(token1, token2));
    ASSERT_FALSE(token_equals(token1, token3));

    token_destroy(token1);
    token_destroy(token2);
    token_destroy(token3);
}

CTEST(token_list_suite, create_and_manipulate_list)
{
    token_list_t *list = token_list_create();

    ASSERT_NOT_NULL(list);
    ASSERT_EQUAL(0, token_list_size(list));

    token_t *token1 = token_create(TOKEN_TYPE_CHAR, 'a');
    token_list_add(list, token1);
    ASSERT_EQUAL(1, token_list_size(list));

    token_t *get1 = token_list_get(list, 0);
    ASSERT_EQUAL(TOKEN_TYPE_CHAR, get1->type);
    ASSERT_EQUAL('a', get1->value);

    token_list_destroy(list);
}
