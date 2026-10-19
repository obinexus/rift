#include "rift_check.h"
#include <stdio.h>
#include <stdlib.h>
#include "rift/rifttest.h"

int main(void) {
    RTestSuite *suite = rtest_suite_create("matrix", 4);
    RTestCase pending;
    bool added;
    RIFT_CHECK(suite != NULL);
    added = rtest_suite_add(suite, rtest_case_create(
        "tp", "^yes$", "yes", true));
    RIFT_CHECK(added);
    added = rtest_suite_add(suite, rtest_case_create(
        "tn", "^yes$", "no", false));
    RIFT_CHECK(added);
    added = rtest_suite_add(suite, rtest_case_create(
        "fp", "yes", "yes", false));
    RIFT_CHECK(added);
    added = rtest_suite_add(suite, rtest_case_create(
        "fn", "^yes$", "no", true));
    RIFT_CHECK(added);
    /* "(" does not compile: an ERROR, which is outside the matrix. */
    added = rtest_suite_add(suite, rtest_case_create(
        "error", "(", "(", true));
    RIFT_CHECK(added);
    added = rtest_run_suite(suite);
    RIFT_CHECK(!added);
    RIFT_CHECK(suite->cases[4].result == RTEST_ERROR);
    RIFT_CHECK(suite->cases[4].outcome == RTEST_OUTCOME_NONE);
    RIFT_CHECK(suite->cases[4].error_message != NULL);
    RIFT_CHECK(suite->passed == 2);
    RIFT_CHECK(suite->failed == 3);
    RIFT_CHECK(suite->true_positives == 1);
    RIFT_CHECK(suite->true_negatives == 1);
    RIFT_CHECK(suite->false_positives == 1);
    RIFT_CHECK(suite->false_negatives == 1);

    /* Running again must not keep a stale outcome on the erroring case. */
    RIFT_CHECK(rtest_run_case(&suite->cases[4]) == RTEST_ERROR);
    RIFT_CHECK(suite->cases[4].outcome == RTEST_OUTCOME_NONE);
    rtest_suite_free(suite);

    /* A case that has not run is not classified either. */
    pending = rtest_case_create("pending", "^a$", "a", true);
    RIFT_CHECK(pending.result == RTEST_SKIP);
    RIFT_CHECK(pending.outcome == RTEST_OUTCOME_NONE);
    free(pending.name);
    free(pending.pattern);
    free(pending.input);

    /* An invalid pattern never satisfies an assertion. */
    RIFT_CHECK(RTEST_ASSERT_MATCH("^a$", "a"));
    RIFT_CHECK(!RTEST_ASSERT_MATCH("(", "("));
    RIFT_CHECK(!RTEST_ASSERT_NO_MATCH("(", "x"));

    puts("test_rifttest: PASS");
    return 0;
}
