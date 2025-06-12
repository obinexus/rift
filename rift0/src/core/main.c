#include "../../include/rift0/rift.h"

void print_token(RiftToken* token) {
    if (!token) return;
    
    printf("Token: %-15s | Value: %-20s | Position: (%zu,%zu)", 
           token->type, token->value, token->line, token->column);
    
    if (token->matched_state) {
        printf(" | State: %u", token->matched_state->id);
        if (token->matched_state->pattern) {
            printf(" | Pattern: %s | Mode: %c", 
                   token->matched_state->pattern->regex_pattern,
                   token->matched_state->pattern->mode);
        }
    }
    printf("\n");
}

int main(void) {
    printf("🚀 RIFT Stage 0 - Tokenization/Lexical Analysis Demo\n");
    printf("📋 AEGIS Automaton Engine for Generative Interpretation & Syntax\n\n");
    
    // Create RIFT engine
    RiftEngine* engine = rift_engine_create();
    if (!engine) {
        fprintf(stderr, "Failed to create RIFT engine\n");
        return 1;
    }
    
    // Add some example RIFT patterns
    printf("🔧 Adding RIFT pattern states to automaton...\n");
    
    // Pattern 1: Uppercase letters and digits (PRESERVE TYPE/VALUE SEPARATION)
    RiftState* state1 = rift_automaton_add_state(engine->automaton, 
        "R\"[A-Z0-9]+/gmi[t]\"", false);
    
    // Pattern 2: Whitespace (bottom-up mode, PRESERVE matched_state)
    RiftState* state2 = rift_automaton_add_state(engine->automaton,
        "R\"\\s+/gm[b]\"", false);
    
    // Pattern 3: Special characters (MAINTAIN token memory via matched_state)
    RiftState* state3 = rift_automaton_add_state(engine->automaton,
        "R\"[^A-Z0-9\\s]+/gmi[t]\"", true); // Final state
    
    if (state1 && state2 && state3) {
        printf("✅ Added %zu states to automaton\n\n", engine->automaton->state_count);
    } else {
        fprintf(stderr, "❌ Failed to add states\n");
        rift_engine_destroy(engine);
        return 1;
    }
    
    // Test input string
    const char* test_input = "HELLO123 world! #$% ABC";
    printf("📝 Processing input: \"%s\"\n\n", test_input);
    
    // Process the input
    RiftResult result = rift_engine_process_input(engine, test_input);
    if (result != RIFT_SUCCESS) {
        fprintf(stderr, "Failed to process input: %s\n", rift_result_string(result));
        rift_engine_destroy(engine);
        return 1;
    }
    
    printf("🎯 Generated tokens (TYPE + VALUE SEPARATE, matched_state PRESERVED):\n");
    printf("================================================================================\n");
    
    // Tokenize (CRITICAL: maintain type/value separation and matched_state)
    RiftToken* token;
    int token_count = 0;
    while ((token = rift_engine_next_token(engine)) != NULL) {
        // Verify architecture compliance
        if (!token->type || !token->value) {
            fprintf(stderr, "❌ ARCHITECTURE VIOLATION: Token missing type or value!\n");
            break;
        }
        
        print_token(token);
        rift_token_destroy(token);
        token_count++;
        
        if (token_count > 50) { // Safety limit
            printf("... (truncated for safety)\n");
            break;
        }
    }
    
    printf("================================================================================\n");
    printf("✅ Generated %d tokens\n\n", token_count);
    
    // Print automaton summary
    printf("📊 Automaton Summary:\n");
    printf("   States: %zu (Initial: %u, Final: %zu)\n", 
           engine->automaton->state_count,
           engine->automaton->initial_state ? engine->automaton->initial_state->id : 999,
           engine->automaton->final_count);
    printf("   Transitions: %zu\n", engine->automaton->transition_count);
    printf("   Alphabet: %s\n", engine->automaton->input_alphabet);
    
    // Cleanup
    rift_engine_destroy(engine);
    
    printf("\n🎉 RIFT Stage 0 tokenization complete!\n");
    printf("💡 Ready for Stage 1 (Parsing/Grammar Analysis)\n");
    printf("🏗️  Architecture: Token type/value separation maintained\n");
    printf("🧠 Token memory: matched_state preserved for AST minimization\n");
    
    return 0;
} 

