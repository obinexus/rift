#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Include the library headers
#include "librift/parser/token.h"
#include "librift/parser/token_type.h"
#include "librift/parser/tokenizer.h"

// Define the ctest macros
#define CTEST_MAIN
#define CTEST_COLOR_OUTPUT
#include "ctest.h"

// Test creation and destruction of tokenizer
CTEST(tokenizer_suite, create_destroy_tokenizer)
{
    const char *pattern = "a+b*c?";
    rift_regex_tokenizer_t *tokenizer = rift_regex_tokenizer_create(pattern);
    ASSERT_NOT_NULL(tokenizer);
    rift_regex_tokenizer_free(tokenizer);
}

// Test handling of NULL input
CTEST(tokenizer_suite, null_input)
{
    rift_regex_tokenizer_t *tokenizer = rift_regex_tokenizer_create(NULL);
    ASSERT_NULL(tokenizer);
}

// Test basic tokenization
CTEST(tokenizer_suite, basic_tokenization)
{
    const char *pattern = "abc";
    rift_regex_tokenizer_t *tokenizer = rift_regex_tokenizer_create(pattern);
    ASSERT_NOT_NULL(tokenizer);

    // Get first token
    rift_regex_token_t token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('a', token.value.ch);

    // Get second token
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('b', token.value.ch);

    // Get third token
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('c', token.value.ch);

    // Should be at end
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_EOF, token.type);

    rift_regex_tokenizer_free(tokenizer);
}

// Test special operators
CTEST(tokenizer_suite, special_operators)
{
    const char *pattern = "a+b*c?";
    rift_regex_tokenizer_t *tokenizer = rift_regex_tokenizer_create(pattern);
    ASSERT_NOT_NULL(tokenizer);

    // 'a'
    rift_regex_token_t token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('a', token.value.ch);

    // '+'
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_PLUS, token.type);

    // 'b'
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('b', token.value.ch);

    // '*'
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_STAR, token.type);

    // 'c'
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('c', token.value.ch);

    // '?'
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_QUESTION, token.type);

    rift_regex_tokenizer_free(tokenizer);
}

// Test parentheses and grouping
CTEST(tokenizer_suite, parentheses)
{
    const char *pattern = "(ab|cd)";
    rift_regex_tokenizer_t *tokenizer = rift_regex_tokenizer_create(pattern);
    ASSERT_NOT_NULL(tokenizer);

    // '('
    rift_regex_token_t token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_LPAREN, token.type);

    // 'a'
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('a', token.value.ch);

    // 'b'
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('b', token.value.ch);

    // '|'
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_PIPE, token.type);

    // 'c'
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('c', token.value.ch);

    // 'd'
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('d', token.value.ch);

    // ')'
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_RPAREN, token.type);

    rift_regex_tokenizer_free(tokenizer);
}

// Test character classes
CTEST(tokenizer_suite, character_classes)
{
    const char *pattern = "[a-z]";
    rift_regex_tokenizer_t *tokenizer = rift_regex_tokenizer_create(pattern);
    ASSERT_NOT_NULL(tokenizer);

    // '['
    rift_regex_token_t token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_LBRACKET, token.type);

    // Character class elements would continue here...

    rift_regex_tokenizer_free(tokenizer);
}

// Test escape sequences
CTEST(tokenizer_suite, escape_sequences)
{
    const char *pattern = "\\d\\w\\s";
    rift_regex_tokenizer_t *tokenizer = rift_regex_tokenizer_create(pattern);
    ASSERT_NOT_NULL(tokenizer);

    // \d token
    rift_regex_token_t token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_DIGIT_CLASS, token.type);

    // \w token
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_WORD_CLASS, token.type);

    // \s token
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_WHITESPACE_CLASS, token.type);

    rift_regex_tokenizer_free(tokenizer);
}

// Test peeking
CTEST(tokenizer_suite, peek_token)
{
    const char *pattern = "ab";
    rift_regex_tokenizer_t *tokenizer = rift_regex_tokenizer_create(pattern);
    ASSERT_NOT_NULL(tokenizer);

    // Peek first token
    rift_regex_token_t token = rift_regex_tokenizer_peek_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('a', token.value.ch);

    // Peek again (should be the same)
    token = rift_regex_tokenizer_peek_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('a', token.value.ch);

    // Get first token
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('a', token.value.ch);

    // Peek next token
    token = rift_regex_tokenizer_peek_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('b', token.value.ch);

    rift_regex_tokenizer_free(tokenizer);
}

// Test reset functionality
CTEST(tokenizer_suite, reset_tokenizer)
{
    const char *pattern = "abc";
    rift_regex_tokenizer_t *tokenizer = rift_regex_tokenizer_create(pattern);
    ASSERT_NOT_NULL(tokenizer);

    // Get first token
    rift_regex_token_t token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('a', token.value.ch);

    // Reset tokenizer
    rift_regex_tokenizer_reset(tokenizer);

    // Get first token again
    token = rift_regex_tokenizer_next_token(tokenizer);
    ASSERT_EQUAL(RIFT_REGEX_TOKEN_CHAR, token.type);
    ASSERT_EQUAL('a', token.value.ch);

    rift_regex_tokenizer_free(tokenizer);
}

// Main function to run the tests
int
main(int argc, const char *argv[])
{
    return ctest_main(argc, argv);
}
