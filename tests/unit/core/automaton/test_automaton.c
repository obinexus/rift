#include "test_automaton.h"

#include <check.h>

#include "librift/core/regex/automaton.h" // Adjust include path as needed

/* Basic Test Case Template */
START_TEST(test_automaton_initialization)
{
    /* TODO: Implement initialization test */
    ck_assert(1); // Placeholder assertion
}
END_TEST

/* Test Suite Creation */
Suite *
automaton_test_suite(void)
{
    Suite *s = suite_create("Automaton Module");

    TCase *tc_core = tcase_create("Core Functionality");
    tcase_add_test(tc_core, test_automaton_initialization);
    suite_add_tcase(s, tc_core);

    return s;
}

/* Main Test Runner (Optional, can be separate) */
int
main(void)
{
    Suite *suite = automaton_test_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    int number_failed = srunner_ntests_failed(runner);

    srunner_free(runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
