/**
engine/engine.h"
 * @file engine.c
 * @brief Implementation of the LibRift regex engine
 *
 * This file implements the main regex engine functionality, including
 * pattern compilation, matching, and associated utilities.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/engine/engine.h

engine/engine.h
tmp/librift_analysis/librift-4933472-W/backup/v1/src/engine/engine.h
tmp/librift_analysis/librift-4933472-W/backup/v2/engine/engine.h
tmp/librift_analysis/librift-4933472-W/src/core/engine/engine.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v1/src/engine/engine.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v2/engine/engine.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/engine/engine.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/engine/engine.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v1/src/engine/engine.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v2/engine/engine.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/engine/engine.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/engine/engine.h"
static bool
parse_pattern(const char *pattern, struct rift_regex_ast **ast, rift_regex_flags_t flags,
              rift_regex_error_t *error)
{
    if (!pattern || !ast) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
        }
        return false;
    }

    /* Allocate the AST structure */
    struct rift_regex_ast *new_ast = (struct rift_regex_ast *)malloc(sizeof(struct rift_regex_ast));
    if (!new_ast) {
        if (error) {
            error->code = RIFT_REGEX_ERROR_MEMORY_ALLOCATION;
        }
        return false;
    }

    /* Initialize AST structure */
    new_ast->root = NULL;
    new_ast->flags = flags;

    /* Check for r'' syntax and handle appropriately */
    bool is_rift_syntax = (pattern[0] == 'r' && (pattern[1] == '\'' || pattern[1] == '"'));
    const char *actual_pattern = pattern;

    if (is_rift_syntax) {
        /* Skip the 'r' prefix and quote */
        actual_pattern = pattern + 2;

        /* Find the closing quote */
        char quote_char = pattern[1];
        size_t len = strlen(actual_pattern);

        if (len > 0 && actual_pattern[len - 1] == quote_char) {
            /* Create a temporary copy without the closing quote */
            char *temp_pattern = strdup(actual_pattern);
            if (!temp_pattern) {
                free(new_ast);
                if (error) {
                    error->code = RIFT_REGEX_ERROR_MEMORY_ALLOCATION;
                }
                return false;
            }

            temp_pattern[len - 1] = '\0';

            /* Create AST root node for the pattern */
            new_ast->root = create_ast_node(RIFT_REGEX_AST_NODE_ROOT);
            if (!new_ast->root) {
                free(temp_pattern);
                free(new_ast);
                if (error) {
                    error->code = RIFT_REGEX_ERROR_MEMORY_ALLOCATION;
                }
                return false;
            }

            /* Parse the pattern without quotes */
            if (!build_ast_from_pattern(new_ast->root, temp_pattern, flags, error)) {
                free(temp_pattern);
                free_ast(new_ast);
                if (error && error->code == RIFT_REGEX_ERROR_NONE) {
                    error->code = RIFT_REGEX_ERROR_SYNTAX;
                }
                return false;
            }

            free(temp_pattern);
        } else {
            /* Unterminated r'' string */
            free(new_ast);
            if (error) {
                error->code = RIFT_REGEX_ERROR_SYNTAX;
            }
            return false;
        }
    } else {
        /* Create AST root node for the pattern */
        new_ast->root = create_ast_node(RIFT_REGEX_AST_NODE_ROOT);
        if (!new_ast->root) {
            free(new_ast);
            if (error) {
                error->code = RIFT_REGEX_ERROR_MEMORY_ALLOCATION;
            }
            return false;
        }

        /* Parse the standard pattern */
        if (!build_ast_from_pattern(new_ast->root, actual_pattern, flags, error)) {
            free_ast(new_ast);
            if (error && error->code == RIFT_REGEX_ERROR_NONE) {
                error->code = RIFT_REGEX_ERROR_SYNTAX;
            }
            return false;
        }
    }

    /* Set the output parameter */
    *ast = new_ast;
    return true;
}
