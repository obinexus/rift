#include "rift/riftlang.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage(void) {
    puts("Usage: riftlang [options] <input.rift>");
    puts("Options:");
    puts("  -o <file>          Write generated regex or C source");
    puts("  -p, --pattern <n>  Select a named pattern");
    puts("  --validate         Validate without emitting");
    puts("  --allow-unknown    Warn about, instead of rejecting, top-level");
    puts("                     constructs other than pattern and let");
    puts("  --version          Print version");
}

static int has_suffix(const char *value, const char *suffix) {
    size_t value_length = strlen(value);
    size_t suffix_length = strlen(suffix);
    return value_length >= suffix_length &&
        strcmp(value + value_length - suffix_length, suffix) == 0;
}

int main(int argc, char **argv) {
    const char *input = NULL;
    const char *output = NULL;
    const char *selected_pattern = NULL;
    int validate_only = 0;
    int allow_unknown = 0;
    int i;
    RLangContext *context;
    RLangCompileResult result;
    const char **patterns = NULL;
    size_t pattern_count = 0;
    char *regex;
    FILE *stream;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 0;
        }
        if (strcmp(argv[i], "--version") == 0) {
            puts("riftlang 5.0.0");
            return 0;
        }
        if (strcmp(argv[i], "--validate") == 0) {
            validate_only = 1;
        } else if (strcmp(argv[i], "--allow-unknown") == 0) {
            allow_unknown = 1;
        } else if ((strcmp(argv[i], "-o") == 0) && i + 1 < argc) {
            output = argv[++i];
        } else if ((strcmp(argv[i], "-p") == 0 ||
                    strcmp(argv[i], "--pattern") == 0) && i + 1 < argc) {
            selected_pattern = argv[++i];
        } else if (!input) {
            input = argv[i];
        } else {
            fprintf(stderr, "riftlang: unexpected argument '%s'\n", argv[i]);
            return 2;
        }
    }
    if (!input) {
        print_usage();
        return 2;
    }

    context = rlang_context_create();
    if (!context) return 1;
    rlang_context_set_allow_unknown(context, allow_unknown != 0);
    result = rlang_load_file(context, input);
    if (result.warnings) fputs(result.warnings, stderr);
    if (!result.success) {
        /* Diagnostics already carry "<input>:<line>:<column>: error:". */
        fprintf(stderr, "%s\n",
                result.error_message ? result.error_message
                                     : "riftlang: compilation failed");
        rlang_compile_result_free(&result);
        rlang_context_free(context);
        return 1;
    }
    if (validate_only) {
        printf("%s: valid\n", input);
        rlang_compile_result_free(&result);
        rlang_context_free(context);
        return 0;
    }
    if (output && has_suffix(output, ".c")) {
        int ok = rlang_export_c(result.program, output);
        rlang_compile_result_free(&result);
        rlang_context_free(context);
        return ok ? 0 : 1;
    }
    if (!selected_pattern) {
        rlang_get_patterns(result.program, &patterns, &pattern_count);
        if (pattern_count) selected_pattern = patterns[0];
    }
    regex = selected_pattern
        ? rlang_execute_pattern(result.program, selected_pattern) : NULL;
    if (!regex) {
        fprintf(stderr, "riftlang: pattern not found\n");
        rlang_compile_result_free(&result);
        rlang_context_free(context);
        return 1;
    }
    stream = output ? fopen(output, "wb") : stdout;
    if (!stream) {
        free(regex);
        rlang_compile_result_free(&result);
        rlang_context_free(context);
        return 1;
    }
    fprintf(stream, "%s\n", regex);
    if (output) fclose(stream);
    free(regex);
    rlang_compile_result_free(&result);
    rlang_context_free(context);
    return 0;
}
