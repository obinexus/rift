#include "rift/bytecode.h"

#include <stdlib.h>

rift_bytecode_program_t *rift_bytecode_create(void) {
    return (rift_bytecode_program_t *)calloc(1, sizeof(rift_bytecode_program_t));
}

void rift_bytecode_destroy(rift_bytecode_program_t *program) {
    if (!program) return;
    free(program->instructions);
    free(program);
}

int rift_bytecode_emit(rift_bytecode_program_t *program,
                       rift_opcode_t opcode,
                       int64_t operand) {
    rift_instruction_t *instructions;
    if (!program || opcode < RIFT_OP_NOP || opcode > RIFT_OP_HALT) return -1;
    if (program->count == program->capacity) {
        size_t capacity = program->capacity ? program->capacity * 2 : 16;
        instructions = (rift_instruction_t *)realloc(
            program->instructions, capacity * sizeof(*instructions));
        if (!instructions) return -1;
        program->instructions = instructions;
        program->capacity = capacity;
    }
    program->instructions[program->count].opcode = opcode;
    program->instructions[program->count].operand = operand;
    program->count++;
    return 0;
}

int rift_bytecode_execute(const rift_bytecode_program_t *program,
                          int64_t *result) {
    int64_t stack[256];
    size_t stack_size = 0;
    size_t ip;
    if (!program || !result) return -1;
    for (ip = 0; ip < program->count; ip++) {
        const rift_instruction_t *instruction = &program->instructions[ip];
        int64_t right;
        int64_t left;
        switch (instruction->opcode) {
            case RIFT_OP_NOP:
                break;
            case RIFT_OP_PUSH_I64:
                if (stack_size == 256) return -1;
                stack[stack_size++] = instruction->operand;
                break;
            case RIFT_OP_ADD:
            case RIFT_OP_SUB:
            case RIFT_OP_MUL:
            case RIFT_OP_DIV:
                if (stack_size < 2) return -1;
                right = stack[--stack_size];
                left = stack[--stack_size];
                if (instruction->opcode == RIFT_OP_ADD) left += right;
                else if (instruction->opcode == RIFT_OP_SUB) left -= right;
                else if (instruction->opcode == RIFT_OP_MUL) left *= right;
                else {
                    if (right == 0) return -1;
                    left /= right;
                }
                stack[stack_size++] = left;
                break;
            case RIFT_OP_HALT:
                ip = program->count;
                break;
        }
    }
    if (stack_size != 1) return -1;
    *result = stack[0];
    return 0;
}
