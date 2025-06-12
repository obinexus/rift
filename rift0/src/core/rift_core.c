// rift_core.c - Consolidated core library (prevents multiple definitions)
#include "../../include/rift.h"

// ===== CONFIGURATION FUNCTIONS =====
RiftConfig* rift_config_create(void) {
    RiftConfig* config = calloc(1, sizeof(RiftConfig));
    if (!config) return NULL;
    
    config->output_dir = strdup("./output");
    config->stage_name = strdup("rift.0");
    config->debug_mode = false;
    config->verbose = false;
    config->log_level = strdup("info");
    
    return config;
}

void rift_config_destroy(RiftConfig* config) {
    if (!config) return;
    
    free(config->output_dir);
    free(config->stage_name);
    free(config->log_level);
    free(config);
}

RiftResult rift_config_load(RiftConfig* config, const char* config_file) {
    if (!config || !config_file) return RIFT_ERROR_NULL_POINTER;
    
    FILE* file = fopen(config_file, "r");
    if (!file) return RIFT_ERROR_IO;
    
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        char* equals = strchr(line, '=');
        if (!equals) continue;
        
        *equals = '\0';
        char* key = line;
        char* value = equals + 1;
        
        char* newline = strchr(value, '\n');
        if (newline) *newline = '\0';
        
        if (strcmp(key, "output_dir") == 0) {
            free(config->output_dir);
            config->output_dir = strdup(value);
        } else if (strcmp(key, "stage_name") == 0) {
            free(config->stage_name);
            config->stage_name = strdup(value);
        } else if (strcmp(key, "debug") == 0) {
            config->debug_mode = (strcmp(value, "true") == 0);
        } else if (strcmp(key, "verbose") == 0) {
            config->verbose = (strcmp(value, "true") == 0);
        } else if (strcmp(key, "log_level") == 0) {
            free(config->log_level);
            config->log_level = strdup(value);
        }
    }
    
    fclose(file);
    return RIFT_SUCCESS;
}

// ===== PATTERN FUNCTIONS =====
RiftPattern* rift_parse_pattern(const char* pattern_string) {
    if (!pattern_string) return NULL;
    
    RiftPattern* pattern = calloc(1, sizeof(RiftPattern));
    if (!pattern) return NULL;
    
    pattern->raw_pattern = strdup(pattern_string);
    pattern->is_compiled = false;
    
    const char* input = pattern_string;
    
    if (strncmp(input, "R\"", 2) == 0) {
        input += 2;
    } else if (*input == '"') {
        input += 1;
    }
    
    const char* pattern_end = strchr(input, '/');
    if (pattern_end) {
        pattern->regex_pattern = strndup(input, pattern_end - input);
        
        const char* flags_start = pattern_end + 1;
        const char* flags_end = strchr(flags_start, '[');
        if (!flags_end) flags_end = strchr(flags_start, '"');
        if (!flags_end) flags_end = flags_start + strlen(flags_start);
        
        pattern->flags = strndup(flags_start, flags_end - flags_start);
        
        const char* mode_start = strchr(flags_start, '[');
        if (mode_start && mode_start[1] && mode_start[2] == ']') {
            pattern->mode = (RiftMatchMode)mode_start[1];
        } else {
            pattern->mode = RIFT_MODE_TOP_DOWN;
        }
    } else {
        const char* end_quote = strchr(input, '"');
        if (end_quote) {
            pattern->regex_pattern = strndup(input, end_quote - input);
        } else {
            pattern->regex_pattern = strdup(input);
        }
        pattern->flags = strdup("");
        pattern->mode = RIFT_MODE_TOP_DOWN;
    }
    
    return pattern;
}

void rift_pattern_destroy(RiftPattern* pattern) {
    if (!pattern) return;
    
    if (pattern->is_compiled) {
        regfree(&pattern->compiled_regex);
    }
    
    free(pattern->raw_pattern);
    free(pattern->regex_pattern);
    free(pattern->flags);
    free(pattern);
}

RiftResult rift_pattern_compile(RiftPattern* pattern) {
    if (!pattern || !pattern->regex_pattern) {
        return RIFT_ERROR_NULL_POINTER;
    }
    
    if (pattern->is_compiled) {
        regfree(&pattern->compiled_regex);
    }
    
    int regex_flags = REG_EXTENDED;
    if (pattern->flags && strchr(pattern->flags, 'i')) {
        regex_flags |= REG_ICASE;
    }
    
    int result = regcomp(&pattern->compiled_regex, pattern->regex_pattern, regex_flags);
    if (result != 0) {
        return RIFT_ERROR_COMPILATION_FAILED;
    }
    
    pattern->is_compiled = true;
    return RIFT_SUCCESS;
}

// ===== AUTOMATON FUNCTIONS =====
RiftAutomaton* rift_automaton_create(void) {
    RiftAutomaton* automaton = calloc(1, sizeof(RiftAutomaton));
    if (!automaton) return NULL;
    
    automaton->state_capacity = 16;
    automaton->states = calloc(automaton->state_capacity, sizeof(RiftState*));
    
    automaton->transition_capacity = 32;
    automaton->transitions = calloc(automaton->transition_capacity, sizeof(RiftTransition*));
    
    automaton->final_capacity = 8;
    automaton->final_states = calloc(automaton->final_capacity, sizeof(RiftState*));
    
    automaton->input_alphabet = strdup("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-+*/=<>!&|(){}[];,\"' \t\n");
    
    return automaton;
}

