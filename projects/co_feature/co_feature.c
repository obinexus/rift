/**
 * @file librift_demo.c
 * @brief Demonstration comparing YACC and LibRift for language feature development
 *
 * This file demonstrates how LibRift can be used to rapidly prototype
 * language features compared to traditional YACC/Bison approaches.
 *
 * Compile with: gcc -o librift_demo librift_demo.c -lrift
 *
 * @copyright Copyright (c) 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "librift/regex/automaton/automaton.h"
#include "librift/regex/engine/pattern.h"
#include "librift/regex/runtime/context.h"
#include "librift/regex/syntax/integration.h"

/* Function declarations */
void demonstrate_yacc_approach(void);
void demonstrate_librift_approach(void);
void demonstrate_feature_extension(void);

/**
 * @brief Main entry point
 */
int
main(int argc, char *argv[])
{
    printf("==========================================\n");
    printf("  YACC vs. LibRift Feature Development\n");
    printf("==========================================\n\n");

    printf("1. Traditional YACC/Bison Approach\n");
    printf("------------------------------------------\n");
    demonstrate_yacc_approach();

    printf("\n2. LibRift Automaton-Based Approach\n");
    printf("------------------------------------------\n");
    demonstrate_librift_approach();

    printf("\n3. Extending with New Feature: 'co function'\n");
    printf("------------------------------------------\n");
    demonstrate_feature_extension();

    return 0;
}

/**
 * @brief Demonstrate the YACC approach with pseudocode
 */
void
demonstrate_yacc_approach(void)
{
    printf("YACC Implementation (Pseudocode):\n\n");

    /* Display the lexer code */
    printf("/* lexer.l */\n");
    printf("%%{\n");
    printf("#include \"parser.h\"\n");
    printf("%%}\n\n");
    printf("%%option noyywrap\n\n");
    printf("%%\n");
    printf("\"function\"     { return FUNCTION; }\n");
    printf("[a-zA-Z][a-zA-Z0-9_]* { yylval.string = strdup(yytext); return IDENTIFIER; }\n");
    printf("\"(\"            { return LPAREN; }\n");
    printf("\")\"            { return RPAREN; }\n");
    printf("\"{\"            { return LBRACE; }\n");
    printf("\"}\"            { return RBRACE; }\n");
    printf("[ \\t\\n]        { /* Ignore whitespace */ }\n");
    printf(".              { /* Ignore other characters */ }\n");
    printf("%%\n\n");

    /* Display the parser code */
    printf("/* parser.y */\n");
    printf("%%{\n");
    printf("#include <stdio.h>\n");
    printf("#include <stdlib.h>\n");
    printf("%%}\n\n");
    printf("%%union {\n");
    printf("    char *string;\n");
    printf("}\n\n");
    printf("%%token FUNCTION LPAREN RPAREN LBRACE RBRACE\n");
    printf("%%token <string> IDENTIFIER\n\n");
    printf("%%\n");
    printf("program: function_declaration\n");
    printf("       ;\n\n");
    printf("function_declaration: FUNCTION IDENTIFIER LPAREN RPAREN LBRACE RBRACE\n");
    printf("                    { printf(\"Function declared: %%s\\n\", $2); free($2); }\n");
    printf("                    ;\n");
    printf("%%\n\n");

    /* Execution steps */
    printf("Compilation Steps:\n");
    printf("1. bison -d parser.y     # Generate parser.c and parser.h\n");
    printf("2. flex lexer.l          # Generate lex.yy.c\n");
    printf("3. gcc -o parser lex.yy.c parser.c -ly -lfl\n\n");

    /* Execution example */
    printf("Example execution:\n");
    printf("$ ./parser\n");
    printf("function test() {}\n");
    printf("Function declared: test\n");
}

/**
 * @brief Demonstrate the LibRift approach with actual code
 */
