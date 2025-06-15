// ===== include/rift1/core/rift_parser.h =====
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "rift_types.h"
#include "rift_ast.h"
#include "rift_token_memory.h"
#include "rift_automaton.h"

// ===== Forward Declarations =====
typedef struct Rift1ParseEngine Rift1ParseEngine;

// ===== Parse Modes =====
typedef enum {
    RIFT_PARSE_DEFAULT = 0,
    RIFT_PARSE_BOTTOM_UP = 1,
    RIFT_PARSE_TOP_DOWN = 2
} RiftParseMode;

// ===== Engine Management =====
Rift1ParseEngine* rift1_engine_create(void);
void rift1_engine_destroy(Rift1ParseEngine* engine);

// ===== Configuration =====
RiftResult rift1_engine_set_parse_mode(Rift1ParseEngine* engine, RiftParseMode mode);
RiftResult rift1_engine_load_tokenmemory(Rift1ParseEngine* engine, RiftTokenMemory* memory);

// ===== Core Processing =====
RiftResult rift1_process(Rift1ParseEngine* engine, const char* input_file, const char* output_file);
RiftResult rift1_parse_tokens(Rift1ParseEngine* engine);
RiftResult rift1_generate_ast(Rift1ParseEngine* engine);

// ===== Pipeline Integration =====
RiftResult rift1_process_stage0_to_stage1(const char* rift0_file, const char* rift1_file, RiftConfig* config);

// ===== Parser Strategy Functions =====
RiftResult rift1_parse_bottom_up(Rift1ParseEngine* engine);
RiftResult rift1_parse_top_down(Rift1ParseEngine* engine);

// ===== Grammar Production Functions =====
RiftASTNode* rift1_parse_program(Rift1ParseEngine* engine);
RiftASTNode* rift1_parse_statement(Rift1ParseEngine* engine);
RiftASTNode* rift1_parse_keyword_statement(Rift1ParseEngine* engine);
RiftASTNode* rift1_parse_assignment_or_call(Rift1ParseEngine* engine);
RiftASTNode* rift1_parse_expression_statement(Rift1ParseEngine* engine);

// ===== Stack Management =====
RiftResult rift1_push_stack_frame(Rift1ParseEngine* engine, RiftStackFrame* frame);
RiftStackFrame* rift1_pop_stack_frame(Rift1ParseEngine* engine);
RiftResult rift1_attempt_reductions(Rift1ParseEngine* engine);

// ===== Token Classification =====
RiftTokenType rift_classify_token_type(Rift1ParseEngine* engine, const char* lexeme);
RiftState* rift_automaton_match_regex(RiftAutomaton* automaton, const char* input);

#ifdef __cplusplus
}
#endif
