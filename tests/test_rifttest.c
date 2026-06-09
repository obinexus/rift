#include <assert.h>
#include <stdio.h>
#include "rift/rifttest.h"

int main(void) {
    RTestSuite *suite = rtest_suite_create("matrix", 4);
    bool added;
    assert(suite != NULL);
    added = rtest_suite_add(suite, rtest_case_create(
        "tp", "^yes$", "yes", true));
    assert(added);
    added = rtest_suite_add(suite, rtest_case_create(
        "tn", "^yes$", "no", false));
    assert(added);
    added = rtest_suite_add(suite, rtest_case_create(
        "fp", "yes", "yes", false));
    assert(added);
    added = rtest_suite_add(suite, rtest_case_create(
        "fn", "^yes$", "no", true));
    assert(added);
    added = rtest_run_suite(suite);
    assert(!added);
    assert(suite->true_positives == 1);
    assert(suite->true_negatives == 1);
    assert(suite->false_positives == 1);
    assert(suite->false_negatives == 1);
    rtest_suite_free(suite);
    puts("test_rifttest: PASS");
    return 0;
}
