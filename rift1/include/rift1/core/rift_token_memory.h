
#ifdef __cplusplus
}
#endif

// ===== include/rift1/core/rift_token_memory.h =====
#pragma once

#include "rift_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// ===== Token Memory Structure =====
typedef struct RiftTokenMemory {
    RiftToken** tokens;
    size_t token_count;
    size_t token_capacity;
    char* source_file;
    char* stage_identifier;
} RiftTokenMemory;

// ===== Stack Frame for Parser =====
struct RiftStackFrame {
    RiftToken* token;
    RiftASTNode* node;
    size_t reduction_count;
    bool can_reduce;
};

// ===== Token Memory Functions =====
RiftTokenMemory* rift_token_memory_create(void);
void rift_token_memory_destroy(RiftTokenMemory* memory);
RiftResult rift_token_memory_add_token(RiftTokenMemory* memory, RiftToken* token);
RiftToken* rift_token_create(const char* type, const char* value, const char* lexeme);
void rift_token_destroy(RiftToken* token);

// ===== Stack Frame Functions =====
RiftStackFrame* rift_stack_frame_create(void);
void rift_stack_frame_destroy(RiftStackFrame* frame);
RiftResult rift_stack_frame_add_token(RiftStackFrame* frame, RiftToken* token);
RiftResult rift_stack_frame_add_node(RiftStackFrame* frame, RiftASTNode* node);
RiftResult rift_stack_frame_reduce(RiftStackFrame* frame, size_t reduction_count);
RiftResult rift_stack_frame_clear(RiftStackFrame* frame);
RiftResult rift_stack_frame_print(RiftStackFrame* frame);


#ifdef __cplusplus
}
#endif