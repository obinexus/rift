/*
 * Allocation-failure injection for the lexer and token triplet.
 *
 * Linked with -Wl,--wrap=malloc,--wrap=posix_memalign so that every
 * allocation made by librift's lexer and rift_token_create goes through the
 * wrappers below. For each k, the k-th allocation fails; every token the
 * lexer returns must then be either valid with the expected value, or the
 * invalid error token with rift_lexer_has_error() set. A "valid" token
 * whose value was lost must never appear.
 */
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "rift_check.h"
#include "rift/lexer.h"

void *__real_malloc(size_t size);
int __real_posix_memalign(void **ptr, size_t alignment, size_t size);
void *__wrap_malloc(size_t size);
int __wrap_posix_memalign(void **ptr, size_t alignment, size_t size);

static long allocation_count;
static long fail_at; /* 1-based index of the allocation to fail; 0 = none */

static int next_allocation_fails(void) {
    allocation_count++;
    return fail_at > 0 && allocation_count == fail_at;
}

void *__wrap_malloc(size_t size) {
    if (next_allocation_fails()) return NULL;
    return __real_malloc(size);
}

int __wrap_posix_memalign(void **ptr, size_t alignment, size_t size) {
    if (next_allocation_fails()) return ENOMEM;
    return __real_posix_memalign(ptr, alignment, size);
}

/* One token of every kind that carries a value. */
static const char SOURCE[] = "let x = R\"ab\" + 4.2 \"str\" ;";

typedef struct expected_token {
    rift_token_type_t type;
    const char *value;
} expected_token_t;

static const expected_token_t EXPECTED[] = {
    { RIFT_TOKEN_KEYWORD, "let" },
    { RIFT_TOKEN_IDENTIFIER, "x" },
    { RIFT_TOKEN_OPERATOR, "=" },
    { RIFT_TOKEN_PATTERN_STATIC, "ab" },
    { RIFT_TOKEN_OPERATOR, "+" },
    { RIFT_TOKEN_LITERAL_FLOAT, "4.2" },
    { RIFT_TOKEN_LITERAL_STRING, "str" },
    { RIFT_TOKEN_DELIMITER, ";" },
    { RIFT_TOKEN_EOF, NULL }
};
#define EXPECTED_COUNT (sizeof(EXPECTED) / sizeof(EXPECTED[0]))

/* Lexes SOURCE with the current fail_at. Returns the number of valid
 * tokens before the failure (or EXPECTED_COUNT when nothing failed). */
static size_t lex_all(int *failed) {
    rift_lexer_t *lexer;
    size_t index = 0;
    size_t valid = 0;
    int saw_error = 0;

    allocation_count = 0;
    lexer = rift_lexer_create(SOURCE, sizeof(SOURCE) - 1);
    if (!lexer) {
        /* The lexer itself could not be allocated: a reported failure. */
        *failed = 1;
        return 0;
    }
    for (;;) {
        rift_token_t token = rift_lexer_next(lexer);
        RIFT_CHECK(index < EXPECTED_COUNT + 1);
        if (rift_token_validate(&token)) {
            /* A valid token must hold exactly the expected value. */
            RIFT_CHECK(!saw_error || token.type == RIFT_TOKEN_EOF);
            if (!saw_error) {
                RIFT_CHECK(index < EXPECTED_COUNT);
                RIFT_CHECK(token.type == EXPECTED[index].type);
                if (EXPECTED[index].value) {
                    RIFT_CHECK(token.value.str != NULL);
                    RIFT_CHECK(strcmp(token.value.str, EXPECTED[index].value) == 0);
                } else {
                    RIFT_CHECK(token.value.str == NULL);
                }
                valid++;
            }
        } else {
            /* An invalid token is only allowed as the reported failure. */
            RIFT_CHECK(rift_lexer_has_error(lexer));
            RIFT_CHECK(!saw_error);
            RIFT_CHECK(token.value.str == NULL);
            saw_error = 1;
        }
        index++;
        if (token.type == RIFT_TOKEN_EOF) {
            rift_token_destroy(&token);
            break;
        }
        rift_token_destroy(&token);
    }
    RIFT_CHECK(saw_error == rift_lexer_has_error(lexer));
    RIFT_CHECK(rift_lexer_eof(lexer));
    rift_lexer_destroy(lexer);
    *failed = saw_error;
    return valid;
}

int main(void) {
    long total;
    long k;
    int failed;

    /* Reference run: no failures. */
    fail_at = 0;
    RIFT_CHECK(lex_all(&failed) == EXPECTED_COUNT);
    RIFT_CHECK(!failed);
    total = allocation_count;
    RIFT_CHECK(total > (long)EXPECTED_COUNT);

    for (k = 1; k <= total; k++) {
        size_t valid;
        fail_at = k;
        valid = lex_all(&failed);
        if (!failed) {
            fprintf(stderr, "allocation %ld of %ld failed unreported\n", k, total);
        }
        RIFT_CHECK(failed);
        RIFT_CHECK(valid < EXPECTED_COUNT);
    }

    printf("test_lexer_oom: PASS (%ld allocation sites)\n", total);
    return 0;
}
