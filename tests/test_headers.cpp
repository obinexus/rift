#include <rift/rift.h>
#include <rift/riftlang.h>
#include <rift/rifttest.h>
#include <rift/riftbridge.h>

int main() {
    RIFTRegex regex{};
    return rift_regex_compile(&regex, "^cpp$") == 0
        ? (rift_regex_free(&regex), 0) : 1;
}
