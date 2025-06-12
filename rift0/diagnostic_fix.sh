#!/bin/bash
# RIFT Stage 0 Diagnostic and Fix Script
# Diagnose and fix library linking issues

set -e

echo "🔍 RIFT Stage 0 Diagnostic and Fix"
echo "==================================="
echo ""

# Step 1: Clean everything first
echo "🧹 Cleaning all build artifacts..."
rm -rf build bin lib output 2>/dev/null || true
make clean 2>/dev/null || true

# Step 2: Check what source files actually exist
echo ""
echo "📋 Checking existing source structure..."
echo "----------------------------------------"

if [ ! -d "src/core" ]; then
    echo "❌ Missing src/core directory"
    mkdir -p src/core
fi

if [ ! -d "src/cli" ]; then
    echo "❌ Missing src/cli directory"  
    mkdir -p src/cli
fi

if [ ! -d "src/cli/command" ]; then
    echo "❌ Missing src/cli/command directory"
    mkdir -p src/cli/command
fi

echo "📁 Source files found:"
find src -name "*.c" -type f | sort
echo ""

# Step 3: Check if we have the essential core files
MISSING_FILES=()

if [ ! -f "src/core/rift_config.c" ]; then
    MISSING_FILES+=("src/core/rift_config.c")
fi

if [ ! -f "src/core/rift_engine.c" ]; then
    MISSING_FILES+=("src/core/rift_engine.c")
fi

if [ ! -f "src/core/rift_pattern.c" ]; then
    MISSING_FILES+=("src/core/rift_pattern.c")
fi

if [ ! -f "src/core/rift_automaton.c" ]; then
    MISSING_FILES+=("src/core/rift_automaton.c")
fi

