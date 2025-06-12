#include "../../include/rift0/rift.h"
#include <ctype.h>

RiftEngine* rift_engine_create(void) {
    RiftEngine* engine = calloc(1, sizeof(RiftEngine));
    if (!engine) return NULL;
    
    engine->automaton = rift_automaton_create();
    if (!engine->automaton) {
        free(engine);
        return NULL;
    }
    
    engine->token_capacity = 64;
    engine->tokens = calloc(engine->token_capacity, sizeof(RiftToken*));
    if (!engine->tokens) {
        rift_automaton_destroy(engine->automaton);
        free(engine);
        return NULL;
    }
    
    return engine;
}

void rift_engine_destroy(RiftEngine* engine) {
    if (!engine) return;
    
    rift_automaton_destroy(engine->automaton);
    
    for (size_t i = 0; i < engine->token_count; i++) {
        rift_token_destroy(engine->tokens[i]);
    }
    free(engine->tokens);
    
    free(engine->current_input);
    free(engine);
}

RiftResult rift_engine_process_input(RiftEngine* engine, const char* input) {
    if (!engine || !input) return RIFT_ERROR_NULL_POINTER;
    
    free(engine->current_input);
    engine->current_input = strdup(input);
    if (!engine->current_input) return RIFT_ERROR_MEMORY_ALLOCATION;
    
    engine->input_length = strlen(input);
    engine->input_position = 0;
    
    // Reset automaton state
    engine->automaton->current_state = engine->automaton->initial_state;
    
    return RIFT_SUCCESS;
}
// Fixed rift_process_file function in src/core/rift_engine.c
// This replaces the existing function to properly use the config parameter

