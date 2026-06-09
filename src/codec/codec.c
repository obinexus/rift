#include "rift/codec.h"
#include <stdlib.h>
#include <string.h>

int rift_link(const char *source, size_t length, rift_cir_program_t *out) {
    if (!source || !out || length == 0) return -1;

    memset(out, 0, sizeof(rift_cir_program_t));

    /* TODO: single-pass forward linker
     * Memory-first ordering: SPAN before ASSIGN
     * No backtracking allowed */

    return 0;
}

int rift_codec_emit(const rift_cir_program_t *program, const char *target,
                     const char *output_path) {
    if (!program || !target) return -1;
    (void)output_path;

    if (strcmp(target, "c") == 0) {
        /* TODO: emit C target */
        return 0;
    } else if (strcmp(target, "hexdump") == 0) {
        /* TODO: emit hexdump canonical */
        return 0;
    }

    return -1;
}
