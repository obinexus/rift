#include <stdio.h>
#include <string.h>
#include "rift/cli.h"

int rift_cmd_emit(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: rift emit [options] <input-file> [output-file]\n\n");
        printf("Emit target code from compiled IR.\n");
        printf("Not implemented yet: exits with status %d.\n\n",
               RIFT_CLI_EXIT_NOT_IMPLEMENTED);
        printf("Options:\n");
        printf("  --target <t>  Output target (c, hexdump)\n");
        printf("  --help        Show this help message\n");
        return 0;
    }

    fprintf(stderr, "rift emit: not implemented\n");
    return RIFT_CLI_EXIT_NOT_IMPLEMENTED;
}
