/**
 * @file bailout_strategy_manager.h
 * @brief Manager for multiple bailout strategies
 */

typedef struct rift_bailout_strategy_manager {
    rift_bailout_strategy_t **strategies;
    size_t strategy_count;
    size_t strategy_capacity;
} rift_bailout_strategy_manager_t;

/**
 * @brief Create a new bailout strategy manager
 * @return New manager instance
 */
rift_bailout_strategy_manager_t *rift_bailout_strategy_manager_create(void);

/**
 * @brief Add a strategy to the manager
 * @param manager The manager to modify
 * @param strategy The strategy to add
 * @return true if successful, false otherwise
 */
bool rift_bailout_strategy_manager_add_strategy(rift_bailout_strategy_manager_t *manager,
                                                rift_bailout_strategy_t *strategy);

/**
 * @brief Evaluate all strategies and select the highest priority active one
 * @param manager The manager to query
 * @param state Current backtracking state
 * @return The selected strategy or NULL if none are active
 */
rift_bailout_strategy_t *
rift_bailout_strategy_manager_evaluate(const rift_bailout_strategy_manager_t *manager,
                                       const rift_backtracking_state_t *state);

/**
 * @brief Execute the selected strategy
 * @param manager The manager to use
 * @param strategy The strategy to execute
 * @param state Current backtracking state
 * @return Result code from strategy execution
 */
int rift_bailout_strategy_manager_execute(rift_bailout_strategy_manager_t *manager,
                                          rift_bailout_strategy_t *strategy,
                                          const rift_backtracking_state_t *state);

/**
 * @brief Free manager resources
 * @param manager The manager to free
 * @param free_strategies Whether to also free contained strategies
 */
void rift_bailout_strategy_manager_free(rift_bailout_strategy_manager_t *manager,
                                        bool free_strategies);