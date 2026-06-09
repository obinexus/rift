#include "rift/codec.h"
#include <stdio.h>
#include <string.h>

int rift_emit_hexdump_target(const rift_cir_program_t *program, FILE *out) {
    if (!program || !out) return -1;

    fprintf(out, "/* RIFT Hexdump Canonical Form */\n");

    for (int i = 0; i < program->node_count; i++) {
        const rift_cir_node_t *node = &program->nodes[i];
        const unsigned char *bytes = (const unsigned char *)node->value;
        size_t len = strlen(node->value);

        fprintf(out, "%04x: ", i);
        for (size_t j = 0; j < len && j < 16; j++) {
            fprintf(out, "%02x ", bytes[j]);
        }
        fprintf(out, " |%s|\n", node->name);
    }

    return 0;
}
