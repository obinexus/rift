#include <stdio.h>
#include <string.h>
#include "rift/cli.h"
#include "rift/version.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        rift_cli_print_usage();
        return 0;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        rift_cli_print_usage();
        return 0;
    }

    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        rift_cli_print_version();
        return 0;
    }

    const rift_command_t *cmd = rift_cli_find_command(argv[1]);
    if (!cmd) {
        fprintf(stderr, "rift: unknown command '%s'\n", argv[1]);
        fprintf(stderr, "Run 'rift --help' for usage.\n");
        return 1;
    }

    return cmd->handler(argc - 1, argv + 1);
}
