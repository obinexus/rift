/**
 * @file backtrack_limit_registry.h
 * @brief Registry for backtracking limit configurations
 */

typedef struct rift_backtrack_limit_registry rift_backtrack_limit_registry_t;

/**
 * @brief Create a new limit registry
 * @return New registry instance
 */
rift_backtrack_limit_registry_t *rift_backtrack_limit_registry_create(void);

/**
 * @brief Set global limit configuration
 * @param registry The registry to modify
 * @param config The global configuration
 * @return true if successful, false otherwise
 */
bool rift_backtrack_limit_registry_set_global(rift_backtrack_limit_registry_t *registry,
                                              const rift_backtrack_limit_config_t *config);

/**
 * @brief Register pattern-specific configuration
 * @param registry The registry to modify
 * @param pattern_id Pattern identifier
 * @param config The pattern configuration
 * @return true if successful, false otherwise
 */
bool rift_backtrack_limit_registry_register_pattern(rift_backtrack_limit_registry_t *registry,
                                                    uint32_t pattern_id,
                                                    const rift_backtrack_limit_config_t *config);

/**
 * @brief Register match-specific configuration
 * @param registry The registry to modify
 * @param match_id Match operation identifier
 * @param config The match configuration
 * @return true if successful, false otherwise
 */
bool rift_backtrack_limit_registry_register_match(rift_backtrack_limit_registry_t *registry,
                                                  uint32_t match_id,
                                                  const rift_backtrack_limit_config_t *config);

/**
 * @brief Get effective configuration for a match operation
 * @param registry The registry to query
 * @param pattern_id Pattern identifier
 * @param match_id Match operation identifier
 * @return Effective configuration considering all scopes and overrides
 */
rift_backtrack_limit_config_t *
rift_backtrack_limit_registry_get_effective_config(const rift_backtrack_limit_registry_t *registry,
                                                   uint32_t pattern_id, uint32_t match_id);

/**
 * @brief Free registry resources
 * @param registry The registry to free
 */
void rift_backtrack_limit_registry_free(rift_backtrack_limit_registry_t *registry);