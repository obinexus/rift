#include "rift1/core/rift_parser.h"
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdio.h>

// ===== AEGIS Pattern-Based Parser Implementation =====
// OBINexus Computing - TokenMemory → Parser Pipeline
// Part of: riftlang.exe → .so.a → rift.exe → gosilang toolchain

struct Rift1ParseEngine {
    RiftToken** input_tokens;
    size_t token_count;
    size_t current_position;

    // AEGIS Automaton Components
    RiftAutomaton* syntax_automaton;
    RiftPattern** grammar_patterns;
    size_t pattern_count;

    // Dual Parser Strategy
    RiftParseMode parse_mode;  // BOTTOM_UP or TOP_DOWN
    RiftStackFrame** parse_stack;
    size_t stack_size;
    size_t stack_capacity;

    // AST Generation
    RiftASTNode* parse_tree;
    bool parsing_complete;
    
    // Token Type Regex Matching
    regex_t* token_type_patterns;
    size_t regex_pattern_count;
};

// ===== Token Type Regex Patterns =====
static const char* RIFT_TOKEN_PATTERNS[] = {
    "^[a-zA-Z_][a-zA-Z0-9_]*$",           // IDENTIFIER
    "^[0-9]+$",                           // INTEGER  
    "^[0-9]+\\.[0-9]+$",                  // FLOAT
    "^\"[^\"]*\"$",                       // STRING
    "^'[^']*'$",                          // CHAR
    "^\\+|\\-|\\*|\\/|\\%$",              // OPERATOR
    "^\\(|\\)|\\{|\\}|\\[|\\]$",          // DELIMITER
    "^;|,|\\.\\.$",                       // SEPARATOR
    "^if|else|while|for|return|var$",     // KEYWORD
    "^[ \\t\\n\\r]+$"                     // WHITESPACE
};

static const RiftTokenType RIFT_TOKEN_TYPES[] = {
    RIFT_TOKEN_IDENTIFIER,
    RIFT_TOKEN_INTEGER,
    RIFT_TOKEN_FLOAT, 
    RIFT_TOKEN_STRING,
    RIFT_TOKEN_CHAR,
    RIFT_TOKEN_OPERATOR,
    RIFT_TOKEN_DELIMITER,
    RIFT_TOKEN_SEPARATOR,
    RIFT_TOKEN_KEYWORD,
    RIFT_TOKEN_WHITESPACE
};

// ===== AEGIS Automaton Functions =====

RiftState* rift_automaton_match_regex(RiftAutomaton* automaton, const char* input) {
    if (!automaton || !input) return NULL;
    
    for (size_t i = 0; i < automaton->state_count; ++i) {
        RiftState* state = automaton->states[i];
        if (!state->pattern) continue;
        
        regex_t regex;
        int regex_result = regcomp(&regex, state->pattern, REG_EXTENDED | REG_NOSUB);
        if (regex_result != 0) {
            // Log regex compilation error
            char error_buffer[256];
            regerror(regex_result, &regex, error_buffer, sizeof(error_buffer));
            fprintf(stderr, "Regex compilation failed: %s\n", error_buffer);
            continue;
        }
        
        int match_result = regexec(&regex, input, 0, NULL, 0);
        regfree(&regex);
        
        if (match_result == 0) {
            return state;
        }
    }
    
    return NULL;
}

RiftTokenType rift_classify_token_type(Rift1ParseEngine* engine, const char* lexeme) {
    if (!engine || !lexeme) return RIFT_TOKEN_UNKNOWN;
    
    for (size_t i = 0; i < engine->regex_pattern_count; ++i) {
        int match_result = regexec(&engine->token_type_patterns[i], lexeme, 0, NULL, 0);
        if (match_result == 0) {
            return RIFT_TOKEN_TYPES[i];
        }
    }
    
    return RIFT_TOKEN_UNKNOWN;
}

// ===== Engine Management =====

