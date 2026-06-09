/**
 * @file memory.c
 * @brief Memory management facilities for the LibRift library
 *
 * This file implements memory allocation, tracking, and management
 * for the LibRift library, with support for custom allocators.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/memory/memory.h
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tmp/librift_analysis/librift-4933472-W/backup/v1/src/memory/memory.h
tmp/librift_analysis/librift-4933472-W/src/core/memory/memory.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/memory/memory.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/memory/memory.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/memory/memory.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/memory/memory.h"
/* Memory tracking statistics */
static rift_memory_stats_t memory_stats = {0};

/* Memory tracking enabled flag */
static bool memory_tracking_enabled = false;

/**
 * @brief Enable memory usage tracking
 *
 * @param enabled Whether to enable memory tracking
 * @return Previous state of memory tracking
 */
bool
rift_memory_tracking_enable(bool enabled)
{
    bool previous = memory_tracking_enabled;
    memory_tracking_enabled = enabled;
    return previous;
}

/**
 * @brief Reset memory tracking statistics
 */
void
rift_memory_tracking_reset(void)
{
    memset(&memory_stats, 0, sizeof(rift_memory_stats_t));
}

/**
 * @brief Get memory usage statistics
 *
 * @param current_usage Current memory usage in bytes
 * @param peak_usage Peak memory usage in bytes
 * @param total_allocs Total number of allocations
 * @param total_frees Total number of frees
 * @param active_allocs Number of active allocations
 */
void
rift_memory_get_stats(size_t *current_usage, size_t *peak_usage, size_t *total_allocs,
                      size_t *total_frees, size_t *active_allocs)
{
    if (current_usage)
        *current_usage = memory_stats.current_usage;
    if (peak_usage)
        *peak_usage = memory_stats.peak_usage;
    if (total_allocs)
        *total_allocs = memory_stats.total_allocs;
    if (total_frees)
        *total_frees = memory_stats.total_frees;
    if (active_allocs)
        *active_allocs = memory_stats.active_allocs;
}

/**
 * @brief Standard allocation implementation
 *
 * @param size Size to allocate in bytes
 * @return Allocated memory or NULL on failure
 */
static void *
standard_malloc(size_t size)
{
    return malloc(size);
}

/**
 * @brief Standard reallocation implementation
 *
 * @param ptr Pointer to memory to reallocate
 * @param size New size in bytes
 * @return Reallocated memory or NULL on failure
 */
static void *
standard_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

/**
 * @brief Standard free implementation
 *
 * @param ptr Pointer to memory to free
 */
static void
standard_free(void *ptr)
{
    free(ptr);
}

/**
 * @brief Update memory tracking statistics for allocation
 *
 * @param size Size allocated in bytes
 */
static void
update_stats_alloc(size_t size)
{
    if (!memory_tracking_enabled) {
        return;
    }

    memory_stats.current_usage += size;
    if (memory_stats.current_usage > memory_stats.peak_usage) {
        memory_stats.peak_usage = memory_stats.current_usage;
    }
    memory_stats.total_allocs++;
    memory_stats.active_allocs++;
}

/**
 * @brief Update memory tracking statistics for free
 *
 * @param size Size freed in bytes
 */
static void
update_stats_free(size_t size)
{
    if (!memory_tracking_enabled) {
        return;
    }

    memory_stats.current_usage -= size;
    memory_stats.total_frees++;
    memory_stats.active_allocs--;
}

/**
 * @brief Memory block header structure
 * Contains size information for memory tracking
 */
typedef struct rift_mem_header {
    size_t size;        /**< Size of allocated block (excluding header) */
    uint32_t magic;     /**< Magic number for validation */
    uint8_t padding[4]; /**< Padding for alignment */
} rift_mem_header_t;

/* Magic value for memory block validation */
#define RIFT_MEMORY_MAGIC 0x52494654 /* "RIFT" in ASCII */

/**
 * @brief Allocate memory with tracking
 *
 * @param size Size to allocate in bytes
 * @return Allocated memory or NULL on failure
 */
void *
rift_malloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }

    /* Check allocation limit if set */
    const rift_config_t *config = rift_config_get();
    if (config->memory.allocation_limit > 0 &&
        memory_stats.current_usage + size > config->memory.allocation_limit) {
        return NULL;
    }

    /* Determine which allocator to use */
    void *(*alloc_func)(size_t) = standard_malloc;
    if (config->memory.use_custom_allocator && config->memory.custom_malloc) {
        alloc_func = config->memory.custom_malloc;
    }

    /* Allocate memory with header */
    size_t total_size = size + sizeof(rift_mem_header_t);
    rift_mem_header_t *header = (rift_mem_header_t *)alloc_func(total_size);

    if (!header) {
        return NULL;
    }

    /* Initialize header */
    header->size = size;
    header->magic = RIFT_MEMORY_MAGIC;

    /* Update statistics */
    update_stats_alloc(size);

    /* Return pointer to the usable memory (after header) */
    return (void *)(header + 1);
}

/**
 * @brief Reallocate memory with tracking
 *
 * @param ptr Pointer to memory to reallocate
 * @param size New size in bytes
 * @return Reallocated memory or NULL on failure
 */
