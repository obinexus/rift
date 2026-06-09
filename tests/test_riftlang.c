#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rift/riftlang.h"

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

    assert(result.success);
    pattern = rlang_execute_pattern(result.program, "number");
    assert(pattern != NULL);
    assert(strstr(pattern, "[0-9]") != NULL);
    free(pattern);

    engine = rift_pattern_engine_create(RIFT_MODE_CLASSICAL);
    assert(engine != NULL);
    added = rift_pattern_engine_add_pair(engine, "^yes$", "true", 1);
    assert(added);
    assert(rift_pattern_engine_compile(engine));
    output = rift_pattern_engine_match(engine, "yes", &output_length, NULL);
    assert(output != NULL && strcmp(output, "true") == 0);
    free(output);
    rift_pattern_engine_destroy(engine);

    rlang_program_free(result.program);
    rlang_context_free(context);
    puts("test_riftlang: PASS");
    return 0;
}
