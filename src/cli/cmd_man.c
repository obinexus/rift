#include <stdio.h>
#include <string.h>
#include "rift/cli.h"
#include "rift/version.h"

int rift_cmd_man(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "--help") != 0) {
        printf("RIFT MANUAL: %s\n\n", argv[1]);
        printf("No manual entry for '%s' yet.\n", argv[1]);
        return 0;
    }

    printf("%s v%s\n", RIFT_NAME, RIFT_VERSION_STRING);
    printf("Manual Pages\n\n");
    printf("Available topics:\n");
    printf("  rift man bridge     Polyglot interop bridge\n");
    printf("  rift man lang       Pattern matching engine\n");
    printf("  rift man compile    Compilation pipeline\n");
    printf("  rift man tokenize   Tokenization\n");
    printf("  rift man parse      Parsing\n");
    printf("  rift man emit       Code emission\n");
    printf("  rift man raf        Regulation as firmware\n");
    printf("  rift man doctor     Diagnostics\n");
    return 0;
}