void
demonstrate_librift_approach(void)
{
    rift_regex_error_t error;
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_RIFT_SYNTAX;

    printf("LibRift Implementation:\n\n");

    /* Display the pattern definition */
    printf("// Define function declaration pattern using r'' syntax\n");
    printf("rift_regex_pattern_t *func_pattern = rift_regex_compile(\n");
    printf("    r'function\\s+([a-zA-Z][a-zA-Z0-9_]*)\\s*\\(\\s*\\)\\s*\\{\\s*\\}',\n");
    printf("    flags,\n");
    printf("    &error\n");
    printf(");\n\n");

    /* Create the pattern */
    rift_regex_pattern_t *func_pattern = rift_regex_compile(
        "function\\s+([a-zA-Z][a-zA-Z0-9_]*)\\s*\\(\\s*\\)\\s*\\{\\s*\\}", flags, &error);

    if (!func_pattern) {
        printf("Pattern compilation failed: %s\n", error.message);
        return;
    }

    /* Display the matching code */
    printf("// Create matcher and process input\n");
    printf("rift_regex_matcher_t *matcher = rift_matcher_create(func_pattern, "
           "RIFT_MATCHER_OPTION_NONE);\n");
    printf("const char *input = \"function test() {}\";\n");
    printf("rift_matcher_set_input(matcher, input, strlen(input));\n\n");

    printf("// Match and extract function name\n");
    printf("rift_regex_match_t match;\n");
    printf("if (rift_matcher_matches(matcher, &match)) {\n");
    printf("    printf(\"Function declared: %%s\\n\", match.groups[1].value);\n");
    printf("}\n\n");

    /* Create matcher and process input */
    rift_regex_matcher_t *matcher = rift_matcher_create(func_pattern, RIFT_MATCHER_OPTION_NONE);
    const char *input = "function test() {}";

    if (!rift_matcher_set_input(matcher, input, strlen(input))) {
        printf("Failed to set input\n");
        rift_regex_pattern_free(func_pattern);
        return;
    }

    /* Match and extract function name */
    rift_regex_match_t match;
    if (rift_matcher_matches(matcher, &match)) {
        printf("Actual execution result:\n");
        printf("Function declared: %s\n\n", match.groups[1].value);
    }

    /* Display compilation steps */
    printf("Compilation Steps:\n");
    printf("1. gcc -o librift_demo librift_demo.c -lrift\n\n");

    /* Cleanup */
    rift_match_free(&match);
    rift_matcher_free(matcher);
    rift_regex_pattern_free(func_pattern);
}

/**
 * @brief Demonstrate extending both approaches with a new feature
 */
