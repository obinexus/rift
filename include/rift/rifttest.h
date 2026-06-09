#ifndef RIFT_RIFTTEST_H
#define RIFT_RIFTTEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "rift/platform.h"
#include "rift/regex.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum RTestResult {
    RTEST_PASS = 0,
    RTEST_FAIL,
    RTEST_ERROR,
    RTEST_SKIP
} RTestResult;

typedef enum RTestMatrixOutcome {
    RTEST_TRUE_POSITIVE = 0,
    RTEST_TRUE_NEGATIVE,
    RTEST_FALSE_POSITIVE,
    RTEST_FALSE_NEGATIVE
} RTestMatrixOutcome;

typedef struct RTestCase {
    char *name;
    char *pattern;
    char *input;
    bool should_match;
    char *expected_group;
    RTestResult result;
    RTestMatrixOutcome outcome;
    char *error_message;
} RTestCase;

typedef struct RTestSuite {
    char *name;
    RTestCase *cases;
    size_t case_count;
    size_t capacity;
    size_t passed;
    size_t failed;
    size_t skipped;
    size_t true_positives;
    size_t true_negatives;
    size_t false_positives;
    size_t false_negatives;
} RTestSuite;

RIFTTEST_API RTestCase rtest_case_create(const char *name,
                                          const char *pattern,
                                          const char *input,
                                          bool should_match);
RIFTTEST_API RTestSuite *rtest_suite_create(const char *name, size_t initial_capacity);
RIFTTEST_API bool rtest_suite_add(RTestSuite *suite, RTestCase test_case);
RIFTTEST_API RTestResult rtest_run_case(RTestCase *test_case);
RIFTTEST_API bool rtest_run_suite(RTestSuite *suite);
RIFTTEST_API void rtest_print_results(const RTestSuite *suite);
RIFTTEST_API void rtest_print_results_to(const RTestSuite *suite, FILE *stream);
RIFTTEST_API void rtest_suite_free(RTestSuite *suite);
RIFTTEST_API bool rtest_assert_match(const char *pattern, const char *input,
                                     const char *file, int line);
RIFTTEST_API bool rtest_assert_no_match(const char *pattern, const char *input,
                                        const char *file, int line);

#define RTEST_ASSERT_MATCH(pattern, input) \
    rtest_assert_match((pattern), (input), __FILE__, __LINE__)
#define RTEST_ASSERT_NO_MATCH(pattern, input) \
    rtest_assert_no_match((pattern), (input), __FILE__, __LINE__)

#ifdef __cplusplus
}
#endif

#endif /* RIFT_RIFTTEST_H */
