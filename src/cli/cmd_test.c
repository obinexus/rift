#include <stdio.h>
#include <string.h>
#include "rift/cli.h"
#include "rift/rifttest.h"

int rift_cmd_test(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: rift test [options] [test-pattern]\n\n");
        printf("Run the RIFT test suite.\n\n");
        printf("Options:\n");
        printf("  --verbose    Show detailed test output\n");
        printf("  --help       Show this help message\n");
        return 0;
    }

    RTestSuite *suite = rtest_suite_create("RIFT CLI smoke tests", 4);
    int success;
    if (!suite) return 1;
    rtest_suite_add(suite, rtest_case_create(
        "identifier", "^[a-zA-Z_][a-zA-Z0-9_]*$", "rift_token", true));
    rtest_suite_add(suite, rtest_case_create(
        "reject leading digit", "^[a-zA-Z_][a-zA-Z0-9_]*$", "9token", false));
    success = rtest_run_suite(suite);
    rtest_print_results(suite);
    rtest_suite_free(suite);
    return success ? 0 : 1;
}
