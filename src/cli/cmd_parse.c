#include <stdio.h>
#include <string.h>
#include "rift/cli.h"

int rift_cmd_parse(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: rift parse [options] <input-file>\n\n");
        printf("Parse a RIFT source file and output the AST.\n");
        printf("Not implemented yet: exits with status %d.\n\n",
               RIFT_CLI_EXIT_NOT_IMPLEMENTED);
        printf("Options:\n");
        printf("  --format <f>  Output format (text, json)\n");
        printf("  --help        Show this help message\n");
        return 0;
    }

    fprintf(stderr, "rift parse: not implemented\n");
    return RIFT_CLI_EXIT_NOT_IMPLEMENTED;
}
