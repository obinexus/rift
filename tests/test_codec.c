#include <stdio.h>
#include <string.h>
#include "rift_check.h"
#include "rift/codec.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) static void name(void)
#define RUN(name) do { \
    printf("  %-40s", #name); \
    name(); \
    printf("PASS\n"); \
    tests_passed++; \
} while(0)

TEST(test_link_empty) {
    rift_cir_program_t prog;
    int result = rift_link("", 0, &prog);
    RIFT_CHECK(result == -1);
}

TEST(test_link_basic) {
    rift_cir_program_t prog;
    const char *src = "let x = 1";
    int result = rift_link(src, strlen(src), &prog);
    RIFT_CHECK(result == 0);
    RIFT_CHECK(prog.node_count == 1);
    RIFT_CHECK(prog.nodes[0].kind == RIFT_CIR_ASSIGN);
}

TEST(test_codec_emit_unknown_target) {
    rift_cir_program_t prog;
    memset(&prog, 0, sizeof(prog));
    int result = rift_codec_emit(&prog, "unknown", NULL);
    RIFT_CHECK(result == -1);
}

int main(void) {
    printf("test_codec:\n");
    RUN(test_link_empty);
    RUN(test_link_basic);
    RUN(test_codec_emit_unknown_target);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed;
}
