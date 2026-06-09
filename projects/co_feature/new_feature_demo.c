/**
 * @file new_feature_demo.c
 * @brief Demonstration of implementing a new language feature with LibRift
 *
 * This file demonstrates how to implement a new language feature using LibRift's
 * automaton-based approach. We will extend a simple language parser to support
 * a new 'co' keyword for coroutine-like functions.
 *
 * Compile with: gcc -o new_feature_demo new_feature_demo.c -lrift
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/regex/automaton/automaton.h"
#include "librift/regex/engine/pattern.h"
#include "librift/regex/runtime/context.h"
#include "librift/regex/syntax/integration.h"

/* Sample input for parsing */
const char *sample_code = "function regularFunction() {\n"
                          "  return 42;\n"
                          "}\n"
                          "\n"
                          "co function asyncFunction() {\n"
                          "  yield 10;\n"
                          "  return 20;\n"
                          "}\n"
                          "\n"
                          "function anotherFunction() {\n"
                          "  const x = 123;\n"
                          "  return x;\n"
                          "}\n";

/* Function declarations */
void parse_traditional_functions(const char *code);
void parse_with_co_extension(const char *code);
void demonstrate_extension_process(void);

/**
 * @brief Main entry point
 */
int
main(int argc, char *argv[])
{
    printf("==============================================\n");
    printf("  LibRift Language Feature Extension Demo\n");
    printf("==============================================\n\n");

    printf("Sample Code to Parse:\n");
    printf("----------------------------------------------\n");
    printf("%s\n", sample_code);

    printf("1. Traditional Function Parsing\n");
    printf("----------------------------------------------\n");
    parse_traditional_functions(sample_code);

    printf("\n2. Extended Parsing with 'co' Keyword\n");
    printf("----------------------------------------------\n");
    parse_with_co_extension(sample_code);

    printf("\n3. Feature Extension Development Process\n");
    printf("----------------------------------------------\n");
    demonstrate_extension_process();

    return 0;
}

/**
 * @brief Parse code with traditional function syntax only
 */
void
parse_traditional_functions(const char *code)
{
    rift_regex_error_t error;
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_RIFT_SYNTAX;

    /* Define traditional function pattern */
    printf("Using pattern: r'function\\s+([a-zA-Z][a-zA-Z0-9_]*)\\s*\\(\\)\\s*\\{[^}]*\\}'\n\n");

    rift_regex_pattern_t *func_pattern = rift_regex_compile(
        "function\\s+([a-zA-Z][a-zA-Z0-9_]*)\\s*\\(\\)\\s*\\{[^}]*\\}", flags, &error);

    if (!func_pattern) {
        printf("Pattern compilation failed: %s\n", error.message);
        return;
    }

    /* Create matcher and process input */
    rift_regex_matcher_t *matcher = rift_matcher_create(func_pattern, RIFT_MATCHER_OPTION_NONE);

    if (!rift_matcher_set_input(matcher, code, strlen(code))) {
        printf("Failed to set input\n");
        rift_regex_pattern_free(func_pattern);
        return;
    }

    /* Find all function declarations */
    rift_regex_match_t match;
    int function_count = 0;

    printf("Functions found:\n");
    while (rift_matcher_find_next(matcher, &match)) {
        if (match.group_count >= 1) {
            printf("%d. %s\n", ++function_count, match.groups[1].value);
        }

        /* Advance past this match for the next search */
        rift_matcher_set_position(matcher, match.end_pos);
        rift_match_free(&match);
    }

    /* Note that we missed the co function */
    printf("\nNote: With this pattern, we missed the 'co function asyncFunction()'\n");
    printf("because it doesn't match our pattern.\n");

    /* Cleanup */
    rift_matcher_free(matcher);
    rift_regex_pattern_free(func_pattern);
}

/**
 * @brief Parse code with support for 'co function' extension
 */
