#include <stdio.h>
#include <string.h>
#include "rift/cli.h"
#include "rift/parser.h"

int rift_cmd_parse(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        printf("Usage: rift parse [options] <input-file>\n\n");
        printf("Parse a RIFT source file and output the AST.\n\n");
        printf("Options:\n");
        printf("  --format <f>  Output format (text, json)\n");
        printf("  --help        Show this help message\n");
        return 0;
    }

    printf("rift parse: parser ready for '%s'\n", argv[1]);
    return 0;
}