RiftResult rift_process_file(const char* input_file, const char* output_file, RiftConfig* config) {
    if (!input_file || !output_file) return RIFT_ERROR_NULL_POINTER;
    
    // Use config for verbose/debug output
    bool verbose = config ? config->verbose : false;
    bool debug = config ? config->debug_mode : false;
    
    if (verbose) {
        printf("📁 Processing file: %s\n", input_file);
        printf("📁 Output file: %s\n", output_file);
        if (config && config->stage_name) {
            printf("📁 Stage: %s\n", config->stage_name);
        }
    }
    
    // Read input file
    FILE* file = fopen(input_file, "r");
    if (!file) {
        if (debug) {
            fprintf(stderr, "❌ Failed to open input file: %s\n", input_file);
        }
        return RIFT_ERROR_IO;
    }
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = malloc(file_size + 1);
    if (!content) {
        fclose(file);
        if (debug) {
            fprintf(stderr, "❌ Memory allocation failed for file content\n");
        }
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    fread(content, 1, file_size, file);
    content[file_size] = '\0';
    fclose(file);
    
    if (verbose) {
        printf("📊 Read %ld bytes from input file\n", file_size);
    }
    
    // Create engine and process
    RiftEngine* engine = rift_engine_create();
    if (!engine) {
        free(content);
        if (debug) {
            fprintf(stderr, "❌ Failed to create RIFT engine\n");
        }
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    RiftResult result = rift_engine_process_input(engine, content);
    if (result != RIFT_SUCCESS) {
        free(content);
        rift_engine_destroy(engine);
        if (debug) {
            fprintf(stderr, "❌ Engine processing failed: %s\n", rift_result_string(result));
        }
        return result;
    }
    
    if (verbose) {
        printf("🔤 Generated %zu tokens during processing\n", engine->token_count);
    }
    
    // Create IR and add tokens
    RiftIR* ir = rift_ir_create(input_file);
    if (!ir) {
        free(content);
        rift_engine_destroy(engine);
        if (debug) {
            fprintf(stderr, "❌ Failed to create IR structure\n");
        }
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    // Use config for IR metadata if available
    if (config) {
        if (config->stage_name) {
            free(ir->stage);
            ir->stage = strdup(config->stage_name);
        }
        
        // Add debug information to metadata if debug mode
        if (debug) {
            size_t metadata_len = strlen(ir->metadata) + 200;
            char* new_metadata = malloc(metadata_len);
            snprintf(new_metadata, metadata_len, 
                    "%s\n# Debug mode: enabled\n# Verbose: %s\n# Config loaded: true", 
                    ir->metadata, verbose ? "true" : "false");
            free(ir->metadata);
            ir->metadata = new_metadata;
        }
    }
    
    // Copy tokens to IR
    for (size_t i = 0; i < engine->token_count; i++) {
        // Create copy of token for IR
        RiftToken* token_copy = rift_token_create(
            engine->tokens[i]->type,
            engine->tokens[i]->value,
            engine->tokens[i]->line,
            engine->tokens[i]->column
        );
        if (token_copy) {
            rift_ir_add_token(ir, token_copy);
        }
    }
    
    if (verbose) {
        printf("📄 Writing IR with %zu tokens to output file\n", ir->token_count);
    }
    
    // Ensure output directory exists if specified in config
    if (config && config->output_dir) {
        // Create output directory if it doesn't exist
        char mkdir_cmd[1024];
        snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s", config->output_dir);
        system(mkdir_cmd);
        
        if (verbose) {
            printf("📁 Ensured output directory exists: %s\n", config->output_dir);
        }
    }
    
    // Write IR file
    result = rift_ir_write_file(ir, output_file);
    if (result != RIFT_SUCCESS && debug) {
        fprintf(stderr, "❌ Failed to write IR file: %s\n", rift_result_string(result));
    }
    
    if (verbose && result == RIFT_SUCCESS) {
        printf("✅ Successfully wrote IR file: %s\n", output_file);
    }
    
    // Cleanup
    free(content);
    rift_engine_destroy(engine);
    rift_ir_destroy(ir);
    
    return result;
}


RiftToken* rift_engine_next_token(RiftEngine* engine) {
    if (!engine || !engine->current_input || 
        engine->input_position >= engine->input_length) {
        return NULL;
    }
    
    // Skip whitespace (simple tokenizer)
    while (engine->input_position < engine->input_length && 
           isspace(engine->current_input[engine->input_position])) {
        engine->input_position++;
    }
    
    if (engine->input_position >= engine->input_length) {
        return NULL;
    }
    
    // Try to match against automaton states
    for (size_t i = 0; i < engine->automaton->state_count; i++) {
        RiftState* state = engine->automaton->states[i];
        if (!state || !state->pattern || !state->pattern->is_compiled) continue;
        
        regmatch_t match;
        const char* current_pos = engine->current_input + engine->input_position;
        
        if (regexec(&state->pattern->compiled_regex, current_pos, 1, &match, 0) == 0 &&
            match.rm_so == 0) { // Match at beginning
            
            size_t match_len = (size_t)(match.rm_eo - match.rm_so);
            char* value = calloc(match_len + 1, 1);
            if (!value) return NULL;
            
            strncpy(value, current_pos, match_len);
            
            RiftToken* token = rift_token_create("PATTERN_MATCH", value, 
                                               1, engine->input_position + 1);
            if (token) {
                token->matched_state = state;
                engine->input_position += match_len;
            }
            
            free(value);
            return token;
        }
    }
    
    // Fallback: single character token
    char single_char[2] = {engine->current_input[engine->input_position], '\0'};
    RiftToken* token = rift_token_create("CHAR", single_char, 
                                       1, engine->input_position + 1);
    if (token) {
        engine->input_position++;
    }
    
    return token;
}

RiftToken* rift_token_create(const char* type, const char* value, 
                           size_t line, size_t column) {
    RiftToken* token = calloc(1, sizeof(RiftToken));
    if (!token) return NULL;
    
    token->type = strdup(type ? type : "UNKNOWN");
    if (!token->type) {
        free(token);
        return NULL;
    }
    
    token->value = strdup(value ? value : "");
    if (!token->value) {
        free(token->type);
        free(token);
        return NULL;
    }
    
    token->lexeme = strdup(token->value);
    if (!token->lexeme) {
        free(token->type);
        free(token->value);
        free(token);
        return NULL;
    }
    
    token->line = line;
    token->column = column;
    
    return token;
}

void rift_token_destroy(RiftToken* token) {
    if (!token) return;
    
    free(token->type);
    free(token->value);
    free(token->lexeme);
    free(token);
}
