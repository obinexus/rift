#include <stdio.h>
#include <string.h>
#include "rift/cli.h"
#include "rift/rift.h"
#include "rift/riftlang.h"

/* rift_compile accepts any text, so check the input against the RiftLang
 * grammar first and refuse what it rejects. Prints the diagnostics. */
static int input_is_valid(const char *input, int allow_unknown) {
    RLangContext *context = rlang_context_create();
    RLangCompileResult result;
    int valid;
    if (!context) {
        fprintf(stderr, "rift compile: out of memory\n");
        return 0;
    }
    rlang_context_set_allow_unknown(context, allow_unknown != 0);
    result = rlang_load_file(context, input);
    if (result.warnings) fputs(result.warnings, stderr);
    valid = result.success;
    if (!valid) {
        fprintf(stderr, "%s\n", result.error_message
                ? result.error_message : "rift compile: invalid input");
    }
    rlang_compile_result_free(&result);
    rlang_context_free(context);
    return valid;
}

int rift_cmd_compile(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        printf("Usage: rift compile [options] <input-file> [output-file]\n\n");
        printf("Compile a RIFT source file through the full pipeline.\n");
        printf("The input must be a valid RiftLang specification.\n\n");
        printf("Options:\n");
        printf("  --verbose        Show detailed compilation output\n");
        printf("  --allow-unknown  Warn about, instead of rejecting, top-level\n");
        printf("                   constructs other than pattern and let\n");
        printf("  --debug          Enable debug mode\n");
        printf("  --target <t>     Set output target (c, hexdump)\n");
        printf("  --help           Show this help message\n");
        return 0;
    }

    const char *input = NULL;
    const char *output = NULL;
    int verbose = 0;
    int allow_unknown = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "--allow-unknown") == 0) {
            allow_unknown = 1;
        } else if (!input) {
            input = argv[i];
        } else if (!output) {
            output = argv[i];
        }
    }

    if (!input) {
        fprintf(stderr, "rift compile: no input file specified\n");
        return 1;
    }

    if (verbose) {
        printf("rift compile: %s", input);
        if (output) printf(" -> %s", output);
        printf("\n");
    }

    if (!input_is_valid(input, allow_unknown)) return 1;

    rift_context_t *ctx = rift_init();
    if (!ctx) {
        fprintf(stderr, "rift compile: failed to initialize context\n");
        return 1;
    }

    ctx->verbose = verbose;
    rift_error_t err = rift_compile(ctx, input, output);
    if (err != RIFT_SUCCESS) {
        fprintf(stderr, "rift compile: %s\n", rift_error_string(err));
        rift_cleanup(ctx);
        return 1;
    }

    rift_cleanup(ctx);
    return 0;
}
