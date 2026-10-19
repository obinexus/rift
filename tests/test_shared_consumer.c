#include "rift_check.h"
#include <stdlib.h>
#include <string.h>
#include "rift/rift.h"
#include "rift/riftlang.h"
#include "rift/rifttest.h"

int main(void) {
    RIFTRegex regex;
    RLangContext *context;
    RLangCompileResult result;
    RTestSuite *suite;
    int compiled;
    bool added;

    compiled = rift_regex_compile(&regex, "^shared$");
    RIFT_CHECK(compiled == 0);
    RIFT_CHECK(rift_regex_match(&regex, "shared") == 0);
    rift_regex_free(&regex);

    context = rlang_context_create();
    result = rlang_compile(context, "pattern shared = r\"^shared$\"");
    RIFT_CHECK(result.success);
    rlang_program_free(result.program);
    rlang_context_free(context);

    suite = rtest_suite_create("shared", 1);
    RIFT_CHECK(suite != NULL);
    added = rtest_suite_add(suite, rtest_case_create(
        "shared match", "^shared$", "shared", true));
    RIFT_CHECK(added);
    RIFT_CHECK(rtest_run_suite(suite));
    rtest_suite_free(suite);
    return 0;
}
