
#ifdef __cplusplus
}
#endif

// ===== include/rift1/core/rift_ir.h =====
#pragma once

#include "rift_types.h"
#include "rift_ast.h"
#include "rift_token_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

// ===== Intermediate Representation =====
typedef struct RiftIR {
    char* source_file;
    char* stage;
    RiftToken** tokens;
    size_t token_count;
    RiftASTNode* ast_root;
    char* metadata;
} RiftIR;

// ===== IR Functions =====
RiftIR* rift_ir_create(const char* source_file);
void rift_ir_destroy(RiftIR* ir);
RiftResult rift_ir_read_file(RiftIR* ir, const char* filename);
RiftResult rift_ir_write_file(RiftIR* ir, const char* filename);

#ifdef __cplusplus
}
#endif