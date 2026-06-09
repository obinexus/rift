#include "rift/bridge.h"
#include <stdlib.h>
#include <string.h>

struct rift_bridge {
    rift_trident_node_t *nodes;
    int                  count;
    int                  capacity;
};

rift_bridge_t *rift_bridge_create(void) {
    rift_bridge_t *bridge = (rift_bridge_t *)calloc(1, sizeof(rift_bridge_t));
    if (!bridge) return NULL;
    bridge->capacity = 32;
    bridge->nodes = (rift_trident_node_t *)calloc(bridge->capacity,
                                                    sizeof(rift_trident_node_t));
    return bridge;
}

void rift_bridge_destroy(rift_bridge_t *bridge) {
    if (!bridge) return;
    free(bridge->nodes);
    free(bridge);
}

int rift_bridge_trident_init(rift_bridge_t *bridge) {
    if (!bridge) return -1;
    if (bridge->capacity < 3) return -1;
    memset(bridge->nodes, 0, 3 * sizeof(*bridge->nodes));
    strcpy(bridge->nodes[0].name, "rift-core");
    strcpy(bridge->nodes[1].name, "riftlang");
    strcpy(bridge->nodes[2].name, "rifttest");
    bridge->nodes[0].polarity = RIFT_POLARITY_POSITIVE;
    bridge->nodes[1].polarity = RIFT_POLARITY_POSITIVE;
    bridge->nodes[2].polarity = RIFT_POLARITY_EPSILON;
    bridge->nodes[0].binding = 1;
    bridge->nodes[1].binding = 1;
    bridge->nodes[2].binding = 1;
    bridge->count = 3;
    return 0;
}

int rift_bridge_consensus(rift_bridge_t *bridge) {
    if (!bridge) return -1;
    int agreeing = 0;
    int i;
    for (i = 0; i < bridge->count; i++) {
        if (bridge->nodes[i].binding) agreeing++;
    }
    return bridge->count > 0 && agreeing * 3 >= bridge->count * 2 ? 0 : 1;
}

int rift_bridge_coherence(rift_bridge_t *bridge) {
    if (!bridge) return -1;
    int i;
    for (i = 0; i < bridge->count; i++) {
        if (!bridge->nodes[i].name[0]) return 1;
    }
    return 0;
}