Rift1ParseEngine* rift1_engine_create(void) {
    Rift1ParseEngine* engine = calloc(1, sizeof(Rift1ParseEngine));
    if (!engine) return NULL;

    // Initialize AEGIS automaton
    engine->syntax_automaton = rift_automaton_create();
    if (!engine->syntax_automaton) {
        free(engine);
        return NULL;
    }

    // Initialize dual parser stack
    engine->stack_capacity = 64;
    engine->parse_stack = calloc(engine->stack_capacity, sizeof(RiftStackFrame*));
    if (!engine->parse_stack) {
        rift_automaton_destroy(engine->syntax_automaton);
        free(engine);
        return NULL;
    }

    // Compile token type regex patterns
    engine->regex_pattern_count = sizeof(RIFT_TOKEN_PATTERNS) / sizeof(RIFT_TOKEN_PATTERNS[0]);
    engine->token_type_patterns = calloc(engine->regex_pattern_count, sizeof(regex_t));
    if (!engine->token_type_patterns) {
        free(engine->parse_stack);
        rift_automaton_destroy(engine->syntax_automaton);
        free(engine);
        return NULL;
    }

    // Compile all regex patterns
    for (size_t i = 0; i < engine->regex_pattern_count; ++i) {
        int regex_result = regcomp(&engine->token_type_patterns[i], 
                                  RIFT_TOKEN_PATTERNS[i], 
                                  REG_EXTENDED | REG_NOSUB);
        if (regex_result != 0) {
            // Cleanup on error
            for (size_t j = 0; j < i; ++j) {
                regfree(&engine->token_type_patterns[j]);
            }
            free(engine->token_type_patterns);
            free(engine->parse_stack);
            rift_automaton_destroy(engine->syntax_automaton);
            free(engine);
            return NULL;
        }
    }

    engine->parse_mode = RIFT_PARSE_BOTTOM_UP; // Default to bottom-up
    engine->parsing_complete = false;
    
    return engine;
}

void rift1_engine_destroy(Rift1ParseEngine* engine) {
    if (!engine) return;
    
    // Cleanup AEGIS components
    rift_automaton_destroy(engine->syntax_automaton);
    
    // Cleanup grammar patterns
    if (engine->grammar_patterns) {
        for (size_t i = 0; i < engine->pattern_count; i++) {
            rift_pattern_destroy(engine->grammar_patterns[i]);
        }
        free(engine->grammar_patterns);
    }
    
    // Cleanup parse stack
    if (engine->parse_stack) {
        for (size_t i = 0; i < engine->stack_size; ++i) {
            rift_stack_frame_destroy(engine->parse_stack[i]);
        }
        free(engine->parse_stack);
    }
    
    // Cleanup regex patterns
    if (engine->token_type_patterns) {
        for (size_t i = 0; i < engine->regex_pattern_count; ++i) {
            regfree(&engine->token_type_patterns[i]);
        }
        free(engine->token_type_patterns);
    }
    
    // Cleanup AST
    if (engine->parse_tree) {
        rift_ast_node_destroy(engine->parse_tree);
    }
    
    free(engine);
}

// ===== Parse Stack Management =====

RiftResult rift1_push_stack_frame(Rift1ParseEngine* engine, RiftStackFrame* frame) {
    if (!engine || !frame) return RIFT_ERROR_NULL_POINTER;
    
    // Resize stack if needed
    if (engine->stack_size >= engine->stack_capacity) {
        size_t new_capacity = engine->stack_capacity * 2;
        RiftStackFrame** new_stack = realloc(engine->parse_stack, 
                                           new_capacity * sizeof(RiftStackFrame*));
        if (!new_stack) return RIFT_ERROR_MEMORY_ALLOCATION;
        
        engine->parse_stack = new_stack;
        engine->stack_capacity = new_capacity;
    }
    
    engine->parse_stack[engine->stack_size++] = frame;
    return RIFT_SUCCESS;
}

RiftStackFrame* rift1_pop_stack_frame(Rift1ParseEngine* engine) {
    if (!engine || engine->stack_size == 0) return NULL;
    
    return engine->parse_stack[--engine->stack_size];
}

