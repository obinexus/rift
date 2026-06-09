#ifndef RIFT_BRIDGE_H
#define RIFT_BRIDGE_H

#include <stdint.h>
#include "rift/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Polarity system */
typedef enum rift_polarity {
    RIFT_POLARITY_POSITIVE = '+',
    RIFT_POLARITY_NEGATIVE = '-',
    RIFT_POLARITY_EPSILON  = '0'
} rift_polarity_t;

/* Trident resolution node */
typedef struct rift_trident_node {
    char              name[64];
    uint32_t          version_major;
    uint32_t          version_minor;
    uint32_t          version_patch;
    rift_polarity_t   polarity;
    int               incoming[2];
    int               outgoing;
    int               binding;
} rift_trident_node_t;

/* Bridge context */
typedef struct rift_bridge rift_bridge_t;

RIFT_API rift_bridge_t *rift_bridge_create(void);
RIFT_API void           rift_bridge_destroy(rift_bridge_t *bridge);
RIFT_API int            rift_bridge_trident_init(rift_bridge_t *bridge);
RIFT_API int            rift_bridge_consensus(rift_bridge_t *bridge);
RIFT_API int            rift_bridge_coherence(rift_bridge_t *bridge);

/* Hex encoding (ropen) */
RIFT_API int            rift_hex_encode(const void *data, size_t size,
                                         char *out, size_t out_size);
RIFT_API uint8_t        rift_conjugate(uint8_t value);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_BRIDGE_H */
