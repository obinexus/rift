#include <stdio.h>
#include <string.h>
#include "rift/cli.h"
#include "rift/version.h"

static const rift_command_t commands[] = {
    { "bridge",   "Polyglot interop bridge",         rift_cmd_bridge   },
    { "test",     "Run test suite",                   rift_cmd_test     },
    { "lang",     "Pattern matching engine",          rift_cmd_lang     },
    { "man",      "Display manual pages",             rift_cmd_man      },
    { "doctor",   "Run diagnostics",                  rift_cmd_doctor   },
    { "raf",      "Regulation as firmware",            rift_cmd_raf      },
    { "compile",  "Compile source file",               rift_cmd_compile  },
    { "tokenize", "Tokenize input file",               rift_cmd_tokenize },
    { "parse",    "Parse to AST",                      rift_cmd_parse    },
    { "emit",     "Emit target code",                  rift_cmd_emit     },
    { NULL, NULL, NULL }
};

const rift_command_t *rift_cli_find_command(const char *name) {
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(commands[i].name, name) == 0) {
            return &commands[i];
        }
    }
    return NULL;
}

void rift_cli_print_usage(void) {
    printf("%s v%s\n\n", RIFT_NAME, RIFT_VERSION_STRING);
    printf("Usage: rift <command> [options] [files]\n\n");
    printf("Commands:\n");
    for (int i = 0; commands[i].name != NULL; i++) {
        printf("  %-12s %s\n", commands[i].name, commands[i].description);
    }
    printf("\nOptions:\n");
    printf("  --help, -h     Show this help message\n");
    printf("  --version, -v  Show version information\n");
}

void rift_cli_print_version(void) {
    printf("rift %s\n", RIFT_VERSION_STRING);
}
