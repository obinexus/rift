/**
 * @file command_factory.h
 * @brief Command factory for LibRift CLI
 *
 * This file defines the command factory used to create command instances
 * based on command names or types.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include "librift/automaton/flags.h"
#include "librift/automaton/regex.h"
#include "librift/cli/command.h"
#include "librift/compiler/compiler.h"
#include "librift/errors/regex_error.h"
#include "librift/parser/ast.h"
#include "librift/parser/token.h"
#ifndef LIBRIFT_CLI_COMMAND_FACTORY_H
#define LIBRIFT_CLI_COMMAND_FACTORY_H



/* Forward declaration for types defined in command.h */
struct rift_command;
typedef struct rift_command rift_command_t;

typedef enum rift_command_type rift_command_type_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a command instance by name
 *
 * @param command_name Name of the command to create
 * @return Command instance or NULL if name is unknown
 */
rift_command_t *rift_command_factory_create(const char *command_name);

/**
 * @brief Create a command instance by type
 *
 * @param type Type of the command to create
 * @return Command instance or NULL if type is unknown
 */
rift_command_t *rift_command_factory_create_by_type(rift_command_type_t type);

/**
 * @brief Get the name of a command type
 *
 * @param type The command type
 * @return The command name
 */
const char *rift_command_factory_get_name(rift_command_type_t type);

/**
 * @brief Get the type for a command name
 *
 * @param command_name The command name
 * @return The command type
 */
rift_command_type_t rift_command_factory_get_type(const char *command_name);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_CLI_COMMAND_FACTORY_H */