void rift_automaton_destroy(RiftAutomaton* automaton) {
    if (!automaton) return;
    
    for (size_t i = 0; i < automaton->state_count; i++) {
        if (automaton->states[i]) {
            free(automaton->states[i]->pattern_string);
            rift_pattern_destroy(automaton->states[i]->pattern);
            free(automaton->states[i]);
        }
    }
    free(automaton->states);
    
    for (size_t i = 0; i < automaton->transition_count; i++) {
        if (automaton->transitions[i]) {
            free(automaton->transitions[i]->input_symbol);
            free(automaton->transitions[i]);
        }
    }
    free(automaton->transitions);
    
    free(automaton->final_states);
    free(automaton->input_alphabet);
    free(automaton);
}

RiftState* rift_automaton_add_state(RiftAutomaton* automaton, const char* pattern_str, bool is_final) {
    if (!automaton || !pattern_str) return NULL;
    
    if (automaton->state_count >= automaton->state_capacity) {
        automaton->state_capacity *= 2;
        automaton->states = realloc(automaton->states, automaton->state_capacity * sizeof(RiftState*));
    }
    
    RiftState* state = calloc(1, sizeof(RiftState));
    if (!state) return NULL;
    
    state->id = (uint32_t)automaton->state_count;
    state->pattern_string = strdup(pattern_str);
    state->pattern = rift_parse_pattern(pattern_str);
    state->is_final = is_final;
    state->is_initial = (automaton->state_count == 0);
    
    if (state->pattern) {
        rift_pattern_compile(state->pattern);
    }
    
    automaton->states[automaton->state_count] = state;
    automaton->state_count++;
    
    if (state->is_initial) {
        automaton->initial_state = state;
    }
    
    if (is_final) {
        if (automaton->final_count >= automaton->final_capacity) {
            automaton->final_capacity *= 2;
            automaton->final_states = realloc(automaton->final_states, automaton->final_capacity * sizeof(RiftState*));
        }
        automaton->final_states[automaton->final_count] = state;
        automaton->final_count++;
    }
    
    return state;
}

RiftResult rift_automaton_add_transition(RiftAutomaton* automaton, RiftState* from, RiftState* to, const char* input_symbol) {
    if (!automaton || !from || !to || !input_symbol) return RIFT_ERROR_NULL_POINTER;
    
    if (automaton->transition_count >= automaton->transition_capacity) {
        automaton->transition_capacity *= 2;
        automaton->transitions = realloc(automaton->transitions, automaton->transition_capacity * sizeof(RiftTransition*));
    }
    
    RiftTransition* transition = calloc(1, sizeof(RiftTransition));
    if (!transition) return RIFT_ERROR_MEMORY_ALLOCATION;
    
    transition->from_state = from;
    transition->to_state = to;
    transition->input_symbol = strdup(input_symbol);
    
    automaton->transitions[automaton->transition_count] = transition;
    automaton->transition_count++;
    
    return RIFT_SUCCESS;
}

// ===== ENGINE FUNCTIONS =====
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
                engine->tokens = realloc(engine->tokens, 
                                       engine->token_capacity * sizeof(RiftToken*));
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

// ===== TOKEN FUNCTIONS =====
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

// ===== IR FUNCTIONS =====
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

// ===== FILE PROCESSING =====
RiftResult rift_process_file(const char* input_file, const char* output_file, RiftConfig* config) {
    if (!input_file || !output_file) return RIFT_ERROR_NULL_POINTER;
    
    bool verbose = config ? config->verbose : false;
    bool debug = config ? config->debug_mode : false;
    
    if (verbose) {
        printf("📁 Processing: %s -> %s\n", input_file, output_file);
    }
    
    FILE* file = fopen(input_file, "r");
    if (!file) {
        if (debug) {
            fprintf(stderr, "❌ Failed to open: %s\n", input_file);
        }
        return RIFT_ERROR_IO;
    }
    
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
    
    if (verbose) {
        printf("📊 Read %ld bytes\n", file_size);
    }
    
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
    
    if (verbose) {
        printf("🔤 Generated %zu tokens\n", engine->token_count);
    }
    
    RiftIR* ir = rift_ir_create(input_file);
    if (!ir) {
        free(content);
        rift_engine_destroy(engine);
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    if (config && config->stage_name) {
        free(ir->stage);
        ir->stage = strdup(config->stage_name);
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
    
    if (verbose && result == RIFT_SUCCESS) {
        printf("✅ Generated IR: %s\n", output_file);
    }
    
    free(content);
    rift_engine_destroy(engine);
    rift_ir_destroy(ir);
    
    return result;
}

// ===== UTILITY FUNCTIONS (ONLY DEFINED ONCE) =====
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
    printf("  --verbose      Enable verbose output\n");
}
