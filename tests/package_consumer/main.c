#include <rift/rift.h>
#include <rift/riftlang.h>
#include <rift/rifttest.h>

int main(void) {
    RIFTRegex regex;
    if (rift_regex_compile(&regex, "^package$") != 0) return 1;
    if (rift_regex_match(&regex, "package") != 0) return 2;
    rift_regex_free(&regex);
    return 0;
}
