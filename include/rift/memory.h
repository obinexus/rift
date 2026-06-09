#ifndef RIFT_MEMORY_H
#define RIFT_MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include "rift/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RIFT_MEMORY_ALIGN_DEFAULT 8
#define RIFT_MEMORY_ALIGN_CACHE   64

typedef struct rift_memory_span {
    void    *ptr;
    size_t   size;
    uint32_t alignment;
    uint16_t flags;
} rift_memory_span_t;

typedef rift_memory_span_t RIFTMemory;

/* Memory span flags */
#define RIFT_MEM_STATIC   0x0001
#define RIFT_MEM_DYNAMIC  0x0002
#define RIFT_MEM_READONLY 0x0004

RIFT_API rift_memory_span_t rift_memory_alloc(size_t size, uint32_t alignment);
RIFT_API void               rift_memory_free(rift_memory_span_t *span);
RIFT_API int                rift_memory_validate(const rift_memory_span_t *span);
RIFT_API RIFTMemory         rift_memory_borrow(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_MEMORY_H */
