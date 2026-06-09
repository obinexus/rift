#include <stdio.h>
#include "librift/regex/syntax/integration.h"
#include "librift/regex/automaton/automaton.h"

int main() {
    // Create syntax context with r'' support enabled
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_RIFT_SYNTAX;
    rift_regex_syntax_context_t *context = rift_regex_syntax_context_create(flags);
    
    if (!context) {
        fprintf(stderr, "Failed to create syntax context\n");
        return 1;
    }
    
    // Parse and compile a pattern using r'' syntax
    const char *pattern = "r'[^A-Z0-9\\b]'";
    
    // Verify this is r'' syntax
    if (!rift_regex_syntax_is_rift_syntax(pattern)) {
        fprintf(stderr, "Expected r'' syntax pattern\n");
        rift_regex_syntax_context_free(context);
        return 1;
    }
    
    // Compile to AST
    rift_regex_ast_t *ast = rift_regex_syntax_compile(context, pattern);
    if (!ast) {
        fprintf(stderr, "Failed to compile pattern: %s\n", 
                rift_regex_syntax_get_error_message(context));
        rift_regex_syntax_context_free(context);
        return 1;
    }
    
    // Convert AST to automaton
    rift_regex_error_t error;
    rift_regex_automaton_t *automaton = 
        rift_regex_compile_ast(ast, flags, &error);
    
    if (!automaton) {
        fprintf(stderr, "Failed to create automaton: %s\n", 
                rift_regex_error_code_to_string(error));
        // Free AST (would be done by integration layer typically)
        // rift_regex_ast_free(ast);
        rift_regex_syntax_context_free(context);
        return 1;
    }
    
    // Use the automaton
    printf("Automaton created successfully from r'' syntax!\n");
    
    // Clean up
    rift_automaton_free(automaton);
    rift_regex_syntax_context_free(context);
    
    return 0;
}