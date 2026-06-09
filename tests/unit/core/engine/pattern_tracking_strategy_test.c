#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "bailout_strategy.h"
#include "pattern_tracking_strategy.h"

// Define the ctest macros
#define CTEST_MAIN
#define CTEST_COLOR
#include "ctest.h"

typedef struct {
    uint32_t progress_marker;
} rift_backtracking_state_t;

// Test creating the strategy
CTEST(progress_tracking_strategy, create)
{
    float progress_threshold = 0.5f;
    uint32_t max_stagnation = 5;

    rift_bailout_strategy_t *strategy =
        rift_bailout_progress_tracking_strategy_create(progress_threshold, max_stagnation);

    ASSERT_NOT_NULL(strategy);
    ASSERT_NOT_NULL(strategy->strategy_data);

    // Clean up
    free(strategy->strategy_data);
    free(strategy);
}

// Test updating the progress marker
CTEST(progress_tracking_strategy, update_marker)
{
    float progress_threshold = 0.5f;
    uint32_t max_stagnation = 5;

    rift_bailout_strategy_t *strategy =
        rift_bailout_progress_tracking_strategy_create(progress_threshold, max_stagnation);

    // Initial update should show progress
    bool result = rift_bailout_progress_update_marker(strategy, 10);
    ASSERT_TRUE(result);

    // Same position, no progress
    result = rift_bailout_progress_update_marker(strategy, 10);
    ASSERT_FALSE(result);

    // Higher position, progress made
    result = rift_bailout_progress_update_marker(strategy, 15);
    ASSERT_TRUE(result);

    // Lower position, no progress
    result = rift_bailout_progress_update_marker(strategy, 5);
    ASSERT_FALSE(result);

    // Clean up
    free(strategy->strategy_data);
    free(strategy);
}

// Test stagnation counting behavior
CTEST(progress_tracking_strategy, stagnation_count)
{
    float progress_threshold = 0.5f;
    uint32_t max_stagnation = 3;

    rift_bailout_strategy_t *strategy =
        rift_bailout_progress_tracking_strategy_create(progress_threshold, max_stagnation);

    // Set initial progress
    rift_bailout_progress_update_marker(strategy, 10);

    // Update same position multiple times
    for (int i = 0; i < max_stagnation - 1; i++) {
        rift_bailout_progress_update_marker(strategy, 10);
    }

    // Make progress, this should reset stagnation count
    bool result = rift_bailout_progress_update_marker(strategy, 15);
    ASSERT_TRUE(result);

    // Clean up
    free(strategy->strategy_data);
    free(strategy);
}

// Test null handling
CTEST(progress_tracking_strategy, null_handling)
{
    // Test update marker with NULL strategy
    bool result = rift_bailout_progress_update_marker(NULL, 10);
    ASSERT_FALSE(result);

    // Try creating with zero values (should still be valid)
    rift_bailout_strategy_t *strategy = rift_bailout_progress_tracking_strategy_create(0.0f, 0);
    ASSERT_NOT_NULL(strategy);
    free(strategy->strategy_data);
    free(strategy);
}

// Main function to run the tests
int
main(int argc, const char *argv[])
{
    return ctest_main(argc, argv);
}