void
demonstrate_feature_extension(void)
{
    rift_regex_error_t error;
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_RIFT_SYNTAX;

    /* Show YACC changes needed */
    printf("Extending YACC Implementation (Pseudocode):\n\n");

    printf("/* Modified lexer.l - new token */\n");
    printf("\"co\"          { return CO; }\n");
    printf("\"function\"    { return FUNCTION; }\n");
    printf("/* ... rest unchanged ... */\n\n");

    printf("/* Modified parser.y - new token and production */\n");
    printf("%%token CO FUNCTION LPAREN RPAREN LBRACE RBRACE\n");
    printf("/* ... */\n\n");
    printf("function_declaration: FUNCTION IDENTIFIER LPAREN RPAREN LBRACE RBRACE\n");
    printf("                    { printf(\"Function declared: %%s\\n\", $2); free($2); }\n");
    printf("                    | CO FUNCTION IDENTIFIER LPAREN RPAREN LBRACE RBRACE\n");
    printf("                    { printf(\"Co-Function declared: %%s\\n\", $3); free($3); }\n");
    printf("                    ;\n\n");

    printf("Recompilation Steps:\n");
    printf("1. Modify lexer.l and parser.y\n");
    printf("2. bison -d parser.y\n");
    printf("3. flex lexer.l\n");
    printf("4. gcc -o parser lex.yy.c parser.c -ly -lfl\n\n");

    /* Show LibRift changes needed */
    printf("Extending LibRift Implementation:\n\n");

    printf("// Extend pattern to include both regular and co-functions\n");
    printf("rift_regex_pattern_t *func_pattern = rift_regex_compile(\n");
    printf("    r'(?:co\\s+)?function\\s+([a-zA-Z][a-zA-Z0-9_]*)\\s*\\(\\s*\\)\\s*\\{\\s*\\}',\n");
    printf("    flags,\n");
    printf("    &error\n");
    printf(");\n\n");

    printf("// Create a pattern to check if it's a co-function\n");
    printf("rift_regex_pattern_t *co_check = rift_regex_compile(\n");
    printf("    r'co\\s+function',\n");
    printf("    flags,\n");
    printf("    &error\n");
    printf(");\n\n");

    /* Create the extended pattern */
    rift_regex_pattern_t *func_pattern = rift_regex_compile(
        "(?:co\\s+)?function\\s+([a-zA-Z][a-zA-Z0-9_]*)\\s*\\(\\s*\\)\\s*\\{\\s*\\}", flags,
        &error);

    if (!func_pattern) {
        printf("Pattern compilation failed: %s\n", error.message);
        return;
    }

    /* Create the co-function check pattern */
    rift_regex_pattern_t *co_check = rift_regex_compile("co\\s+function", flags, &error);

    if (!co_check) {
        printf("Pattern compilation failed: %s\n", error.message);
        rift_regex_pattern_free(func_pattern);
        return;
    }

    /* Show the matching code */
    printf("// In the matching code:\n");
    printf("if (rift_matcher_matches(matcher, &match)) {\n");
    printf("    // Check if it's a co-function\n");
    printf("    rift_regex_matcher_t *co_matcher = rift_matcher_create(co_check, "
           "RIFT_MATCHER_OPTION_NONE);\n");
    printf("    rift_matcher_set_input(co_matcher, input, strlen(input));\n");
    printf("    \n");
    printf("    if (rift_matcher_find_next(co_matcher, NULL)) {\n");
    printf("        printf(\"Co-Function declared: %%s\\n\", match.groups[1].value);\n");
    printf("    } else {\n");
    printf("        printf(\"Function declared: %%s\\n\", match.groups[1].value);\n");
    printf("    }\n");
    printf("    \n");
    printf("    rift_matcher_free(co_matcher);\n");
    printf("}\n\n");

    /* Test both cases */
    printf("Actual execution results:\n");

    /* Test regular function */
    const char *input1 = "function test1() {}";
    rift_regex_matcher_t *matcher1 = rift_matcher_create(func_pattern, RIFT_MATCHER_OPTION_NONE);
    rift_matcher_set_input(matcher1, input1, strlen(input1));

    rift_regex_match_t match1;
    if (rift_matcher_matches(matcher1, &match1)) {
        rift_regex_matcher_t *co_matcher = rift_matcher_create(co_check, RIFT_MATCHER_OPTION_NONE);
        rift_matcher_set_input(co_matcher, input1, strlen(input1));

        if (rift_matcher_find_next(co_matcher, NULL)) {
            printf("Co-Function declared: %s\n", match1.groups[1].value);
        } else {
            printf("Function declared: %s\n", match1.groups[1].value);
        }

        rift_matcher_free(co_matcher);
    }

    rift_match_free(&match1);
    rift_matcher_free(matcher1);

    /* Test co-function */
    const char *input2 = "co function test2() {}";
    rift_regex_matcher_t *matcher2 = rift_matcher_create(func_pattern, RIFT_MATCHER_OPTION_NONE);
    rift_matcher_set_input(matcher2, input2, strlen(input2));

    rift_regex_match_t match2;
    if (rift_matcher_matches(matcher2, &match2)) {
        rift_regex_matcher_t *co_matcher = rift_matcher_create(co_check, RIFT_MATCHER_OPTION_NONE);
        rift_matcher_set_input(co_matcher, input2, strlen(input2));

        if (rift_matcher_find_next(co_matcher, NULL)) {
            printf("Co-Function declared: %s\n", match2.groups[1].value);
        } else {
            printf("Function declared: %s\n", match2.groups[1].value);
        }

        rift_matcher_free(co_matcher);
    }

    rift_match_free(&match2);
    rift_matcher_free(matcher2);

    /* Comparative analysis */
    printf("\nComparative Analysis:\n");
    printf("1. YACC: Required modifying both lexer and parser files\n");
    printf("2. YACC: Required adding a new production rule\n");
    printf("3. YACC: Required complete recompilation of the toolchain\n");
    printf("4. LibRift: Single pattern modification to accommodate both cases\n");
    printf("5. LibRift: No need to regenerate parsers or recompile toolchain\n");
    printf("6. LibRift: More flexible for rapid prototyping of language features\n");

    /* Cleanup */
    rift_regex_pattern_free(func_pattern);
    rift_regex_pattern_free(co_check);
}