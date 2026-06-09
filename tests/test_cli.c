#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "rift/cli.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) static void name(void)
#define RUN(name) do { \
    printf("  %-40s", #name); \
    name(); \
    printf("PASS\n"); \
    tests_passed++; \
} while(0)

TEST(test_find_command_bridge) {
    const rift_command_t *cmd = rift_cli_find_command("bridge");
    assert(cmd != NULL);
    assert(strcmp(cmd->name, "bridge") == 0);
    assert(cmd->handler != NULL);
}

TEST(test_find_command_test) {
    const rift_command_t *cmd = rift_cli_find_command("test");
    assert(cmd != NULL);
    assert(strcmp(cmd->name, "test") == 0);
}

TEST(test_find_command_unknown) {
    const rift_command_t *cmd = rift_cli_find_command("nonexistent");
    assert(cmd == NULL);
}

TEST(test_all_commands_exist) {
    const char *names[] = {
        "bridge", "test", "lang", "man", "doctor",
        "raf", "compile", "tokenize", "parse", "emit", NULL
    };
    for (int i = 0; names[i]; i++) {
        const rift_command_t *cmd = rift_cli_find_command(names[i]);
        assert(cmd != NULL);
    }
}

int main(void) {
    printf("test_cli:\n");
    RUN(test_find_command_bridge);
    RUN(test_find_command_test);
    RUN(test_find_command_unknown);
    RUN(test_all_commands_exist);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed;
}
