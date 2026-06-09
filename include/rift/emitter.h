#ifndef RIFT_EMITTER_H
#define RIFT_EMITTER_H

#include "rift/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum rift_emit_target {
    RIFT_EMIT_C,
    RIFT_EMIT_HEXDUMP
} rift_emit_target_t;

typedef struct rift_emitter rift_emitter_t;

RIFT_API rift_emitter_t *rift_emitter_create(rift_emit_target_t target);
RIFT_API void            rift_emitter_destroy(rift_emitter_t *emitter);
RIFT_API int             rift_emitter_emit(rift_emitter_t *emitter,
                                           const void *ir,
                                           const char *output_path);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_EMITTER_H */
