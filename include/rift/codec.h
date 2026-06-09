#ifndef RIFT_CODEC_H
#define RIFT_CODEC_H

#include <stddef.h>
#include <stdint.h>
#include "rift/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Canonical Intermediate Representation (CIR) */

typedef enum rift_cir_kind {
    RIFT_CIR_GOVERN,
    RIFT_CIR_SPAN,
    RIFT_CIR_TYPE_DEF,
    RIFT_CIR_ASSIGN,
    RIFT_CIR_POLICY,
    RIFT_CIR_WHILE,
    RIFT_CIR_IF,
    RIFT_CIR_VALIDATE,
    RIFT_CIR_RETURN,
    RIFT_CIR_CALL,
    RIFT_CIR_BLOCK
} rift_cir_kind_t;

#define RIFT_CIR_MAX_STRINGS 256
#define RIFT_CIR_MAX_NODES   1024
#define RIFT_CIR_MAX_VARS    64

typedef struct rift_cir_node {
    rift_cir_kind_t kind;
    char            name[64];
    char            value[256];
    int             child_start;
    int             child_count;
} rift_cir_node_t;

typedef struct rift_cir_program {
    rift_cir_node_t nodes[RIFT_CIR_MAX_NODES];
    int             node_count;
    char            strings[RIFT_CIR_MAX_STRINGS][256];
    int             string_count;
} rift_cir_program_t;

/*
 * Two-phase codec:
 *   Phase 1 (LINK): single-pass forward linker -> CIR program
 *   Phase 2 (CODEC): CIR -> target output
 * Memory-first ordering: SPAN before ASSIGN
 */
RIFT_API int rift_link(const char *source, size_t length, rift_cir_program_t *out);
RIFT_API int rift_codec_emit(const rift_cir_program_t *program, const char *target,
                              const char *output_path);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CODEC_H */
