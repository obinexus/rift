


// ===== include/rift1/core/rift_ast.h =====
#pragma once

#include "rift_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// ===== AST Node Types =====
typedef enum {
    AST_NODE_UNKNOWN = 0,
    AST_NODE_PROGRAM = 1,
    AST_NODE_STATEMENT = 2,
    AST_NODE_EXPRESSION = 3,
    AST_NODE_TERMINAL = 4,
    AST_NODE_IDENTIFIER = 5,
    AST_NODE_LITERAL = 6,
    AST_NODE_BINARY_OP = 7,
    AST_NODE_UNARY_OP = 8,
    AST_NODE_ASSIGNMENT = 9,
    AST_NODE_FUNCTION_CALL = 10,
    AST_NODE_BLOCK = 11
} RiftASTNodeType;

// ===== AST Node Structure =====
struct RiftASTNode {
    RiftASTNodeType node_type;
    char* value;
    RiftToken* source_token;
    
    // Tree structure
    RiftASTNode** children;
    size_t child_count;
    size_t child_capacity;
    RiftASTNode* parent;
    
    // Metadata
    size_t depth;
    bool visited;
};

// ===== AST Functions =====
RiftASTNode* rift_ast_node_create(RiftASTNodeType type, const char* value);
void rift_ast_node_destroy(RiftASTNode* node);
RiftResult rift_ast_node_add_child(RiftASTNode* parent, RiftASTNode* child);
RiftResult rift_ast_node_remove_child(RiftASTNode* parent, size_t index);
void rift_ast_print(RiftASTNode* root, int indent);

#ifdef __cplusplus
}
#endif
