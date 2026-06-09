#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rift/cli.h"
#include "rift/riftlang.h"

int rift_cmd_lang(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        printf("Usage: rift lang [options] <file>\n\n");
        printf("RIFTLang pattern matching engine.\n\n");
        printf("Options:\n");
        printf("  --pattern <expr>  Apply pattern expression\n");
        printf("  --static          Use R\"\" static mode\n");
        printf("  --dynamic         Use R'' dynamic mode\n");
        printf("  --help            Show this help message\n");
        return 0;
    }

    RLangContext *context = rlang_context_create();
    RLangCompileResult result;
    const char **patterns;
    size_t count;
    size_t i;
    if (!context) return 1;
    result = rlang_load_file(context, argv[1]);
    if (!result.success) {
        fprintf(stderr, "rift lang: %s\n",
                result.error_message ? result.error_message : "compile failure");
        rlang_compile_result_free(&result);
        rlang_context_free(context);
        return 1;
    }
    rlang_get_patterns(result.program, &patterns, &count);
    for (i = 0; i < count; i++) {
        char *value = rlang_execute_pattern(result.program, patterns[i]);
        printf("%s = %s\n", patterns[i], value ? value : "");
        free(value);
    }
    rlang_program_free(result.program);
    rlang_context_free(context);
    return 0;
}
