#include "rift_check.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rift/riftlang.h"

/* Compiles `source` and returns the regex for `name`; the caller frees it. */
static char *compile_pattern(const char *source, const char *name,
                             bool allow_unknown, char **warnings) {
    RLangContext *context = rlang_context_create();
    RLangCompileResult result;
    char *pattern;
    RIFT_CHECK(context != NULL);
    rlang_context_set_allow_unknown(context, allow_unknown);
    result = rlang_compile(context, source);
    if (!result.success) fprintf(stderr, "unexpected error: %s\n", result.error_message);
    RIFT_CHECK(result.success);
    pattern = rlang_execute_pattern(result.program, name);
    RIFT_CHECK(pattern != NULL);
    if (warnings) {
        *warnings = result.warnings;
        result.warnings = NULL;
    }
    rlang_compile_result_free(&result);
    rlang_context_free(context);
    return pattern;
}

/* Requires rlang_compile to reject `source` with exactly `expected`. */
static void expect_error(const char *source, const char *expected) {
    RLangContext *context = rlang_context_create();
    RLangCompileResult result;
    RIFT_CHECK(context != NULL);
    result = rlang_compile(context, source);
    RIFT_CHECK(!result.success);
    RIFT_CHECK(result.program == NULL);
    RIFT_CHECK(result.error_message != NULL);
    if (strcmp(result.error_message, expected) != 0) {
        fprintf(stderr, "expected: %s\nactual:   %s\n", expected, result.error_message);
    }
    RIFT_CHECK(strcmp(result.error_message, expected) == 0);
    rlang_compile_result_free(&result);
    rlang_context_free(context);
}

static void test_diagnostics(void) {
    /* A name that is never defined used to become the literal text "B". */
    expect_error("pattern A = B\n",
                 "1:13: error: undefined name 'B'");
    expect_error("pattern A = B\npattern B = \"b\"\n",
                 "1:13: error: use of 'B' before its definition at 2:9");
    /* Recursion used to be emitted as the literal text "EXPR". */
    expect_error("let NUM = r\"[0-9]+\"\npattern EXPR = NUM | \"(\" EXPR \")\"\n",
                 "2:26: error: recursive reference to 'EXPR'");
    expect_error("pattern A = \"a\" B\npattern B = \"b\" A\n",
                 "1:17: error: recursive reference to 'B' (A -> B -> A)");
    /* The last definition used to win silently. */
    expect_error("pattern X = \"a\"\nlet X = \"b\"\n",
                 "2:5: error: duplicate definition of 'X' (first defined at 1:9)");
    /* Unsupported lines used to be skipped silently. */
    expect_error("pattern X = \"x\"\n  fn f() { }\n",
                 "2:3: error: unsupported construct 'fn'");
    /* The earliest error in the source is reported. */
    expect_error("pattern A = Q\npattern A = \"a\"\n",
                 "1:13: error: undefined name 'Q'");
    expect_error("pattern X = \"x\" @ /[a-z]\n",
                 "1:19: error: unterminated regular expression");
    expect_error("// nothing\n", "error: no pattern declarations found");
}

static void test_allow_unknown(void) {
    char *warnings = NULL;
    char *pattern = compile_pattern(
        "package demo;\n"
        "pattern X = \"x\"\n"
        "type T {\n"
        "    pattern: X,\n"
        "    let inner = Y   // inside the skipped body, never compiled\n"
        "}\n"
        "pattern Z = X \"z\"\n",
        "Z", true, &warnings);
    RIFT_CHECK(strcmp(pattern, "(x)z") == 0);
    RIFT_CHECK(warnings != NULL);
    RIFT_CHECK(strcmp(warnings,
        "1:1: warning: unsupported construct 'package' ignored\n"
        "3:1: warning: unsupported construct 'type' ignored\n") == 0);
    free(warnings);
    free(pattern);
}

static void test_resolution(void) {
    RLangContext *context = rlang_context_create();
    RLangCompileResult result;
    char *pattern;

    /* Block expressions with local lets, as in examples/floyd. */
    pattern = compile_pattern(
        "let alpha = r\"[a-z]\"\n"
        "pattern word = {\n"
        "    let letters = alpha+\n"
        "    letters @ \"!\"\n"
        "}\n",
        "word", false, NULL);
    RIFT_CHECK(strcmp(pattern, "(([a-z])+)!") == 0);
    free(pattern);

    /* Context bindings resolve like earlier definitions. */
    RIFT_CHECK(rlang_set_binding(context, "digit", "[0-9]"));
    result = rlang_compile(context, "pattern pair = digit digit\n");
    RIFT_CHECK(result.success);
    RIFT_CHECK(result.warnings == NULL);
    pattern = rlang_execute_pattern(result.program, "pair");
    RIFT_CHECK(pattern != NULL && strcmp(pattern, "([0-9])([0-9])") == 0);
    free(pattern);
    rlang_compile_result_free(&result);
    rlang_context_free(context);
}

int main(void) {
    RLangContext *context = rlang_context_create();
    RLangCompileResult result = rlang_compile(context,
        "let digit = r\"[0-9]\"\n"
        "pattern number = digit+\n");
    char *pattern;
    RiftPatternEngine *engine;
    size_t output_length;
    char *output;
    bool added;

    RIFT_CHECK(result.success);
    pattern = rlang_execute_pattern(result.program, "number");
    RIFT_CHECK(pattern != NULL);
    RIFT_CHECK(strcmp(pattern, "([0-9])+") == 0);
    free(pattern);

    engine = rift_pattern_engine_create(RIFT_MODE_CLASSICAL);
    RIFT_CHECK(engine != NULL);
    added = rift_pattern_engine_add_pair(engine, "^yes$", "true", 1);
    RIFT_CHECK(added);
    RIFT_CHECK(rift_pattern_engine_compile(engine));
    output = rift_pattern_engine_match(engine, "yes", &output_length, NULL);
    RIFT_CHECK(output != NULL && strcmp(output, "true") == 0);
    free(output);
    rift_pattern_engine_destroy(engine);

    rlang_compile_result_free(&result);
    rlang_context_free(context);

    test_diagnostics();
    test_allow_unknown();
    test_resolution();
    puts("test_riftlang: PASS");
    return 0;
}
