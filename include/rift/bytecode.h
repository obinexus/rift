#ifndef RIFT_BYTECODE_H
#define RIFT_BYTECODE_H

#include <stddef.h>
#include <stdint.h>
#include "rift/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum rift_opcode {
    RIFT_OP_NOP = 0,
    RIFT_OP_PUSH_I64,
    RIFT_OP_ADD,
    RIFT_OP_SUB,
    RIFT_OP_MUL,
    RIFT_OP_DIV,
    RIFT_OP_HALT
} rift_opcode_t;

typedef struct rift_instruction {
    rift_opcode_t opcode;
    int64_t operand;
} rift_instruction_t;

typedef struct rift_bytecode_program {
    rift_instruction_t *instructions;
    size_t count;
    size_t capacity;
} rift_bytecode_program_t;

RIFT_API rift_bytecode_program_t *rift_bytecode_create(void);
RIFT_API void rift_bytecode_destroy(rift_bytecode_program_t *program);
RIFT_API int rift_bytecode_emit(rift_bytecode_program_t *program,
                                rift_opcode_t opcode,
                                int64_t operand);
RIFT_API int rift_bytecode_execute(const rift_bytecode_program_t *program,
                                   int64_t *result);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_BYTECODE_H */
