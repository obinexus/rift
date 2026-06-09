
#include "core/runtime/pattern_tracking_strategy.h
#include "bailout_strategy.h"
#include "progress_tracking_strategy.h"
runtime/pattern_tracking_strategy.h
tmp/librift_analysis/librift-4933472-W/src/core/runtime/pattern_tracking_strategy.h
tmp/librift_analysis/librift-4933472-W/src/runtime/pattern_tracking_strategy.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/runtime/pattern_tracking_strategy.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/runtime/pattern_tracking_strategy.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/runtime/pattern_tracking_strategy.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/runtime/pattern_tracking_strategy.h"
core/runtime/pattern_tracking_strategy.h"
static bool
progress_tracking_should_activate(const rift_bailout_strategy_t *strategy,
                                  const rift_backtracking_state_t *state)
{

    if (!strategy || !state || !strategy->strategy_data) {
        return false;
    }

    rift_progress_tracking_data_t *data = (rift_progress_tracking_data_t *)strategy->strategy_data;

    // If we've made no progress but backtracked several times,
    // that's a sign of ineffective matching
    if (state->progress_marker <= data->last_progress_point) {
        return data->stagnation_count >= data->max_stagnation_allowed;
    }

    // We've made progress, so reset stagnation counter
    data->last_progress_point = state->progress_marker;
    data->stagnation_count = 0;
    return false;
}

static int
progress_tracking_execute(rift_bailout_strategy_t *strategy, const rift_backtracking_state_t *state)
{

    // This strategy simply prevents further backtracking
    // Return code 1 indicates bailout was executed
    return 1;
}

static int
progress_tracking_get_priority(const rift_bailout_strategy_t *strategy)
{

    // This strategy has high priority
    return 80;
}

rift_bailout_strategy_t *
rift_bailout_progress_tracking_strategy_create(float progress_threshold,
                                               uint32_t max_stagnation_allowed)
{

    rift_progress_tracking_data_t *data =
        (rift_progress_tracking_data_t *)malloc(sizeof(rift_progress_tracking_data_t));

    if (!data) {
        return NULL;
    }

    data->progress_threshold = progress_threshold;
    data->last_progress_point = 0;
    data->stagnation_count = 0;
    data->max_stagnation_allowed = max_stagnation_allowed;

    rift_bailout_strategy_t *strategy = rift_bailout_strategy_create(
        "Progress Tracking Strategy", progress_tracking_should_activate, progress_tracking_execute,
        progress_tracking_get_priority, data);

    if (!strategy) {
        free(data);
        return NULL;
    }

    return strategy;
}

bool
rift_bailout_progress_update_marker(rift_bailout_strategy_t *strategy, uint32_t current_position)
{

    if (!strategy || !strategy->strategy_data) {
        return false;
    }

    rift_progress_tracking_data_t *data = (rift_progress_tracking_data_t *)strategy->strategy_data;

    if (current_position > data->last_progress_point) {
        data->last_progress_point = current_position;
        data->stagnation_count = 0;
        return true;
    } else {
        data->stagnation_count++;
        return false;
    }
}