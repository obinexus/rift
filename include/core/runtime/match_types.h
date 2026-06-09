/**
 * @file match_types.h
 * @brief Common match types for the LibRift regex engine
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stddef.h>
#ifndef LIBRIFT_MATCH_TYPES_H
#define LIBRIFT_MATCH_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Forward declaration of capture group structure
 */
struct rift_regex_capture_group;

/**
 * @brief Match result structure
 */
typedef struct rift_regex_match {
    size_t start_pos;                      /**< Start position of the match */
    size_t end_pos;                        /**< End position of the match */
    size_t group_count;                    /**< Number of capture groups */
    struct rift_regex_capture_group *groups; /**< Capture groups */
} rift_regex_match_t;

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_MATCH_TYPES_H */