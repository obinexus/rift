#include "rift/rifttest.h"

#include <stdlib.h>
#include <string.h>

static char *test_strdup(const char *value) {
    size_t length;
    char *copy;
    if (!value) return NULL;
    length = strlen(value) + 1;
    copy = (char *)malloc(length);
    if (copy) memcpy(copy, value, length);
    return copy;
}

static int test_full_match(const char *pattern, const char *input, char **error) {
    size_t length = strlen(pattern);
    char *anchored = (char *)malloc(length + 6);
    RIFTRegex regex;
    int matched;
    if (!anchored) {
        if (error) *error = test_strdup("out of memory");
        return -1;
    }
    memcpy(anchored, "^(", 2);
    memcpy(anchored + 2, pattern, length);
    memcpy(anchored + length + 2, ")$", 3);
    if (rift_regex_compile(&regex, anchored) != 0) {
        if (error) *error = test_strdup(rift_regex_error(&regex));
        free(anchored);
        return -1;
    }
    free(anchored);
    matched = rift_regex_match(&regex, input) == 0;
    rift_regex_free(&regex);
    return matched;
}

RTestCase rtest_case_create(const char *name, const char *pattern,
                            const char *input, bool should_match) {
    RTestCase test_case;
    memset(&test_case, 0, sizeof(test_case));
    test_case.name = test_strdup(name ? name : "unnamed");
    test_case.pattern = test_strdup(pattern ? pattern : "");
    test_case.input = test_strdup(input ? input : "");
    test_case.should_match = should_match;
    test_case.result = RTEST_SKIP;
    return test_case;
}

RTestSuite *rtest_suite_create(const char *name, size_t initial_capacity) {
    RTestSuite *suite = (RTestSuite *)calloc(1, sizeof(*suite));
    if (!suite) return NULL;
    suite->name = test_strdup(name ? name : "RIFT tests");
    suite->capacity = initial_capacity ? initial_capacity : 8;
    suite->cases = (RTestCase *)calloc(suite->capacity, sizeof(*suite->cases));
    if (!suite->name || !suite->cases) {
        rtest_suite_free(suite);
        return NULL;
    }
    return suite;
}

bool rtest_suite_add(RTestSuite *suite, RTestCase test_case) {
    RTestCase *cases;
    if (!suite) return false;
    if (suite->case_count == suite->capacity) {
        size_t capacity = suite->capacity ? suite->capacity * 2 : 8;
        cases = (RTestCase *)realloc(suite->cases, capacity * sizeof(*cases));
        if (!cases) return false;
        suite->cases = cases;
        suite->capacity = capacity;
    }
    suite->cases[suite->case_count++] = test_case;
    return true;
}

RTestResult rtest_run_case(RTestCase *test_case) {
    int matched;
    if (!test_case || !test_case->pattern || !test_case->input) return RTEST_ERROR;
    free(test_case->error_message);
    test_case->error_message = NULL;

    matched = test_full_match(
        test_case->pattern, test_case->input, &test_case->error_message);
    if (matched < 0) {
        test_case->result = RTEST_ERROR;
        return test_case->result;
    }

    if (matched && test_case->should_match) {
        test_case->outcome = RTEST_TRUE_POSITIVE;
        test_case->result = RTEST_PASS;
    } else if (!matched && !test_case->should_match) {
        test_case->outcome = RTEST_TRUE_NEGATIVE;
        test_case->result = RTEST_PASS;
    } else if (matched) {
        test_case->outcome = RTEST_FALSE_POSITIVE;
        test_case->result = RTEST_FAIL;
        test_case->error_message = test_strdup("pattern matched unexpectedly");
    } else {
        test_case->outcome = RTEST_FALSE_NEGATIVE;
        test_case->result = RTEST_FAIL;
        test_case->error_message = test_strdup("pattern did not match");
    }
    return test_case->result;
}

bool rtest_run_suite(RTestSuite *suite) {
    size_t i;
    if (!suite) return false;
    suite->passed = suite->failed = suite->skipped = 0;
    suite->true_positives = suite->true_negatives = 0;
    suite->false_positives = suite->false_negatives = 0;
    for (i = 0; i < suite->case_count; i++) {
        RTestResult result = rtest_run_case(&suite->cases[i]);
        if (result == RTEST_PASS) suite->passed++;
        else if (result == RTEST_SKIP) suite->skipped++;
        else suite->failed++;
        switch (suite->cases[i].outcome) {
            case RTEST_TRUE_POSITIVE: suite->true_positives++; break;
            case RTEST_TRUE_NEGATIVE: suite->true_negatives++; break;
            case RTEST_FALSE_POSITIVE: suite->false_positives++; break;
            case RTEST_FALSE_NEGATIVE: suite->false_negatives++; break;
        }
    }
    return suite->failed == 0;
}

void rtest_print_results_to(const RTestSuite *suite, FILE *stream) {
    size_t i;
    if (!suite || !stream) return;
    fprintf(stream, "%s\n", suite->name);
    for (i = 0; i < suite->case_count; i++) {
        const RTestCase *test_case = &suite->cases[i];
        const char *status = test_case->result == RTEST_PASS ? "PASS"
            : test_case->result == RTEST_SKIP ? "SKIP"
            : test_case->result == RTEST_ERROR ? "ERROR" : "FAIL";
        fprintf(stream, "  %-5s %s", status, test_case->name);
        if (test_case->error_message) {
            fprintf(stream, ": %s", test_case->error_message);
        }
        fputc('\n', stream);
    }
    fprintf(stream, "  passed=%u failed=%u skipped=%u\n",
            (unsigned)suite->passed, (unsigned)suite->failed,
            (unsigned)suite->skipped);
    fprintf(stream, "  TP=%u TN=%u FP=%u FN=%u\n",
            (unsigned)suite->true_positives, (unsigned)suite->true_negatives,
            (unsigned)suite->false_positives, (unsigned)suite->false_negatives);
}

void rtest_print_results(const RTestSuite *suite) {
    rtest_print_results_to(suite, stdout);
}

void rtest_suite_free(RTestSuite *suite) {
    size_t i;
    if (!suite) return;
    for (i = 0; i < suite->case_count; i++) {
        free(suite->cases[i].name);
        free(suite->cases[i].pattern);
        free(suite->cases[i].input);
        free(suite->cases[i].expected_group);
        free(suite->cases[i].error_message);
    }
    free(suite->cases);
    free(suite->name);
    free(suite);
}

bool rtest_assert_match(const char *pattern, const char *input,
                        const char *file, int line) {
    int matched = test_full_match(pattern, input, NULL);
    if (!matched) fprintf(stderr, "%s:%d: expected pattern to match\n", file, line);
    return matched != 0;
}

bool rtest_assert_no_match(const char *pattern, const char *input,
                           const char *file, int line) {
    int matched = test_full_match(pattern, input, NULL);
    if (matched) fprintf(stderr, "%s:%d: expected pattern not to match\n", file, line);
    return matched == 0;
}
