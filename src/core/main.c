#include "../../include/rift.h"

int main(void) {
    printf("🚀 RIFT Stage 0 - Enhanced Tokenization Demo\n");
    printf("📋 AEGIS Automaton Engine\n\n");
    
    RiftAutomaton* automaton = rift_automaton_create();
    printf("🔧 Adding RIFT pattern states to automaton...\n");
    
    rift_automaton_add_state(automaton, "[A-Z]+/gi[t]", false);
    rift_automaton_add_state(automaton, "[a-z]+/gi[t]", false);
    rift_automaton_add_state(automaton, "[0-9]+/gi[t]", true);
    
    printf("✅ Added 3 states to automaton\n\n");
    
    RiftEngine* engine = rift_engine_create();
    engine->automaton = automaton;
    
    const char* test_input = "HELLO123 world! #$% ABC";
    printf("📝 Processing input: \"%s\"\n\n", test_input);
    
    rift_engine_process_input(engine, test_input);
    
    printf("🎯 Generated tokens (TYPE + VALUE SEPARATE):\n");
    printf("================================================================================\n");
    
    for (size_t i = 0; i < engine->token_count; i++) {
        RiftToken* token = engine->tokens[i];
        printf("Token: %-15s | Value: %-20s | Position: (%zu,%zu)\n",
               token->type, token->value, token->line, token->column);
    }
    
    printf("================================================================================\n");
    printf("✅ Generated %zu tokens\n\n", engine->token_count);
    
    printf("📊 Automaton Summary:\n");
    printf("   States: %zu\n", automaton->state_count);
    printf("   Transitions: %zu\n", automaton->transition_count);
    printf("   Final states: %zu\n", automaton->final_count);
    
    printf("\n🎉 RIFT Stage 0 tokenization complete!\n");
    printf("💡 Ready for Stage 1 (Parsing/Grammar Analysis)\n");
    
    rift_engine_destroy(engine);
    rift_automaton_destroy(automaton);
    
    return 0;
}
