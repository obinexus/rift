#include <stdio.h>
#include <string.h>
#include "rift/cli.h"
#include "rift/bridge.h"

int rift_cmd_bridge(int argc, char **argv) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        printf("Usage: rift bridge [options]\n\n");
        printf("Polyglot interop bridge for cross-language dependency resolution.\n\n");
        printf("Options:\n");
        printf("  --init       Initialize trident topology\n");
        printf("  --consensus  Check 2/3 consensus agreement\n");
        printf("  --coherence  Validate row-col matrix coherence\n");
        printf("  --help       Show this help message\n");
        return 0;
    }

    rift_bridge_t *bridge = rift_bridge_create();
    if (!bridge) {
        fprintf(stderr, "rift bridge: failed to initialize\n");
        return 1;
    }

    int result = 0;
    if (strcmp(argv[1], "--init") == 0) {
        result = rift_bridge_trident_init(bridge);
    } else if (strcmp(argv[1], "--consensus") == 0) {
        result = rift_bridge_consensus(bridge);
    } else if (strcmp(argv[1], "--coherence") == 0) {
        result = rift_bridge_coherence(bridge);
    } else {
        fprintf(stderr, "rift bridge: unknown option '%s'\n", argv[1]);
        result = 1;
    }

    rift_bridge_destroy(bridge);
    return result;
}
