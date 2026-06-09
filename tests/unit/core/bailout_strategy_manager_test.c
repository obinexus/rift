#include <stdio.h>
#include <stdlib.h>

#include "ctest.h"
#include "regex/runtime/backtracker/bailout_strategy_manager.h"

// Test fixture setup
CTEST_DATA(bailout_manager)
{
    RiftRegexBailoutStrategyManager *manager;
};

CTEST_SETUP(bailout_manager)
{
    data->manager = rift_regex_bailout_strategy_manager_create();
}

CTEST_TEARDOWN(bailout_manager)
{
    rift_regex_bailout_strategy_manager_destroy(data->manager);
}

// Test cases
CTEST2(bailout_manager, creation)
{
    ASSERT_NOT_NULL(data->manager);
}

CTEST2(bailout_manager, default_strategy)
{
    ASSERT_EQUAL(RIFT_REGEX_BAILOUT_STRATEGY_NONE,
                 rift_regex_bailout_strategy_manager_get_strategy(data->manager));
}

CTEST2(bailout_manager, set_and_get_strategy)
{
    rift_regex_bailout_strategy_manager_set_strategy(data->manager,
                                                     RIFT_REGEX_BAILOUT_STRATEGY_TIMEOUT);
    ASSERT_EQUAL(RIFT_REGEX_BAILOUT_STRATEGY_TIMEOUT,
                 rift_regex_bailout_strategy_manager_get_strategy(data->manager));
}

CTEST2(bailout_manager, set_timeout_value)
{
    rift_regex_bailout_strategy_manager_set_strategy(data->manager,
                                                     RIFT_REGEX_BAILOUT_STRATEGY_TIMEOUT);
    rift_regex_bailout_strategy_manager_set_timeout(data->manager, 1000);
    ASSERT_EQUAL(1000, rift_regex_bailout_strategy_manager_get_timeout(data->manager));
}

CTEST2(bailout_manager, check_no_bailout)
{
    rift_regex_bailout_strategy_manager_set_strategy(data->manager,
                                                     RIFT_REGEX_BAILOUT_STRATEGY_NONE);
    ASSERT_FALSE(rift_regex_bailout_strategy_manager_should_bailout(data->manager));
}

CTEST2(bailout_manager, check_steps_bailout_not_triggered)
{
    rift_regex_bailout_strategy_manager_set_strategy(data->manager,
                                                     RIFT_REGEX_BAILOUT_STRATEGY_MAX_STEPS);
    rift_regex_bailout_strategy_manager_set_max_steps(data->manager, 1000);

    for (int i = 0; i < 500; i++) {
        rift_regex_bailout_strategy_manager_record_step(data->manager);
    }

    ASSERT_FALSE(rift_regex_bailout_strategy_manager_should_bailout(data->manager));
}

CTEST2(bailout_manager, check_steps_bailout_triggered)
{
    rift_regex_bailout_strategy_manager_set_strategy(data->manager,
                                                     RIFT_REGEX_BAILOUT_STRATEGY_MAX_STEPS);
    rift_regex_bailout_strategy_manager_set_max_steps(data->manager, 1000);

    for (int i = 0; i < 1001; i++) {
        rift_regex_bailout_strategy_manager_record_step(data->manager);
    }

    ASSERT_TRUE(rift_regex_bailout_strategy_manager_should_bailout(data->manager));
}

CTEST2(bailout_manager, reset_counters)
{
    rift_regex_bailout_strategy_manager_set_strategy(data->manager,
                                                     RIFT_REGEX_BAILOUT_STRATEGY_MAX_STEPS);
    rift_regex_bailout_strategy_manager_set_max_steps(data->manager, 1000);

    for (int i = 0; i < 500; i++) {
        rift_regex_bailout_strategy_manager_record_step(data->manager);
    }

    rift_regex_bailout_strategy_manager_reset(data->manager);
    ASSERT_EQUAL(0, rift_regex_bailout_strategy_manager_get_steps(data->manager));
}

CTEST2(bailout_manager, multiple_strategies)
{
    rift_regex_bailout_strategy_manager_set_strategy(
        data->manager, RIFT_REGEX_BAILOUT_STRATEGY_TIMEOUT | RIFT_REGEX_BAILOUT_STRATEGY_MAX_STEPS);

    rift_regex_bailout_strategy_manager_set_timeout(data->manager, 1000);
    rift_regex_bailout_strategy_manager_set_max_steps(data->manager, 1000);

    ASSERT_TRUE(rift_regex_bailout_strategy_manager_has_strategy(
        data->manager, RIFT_REGEX_BAILOUT_STRATEGY_TIMEOUT));
    ASSERT_TRUE(rift_regex_bailout_strategy_manager_has_strategy(
        data->manager, RIFT_REGEX_BAILOUT_STRATEGY_MAX_STEPS));
}

int
main(int argc, const char *argv[])
{
    return ctest_main(argc, argv);
}