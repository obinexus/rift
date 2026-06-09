#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "rift/token.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) static void name(void)
#define RUN(name) do { \
    printf("  %-40s", #name); \
    name(); \
    printf("PASS\n"); \
    tests_passed++; \
} while(0)

TEST(test_token_create) {
    rift_token_t t = rift_token_create(RIFT_TOKEN_IDENTIFIER, "hello");
    assert(t.type == RIFT_TOKEN_IDENTIFIER);
    assert(t.value.str != NULL);
    assert(strcmp(t.value.str, "hello") == 0);
    assert(rift_token_validate(&t));
    rift_token_destroy(&t);
}

TEST(test_token_create_null_value) {
    rift_token_t t = rift_token_create(RIFT_TOKEN_EOF, NULL);
    assert(t.type == RIFT_TOKEN_EOF);
    rift_token_destroy(&t);
}

TEST(test_token_type_name) {
    assert(strcmp(rift_token_type_name(RIFT_TOKEN_KEYWORD), "KEYWORD") == 0);
    assert(strcmp(rift_token_type_name(RIFT_TOKEN_PATTERN_STATIC), "PATTERN_STATIC") == 0);
    assert(strcmp(rift_token_type_name(RIFT_TOKEN_HEXDUMP), "HEXDUMP") == 0);
}

TEST(test_token_triplet_validation) {
    rift_token_t t = rift_token_create(RIFT_TOKEN_LITERAL_STRING, "test");
    assert(rift_token_validate(&t) == 1);
    rift_token_destroy(&t);

    rift_token_t invalid;
    memset(&invalid, 0, sizeof(invalid));
    invalid.type = RIFT_TOKEN_UNKNOWN;
    assert(rift_token_validate(&invalid) == 0);
}

int main(void) {
    printf("test_token:\n");
    RUN(test_token_create);
    RUN(test_token_create_null_value);
    RUN(test_token_type_name);
    RUN(test_token_triplet_validation);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed;
}
