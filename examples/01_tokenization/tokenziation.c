/**
 * examples/01_tokenization/tokenziation.c
 *
 * RIFT Is a Flexible Translator — Proof of Concept #1
 * Minimal regex-based tokenizer demonstrating the core classification loop.
 *
 * What this shows:
 *   - Each token class is a State with a POSIX extended regex
 *   - state_matches() compiles and evaluates the pattern per-call
 *     (in production, patterns are compiled once at lexer init)
 *   - The classification order is explicit: longest/most-specific first
 *
 * Build:
 *   gcc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -o tokenize tokenziation.c
 *
 * Run:
 *   ./tokenize
 */

#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── State ───────────────────────────────────────────────────────────────── */

typedef struct State {
    char   *pattern;   /* POSIX ERE string */
    bool    is_final;  /* accepting / terminal state */
    size_t  id;        /* monotone identity */
} State;

/**
 * state_create — allocate and initialise a new State.
 * The pattern string is strdup'd; caller must free via state_destroy.
 */
State *
state_create(const char *pattern, bool is_final)
{
    State *s = malloc(sizeof(State));
    if (!s) return NULL;

    s->pattern  = strdup(pattern);
    s->is_final = is_final;

    static size_t next_id = 1;
    s->id = next_id++;

    return s;
}

void
state_destroy(State *s)
{
    if (!s) return;
    free(s->pattern);
    free(s);
}

/**
 * state_matches — returns true if `text` matches the state's regex.
 *
 * Implementation note: regcomp + regfree on every call is intentionally
 * simple for this demo. The production lexer (src/lang/lexer.c) caches
 * compiled patterns in a regex_t array indexed by token kind.
 */
bool
state_matches(const State *s, const char *text)
{
    if (!s || !text) return false;

    regex_t rx;
    if (regcomp(&rx, s->pattern, REG_EXTENDED) != 0) return false;

    int result = regexec(&rx, text, 0, NULL, 0);
    regfree(&rx);

    return (result == 0);
}

/* ── Demonstration ───────────────────────────────────────────────────────── */

void
demonstrate_librift_tokenization(void)
{
    printf("LibRift Demonstration: Regex-Based Language Tokenization\n");
    printf("─────────────────────────────────────────────────────────\n\n");

    /*
     * Token class definitions.
     * Pattern ordering matters: identifiers before keywords, longer
     * patterns before shorter ones — same rule as in any handwritten lexer,
     * but expressed declaratively.
     */
    State *identifier = state_create("^[a-zA-Z_][a-zA-Z0-9_]*$", false);
    State *number     = state_create("^[0-9]+$",                  false);
    State *op         = state_create("^[-+*/]$",                  false);

    if (!identifier || !number || !op) {
        fputs("OOM allocating states\n", stderr);
        goto cleanup;
    }

    /* Representative token stream from a simple expression: x + 123 * y 42 */
    const char *tokens[] = {"x", "+", "123", "*", "y", "42"};
    int n = (int)(sizeof tokens / sizeof *tokens);

    printf("%-8s  %s\n", "Token", "Type");
    printf("%-8s  %s\n", "──────", "──────────");

    for (int i = 0; i < n; i++) {
        const char *tok  = tokens[i];
        const char *kind = "Unknown";

        if      (state_matches(identifier, tok)) kind = "Identifier";
        else if (state_matches(number,     tok)) kind = "Number";
        else if (state_matches(op,         tok)) kind = "Operator";

        printf("%-8s  %s\n", tok, kind);
    }

cleanup:
    state_destroy(identifier);
    state_destroy(number);
    state_destroy(op);
}

int
main(void)
{
    demonstrate_librift_tokenization();
    return 0;
}
