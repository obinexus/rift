/**
 * @file command.h
 * @brief Command interface for LibRift CLI
 *
 * This file defines the command interface used by the LibRift CLI
 * to execute different regex engine operations.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "librift/automaton/automaton.h"
#include "librift/automaton/flags.h"
#include "librift/automaton/regex.h"
#include "librift/cli/command.h"
#include "librift/compiler/compiler.h"
#include "librift/errors/regex_error.h"
#include "librift/errors/regex_error_compat.h"
#include "librift/parser/ast.h"
#include "librift/parser/token.h"
#ifndef LIBRIFT_CLI_COMMAND_H
#define LIBRIFT_CLI_COMMAND_H



#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Command type enumeration
 */
typedef enum rift_command_type {
    RIFT_COMMAND_COMPILE,   /**< Compile a regex pattern */
    RIFT_COMMAND_MATCH,     /**< Match input against a pattern */
    RIFT_COMMAND_TOKENIZE,  /**< Tokenize input based on a pattern */
    RIFT_COMMAND_VISUALIZE, /**< Visualize a pattern automaton */
    RIFT_COMMAND_BENCHMARK, /**< Benchmark regex performance */
    RIFT_COMMAND_CONFIG,    /**< Manage configuration */
    RIFT_COMMAND_UNKNOWN    /**< Unknown command */
} rift_command_type_t;

/**
 * @brief Opaque command structure
 */
typedef struct rift_command rift_command_t;

/**
 * @brief Command type enumeration
 */
typedef enum rift_command_type {
    RIFT_COMMAND_COMPILE,   /**< Compile a regex pattern */
    RIFT_COMMAND_MATCH,     /**< Match input against a pattern */
    RIFT_COMMAND_VISUALIZE, /**< Visualize a pattern automaton */
    RIFT_COMMAND_BENCHMARK, /**< Benchmark regex performance */
    RIFT_COMMAND_CONFIG,    /**< Manage configuration */
    RIFT_COMMAND_UNKNOWN    /**< Unknown command */
} rift_command_type_t;

/**
 * @brief Get the type of a command
 *
 * @param command The command
 * @return The command type
 */
rift_command_type_t rift_command_get_type(const rift_command_t *command);

/**
 * @brief Set verbose flag for a command
 *
 * @param command The command
 * @param verbose Whether to enable verbose output
 */
void rift_command_set_verbose(rift_command_t *command, bool verbose);

/**
 * @brief Set quiet flag for a command
 *
 * @param command The command
 * @param quiet Whether to enable quiet mode
 */
void rift_command_set_quiet(rift_command_t *command, bool quiet);

/**
 * @brief Parse command-specific arguments
 *
 * @param command The command
 * @param argc Argument count
 * @param argv Argument vector
 * @return true if parsing was successful, false otherwise
 */
bool rift_command_parse_args(rift_command_t *command, int argc, char *argv[]);

/**
 * @brief Execute a command
 *
 * @param command The command
 * @return 0 on success, non-zero on failure
 */
int rift_command_execute(rift_command_t *command);

/**
 * @brief Get help information for a command
 *
 * @param command The command
 * @return Help string for the command
 */
const char *rift_command_get_help(const rift_command_t *command);

/**
 * @brief Free resources associated with a command
 *
 * @param command The command to free
 */
void rift_command_free(rift_command_t *command);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_CLI_COMMAND_H */