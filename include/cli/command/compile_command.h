/**
 * @file compile_command.h
 * @brief Command implementation for compiling regex patterns
 *
 * This file defines the interface for the compile command, which compiles
 * regex patterns to their automaton representation.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "librift/automaton/flags.h"
#include "librift/automaton/regex.h"
#include "librift/cli/command.h"
#include "librift/cli/command_factory.h"
#include "librift/compiler/compiler.h"
#include "librift/core/error.h"
#include "librift/core/memory.h"
#include "librift/engine/pattern.h"
#include "librift/errors/regex_error.h"
#include "librift/errors/regex_error_compat.h"
#include "librift/parser/ast.h"
#include "librift/parser/ast_node.h"
#include "librift/parser/parser.h"
#include "librift/parser/token.h"
#include "librift/parser/token_type.h"
#include "librift/runtime/context.h"
#include "librift/runtime/groups.h"
#ifndef LIBRIFT_CLI_COMMANDS_COMPILE_COMMAND_H
#define LIBRIFT_CLI_COMMANDS_COMPILE_COMMAND_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compile command options structure
 */

typedef struct rift_compile_options rift_compile_options_t;
struct rift_compile_options {
    char *pattern;            /**< Regex pattern string */
    char *output_file;        /**< Output file path */
    bool use_rift_syntax;     /**< Whether to use LibRift r'' syntax */
    bool optimize;            /**< Whether to optimize the automaton */
    bool use_dfa;             /**< Whether to use DFA when possible */
    bool print_ast;           /**< Whether to print the AST */
    bool print_automaton;     /**< Whether to print the automaton */
    rift_regex_flags_t flags; /**< Compilation flags */
};

/**
 * @brief Command structure for compile command
 */
typedef struct rift_compile_command rift_compile_command_t;
struct rift_compile_command {
    rift_command_type_t type;       /**< Command type */
    bool verbose;                   /**< Verbose output flag */
    bool quiet;                     /**< Quiet mode flag */
    rift_compile_options_t options; /**< Command-specific options */
};

/**
 * @brief Create a new compile command instance
 *
 * @return A new compile command or NULL on failure
 */
rift_command_t *rift_compile_command_create(void);

/**
 * @brief Get the options for a compile command
 *
 * @param command The compile command
 * @return Pointer to the compile options or NULL on error
 */
rift_compile_options_t *rift_compile_command_get_options(rift_command_t *command);

/**
 * @brief Set the options for a compile command
 *
 * @param command The compile command
 * @param options The options to set
 * @return true if successful, false otherwise
 */
bool rift_compile_command_set_options(rift_command_t *command,
                                      const rift_compile_options_t *options);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_CLI_COMMANDS_COMPILE_COMMAND_H */