#include "rift_check.h"
#include <stdio.h>
#include "rift/regex.h"

int main(void) {
    RIFTRegex regex;
    int result = rift_regex_compile(&regex,
        "^[a-zA-Z_][a-zA-Z0-9_]*$");
    RIFT_CHECK(result == 0);
    RIFT_CHECK(rift_regex_match(&regex, "rift_token") == 0);
    RIFT_CHECK(rift_regex_match(&regex, "9token") == 1);
    rift_regex_free(&regex);

    result = rift_regex_compile(&regex, "^[0-9]{2,4}$");
    RIFT_CHECK(result == 0);
    RIFT_CHECK(rift_regex_match(&regex, "123") == 0);
    RIFT_CHECK(rift_regex_match(&regex, "1") == 1);
    rift_regex_free(&regex);

    result = rift_regex_compile(&regex, "(");
    RIFT_CHECK(result != 0);
    puts("test_regex: PASS");
    return 0;
}
