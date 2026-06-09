/**
 * @file rift_tokenize_command.h
 * @brief Tokenize command implementation for LibRift CLI
 *
 * This file defines the interface for the tokenize command, which
 * tokenizes input files using the LibRift tokenizer engine.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "librift/cli/command.h"
#ifndef LIBRIFT_CLI_TOKENIZE_COMMAND_H
#define LIBRIFT_CLI_TOKENIZE_COMMAND_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Tokenization command options structure
 */
typedef struct {
    char *input_file;         /**< Input file path */
    char *output_file;        /**< Output file path for tokenization results */
    char *rules_file;         /**< Custom tokenization rules file path */
    char *ignore_pattern;     /**< Pattern to ignore during tokenization */
    char *format;             /**< Output format (json, text) */
    bool case_sensitive;      /**< Whether tokenization is case sensitive */
    bool debug;               /**< Whether to include debug information */
    rift_regex_flags_t flags; /**< Tokenization flags */
} rift_tokenize_options_t;

/**
 * @brief Tokenize command implementation structure
 */
typedef struct {
    rift_command_t base;             /**< Base command structure */
    rift_tokenize_options_t options; /**< Tokenize command options */
    bool verbose;                    /**< Verbose output flag */
    bool quiet;                      /**< Quiet output flag */
} rift_tokenize_command_t;

/**
 * @brief Create a new tokenize command
 *
 * @return A new tokenize command instance or NULL on failure
 */
rift_command_t *rift_tokenize_command_create(void);

/**
 * @brief Get the options for a tokenize command
 *
 * @param command The tokenize command
 * @return Pointer to the tokenize options or NULL on failure
 */
rift_tokenize_options_t *rift_tokenize_command_get_options(rift_command_t *command);

/**
 * @brief Set the options for a tokenize command
 *
 * @param command The tokenize command
 * @param options The options to set
 * @return true if successful, false otherwise
 */
bool rift_tokenize_command_set_options(rift_command_t *command,
                                       const rift_tokenize_options_t *options);

/**
 * @brief Execute the tokenization process on a file
 *
 * @param options The tokenization options
 * @param verbose Whether to show verbose output
 * @param quiet Whether to suppress all output
 * @return int 0 on success, non-zero on failure
 */
int rift_tokenize_file(const rift_tokenize_options_t *options, bool verbose, bool quiet);

#ifdef __cplusplus
}
#endif

#endif /* LIBRIFT_CLI_TOKENIZE_COMMAND_H */