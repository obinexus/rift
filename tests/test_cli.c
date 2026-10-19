#include <stdio.h>
#include <string.h>
#include "rift_check.h"
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
    RIFT_CHECK(cmd != NULL);
    RIFT_CHECK(strcmp(cmd->name, "bridge") == 0);
    RIFT_CHECK(cmd->handler != NULL);
}

TEST(test_find_command_test) {
    const rift_command_t *cmd = rift_cli_find_command("test");
    RIFT_CHECK(cmd != NULL);
    RIFT_CHECK(strcmp(cmd->name, "test") == 0);
}

TEST(test_find_command_unknown) {
    const rift_command_t *cmd = rift_cli_find_command("nonexistent");
    RIFT_CHECK(cmd == NULL);
}

TEST(test_all_commands_exist) {
    const char *names[] = {
        "bridge", "test", "lang", "man", "doctor",
        "raf", "compile", "tokenize", "parse", "emit", NULL
    };
    for (int i = 0; names[i]; i++) {
        const rift_command_t *cmd = rift_cli_find_command(names[i]);
        RIFT_CHECK(cmd != NULL);
    }
}

/* parse, emit and raf must not report success they have not earned. */
TEST(test_unimplemented_commands) {
    const char *names[] = { "parse", "emit", "raf", NULL };
    for (int i = 0; names[i]; i++) {
        const rift_command_t *cmd = rift_cli_find_command(names[i]);
        char *bare[] = { (char *)names[i], NULL };
        char *args[] = { (char *)names[i], (char *)"--validate", (char *)"x", NULL };
        char *help[] = { (char *)names[i], (char *)"--help", NULL };
        RIFT_CHECK(cmd != NULL);
        RIFT_CHECK(cmd->handler(1, bare) == RIFT_CLI_EXIT_NOT_IMPLEMENTED);
        RIFT_CHECK(cmd->handler(3, args) == RIFT_CLI_EXIT_NOT_IMPLEMENTED);
        RIFT_CHECK(cmd->handler(2, help) == 0);
    }
}

int main(void) {
    printf("test_cli:\n");
    RUN(test_find_command_bridge);
    RUN(test_find_command_test);
    RUN(test_find_command_unknown);
    RUN(test_all_commands_exist);
    RUN(test_unimplemented_commands);
    printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
    return tests_failed;
}
