/**
 * @file memory.h
 * @brief Memory management facilities for the LibRift library
 *
 * This file defines memory allocation, tracking, and management
 * for the LibRift library, with support for custom allocators.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#ifndef LIBRIFT_CORE_MEMORY_H
#define LIBRIFT_CORE_MEMORY_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure holding memory tracking statistics
 */
typedef struct rift_memory_stats {
    size_t current_usage; /**< Current memory usage in bytes */
    size_t peak_usage;    /**< Peak memory usage in bytes */
    size_t total_allocs;  /**< Total number of allocations */
    size_t total_frees;   /**< Total number of frees */
    size_t active_allocs; /**< Currently active allocations */
} rift_memory_stats_t;

/**
 * @brief Function type for memory allocation
 */
typedef void *(*rift_malloc_func_t)(size_t size);

/**
 * @brief Function type for memory reallocation
 */
typedef void *(*rift_realloc_func_t)(void *ptr, size_t size);

/**
 * @brief Function type for memory deallocation
 */
typedef void (*rift_free_func_t)(void *ptr);

/**
 * @brief Allocate memory with tracking
 *
 * @param size Size to allocate in bytes
 * @return Allocated memory or NULL on failure
 */
void *rift_malloc(size_t size);

/**
 * @brief Reallocate memory with tracking
 *
 * @param ptr Pointer to memory to reallocate
 * @param size New size in bytes
 * @return Reallocated memory or NULL on failure
 */
void *rift_realloc(void *ptr, size_t size);

/**
 * @brief Free allocated memory with tracking
 *
 * @param ptr Pointer to memory to free
 */
void rift_free(void *ptr);

/**
 * @brief Allocate and zero-initialize memory
 *
 * @param num Number of elements
 * @param size Size of each element in bytes
 * @return Allocated memory or NULL on failure
 */
void *rift_calloc(size_t num, size_t size);

/**
 * @brief Duplicate a string
 *
 * @param str String to duplicate
 * @return Duplicated string or NULL on failure
 */
char *rift_strdup(const char *str);

/**
 * @brief Enable memory usage tracking
 *
 * @param enabled Whether to enable memory tracking
 * @return Previous state of memory tracking
 */
bool rift_memory_tracking_enable(bool enabled);

/**
 * @brief Reset memory tracking statistics
 */
void rift_memory_tracking_reset(void);

/**
 * @brief Get memory usage statistics
 *
 * @param current_usage Current memory usage in bytes
 * @param peak_usage Peak memory usage in bytes
 * @param total_allocs Total number of allocations
 * @param total_frees Total number of frees
 * @param active_allocs Number of active allocations
 */
void rift_memory_get_stats(size_t *current_usage, size_t *peak_usage, size_t *total_allocs,
                           size_t *total_frees, size_t *active_allocs);

/**
 * @brief Generate a memory usage report
 *
 * @param buffer Buffer to store the report
 * @param buffer_size Size of the buffer
 * @return RIFT_OK on success, error code on failure
 */
rift_status_t rift_memory_report(char *buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_CORE_MEMORY_H */