// ===== Bottom-Up Parser Implementation =====

RiftResult rift1_parse_bottom_up(Rift1ParseEngine* engine) {
    if (!engine) return RIFT_ERROR_NULL_POINTER;
    
    // Shift-Reduce parsing algorithm
    for (size_t i = 0; i < engine->token_count; ++i) {
        RiftToken* token = engine->input_tokens[i];
        
        // Classify token type using regex
        RiftTokenType token_type = rift_classify_token_type(engine, token->lexeme);
        token->type_enum = token_type;
        
        // Skip whitespace tokens
        if (token_type == RIFT_TOKEN_WHITESPACE) continue;
        
        // SHIFT: Push token onto stack
        RiftStackFrame* frame = rift_stack_frame_create();
        frame->token = token;
        frame->node = rift_ast_node_create(AST_NODE_TERMINAL, token->value);
        frame->node->source_token = token;
        
        RiftResult push_result = rift1_push_stack_frame(engine, frame);
        if (push_result != RIFT_SUCCESS) {
            rift_stack_frame_destroy(frame);
            return push_result;
        }
        
        // REDUCE: Check for reduction patterns
        RiftResult reduce_result = rift1_attempt_reductions(engine);
        if (reduce_result != RIFT_SUCCESS) {
            return reduce_result;
        }
    }
    
    // Final reduction to start symbol
    if (engine->stack_size == 1) {
        engine->parse_tree = engine->parse_stack[0]->node;
        engine->parsing_complete = true;
        return RIFT_SUCCESS;
    }
    
    return RIFT_ERROR_PARSE_INCOMPLETE;
}

// ===== Top-Down Parser Implementation =====

RiftResult rift1_parse_top_down(Rift1ParseEngine* engine) {
    if (!engine) return RIFT_ERROR_NULL_POINTER;
    
    // Recursive descent parsing
    engine->current_position = 0;
    
    // Start with root production rule
    RiftASTNode* root = rift1_parse_program(engine);
    if (!root) {
        return RIFT_ERROR_PARSE_FAILED;
    }
    
    engine->parse_tree = root;
    engine->parsing_complete = true;
    
    return RIFT_SUCCESS;
}

RiftASTNode* rift1_parse_program(Rift1ParseEngine* engine) {
    if (!engine) return NULL;
    
    RiftASTNode* program = rift_ast_node_create(AST_NODE_PROGRAM, NULL);
    if (!program) return NULL;
    
    // Parse statements until end of input
    while (engine->current_position < engine->token_count) {
        RiftASTNode* statement = rift1_parse_statement(engine);
        if (!statement) {
            rift_ast_node_destroy(program);
            return NULL;
        }
        
        rift_ast_node_add_child(program, statement);
    }
    
    return program;
}

RiftASTNode* rift1_parse_statement(Rift1ParseEngine* engine) {
    if (!engine || engine->current_position >= engine->token_count) return NULL;
    
    RiftToken* current_token = engine->input_tokens[engine->current_position];
    RiftTokenType token_type = rift_classify_token_type(engine, current_token->lexeme);
    
    switch (token_type) {
        case RIFT_TOKEN_KEYWORD:
            return rift1_parse_keyword_statement(engine);
        case RIFT_TOKEN_IDENTIFIER:
            return rift1_parse_assignment_or_call(engine);
        default:
            return rift1_parse_expression_statement(engine);
    }
}

// ===== Main Processing Functions =====

