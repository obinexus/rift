#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/regex/syntax/lexer.h"

// Simple test framework
#define TEST(name) static bool test_##name(void)
#define RUN_TEST(name)                                                                             \
    do {                                                                                           \
        printf("Running test: %s ... ", #name);                                                    \
        bool result = test_##name();                                                               \
        if (result) {                                                                              \
            printf("PASS\n");                                                                      \
            passed_tests++;                                                                        \
        } else {                                                                                   \
            printf("FAIL\n");                                                                      \
            failed_tests++;                                                                        \
        }                                                                                          \
        total_tests++;                                                                             \
    } while (0)

// Test cases

TEST(lexer_init)
{
    Lexer *lexer = lexer_init("abc");
    bool result = (lexer != NULL);
    lexer_free(lexer);
    return result;
}

TEST(lexer_empty_input)
{
    Lexer *lexer = lexer_init("");
    Token token = lexer_next_token(lexer);
    bool result = (token.type == TOKEN_EOF);
    lexer_free(lexer);
    return result;
}

TEST(lexer_character_literal)
{
    Lexer *lexer = lexer_init("a");
    Token token = lexer_next_token(lexer);
    bool result = (token.type == TOKEN_CHAR && token.value.c == 'a');
    lexer_free(lexer);
    return result;
}

TEST(lexer_multiple_characters)
{
    Lexer *lexer = lexer_init("abc");

    Token token1 = lexer_next_token(lexer);
    bool result = (token1.type == TOKEN_CHAR && token1.value.c == 'a');

    Token token2 = lexer_next_token(lexer);
    result = result && (token2.type == TOKEN_CHAR && token2.value.c == 'b');

    Token token3 = lexer_next_token(lexer);
    result = result && (token3.type == TOKEN_CHAR && token3.value.c == 'c');

    Token token4 = lexer_next_token(lexer);
    result = result && (token4.type == TOKEN_EOF);

    lexer_free(lexer);
    return result;
}

TEST(lexer_operators)
{
    Lexer *lexer = lexer_init("*+?|");

    Token token1 = lexer_next_token(lexer);
    bool result = (token1.type == TOKEN_STAR);

    Token token2 = lexer_next_token(lexer);
    result = result && (token2.type == TOKEN_PLUS);

    Token token3 = lexer_next_token(lexer);
    result = result && (token3.type == TOKEN_QUESTION);

    Token token4 = lexer_next_token(lexer);
    result = result && (token4.type == TOKEN_PIPE);

    lexer_free(lexer);
    return result;
}

TEST(lexer_parentheses)
{
    Lexer *lexer = lexer_init("()");

    Token token1 = lexer_next_token(lexer);
    bool result = (token1.type == TOKEN_LPAREN);

    Token token2 = lexer_next_token(lexer);
    result = result && (token2.type == TOKEN_RPAREN);

    lexer_free(lexer);
    return result;
}

TEST(lexer_brackets)
{
    Lexer *lexer = lexer_init("[]");

    Token token1 = lexer_next_token(lexer);
    bool result = (token1.type == TOKEN_LBRACKET);

    Token token2 = lexer_next_token(lexer);
    result = result && (token2.type == TOKEN_RBRACKET);

    lexer_free(lexer);
    return result;
}

TEST(lexer_escaped_characters)
{
    Lexer *lexer = lexer_init("\\*\\+\\?\\|\\(\\)\\[\\]\\\\");

    // All should be recognized as character literals
    for (char c : {'*', '+', '?', '|', '(', ')', '[', ']', '\\'}) {
        Token token = lexer_next_token(lexer);
        if (!(token.type == TOKEN_CHAR && token.value.c == c)) {
            lexer_free(lexer);
            return false;
        }
    }

    lexer_free(lexer);
    return true;
}

TEST(lexer_dot)
{
    Lexer *lexer = lexer_init(".");
    Token token = lexer_next_token(lexer);
    bool result = (token.type == TOKEN_DOT);
    lexer_free(lexer);
    return result;
}

TEST(lexer_complex_expression)
{
    // Test a more complex regex pattern
    Lexer *lexer = lexer_init("a(b|c)*d+e?");

    // Expected token sequence
    int expected_types[] = {
        TOKEN_CHAR,     // 'a'
        TOKEN_LPAREN,   // '('
        TOKEN_CHAR,     // 'b'
        TOKEN_PIPE,     // '|'
        TOKEN_CHAR,     // 'c'
        TOKEN_RPAREN,   // ')'
        TOKEN_STAR,     // '*'
        TOKEN_CHAR,     // 'd'
        TOKEN_PLUS,     // '+'
        TOKEN_CHAR,     // 'e'
        TOKEN_QUESTION, // '?'
        TOKEN_EOF       // end of input
    };

    char expected_chars[] = {'a', 0, 'b', 0, 'c', 0, 0, 'd', 0, 'e', 0, 0};

    bool result = true;
    for (int i = 0; i < sizeof(expected_types) / sizeof(expected_types[0]); i++) {
        Token token = lexer_next_token(lexer);
        result = result && (token.type == expected_types[i]);
        if (token.type == TOKEN_CHAR) {
            result = result && (token.value.c == expected_chars[i]);
        }
    }

    lexer_free(lexer);
    return result;
}

TEST(lexer_error_handling)
{
    // Test error handling for unclosed constructs or invalid escape sequences
    // This depends on how your lexer handles errors
    Lexer *lexer = lexer_init("(abc"); // Unclosed parenthesis

    // Skip all valid tokens
    while (lexer_next_token(lexer).type != TOKEN_EOF && !lexer_has_error(lexer)) {
    }

    bool result = lexer_has_error(lexer);
    lexer_free(lexer);
    return result;
}

int
main(void)
{
    int total_tests = 0;
    int passed_tests = 0;
    int failed_tests = 0;

    printf("Running lexer tests...\n");

    RUN_TEST(lexer_init);
    RUN_TEST(lexer_empty_input);
    RUN_TEST(lexer_character_literal);
    RUN_TEST(lexer_multiple_characters);
    RUN_TEST(lexer_operators);
    RUN_TEST(lexer_parentheses);
    RUN_TEST(lexer_brackets);
    RUN_TEST(lexer_escaped_characters);
    RUN_TEST(lexer_dot);
    RUN_TEST(lexer_complex_expression);
    RUN_TEST(lexer_error_handling);

    printf("\nTest summary: %d tests, %d passed, %d failed\n", total_tests, passed_tests,
           failed_tests);

    return (failed_tests == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}