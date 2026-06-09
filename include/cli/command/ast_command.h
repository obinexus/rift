/**
 * @file ast_command.h
 * @brief Declaration of AST command for LibRift CLI
 *
 * This file declares the AST command implementation for the LibRift CLI,
 * which processes input files or parse trees to generate Abstract Syntax Trees.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "librift/cli/command.h"
#ifndef RIFT_AST_COMMAND_H
#define RIFT_AST_COMMAND_H


/**
 * @brief AST command options structure
 */
typedef struct {
    char *input_file;      /**< Input file path (source or parse tree) */
    char *output_file;     /**< Output file path for AST */
    char *format;          /**< Output format (json, dot, text) */
    char *transform_file;  /**< Transformation rules file */
    bool optimize;         /**< Apply AST optimizations */
    char *parse_tree_file; /**< Parse tree file to use as input (if not processing source) */
    bool visualize;        /**< Generate graphical AST view */
    int flags;             /**< AST builder flags */
} rift_ast_options_t;

/**
 * @brief AST command implementation
 */
typedef struct {
    rift_command_t base;        /**< Base command structure */
    rift_ast_options_t options; /**< AST command options */
    bool verbose;               /**< Verbose output flag */
    bool quiet;                 /**< Quiet mode flag */
} rift_ast_command_t;

/**
 * @brief Create a new AST command
 *
 * @return A new AST command instance or NULL on failure
 */
rift_command_t *rift_ast_command_create(void);

/**
 * @brief Get the options for an AST command
 *
 * @param command The AST command
 * @return Pointer to the AST options or NULL on error
 */
rift_ast_options_t *rift_ast_command_get_options(rift_command_t *command);

/**
 * @brief Set the options for an AST command
 *
 * @param command The AST command
 * @param options The options to set
 * @return true if successful, false otherwise
 */
bool rift_ast_command_set_options(rift_command_t *command, const rift_ast_options_t *options);

#endif /* RIFT_AST_COMMAND_H */