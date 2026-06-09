#include <assert.h>
#include <stdio.h>
#include "rift/regex.h"

int main(void) {
    RIFTRegex regex;
    int result = rift_regex_compile(&regex,
        "^[a-zA-Z_][a-zA-Z0-9_]*$");
    assert(result == 0);
    assert(rift_regex_match(&regex, "rift_token") == 0);
    assert(rift_regex_match(&regex, "9token") == 1);
    rift_regex_free(&regex);

    result = rift_regex_compile(&regex, "^[0-9]{2,4}$");
    assert(result == 0);
    assert(rift_regex_match(&regex, "123") == 0);
    assert(rift_regex_match(&regex, "1") == 1);
    rift_regex_free(&regex);

    result = rift_regex_compile(&regex, "(");
    assert(result != 0);
    puts("test_regex: PASS");
    return 0;
}
