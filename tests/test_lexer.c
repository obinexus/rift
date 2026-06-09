#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "rift/lexer.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) static void name(void)
#define RUN(name) do { \
    printf("  %-40s", #name); \
    name(); \
    printf("PASS\n"); \
    tests_passed++; \
} while(0)

TEST(test_lexer_keywords) {
    const char *src = "let x = 42";
    rift_lexer_t *lex = rift_lexer_create(src, strlen(src));
    assert(lex != NULL);

    rift_token_t t1 = rift_lexer_next(lex);
    assert(t1.type == RIFT_TOKEN_KEYWORD);
    rift_token_destroy(&t1);

    rift_token_t t2 = rift_lexer_next(lex);
    assert(t2.type == RIFT_TOKEN_IDENTIFIER);
    rift_token_destroy(&t2);

    rift_token_t t3 = rift_lexer_next(lex);
    assert(t3.type == RIFT_TOKEN_OPERATOR);
    rift_token_destroy(&t3);

    rift_token_t t4 = rift_lexer_next(lex);
    assert(t4.type == RIFT_TOKEN_LITERAL_INT);
    rift_token_destroy(&t4);

    rift_lexer_destroy(lex);
}

TEST(test_lexer_string_literal) {
    const char *src = "\"hello world\"";
    rift_lexer_t *lex = rift_lexer_create(src, strlen(src));
    rift_token_t t = rift_lexer_next(lex);
    assert(t.type == RIFT_TOKEN_LITERAL_STRING);
    assert(strcmp(t.value.str, "hello world") == 0);
    rift_token_destroy(&t);
    rift_lexer_destroy(lex);
}

TEST(test_lexer_eof) {
    const char *src = "";
    rift_lexer_t *lex = rift_lexer_create(src, 0);
    assert(rift_lexer_eof(lex));
    rift_token_t t = rift_lexer_next(lex);
    assert(t.type == RIFT_TOKEN_EOF);
    rift_token_destroy(&t);
    rift_lexer_destroy(lex);
}

int main(void) {
    printf("test_lexer:\n");
    RUN(test_lexer_keywords);
    RUN(test_lexer_string_literal);
    RUN(test_lexer_eof);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed;
}