RiftResult rift1_process(Rift1ParseEngine* engine, 
                        const char* input_file, 
                        const char* output_file) {
    if (!engine || !input_file || !output_file) {
        return RIFT_ERROR_NULL_POINTER;
    }

    // Stage 1: Read .rift.0 tokens from Stage 0
    RiftIR* input_ir = rift_ir_create(input_file);
    if (!input_ir) return RIFT_ERROR_MEMORY_ALLOCATION;
    
    RiftResult read_result = rift_ir_read_file(input_ir, input_file);
    if (read_result != RIFT_SUCCESS) {
        rift_ir_destroy(input_ir);
        return read_result;
    }

    // Stage 2: Load tokens into engine
    engine->input_tokens = input_ir->tokens;
    engine->token_count = input_ir->token_count;

    // Stage 3: Apply AEGIS pattern matching for syntax recognition
    RiftResult parse_result = rift1_parse_tokens(engine);
    if (parse_result != RIFT_SUCCESS) {
        rift_ir_destroy(input_ir);
        return parse_result;
    }

    // Stage 4: Generate .rift.1 output with AST
    RiftIR* output_ir = rift_ir_create(input_file);
    if (!output_ir) {
        rift_ir_destroy(input_ir);
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    output_ir->stage = strdup("rift.1");
    output_ir->ast_root = engine->parse_tree;
    
    RiftResult write_result = rift_ir_write_file(output_ir, output_file);

    rift_ir_destroy(input_ir);
    rift_ir_destroy(output_ir);

    return write_result;
}

RiftResult rift1_parse_tokens(Rift1ParseEngine* engine) {
    if (!engine || !engine->syntax_automaton || !engine->input_tokens) {
        return RIFT_ERROR_NULL_POINTER;
    }

    // Apply regex-based token classification to all tokens
    for (size_t i = 0; i < engine->token_count; ++i) {
        RiftToken* token = engine->input_tokens[i];
        
        // Match against AEGIS automaton states
        RiftState* matched_state = rift_automaton_match_regex(engine->syntax_automaton, 
                                                             token->lexeme);
        if (matched_state) {
            token->matched_state = matched_state;
        }
        
        // Classify token type using regex patterns
        RiftTokenType token_type = rift_classify_token_type(engine, token->lexeme);
        token->type_enum = token_type;
    }

    // Choose parsing strategy based on parse mode
    RiftResult parse_result;
    switch (engine->parse_mode) {
        case RIFT_PARSE_BOTTOM_UP:
            parse_result = rift1_parse_bottom_up(engine);
            break;
        case RIFT_PARSE_TOP_DOWN:
            parse_result = rift1_parse_top_down(engine);
            break;
        default:
            parse_result = RIFT_ERROR_INVALID_PARSE_MODE;
    }

    return parse_result;
}

// ===== Configuration Functions =====

RiftResult rift1_engine_set_parse_mode(Rift1ParseEngine* engine, RiftParseMode mode) {
    if (!engine) return RIFT_ERROR_NULL_POINTER;
    
    engine->parse_mode = mode;
    return RIFT_SUCCESS;
}

RiftResult rift1_engine_load_tokenmemory(Rift1ParseEngine* engine, RiftTokenMemory* memory) {
    if (!engine || !memory) return RIFT_ERROR_NULL_POINTER;
    
    // Load tokens from TokenMemory into engine
    engine->input_tokens = memory->tokens;
    engine->token_count = memory->token_count;
    
    return RIFT_SUCCESS;
}

RiftResult rift1_generate_ast(Rift1ParseEngine* engine) {
    if (!engine) return RIFT_ERROR_NULL_POINTER;
    
    if (!engine->parsing_complete) {
        return RIFT_ERROR_PARSING_INCOMPLETE;
    }
    
    // AST already generated during parsing
    return RIFT_SUCCESS;
}

RiftResult rift1_process_stage0_to_stage1(const char* rift0_file, 
                                         const char* rift1_file, 
                                         RiftConfig* config) {
    if (!rift0_file || !rift1_file) return RIFT_ERROR_NULL_POINTER;
    
    Rift1ParseEngine* engine = rift1_engine_create();
    if (!engine) return RIFT_ERROR_MEMORY_ALLOCATION;
    
    // Configure parsing mode if specified
    if (config && config->parse_mode != RIFT_PARSE_DEFAULT) {
        rift1_engine_set_parse_mode(engine, config->parse_mode);
    }
    
    RiftResult result = rift1_process(engine, rift0_file, rift1_file);
    
    rift1_engine_destroy(engine);
    return result;
}