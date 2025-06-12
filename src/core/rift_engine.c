#include "../../include/rift.h"

RiftEngine* rift_engine_create(void) {
    RiftEngine* engine = calloc(1, sizeof(RiftEngine));
    if (!engine) return NULL;
    
    engine->token_capacity = 1024;
    engine->tokens = calloc(engine->token_capacity, sizeof(RiftToken*));
    
    return engine;
}

void rift_engine_destroy(RiftEngine* engine) {
    if (!engine) return;
    
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
    engine->input_length = strlen(input);
    engine->input_position = 0;
    engine->token_count = 0;
    
    size_t line = 1;
    size_t column = 1;
    
    for (size_t i = 0; i < engine->input_length; i++) {
        char c = input[i];
        
        if (c == ' ') {
            column++;
            continue;
        } else if (c == '\t') {
            column += 4;
            continue;
        } else if (c == '\n') {
            line++;
            column = 1;
            continue;
        }
        
        char value[2] = {c, '\0'};
        RiftToken* token = rift_token_create("CHAR", value, line, column);
        if (token) {
            if (engine->token_count >= engine->token_capacity) {
                engine->token_capacity *= 2;
                engine->tokens = realloc(engine->tokens, engine->token_capacity * sizeof(RiftToken*));
            }
            
            engine->tokens[engine->token_count] = token;
            engine->token_count++;
        }
        
        column++;
    }
    
    return RIFT_SUCCESS;
}

RiftToken* rift_engine_next_token(RiftEngine* engine) {
    if (!engine || engine->input_position >= engine->token_count) {
        return NULL;
    }
    
    return engine->tokens[engine->input_position++];
}

RiftToken* rift_token_create(const char* type, const char* value, size_t line, size_t column) {
    if (!type || !value) return NULL;
    
    RiftToken* token = calloc(1, sizeof(RiftToken));
    if (!token) return NULL;
    
    token->type = strdup(type);
    token->value = strdup(value);
    token->lexeme = strdup(value);
    token->line = line;
    token->column = column;
    token->position = 0;
    token->matched_state = NULL;
    
    return token;
}

void rift_token_destroy(RiftToken* token) {
    if (!token) return;
    
    free(token->type);
    free(token->value);
    free(token->lexeme);
    free(token);
}

RiftIR* rift_ir_create(const char* source_file) {
    if (!source_file) return NULL;
    
    RiftIR* ir = calloc(1, sizeof(RiftIR));
    if (!ir) return NULL;
    
    ir->stage = strdup("rift.0");
    ir->version = strdup("1.0.0");
    ir->source_file = strdup(source_file);
    ir->token_count = 0;
    ir->tokens = NULL;
    ir->metadata = strdup("# RIFT Intermediate Representation");
    
    return ir;
}

void rift_ir_destroy(RiftIR* ir) {
    if (!ir) return;
    
    free(ir->stage);
    free(ir->version);
    free(ir->source_file);
    free(ir->metadata);
    
    if (ir->tokens) {
        for (size_t i = 0; i < ir->token_count; i++) {
            rift_token_destroy(ir->tokens[i]);
        }
        free(ir->tokens);
    }
    
    free(ir);
}

RiftResult rift_ir_add_token(RiftIR* ir, RiftToken* token) {
    if (!ir || !token) return RIFT_ERROR_NULL_POINTER;
    
    ir->tokens = realloc(ir->tokens, (ir->token_count + 1) * sizeof(RiftToken*));
    if (!ir->tokens) return RIFT_ERROR_MEMORY_ALLOCATION;
    
    ir->tokens[ir->token_count] = token;
    ir->token_count++;
    
    return RIFT_SUCCESS;
}

RiftResult rift_ir_write_file(RiftIR* ir, const char* output_file) {
    if (!ir || !output_file) return RIFT_ERROR_NULL_POINTER;
    
    FILE* file = fopen(output_file, "w");
    if (!file) return RIFT_ERROR_IO;
    
    fprintf(file, "# RIFT Intermediate Representation\n");
    fprintf(file, "stage=%s\n", ir->stage);
    fprintf(file, "version=%s\n", ir->version);
    fprintf(file, "source=%s\n", ir->source_file);
    fprintf(file, "token_count=%zu\n\n", ir->token_count);
    fprintf(file, "# Tokens\n");
    
    for (size_t i = 0; i < ir->token_count; i++) {
        RiftToken* token = ir->tokens[i];
        fprintf(file, "TOKEN %zu: type=\"%s\" value=\"%s\" pos=(%zu,%zu)\n",
                i, token->type, token->value, token->line, token->column);
    }
    
    fclose(file);
    return RIFT_SUCCESS;
}

RiftResult rift_process_file(const char* input_file, const char* output_file, RiftConfig* config) {
    if (!input_file || !output_file) return RIFT_ERROR_NULL_POINTER;
    
    FILE* file = fopen(input_file, "r");
    if (!file) return RIFT_ERROR_IO;
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = malloc(file_size + 1);
    if (!content) {
        fclose(file);
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    fread(content, 1, file_size, file);
    content[file_size] = '\0';
    fclose(file);
    
    RiftEngine* engine = rift_engine_create();
    if (!engine) {
        free(content);
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    RiftResult result = rift_engine_process_input(engine, content);
    if (result != RIFT_SUCCESS) {
        free(content);
        rift_engine_destroy(engine);
        return result;
    }
    
    RiftIR* ir = rift_ir_create(input_file);
    if (!ir) {
        free(content);
        rift_engine_destroy(engine);
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    for (size_t i = 0; i < engine->token_count; i++) {
        RiftToken* token_copy = rift_token_create(
            engine->tokens[i]->type,
            engine->tokens[i]->value,
            engine->tokens[i]->line,
            engine->tokens[i]->column
        );
        rift_ir_add_token(ir, token_copy);
    }
    
    result = rift_ir_write_file(ir, output_file);
    
    free(content);
    rift_engine_destroy(engine);
    rift_ir_destroy(ir);
    
    return result;
}

const char* rift_result_string(RiftResult result) {
    switch (result) {
        case RIFT_SUCCESS: return "Success";
        case RIFT_ERROR_NULL_POINTER: return "Null pointer error";
        case RIFT_ERROR_MEMORY_ALLOCATION: return "Memory allocation error";
        case RIFT_ERROR_PATTERN_INVALID: return "Invalid pattern";
        case RIFT_ERROR_COMPILATION_FAILED: return "Compilation failed";
        case RIFT_ERROR_IO: return "I/O error";
        default: return "Unknown error";
    }
}

void rift_print_version(void) {
    printf("RIFT Stage 0 Tokenizer v1.0.0\n");
    printf("AEGIS Automaton Engine for Generative Interpretation & Syntax\n");
    printf("OBINexus Computing - \"Structure IS the syntax\"\n");
}

void rift_print_usage(const char* program_name) {
    printf("Usage: %s [options] <input_file>\n", program_name);
    printf("Options:\n");
    printf("  -h, --help     Show help\n");
    printf("  -v, --version  Show version\n");
}
