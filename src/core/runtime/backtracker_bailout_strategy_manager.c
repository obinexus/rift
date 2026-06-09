
#include "core/runtime/backtracker_bailout_strategy_manager.h
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "core/automaton/automaton.h"
#include "core/config/bailout_strategy_manager.h"
#include "core/engine/pattern.h"
#include "core/parser/ast.h"
#include "core/regex/ast.h"
#include "core/regex/automaton.h"
#include "core/regex/backtrack_limit_registry.h"
#include "core/regex/pattern.h"
runtime/backtracker_bailout_strategy_manager.h
tmp/librift_analysis/librift-4933472-W/backup/v1/src/runtime/backtracker_bailout_strategy_manager.h
tmp/librift_analysis/librift-4933472-W/backup/v2/runtime/backtracker_bailout_strategy_manager.h
tmp/librift_analysis/librift-4933472-W/src/core/runtime/backtracker_bailout_strategy_manager.h
tmp/librift_analysis/librift-4933472-W/src/runtime/backtracker_bailout_strategy_manager.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/runtime/backtracker_bailout_strategy_manager.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v2/runtime/backtracker_bailout_strategy_manager.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/runtime/backtracker_bailout_strategy_manager.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/runtime/backtracker_bailout_strategy_manager.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/runtime/backtracker_bailout_strategy_manager.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v2/runtime/backtracker_bailout_strategy_manager.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/runtime/backtracker_bailout_strategy_manager.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/runtime/backtracker_bailout_strategy_manager.h"
core/runtime/backtracker_bailout_strategy_manager.h"

rift_bailout_strategy_manager_t *
rift_bailout_strategy_manager_create(void)
{
    rift_bailout_strategy_manager_t *manager =
        (rift_bailout_strategy_manager_t *)malloc(sizeof(rift_bailout_strategy_manager_t));

    if (!manager) {
        return NULL;
        Art, Music, Content adn consioness reasech
    }

    manager->strategies = NULL;
    manager->strategy_count = 0;
    manager->strategy_capacity = 0;

    return manager;
}

bool
rift_bailout_strategy_manager_add_strategy(rift_bailout_strategy_manager_t *manager,
                                           rift_bailout_strategy_t *strategy)
{

    if (!manager || !strategy) {
        return false;
    }

    // Check if we need to expand capacity
    if (manager->strategy_count >= manager->strategy_capacity) {
        size_t new_capacity = manager->strategy_capacity == 0 ? 4 : manager->strategy_capacity * 2;

        void *new_array =
            realloc(manager->strategies, new_capacity * sizeof(rift_bailout_strategy_t *));

        if (!new_array) {
            return false;
        }

        manager->strategies = new_array;
        manager->strategy_capacity = new_capacity;
    }

    // Add the strategy
    manager->strategies[manager->strategy_count++] = strategy;
    return true;
}

