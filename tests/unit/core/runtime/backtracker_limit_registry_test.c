#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "librift/core/config/backtrack_limit_config.h"
#include "librift/core/config/backtracker_limit_registry.h"

// Test helper functions
static void test_registry_creation(void);
static void test_global_config(void);
static void test_pattern_config(void);
static void test_match_config(void);
static void test_effective_config(void);

int
main(void)
{
    printf("Running backtracker limit registry tests\n");

    test_registry_creation();
    test_global_config();
    test_pattern_config();
    test_match_config();
    test_effective_config();

    printf("All tests passed!\n");
    return 0;
}

static void
test_registry_creation(void)
{
    rift_backtrack_limit_registry_t *registry = rift_backtrack_limit_registry_create();
    assert(registry != NULL);
    rift_backtrack_limit_registry_free(registry);
    printf("Registry creation test passed\n");
}

static void
test_global_config(void)
{
    rift_backtrack_limit_registry_t *registry = rift_backtrack_limit_registry_create();
    assert(registry != NULL);

    // Create a global config
    rift_backtrack_limit_config_t *global_config =
        rift_backtrack_limit_config_create_global(100,  // max depth
                                                  5000, // max duration
                                                  10000 // max transitions
        );
    assert(global_config != NULL);

    // Set global config
    bool result = rift_backtrack_limit_registry_set_global(registry, global_config);
    assert(result == true);

    // Free resources
    rift_backtrack_limit_config_free(global_config);
    rift_backtrack_limit_registry_free(registry);
    printf("Global config test passed\n");
}

static void
test_pattern_config(void)
{
    rift_backtrack_limit_registry_t *registry = rift_backtrack_limit_registry_create();
    assert(registry != NULL);

    uint32_t pattern_id = 1;

    // Create a pattern config
    rift_backtrack_limit_config_t *pattern_config =
        rift_backtrack_limit_config_create_pattern(20000,     // max transitions
                                                   pattern_id // pattern ID
        );
    assert(pattern_config != NULL);

    // Register pattern config
    bool result =
        rift_backtrack_limit_registry_register_pattern(registry, pattern_id, pattern_config);
    assert(result == true);

    // Free resources
    rift_backtrack_limit_config_free(pattern_config);
    rift_backtrack_limit_registry_free(registry);
    printf("Pattern config test passed\n");
}

static void
test_match_config(void)
{
    rift_backtrack_limit_registry_t *registry = rift_backtrack_limit_registry_create();
    assert(registry != NULL);

    uint32_t match_id = 2;

    // Create a match config
    rift_backtrack_limit_config_t *match_config =
        rift_backtrack_limit_config_create_match(30000,   // max transitions
                                                 match_id // match ID
        );
    assert(match_config != NULL);

    // Register match config
    bool result = rift_backtrack_limit_registry_register_match(registry, match_id, match_config);
    assert(result == true);

    // Free resources
    rift_backtrack_limit_config_free(match_config);
    rift_backtrack_limit_registry_free(registry);
    printf("Match config test passed\n");
}

static void
test_effective_config(void)
{
    rift_backtrack_limit_registry_t *registry = rift_backtrack_limit_registry_create();
    assert(registry != NULL);

    uint32_t pattern_id = 1;
    uint32_t match_id = 2;

    // Create and set global config
    rift_backtrack_limit_config_t *global_config =
        rift_backtrack_limit_config_create_global(100,  // max depth
                                                  5000, // max duration
                                                  10000 // max transitions
        );
    bool result = rift_backtrack_limit_registry_set_global(registry, global_config);
    assert(result == true);

    // Create and register pattern config
    rift_backtrack_limit_config_t *pattern_config =
        rift_backtrack_limit_config_create_pattern(20000,     // max transitions
                                                   pattern_id // pattern ID
        );
    result = rift_backtrack_limit_registry_register_pattern(registry, pattern_id, pattern_config);
    assert(result == true);

    // Create and register match config
    rift_backtrack_limit_config_t *match_config =
        rift_backtrack_limit_config_create_match(30000,   // max transitions
                                                 match_id // match ID
        );
    result = rift_backtrack_limit_registry_register_match(registry, match_id, match_config);
    assert(result == true);

    // Get effective config
    rift_backtrack_limit_config_t *effective_config =
        rift_backtrack_limit_registry_get_effective_config(registry, pattern_id, match_id);
    assert(effective_config != NULL);

    // Check the effective config values
    assert(effective_config->max_depth == 100);
    assert(effective_config->max_duration == 5000);
    assert(effective_config->max_transitions == 30000);

    // Free resources
    rift_backtrack_limit_config_free(global_config);
    rift_backtrack_limit_config_free(pattern_config);
    rift_backtrack_limit_config_free(match_config);
    rift_backtrack_limit_config_free(effective_config);
    rift_backtrack_limit_registry_free(registry);
    printf("Effective config test passed\n");
}