void
parse_with_co_extension(const char *code)
{
    rift_regex_error_t error;
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_RIFT_SYNTAX;

    /* Define extended function pattern */
    printf("Using extended pattern: "
           "r'(?:co\\s+)?function\\s+([a-zA-Z][a-zA-Z0-9_]*)\\s*\\(\\)\\s*\\{[^}]*\\}'\n\n");

    rift_regex_pattern_t *func_pattern = rift_regex_compile(
        "(?:co\\s+)?function\\s+([a-zA-Z][a-zA-Z0-9_]*)\\s*\\(\\)\\s*\\{[^}]*\\}", flags, &error);

    if (!func_pattern) {
        printf("Pattern compilation failed: %s\n", error.message);
        return;
    }

    /* Create a pattern to check if it's a co-function */
    rift_regex_pattern_t *co_check = rift_regex_compile("co\\s+function", flags, &error);

    if (!co_check) {
        printf("Pattern compilation failed: %s\n", error.message);
        rift_regex_pattern_free(func_pattern);
        return;
    }

    /* Create matcher and process input */
    rift_regex_matcher_t *matcher = rift_matcher_create(func_pattern, RIFT_MATCHER_OPTION_NONE);

    if (!rift_matcher_set_input(matcher, code, strlen(code))) {
        printf("Failed to set input\n");
        rift_regex_pattern_free(func_pattern);
        rift_regex_pattern_free(co_check);
        return;
    }

    /* Find all function declarations */
    rift_regex_match_t match;
    int regular_count = 0;
    int co_count = 0;

    printf("Functions found:\n");
    while (rift_matcher_find_next(matcher, &match)) {
        if (match.group_count >= 1) {
            /* Extract the matched function text */
            size_t len = match.end_pos - match.start_pos;
            char *func_text = (char *)malloc(len + 1);
            if (!func_text) {
                printf("Memory allocation failed\n");
                break;
            }
            strncpy(func_text, code + match.start_pos, len);
            func_text[len] = '\0';

            /* Check if it's a co-function */
            rift_regex_matcher_t *co_matcher =
                rift_matcher_create(co_check, RIFT_MATCHER_OPTION_NONE);
            rift_matcher_set_input(co_matcher, func_text, strlen(func_text));

            if (rift_matcher_find_next(co_matcher, NULL)) {
                printf("%d. [CO] %s\n", ++co_count, match.groups[1].value);
            } else {
                printf("%d. [Regular] %s\n", ++regular_count, match.groups[1].value);
            }

            /* Cleanup */
            rift_matcher_free(co_matcher);
            free(func_text);
        }

        /* Advance past this match for the next search */
        rift_matcher_set_position(matcher, match.end_pos);
        rift_match_free(&match);
    }

    printf("\nSummary: Found %d regular functions and %d co-functions\n", regular_count, co_count);

    /* Cleanup */
    rift_matcher_free(matcher);
    rift_regex_pattern_free(func_pattern);
    rift_regex_pattern_free(co_check);
}

/**
 * @brief Demonstrate the process of extending the language
 */
void
demonstrate_extension_process(void)
{
    printf("Step-by-Step Feature Extension Process:\n\n");

    /* Step 1: Identify the need for a new feature */
    printf("1. Identify the Need:\n");
    printf("   - We need to support coroutine-like functions with a 'co' keyword\n");
    printf("   - These functions should be parsed differently from regular functions\n\n");

    /* Step 2: Design the syntax */
    printf("2. Design the Syntax:\n");
    printf("   - 'co function name() { ... }' for coroutine functions\n");
    printf("   - Regular functions remain as 'function name() { ... }'\n\n");

    /* Step 3: Extend the pattern */
    printf("3. Extend the Automaton Pattern:\n");
    printf("   - Original: r'function\\s+([a-zA-Z][a-zA-Z0-9_]*)\\s*\\(\\)\\s*\\{[^}]*\\}'\n");
    printf("   - Extended: "
           "r'(?:co\\s+)?function\\s+([a-zA-Z][a-zA-Z0-9_]*)\\s*\\(\\)\\s*\\{[^}]*\\}'\n");
    printf("   - Note how we simply made the 'co' keyword optional using '(?:co\\s+)?'\n\n");

    /* Step 4: Add a discriminator */
    printf("4. Add a Discriminator Pattern:\n");
    printf("   - r'co\\s+function' to identify 'co' functions\n");
    printf("   - This lets us differentiate between regular and co functions\n");
    printf("   - No need to modify existing infrastructure or rebuild parsers\n\n");

    /* Step 5: Demonstrate semantic differences */
    printf("5. Implement Semantic Differences:\n");
    printf("   Code pseudocode for semantic actions:\n\n");
    printf("   ```c\n");
    printf("   // Check if it's a co-function and apply different semantics\n");
    printf("   if (is_co_function) {\n");
    printf("       // Process as coroutine - allow yield statements\n");
    printf("       process_as_coroutine(func_name);\n");
    printf("   } else {\n");
    printf("       // Process as regular function - no yields allowed\n");
    printf("       process_as_regular_function(func_name);\n");
    printf("   }\n");
    printf("   ```\n\n");

    /* Step 6: Integration benefits */
    printf("6. Integration Benefits:\n");
    printf("   - No need to modify lexer/parser grammar files\n");
    printf("   - No need to regenerate parsers\n");
    printf("   - No recompilation of the parsing infrastructure\n");
    printf("   - Changes can be applied dynamically at runtime\n");
    printf("   - Incremental feature adoption without breaking existing code\n\n");

    /* Step 7: Compare with traditional methods */
    printf("7. Comparison with YACC/Bison:\n");
    printf("   - YACC: Add new token to lexer, extend grammar, regenerate parser\n");
    printf("   - YACC: Need to carefully consider grammar conflicts\n");
    printf("   - YACC: Full recompilation required for each change\n");
    printf("   - LibRift: Pattern modification only, no recompilation\n");
    printf("   - LibRift: More flexible, dynamic approach to language extension\n");
}