if [ ${#MISSING_FILES[@]} -gt 0 ]; then
    echo "❌ Missing essential core files:"
    printf '%s\n' "${MISSING_FILES[@]}"
    echo ""
    echo "🔧 Creating missing core files..."
    
    # Create essential core files
    if [ ! -f "src/core/rift_config.c" ]; then
        echo "📝 Creating src/core/rift_config.c..."
        cat > src/core/rift_config.c << 'EOF'
#include "../../include/rift.h"

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
EOF
    fi
    
    if [ ! -f "src/core/rift_pattern.c" ]; then
        echo "📝 Creating src/core/rift_pattern.c..."
        cat > src/core/rift_pattern.c << 'EOF'
#include "../../include/rift.h"

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
EOF
    fi
    
    if [ ! -f "src/core/rift_automaton.c" ]; then
        echo "📝 Creating src/core/rift_automaton.c..."
        cat > src/core/rift_automaton.c << 'EOF'
#include "../../include/rift.h"

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
EOF
    fi
    
    echo "✅ Created missing core files"
fi

# Step 4: Check if we have the header file
if [ ! -f "include/rift.h" ]; then
    echo "📝 Creating include/rift.h..."
    mkdir -p include
    cat > include/rift.h << 'EOF'
#ifndef RIFT_H
#define RIFT_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef _POSIX_C_SOURCE  
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <errno.h>

typedef enum {
    RIFT_MODE_TOP_DOWN = 't',
    RIFT_MODE_BOTTOM_UP = 'b'
} RiftMatchMode;

typedef enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_NULL_POINTER = -1,
    RIFT_ERROR_MEMORY_ALLOCATION = -2,
    RIFT_ERROR_PATTERN_INVALID = -3,
    RIFT_ERROR_COMPILATION_FAILED = -4,
    RIFT_ERROR_IO = -5
} RiftResult;

typedef struct RiftPattern {
    char* raw_pattern;
    char* regex_pattern;
    char* flags;
    RiftMatchMode mode;
    regex_t compiled_regex;
    bool is_compiled;
} RiftPattern;

typedef struct RiftState {
    uint32_t id;
    char* pattern_string;
    RiftPattern* pattern;
    bool is_final;
    bool is_initial;
    void* semantic_data;
} RiftState;

typedef struct RiftTransition {
    RiftState* from_state;
    char* input_symbol;
    RiftState* to_state;
    bool (*condition)(const char* input, size_t pos);
} RiftTransition;

typedef struct RiftAutomaton {
    RiftState** states;
    size_t state_count;
    size_t state_capacity;
    RiftTransition** transitions;
    size_t transition_count;
    size_t transition_capacity;
    RiftState* initial_state;
    RiftState** final_states;
    size_t final_count;
    size_t final_capacity;
    RiftState* current_state;
    char* input_alphabet;
} RiftAutomaton;

typedef struct RiftToken {
    char* type;
    char* value;
    char* lexeme;
    size_t line;
    size_t column;
    size_t position;
    RiftState* matched_state;
} RiftToken;

typedef struct RiftEngine {
    RiftAutomaton* automaton;
    RiftToken** tokens;
    size_t token_count;
    size_t token_capacity;
    char* current_input;
    size_t input_position;
    size_t input_length;
} RiftEngine;

typedef struct RiftConfig {
    char* output_dir;
    char* stage_name;
    bool debug_mode;
    bool verbose;
    char* log_level;
} RiftConfig;

typedef struct RiftIR {
    char* stage;
    char* version;
    char* source_file;
    RiftToken** tokens;
    size_t token_count;
    char* metadata;
} RiftIR;

// Core API Functions
RiftConfig* rift_config_create(void);
void rift_config_destroy(RiftConfig* config);
RiftResult rift_config_load(RiftConfig* config, const char* config_file);

RiftPattern* rift_parse_pattern(const char* pattern_string);
void rift_pattern_destroy(RiftPattern* pattern);
RiftResult rift_pattern_compile(RiftPattern* pattern);

RiftAutomaton* rift_automaton_create(void);
void rift_automaton_destroy(RiftAutomaton* automaton);
RiftState* rift_automaton_add_state(RiftAutomaton* automaton, const char* pattern_str, bool is_final);
RiftResult rift_automaton_add_transition(RiftAutomaton* automaton, RiftState* from, RiftState* to, const char* input_symbol);

RiftEngine* rift_engine_create(void);
void rift_engine_destroy(RiftEngine* engine);
RiftResult rift_engine_process_input(RiftEngine* engine, const char* input);
RiftToken* rift_engine_next_token(RiftEngine* engine);

RiftToken* rift_token_create(const char* type, const char* value, size_t line, size_t column);
void rift_token_destroy(RiftToken* token);

RiftIR* rift_ir_create(const char* source_file);
void rift_ir_destroy(RiftIR* ir);
RiftResult rift_ir_add_token(RiftIR* ir, RiftToken* token);
RiftResult rift_ir_write_file(RiftIR* ir, const char* output_file);

RiftResult rift_process_file(const char* input_file, const char* output_file, RiftConfig* config);
void rift_print_version(void);
void rift_print_usage(const char* program_name);

const char* rift_result_string(RiftResult result);

#endif // RIFT_H
EOF
fi

# Step 5: Create a simple CLI to test the library
if [ ! -f "src/cli/main.c" ]; then
    echo "📝 Creating src/cli/main.c..."
    cat > src/cli/main.c << 'EOF'
#include "../../include/rift.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        rift_print_usage(argv[0]);
        return 1;
    }
    
    const char* input_file = argv[1];
    
    RiftConfig* config = rift_config_create();
    if (!config) {
        fprintf(stderr, "❌ Failed to create configuration\n");
        return 1;
    }
    
    // Try to load .riftrc
    rift_config_load(config, ".riftrc");
    
    // Generate output filename
    char output_file[1024];
    snprintf(output_file, sizeof(output_file), "%s.0", input_file);
    
    printf("🔤 RIFT Stage 0 - Tokenization\n");
    printf("Input:  %s\n", input_file);
    printf("Output: %s\n", output_file);
    
    RiftResult result = rift_process_file(input_file, output_file, config);
    
    if (result == RIFT_SUCCESS) {
        printf("✅ Stage 0 processing complete\n");
    } else {
        printf("❌ Processing failed: %s\n", rift_result_string(result));
    }
    
    rift_config_destroy(config);
    return (result == RIFT_SUCCESS) ? 0 : 1;
}
EOF
fi

