/**
 * @file command_factory.c
 * @brief Implementation of the command factory for LibRift CLI
 *
 * This file implements the command factory that creates command instances
 * based on command names or types.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "cli/command/command_factory.h
#include <stdlib.h>
#include <string.h>
#include "cli/command/ast_command.h"
#include "cli/command/command.h"
#include "cli/command/compile_command.h"
#include "librift/cli/command_factory.h"
#include "librift/cli/command.h"
#include "librift/cli/commands/ast_command.h"
#include "librift/cli/commands/compile_command.h"

tmp/librift_analysis/librift-4933472-W/backup/v1/src/cli/command_factory.h
tmp/librift_analysis/librift-4933472-W/backup/v1/src/cli copy/command_factory.h
tmp/librift_analysis/librift-4933472-W/src/cli/command/command_factory.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/cli/command_factory.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/cli copy/command_factory.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/cli/command/command_factory.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/cli/command_factory.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/cli copy/command_factory.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/cli/command/command_factory.h"


/**
 * @brief Mapping between command names and types
 */
typedef struct command_mapping {
    const char *name;
    rift_command_type_t type;
} command_mapping_t;

/**
 * @brief Array of command mappings
static const command_mapping_t COMMAND_MAPPINGS[] = {
    {"compile", RIFT_COMMAND_COMPILE},     {"match", RIFT_COMMAND_MATCH},
    {"tokenize", RIFT_COMMAND_TOKENIZE},   {"visualize", RIFT_COMMAND_VISUALIZE},
    {"benchmark", RIFT_COMMAND_BENCHMARK}, {"config", RIFT_COMMAND_CONFIG},
    {"ast", RIFT_COMMAND_AST},             {"parse", RIFT_COMMAND_PARSE},
    {NULL, RIFT_COMMAND_UNKNOWN}};
    {NULL, RIFT_COMMAND_UNKNOWN}};



/**
 * @brief Get a command mapping by name
 *
 * @param name Command name
 * @return Mapping or NULL if not found
 */
static const command_mapping_t *
get_mapping_by_name(const char *name)
{
    if (!name) {
        return NULL;
    }

    for (size_t i = 0; COMMAND_MAPPINGS[i].name != NULL; i++) {
        if (strcmp(COMMAND_MAPPINGS[i].name, name) == 0) {
            return &COMMAND_MAPPINGS[i];
        }
    }

    return NULL;
}

/**
 * @brief Get a command mapping by type
 *
 * @param type Command type
 * @return Mapping or NULL if not found
 */
static const command_mapping_t *
get_mapping_by_type(rift_command_type_t type)
{
    for (size_t i = 0; COMMAND_MAPPINGS[i].name != NULL; i++) {
        if (COMMAND_MAPPINGS[i].type == type) {
            return &COMMAND_MAPPINGS[i];
        }
    }

    return NULL;
}

/**
 * @brief Create a command instance by name
 *
 * @param command_name Name of the command to create
 * @return Command instance or NULL if name is unknown
 */
rift_command_t *
rift_command_factory_create(const char *command_name)
{
    const command_mapping_t *mapping = get_mapping_by_name(command_name);
    if (!mapping) {
        return NULL;
    }

    return rift_command_factory_create_by_type(mapping->type);
}

/**
 * @brief Create a command instance by type
 *
 * @param type Type of the command to create
 * @return Command instance or NULL if type is unknown
 */
rift_command_t *
rift_command_factory_create_by_type(rift_command_type_t type)
{
    switch (type) {
    case RIFT_COMMAND_COMPILE:
        return rift_compile_command_create();
    case RIFT_COMMAND_MATCH:
        return NULL; /* Not implemented yet */
    case RIFT_COMMAND_TOKENIZE:
        return NULL; /* Not implemented yet */
    case RIFT_COMMAND_VISUALIZE:
        return NULL; /* Not implemented yet */
    case RIFT_COMMAND_BENCHMARK:
        return NULL; /* Not implemented yet */
    case RIFT_COMMAND_AST:
        return rift_ast_command_create();
    case RIFT_COMMAND_PARSE:
        return NULL; /* Not implemented yet */
    case RIFT_COMMAND_CONFIG:
        return NULL; /* Not implemented yet */
    case RIFT_COMMAND_UNKNOWN:
    default:
        return NULL; /* Unknown command type */
    }
}

/**

/**
 * @brief Get the name of a command type
 *
 * @param type The command type
 * @return The command name
 */
const char *
rift_command_factory_get_name(rift_command_type_t type)
{
    const command_mapping_t *mapping = get_mapping_by_type(type);
    if (!mapping) {
        return NULL;
    }

    return mapping->name;
}

/**
 * @brief Get the type for a command name
 *
 * @param command_name The command name
 * @return The command type
 */
rift_command_type_t
rift_command_factory_get_type(const char *command_name)
{
    const command_mapping_t *mapping = get_mapping_by_name(command_name);
    if (!mapping) {
        return RIFT_COMMAND_UNKNOWN;
    }

    return mapping->type;
}
