#include "rift/memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#if defined(_MSC_VER)
#include <malloc.h>
#define RIFT_ALIGNED_ALLOC(size, align) _aligned_malloc(size, align)
#define RIFT_ALIGNED_FREE(ptr) _aligned_free(ptr)
#elif defined(__MINGW32__) || defined(__MINGW64__)
/* MinGW: use __mingw_aligned_malloc or fall back to malloc */
#include <malloc.h>
#define RIFT_ALIGNED_ALLOC(size, align) __mingw_aligned_malloc(size, align)
#define RIFT_ALIGNED_FREE(ptr) __mingw_aligned_free(ptr)
#else
/* POSIX */
static void *rift_posix_aligned_alloc(size_t size, size_t alignment) {
    void *ptr = NULL;
    if (posix_memalign(&ptr, alignment, size) != 0) return NULL;
    return ptr;
}
#define RIFT_ALIGNED_ALLOC(size, align) rift_posix_aligned_alloc(size, align)
#define RIFT_ALIGNED_FREE(ptr) free(ptr)
#endif

rift_memory_span_t rift_memory_alloc(size_t size, uint32_t alignment) {
    rift_memory_span_t span;
    memset(&span, 0, sizeof(span));

    if (size == 0) return span;
    if (alignment == 0) alignment = RIFT_MEMORY_ALIGN_DEFAULT;

    span.ptr = RIFT_ALIGNED_ALLOC(size, alignment);

    if (span.ptr) {
        span.size = size;
        span.alignment = alignment;
        span.flags = RIFT_MEM_DYNAMIC;
        memset(span.ptr, 0, size);
    }

    return span;
}

void rift_memory_free(rift_memory_span_t *span) {
    if (!span || !span->ptr) return;
    if (span->flags & RIFT_MEM_STATIC) return;

    RIFT_ALIGNED_FREE(span->ptr);

    span->ptr = NULL;
    span->size = 0;
}

int rift_memory_validate(const rift_memory_span_t *span) {
    if (!span) return 0;
    if (!span->ptr && span->size > 0) return 0;
    if (span->alignment == 0) return 0;
    if (span->ptr && ((uintptr_t)span->ptr % span->alignment != 0)) return 0;
    return 1;
}

RIFTMemory rift_memory_borrow(void *ptr, size_t size) {
    RIFTMemory span;
    memset(&span, 0, sizeof(span));
    span.ptr = ptr;
    span.size = size;
    span.alignment = 1;
    span.flags = RIFT_MEM_STATIC | RIFT_MEM_READONLY;
    return span;
}
