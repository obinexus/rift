#include "rift/codegen.h"
#include <stdlib.h>

struct rift_codegen {
    int initialized;
    size_t generated_nodes;
};

static size_t count_nodes(const rift_ast_node_t *node) {
    size_t count = 0;
    while (node) {
        count += 1 + count_nodes(node->children);
        node = node->next;
    }
    return count;
}

rift_codegen_t *rift_codegen_create(void) {
    rift_codegen_t *cg = (rift_codegen_t *)calloc(1, sizeof(rift_codegen_t));
    if (cg) cg->initialized = 1;
    return cg;
}

void rift_codegen_destroy(rift_codegen_t *cg) {
    free(cg);
}

int rift_codegen_generate(rift_codegen_t *cg, const rift_ast_node_t *ast) {
    if (!cg || !ast) return -1;
    cg->generated_nodes = count_nodes(ast);
    return 0;
}
