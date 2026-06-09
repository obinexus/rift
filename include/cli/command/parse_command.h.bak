/**
 * @file parse_command.h
 * @brief Declaration of parse command for LibRift CLI
 *
 * This file declares the parse command implementation for the LibRift CLI,
 * which processes input files or token files to generate parse trees.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "librift/cli/command.h"
#ifndef RIFT_PARSE_COMMAND_H
#define RIFT_PARSE_COMMAND_H


/**
 * @brief Parse command options structure
 */
typedef struct {
    char *input_file;   /**< Input file path (source or tokens) */
    char *output_file;  /**< Output file path for parse tree */
    char *format;       /**< Output format (json, dot, text) */
    char *grammar_file; /**< Custom grammar definition file */
    char *start_symbol; /**< Start symbol for parsing */
    char *tokens_file;  /**< Token file to use as input (if not parsing source) */
    bool debug;         /**< Enable debug output */
    bool recovery;      /**< Enable error recovery */
    int flags;          /**< Parser flags */
} rift_parse_options_t;

/**
 * @brief Parse command implementation
 */
typedef struct {
    rift_command_t base;          /**< Base command structure */
    rift_parse_options_t options; /**< Parse command options */
} rift_parse_command_t;

/**
 * @brief Create a new parse command
 *
 * @return A new parse command instance or NULL on failure
 */
rift_command_t *rift_parse_command_create(void);

/**
 * @brief Get the options for a parse command
 *
 * @param command The parse command
 * @return Pointer to the parse options or NULL on error
 */
rift_parse_options_t *rift_parse_command_get_options(rift_command_t *command);

/**
 * @brief Set the options for a parse command
 *
 * @param command The parse command
 * @param options The options to set
 * @return true if successful, false otherwise
 */
bool rift_parse_command_set_options(rift_command_t *command, const rift_parse_options_t *options);

#endif /* RIFT_PARSE_COMMAND_H */