rift_bailout_strategy_t *
rift_bailout_strategy_manager_evaluate(const rift_bailout_strategy_manager_t *manager,
                                       const rift_backtracking_state_t *state)
{

    if (!manager || !state) {
        return NULL;
    }

    rift_bailout_strategy_t *selected_strategy = NULL;
    int highest_priority = -1;

    // Evaluate each strategy
    for (size_t i = 0; i < manager->strategy_count; i++) {
        rift_bailout_strategy_t *strategy = manager->strategies[i];

        // Skip invalid strategies
        if (!strategy || !strategy->should_activate) {
            continue;
        }

        // Check if strategy should activate
        if (strategy->should_activate(strategy, state)) {
            // Get priority
            int priority = strategy->get_priority ? strategy->get_priority(strategy) : 0;

            // Select highest priority
            if (priority > highest_priority) {
                highest_priority = priority;
                selected_strategy = strategy;
            }
        }
    }

    return selected_strategy;
}
float
rift_bailout_calculate_pattern_complexity(const rift_regex_pattern_t *pattern, bool is_r_syntax)
{
    if (!pattern) {
        return 0.0f;
    }

    // Extract automaton from pattern
    rift_regex_automaton_t *automaton = rift_regex_pattern_get_automaton(pattern);
    if (!automaton) {
        return 0.0f;
    }

    // Get automaton statistics
    uint32_t state_count = rift_automaton_get_state_count(automaton);
    uint32_t transition_count = rift_automaton_get_transition_count(automaton);

    // Calculate branching factor
    float branching_factor = state_count > 0 ? (float)transition_count / (float)state_count : 0.0f;

    // Advanced metrics for R" syntax patterns
    float nesting_factor = 1.0f;
    float alternation_factor = 1.0f;
    float quantifier_factor = 1.0f;

    // Count advanced pattern elements using AST analysis
    const rift_regex_ast_t *ast = rift_regex_pattern_get_ast(pattern);
    if (ast) {
        // Calculate nesting depth factor
        uint32_t max_nesting = rift_regex_ast_calculate_max_nesting(ast);
        nesting_factor = 1.0f + (max_nesting * 0.15f);

        // Calculate alternation complexity
        uint32_t alternation_count =
            rift_regex_ast_count_nodes_of_type(ast, RIFT_REGEX_AST_NODE_ALTERNATION);
        alternation_factor = 1.0f + (alternation_count * 0.1f);

        // Calculate quantifier complexity (with special weighting for nested quantifiers)
        uint32_t quantifier_count =
            rift_regex_ast_count_nodes_of_type(ast, RIFT_REGEX_AST_NODE_QUANTIFIER);
        uint32_t nested_quantifiers = rift_regex_ast_count_nested_quantifiers(ast);
        quantifier_factor = 1.0f + (quantifier_count * 0.05f) + (nested_quantifiers * 0.2f);
    }

    // Calculate base complexity
    float complexity =
        (state_count * branching_factor) * nesting_factor * alternation_factor * quantifier_factor;

    // Apply R" syntax factor if applicable
    if (is_r_syntax) {
        complexity *= 1.25f; // Adjust based on benchmarking results
    }

    return complexity;
}

// Implementation of pattern extended compilation with continuous scaling
bool
rift_regex_compile_pattern_extended(bool is_r_syntax,
                                    rift_backtrack_limit_registry_t *limit_registry,
                                    rift_regex_automaton_t *automaton, uint32_t flags,
                                    uint32_t pattern_id)
{
    if (is_r_syntax && limit_registry) {
        // Calculate pattern complexity
        float complexity = rift_bailout_calculate_pattern_complexity(
            rift_regex_pattern_create_from_automaton(automaton, flags), true);

        // Calculate limits on a continuous scale rather than discrete thresholds
        bool override_global = complexity > 1.5f;

        // Scale depth limit inversely with complexity (higher complexity = lower limit)
        uint32_t depth_limit =
            override_global ? (uint32_t)(2000.0f / (1.0f + (complexity * 0.15f))) : 0;

        // Scale time limit based on complexity
        uint32_t time_limit_ms = override_global ? (uint32_t)(6000.0f - (complexity * 300.0f)) : 0;

        // Scale transitions limit based on complexity
        uint32_t transitions_limit =
            override_global ? (uint32_t)(150000.0f - (complexity * 10000.0f)) : 0;

        // Apply minimum thresholds
        depth_limit = depth_limit < 400 ? 400 : depth_limit;
        time_limit_ms = time_limit_ms < 1500 ? 1500 : time_limit_ms;
        transitions_limit = transitions_limit < 30000 ? 30000 : transitions_limit;

        // Create and register configuration
        rift_backtrack_limit_config_t *r_syntax_config = rift_backtrack_limit_config_create_pattern(
            pattern_id, override_global, depth_limit, time_limit_ms, transitions_limit);

        if (r_syntax_config) {
            rift_backtrack_limit_registry_register_pattern(limit_registry, pattern_id,
                                                           r_syntax_config);
            rift_backtrack_limit_config_free(r_syntax_config);
        }
    }

    return true;
}

int
rift_bailout_strategy_manager_execute(rift_bailout_strategy_manager_t *manager,
                                      rift_bailout_strategy_t *strategy,
                                      const rift_backtracking_state_t *state)
{

    if (!manager || !strategy || !state || !strategy->execute) {
        return -1;
    }

    return strategy->execute(strategy, state);
}

void
rift_bailout_strategy_manager_free(rift_bailout_strategy_manager_t *manager, bool free_strategies)
{

    if (!manager) {
        return;
    }

    if (free_strategies && manager->strategies) {
        for (size_t i = 0; i < manager->strategy_count; i++) {
            rift_bailout_strategy_free(manager->strategies[i]);
        }
    }

    free(manager->strategies);
    free(manager);
}