# Step 6: Create demo main
if [ ! -f "src/core/main.c" ]; then
    echo "📝 Creating src/core/main.c (demo)..."
    cat > src/core/main.c << 'EOF'
#include "../../include/rift.h"

int main(void) {
    printf("🚀 RIFT Stage 0 Demo\n");
    printf("====================\n");
    
    RiftAutomaton* automaton = rift_automaton_create();
    printf("🔧 Created automaton\n");
    
    RiftEngine* engine = rift_engine_create();
    engine->automaton = automaton;
    
    const char* test_input = "hello world";
    printf("📝 Processing: \"%s\"\n", test_input);
    
    rift_engine_process_input(engine, test_input);
    
    printf("🎯 Generated %zu tokens\n", engine->token_count);
    
    for (size_t i = 0; i < engine->token_count && i < 10; i++) {
        RiftToken* token = engine->tokens[i];
        printf("  Token %zu: %s = \"%s\"\n", i, token->type, token->value);
    }
    
    printf("✅ Demo complete\n");
    
    rift_engine_destroy(engine);
    rift_automaton_destroy(automaton);
    
    return 0;
}
EOF
fi

# Step 7: Create example file
if [ ! -f "examples/sample.rift" ]; then
    echo "📝 Creating examples/sample.rift..."
    mkdir -p examples
    cat > examples/sample.rift << 'EOF'
// Simple RIFT example
function hello() {
    return "world";
}
EOF
fi

# Step 8: Try building with verbose output to diagnose issues
echo ""
echo "🔨 Attempting build with diagnostics..."
echo "======================================="

echo "📋 Core source files that will be compiled:"
ls -la src/core/*.c 2>/dev/null || echo "❌ No core source files found"

echo ""
echo "🔧 Attempting to build core objects individually..."

mkdir -p build

# Try compiling each core file individually to find issues
for file in src/core/*.c; do
    if [ -f "$file" ]; then
        basename_file=$(basename "$file" .c)
        echo "🔨 Compiling $file..."
        if gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude -g -O2 -fPIC -c "$file" -o "build/core_${basename_file}.o"; then
            echo "✅ $file compiled successfully"
        else
            echo "❌ $file failed to compile"
            exit 1
        fi
    fi
done

echo ""
echo "🔗 Attempting to create shared library..."
if gcc -shared -o lib/librift0.so build/core_*.o -lm; then
    echo "✅ Shared library created successfully"
else
    echo "❌ Shared library creation failed"
    exit 1
fi

echo ""
echo "🔨 Building full project..."
make LIB=SO all

if [ $? -eq 0 ]; then
    echo ""
    echo "🎉 Build successful!"
    echo "=================="
    echo ""
    echo "📁 Generated files:"
    ls -la bin/ lib/ 2>/dev/null || true
    echo ""
    echo "🧪 Testing the build..."
    echo "./bin/rift0.exe examples/sample.rift"
    ./bin/rift0.exe examples/sample.rift
    echo ""
    echo "✅ RIFT Stage 0 is working!"
else
    echo ""
    echo "❌ Build failed. Check the errors above."
    exit 1
fi
EOF

chmod +x diagnostic_fix.sh

echo "🔍 **DIAGNOSTIC SOLUTION**: Run this comprehensive fix:"
echo ""
echo "   chmod +x diagnostic_fix.sh"
echo "   ./diagnostic_fix.sh"
echo ""
echo "This script will:"
echo "   ✅ Clean all artifacts"
echo "   🔍 Check what source files exist"
echo "   📝 Create any missing essential files"
echo "   🔨 Test compile each file individually"
echo "   🔗 Build library step by step"
echo "   ✅ Verify the complete build works"
echo ""
echo "**Alternative quick commands:**"
echo ""
echo "   # Just clean and rebuild"
echo "   rm -rf build bin lib && make LIB=SO bootstrap"
echo ""
echo "   # Or manual step-by-step"
echo "   make clean"
echo "   make directories"
echo "   make lib"
echo "   make all"
