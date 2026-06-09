/**
 * @file backtracker_limit_config.h
 * @brief Configurable backtracking limits at different scopes
 */

#include <stdbool.h>
#include <stdint.h>
#ifndef LIBRIFT_CORE_CONFIG_BACKTRACKER_LIMIT_CONFIG_H
#define LIBRIFT_CORE_CONFIG_BACKTRACKER_LIMIT_CONFIG_H


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RIFT_BACKTRACK_SCOPE_GLOBAL,
    RIFT_BACKTRACK_SCOPE_PATTERN,
    RIFT_BACKTRACK_SCOPE_MATCH
} rift_backtrack_limit_scope_t;

typedef struct rift_backtrack_limit_config {
    uint32_t max_depth;                 /**< Maximum backtracking depth */
    uint32_t max_duration_ms;           /**< Maximum time in milliseconds */
    uint64_t max_transitions;           /**< Maximum state transitions */
    rift_backtrack_limit_scope_t scope; /**< Configuration scope */
    bool override_parent;               /**< Whether to override parent scope */
} rift_backtrack_limit_config_t;

/**
 * @brief Create limit configuration with default values
 * @param scope The scope of this configuration
 * @return New configuration
 */
rift_backtrack_limit_config_t *
rift_backtrack_limit_config_create(rift_backtrack_limit_scope_t scope);

/**
 * @brief Create global limit configuration
 * @param max_depth Maximum backtracking depth
 * @param max_duration_ms Maximum duration in milliseconds
 * @param max_transitions Maximum state transitions
 * @return New global configuration
 */
rift_backtrack_limit_config_t *rift_backtrack_limit_config_create_global(uint32_t max_depth,
                                                                         uint32_t max_duration_ms,
                                                                         uint64_t max_transitions);

/**
 * @brief Create pattern-specific limit configuration
 * @param pattern_id Pattern identifier
 * @param override_global Whether to override global settings
 * @param max_depth Maximum backtracking depth
 * @param max_duration_ms Maximum duration in milliseconds
 * @param max_transitions Maximum state transitions
 * @return New pattern configuration
 */
rift_backtrack_limit_config_t *rift_backtrack_limit_config_create_pattern(uint32_t pattern_id,
                                                                          bool override_global,
                                                                          uint32_t max_depth,
                                                                          uint32_t max_duration_ms,
                                                                          uint64_t max_transitions);

/**
 * @brief Create match-specific limit configuration
 * @param match_id Match operation identifier
 * @param override_pattern Whether to override pattern settings
 * @param max_depth Maximum backtracking depth
 * @param max_duration_ms Maximum duration in milliseconds
 * @param max_transitions Maximum state transitions
 * @return New match configuration
 */
rift_backtrack_limit_config_t *rift_backtrack_limit_config_create_match(uint32_t match_id,
                                                                        bool override_pattern,
                                                                        uint32_t max_depth,
                                                                        uint32_t max_duration_ms,
                                                                        uint64_t max_transitions);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBRIFT_CORE_CONFIG_BACKTRACKER_LIMIT_CONFIG_H */