void *
rift_realloc(void *ptr, size_t size)
{
    if (!ptr) {
        return rift_malloc(size);
    }

    if (size == 0) {
        rift_free(ptr);
        return NULL;
    }

    /* Get the header from the user pointer */
    rift_mem_header_t *header = ((rift_mem_header_t *)ptr) - 1;

    /* Validate the header */
    if (header->magic != RIFT_MEMORY_MAGIC) {
        /* Invalid memory block, not allocated by rift_malloc */
        return NULL;
    }

    /* Check allocation limit if set */
    const rift_config_t *config = rift_config_get();
    size_t old_size = header->size;
    if (config->memory.allocation_limit > 0 &&
        memory_stats.current_usage - old_size + size > config->memory.allocation_limit) {
        return NULL;
    }

    /* Determine which reallocator to use */
    void *(*realloc_func)(void *, size_t) = standard_realloc;
    if (config->memory.use_custom_allocator && config->memory.custom_realloc) {
        realloc_func = config->memory.custom_realloc;
    }

    /* Reallocate with new size */
    size_t new_total_size = size + sizeof(rift_mem_header_t);
    rift_mem_header_t *new_header = (rift_mem_header_t *)realloc_func(header, new_total_size);

    if (!new_header) {
        return NULL;
    }

    /* Update statistics */
    if (memory_tracking_enabled) {
        memory_stats.current_usage = memory_stats.current_usage - old_size + size;
        if (memory_stats.current_usage > memory_stats.peak_usage) {
            memory_stats.peak_usage = memory_stats.current_usage;
        }
    }

    /* Update header */
    new_header->size = size;
    new_header->magic = RIFT_MEMORY_MAGIC;

    return (void *)(new_header + 1);
}

/**
 * @brief Free allocated memory with tracking
 *
 * @param ptr Pointer to memory to free
 */
void
rift_free(void *ptr)
{
    if (!ptr) {
        return;
    }

    /* Get the header from the user pointer */
    rift_mem_header_t *header = ((rift_mem_header_t *)ptr) - 1;

    /* Validate the header */
    if (header->magic != RIFT_MEMORY_MAGIC) {
        /* Invalid memory block, not allocated by rift_malloc */
        return;
    }

    /* Update statistics */
    update_stats_free(header->size);

    /* Invalidate the header to catch use-after-free */
    header->magic = 0;

    /* Determine which free function to use */
    void (*free_func)(void *) = standard_free;
    const rift_config_t *config = rift_config_get();
    if (config->memory.use_custom_allocator && config->memory.custom_free) {
        free_func = config->memory.custom_free;
    }

    /* Free the memory block */
    free_func(header);
}

/**
 * @brief Allocate and zero-initialize memory
 *
 * @param num Number of elements
 * @param size Size of each element in bytes
 * @return Allocated memory or NULL on failure
 */
void *
rift_calloc(size_t num, size_t size)
{
    size_t total_size = num * size;

    /* Check for multiplication overflow */
    if (size != 0 && total_size / size != num) {
        return NULL;
    }

    void *ptr = rift_malloc(total_size);
    if (ptr) {
        memset(ptr, 0, total_size);
    }

    return ptr;
}

/**
 * @brief Duplicate a string
 *
 * @param str String to duplicate
 * @return Duplicated string or NULL on failure
 */
char *
rift_strdup(const char *str)
{
    if (!str) {
        return NULL;
    }

    size_t len = strlen(str) + 1; /* Include null terminator */
    char *dup = (char *)rift_malloc(len);

    if (dup) {
        memcpy(dup, str, len);
    }

    return dup;
}

/**
 * @brief Generate a memory usage report
 *
 * @param buffer Buffer to store the report
 * @param buffer_size Size of the buffer
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t
rift_memory_report(char *buffer, size_t buffer_size)
{
    if (!buffer || buffer_size == 0) {
        return RIFT_ERROR_INVALID_PARAMETER;
    }

    int written = snprintf(buffer, buffer_size,
                           "LibRift Memory Usage Report:\n"
                           "  Current usage: %zu bytes (%.2f KB, %.2f MB)\n"
                           "  Peak usage: %zu bytes (%.2f KB, %.2f MB)\n"
                           "  Total allocations: %zu\n"
                           "  Total frees: %zu\n"
                           "  Active allocations: %zu\n"
                           "  Tracking enabled: %s\n",
                           memory_stats.current_usage, (double)memory_stats.current_usage / 1024.0,
                           (double)memory_stats.current_usage / (1024.0 * 1024.0),
                           memory_stats.peak_usage, (double)memory_stats.peak_usage / 1024.0,
                           (double)memory_stats.peak_usage / (1024.0 * 1024.0),
                           memory_stats.total_allocs, memory_stats.total_frees,
                           memory_stats.active_allocs, memory_tracking_enabled ? "yes" : "no");

    if (written < 0 || (size_t)written >= buffer_size) {
        return RIFT_ERROR_BUFFER_OVERFLOW;
    }

    return RIFT_OK;
}