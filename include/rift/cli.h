#ifndef RIFT_CLI_H
#define RIFT_CLI_H

#include "rift/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*rift_cmd_fn)(int argc, char **argv);

typedef struct rift_command {
    const char   *name;
    const char   *description;
    rift_cmd_fn   handler;
} rift_command_t;

/* Subcommand handlers */
int rift_cmd_bridge(int argc, char **argv);
int rift_cmd_test(int argc, char **argv);
int rift_cmd_lang(int argc, char **argv);
int rift_cmd_man(int argc, char **argv);
int rift_cmd_doctor(int argc, char **argv);
int rift_cmd_raf(int argc, char **argv);
int rift_cmd_compile(int argc, char **argv);
int rift_cmd_tokenize(int argc, char **argv);
int rift_cmd_parse(int argc, char **argv);
int rift_cmd_emit(int argc, char **argv);

/* Dispatch */
const rift_command_t *rift_cli_find_command(const char *name);
void                  rift_cli_print_usage(void);
void                  rift_cli_print_version(void);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CLI_H */
