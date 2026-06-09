#include <stdio.h>
#include <string.h>
#include "rift/cli.h"
#include "rift/rift.h"

int rift_cmd_compile(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        printf("Usage: rift compile [options] <input-file> [output-file]\n\n");
        printf("Compile a RIFT source file through the full pipeline.\n\n");
        printf("Options:\n");
        printf("  --verbose    Show detailed compilation output\n");
        printf("  --debug      Enable debug mode\n");
        printf("  --target <t> Set output target (c, hexdump)\n");
        printf("  --help       Show this help message\n");
        return 0;
    }

    const char *input = NULL;
    const char *output = NULL;
    int verbose = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
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
