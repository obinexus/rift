/**
engine/engine.h"
 #include "core/errors/regex_error.h"
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
 #include "core/errors/regex_error.h"
 #include "core/errors/regex_error.h"
 #include "core/parser/ast.h"
 #include "core/parser/ast.h"
 #include "core/parser/ast.h"
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
#include "core/errors/error.h"
#include "core/errors/regex_error.h"
#include "core/parser/ast.h"
 #include "core/engine/engine.h"
 #include "core/errors/regex_error.h"
 #include "core/parser/ast.h"
 
 /**
  * @brief Parse a regex pattern into an abstract syntax tree
  *
  * @param pattern The pattern string to parse
  * @param ast Pointer to store the resulting AST
  * @param flags Compilation flags
  * @param error Error information (can be NULL)
  * @return true if parsing was successful, false otherwise
  */
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
             error->code = RIFT_REGEX_ERROR_MEMORY;
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
                     error->code = RIFT_REGEX_ERROR_MEMORY;
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
                     error->code = RIFT_REGEX_ERROR_MEMORY;
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
                 error->code = RIFT_REGEX_ERROR_MEMORY;
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
 
 /**
  * @brief Compile a regex pattern
  * 
  * @param pattern The pattern string
  * @param flags Compilation flags
  * @param error Error information (can be NULL)
  * @return Compiled regex pattern or NULL on failure
  */
 rift_regex_pattern_t *
 rift_regex_compile(const char *pattern, rift_regex_flags_t flags, rift_regex_error_t *error)
 {
     if (!pattern) {
         if (error) {
             error->code = RIFT_REGEX_ERROR_INVALID_PARAMETER;
             strncpy(error->message, "Pattern string is NULL", RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
         }
         return NULL;
     }
     
     /* Parse the pattern into an AST */
     struct rift_regex_ast *ast = NULL;
     if (!parse_pattern(pattern, &ast, flags, error)) {
         return NULL;
     }
     
     /* Allocate and initialize pattern structure */
     rift_regex_pattern_t *regex_pattern = (rift_regex_pattern_t *)malloc(sizeof(rift_regex_pattern_t));
     if (!regex_pattern) {
         free_ast(ast);
         if (error) {
             error->code = RIFT_REGEX_ERROR_MEMORY;
             strncpy(error->message, "Failed to allocate memory for pattern", RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
         }
         return NULL;
     }
     
     /* Initialize pattern fields */
     regex_pattern->ast = ast;
     regex_pattern->flags = flags;
     regex_pattern->source = strdup(pattern);
     
     if (!regex_pattern->source) {
         free_ast(ast);
         free(regex_pattern);
         if (error) {
             error->code = RIFT_REGEX_ERROR_MEMORY;
             strncpy(error->message, "Failed to allocate memory for pattern source", RIFT_REGEX_ERROR_MAX_MESSAGE_LENGTH - 1);
         }
         return NULL;
     }
     
     /* Build automaton from AST */
     regex_pattern->automaton = rift_automaton_from_ast(ast, flags, error);
     if (!regex_pattern->automaton) {
         free(regex_pattern->source);
         free_ast(ast);
         free(regex_pattern);
         /* Error already set by rift_automaton_from_ast */
         return NULL;
     }
     
     /* Count capture groups */
     regex_pattern->group_count = rift_regex_ast_count_groups(ast);
     
     /* Additional initialization based on flags */
     regex_pattern->is_rift_syntax = (pattern[0] == 'r' && (pattern[1] == '\'' || pattern[1] == '"'));
     regex_pattern->is_valid = true;
     regex_pattern->error_message = NULL;
     
     return regex_pattern;
 }
 
 /**
  * @brief Find matches in a string
  * 
  * @param pattern Compiled regex pattern
  * @param input Input string to search
  * @param match Match information to fill
  * @return true if a match was found, false otherwise
  */
 bool 
 rift_regex_find(rift_regex_pattern_t *pattern, const char *input, rift_regex_match_t *match)
 {
     if (!pattern || !input || !match) {
         return false;
     }
     
     /* Implement the pattern matching logic here */
     /* This will depend on your automaton implementation and matching algorithm */
     
     /* For now, just a placeholder implementation */
     return false;
 }
 
 /**
  * @brief Check if a string matches a pattern
  * 
  * @param pattern The pattern string
  * @param input Input string to match
  * @param flags Compilation flags
  * @return true if the string matches, false otherwise
  * */


 bool 
 rift_regex_matches(const char *pattern, const char *input, rift_regex_flags_t flags)
 {
     if (!pattern || !input) {
         return false;
     }
     
     /* Compile the pattern */
     rift_regex_error_t error;
     rift_regex_error_init(&error);
     
     rift_regex_pattern_t *compiled = rift_regex_compile(pattern, flags, &error);
     if (!compiled) {
         return false;
     }
     
     /* Create a match object */
     rift_regex_match_t match;
     memset(&match, 0, sizeof(match));
     
     /* Find a match */
     bool result = rift_regex_find(compiled, input, &match);
     
     /* Free resources */
     rift_regex_pattern_free(compiled);
     
     return result;
 }

/**
 * @brief Free a compiled regex pattern
 *
 * @param pattern The pattern to free
 */
void
rift_regex_pattern_free(rift_regex_pattern_t *pattern)
{
    if (pattern) {
        if (pattern->source) {
            free(pattern->source);
        }
        if (pattern->ast) {
            free_ast(pattern->ast);
        }
        if (pattern->automaton) {
            rift_automaton_free(pattern->automaton);
        }
        if (pattern->error_message) {
            free(pattern->error_message);
        }
        free(pattern);
    }
}  
