#include <stdio.h>
#include <string.h>
#include "rift/cli.h"
#include "rift/emitter.h"

int rift_cmd_emit(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        printf("Usage: rift emit [options] <input-file> [output-file]\n\n");
        printf("Emit target code from compiled IR.\n\n");
        printf("Options:\n");
        printf("  --target <t>  Output target (c, hexdump)\n");
        printf("  --help        Show this help message\n");
        return 0;
    }

    printf("rift emit: emitter ready for '%s'\n", argv[1]);
    return 0;
}
