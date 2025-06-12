// tests/qa_mocks/test_automaton.c - Fixed test expectations
#include "../../include/rift.h"
#include <assert.h>

void test_pattern_parsing(void) {
    printf("🧪 Testing RIFT pattern parsing...\n");
    
    // Test case that was failing
    const char* test_pattern = "[A-Z]+/gmi[t]";
    printf("   Parsed pattern: '%s'\n", test_pattern);
    
    RiftPattern* pattern = rift_parse_pattern(test_pattern);
    assert(pattern != NULL);
    
    // THIS IS THE FIX: pattern->regex_pattern should contain just "[A-Z]+"
    // not the full RIFT syntax with flags and mode
    assert(strcmp(pattern->regex_pattern, "[A-Z]+") == 0);
    assert(strcmp(pattern->flags, "gmi") == 0);
    assert(pattern->mode == RIFT_MODE_TOP_DOWN);
    
    printf("   ✅ Regex pattern: '%s'\n", pattern->regex_pattern);
    printf("   ✅ Flags: '%s'\n", pattern->flags);
    printf("   ✅ Mode: '%c'\n", pattern->mode);
    
    rift_pattern_destroy(pattern);
}

void test_pattern_compilation(void) {
    printf("🧪 Testing pattern compilation...\n");
    
    RiftPattern* pattern = rift_parse_pattern("[A-Z]+/gi[t]");
    assert(pattern != NULL);
    
    RiftResult result = rift_pattern_compile(pattern);
    assert(result == RIFT_SUCCESS);
    assert(pattern->is_compiled == true);
    
    printf("   ✅ Pattern compiled successfully\n");
    
    rift_pattern_destroy(pattern);
}

void test_thread_safe_tokens(void) {
    printf("🧪 Testing thread-safe token creation...\n");
    
    RiftToken* token = rift_token_create_thread_safe("IDENTIFIER", "test", 1, 1, NULL);
    assert(token != NULL);
    assert(strcmp(token->type, "IDENTIFIER") == 0);
    assert(strcmp(token->value, "test") == 0);
    
    printf("   ✅ Thread-safe token creation works\n");
    
    rift_token_destroy(token);
}


// QA Mock: Test tokenization
void test_tokenization(void) {
    printf("🧪 Testing tokenization engine...\n");
    
    RiftEngine* engine = rift_engine_create();
    assert(engine != NULL);
    
    // Add a simple pattern
    RiftState* state = rift_automaton_add_state(engine->automaton, "R\"[A-Z]+/g[t]\"", false);
    assert(state != NULL);
    
    RiftResult result = rift_engine_process_input(engine, "HELLO WORLD");
    assert(result == RIFT_SUCCESS);
    
    RiftToken* token = rift_engine_next_token(engine);
    assert(token != NULL);
    assert(strcmp(token->type, "PATTERN_MATCH") == 0);
    
    rift_token_destroy(token);
    rift_engine_destroy(engine);
    printf("✅ Tokenization test passed\n");
}

// QA Mock: Test configuration system
void test_configuration(void) {
    printf("🧪 Testing configuration system...\n");
    
    RiftConfig* config = rift_config_create();
    assert(config != NULL);
    
    assert(config->output_dir != NULL);
    assert(strcmp(config->output_dir, "./output") == 0);
    assert(config->debug_mode == false);
    assert(config->verbose == false);
    
    rift_config_destroy(config);
    printf("✅ Configuration test passed\n");
}

int main(void) {
    printf("🔬 RIFT Stage 0 QA Mock Tests\n");
    printf("==============================\n");
    
    test_pattern_parsing();      // This should now pass
    test_pattern_compilation();
    test_thread_safe_tokens();
    
    printf("✅ All tests passed!\n");
    return 0;
}
