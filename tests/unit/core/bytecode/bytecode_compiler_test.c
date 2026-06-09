// unit/core/bytecode/bytecode_compiler_test.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "core/bytecode/bytecode.h"
#include "core/bytecode/bytecode_compiler.h"
#include "core/errors/regex_error.h"

void test_bytecode_compilation() {
    rift_regex_error_t error;
    const char *pattern = "a(b+)c";
    
    rift_bytecode_program_t *program = rift_bytecode_compile(pattern, 0, &error);
    assert(program != NULL);
    assert(program->group_count == 1);
    
    // Verify bytecode structure
    assert(program->instruction_count > 0);
    
    // Check for specific instruction sequences
    bool found_save_start = false;
    bool found_save_end = false;
    
    for (uint32_t i = 0; i < program->instruction_count; i++) {
        rift_bytecode_instruction_t *instr = &program->instructions[i];
        
        if (instr->opcode == RIFT_OP_SAVE_START && instr->operand.group_index == 1) {
            found_save_start = true;
        }
        else if (instr->opcode == RIFT_OP_SAVE_END && instr->operand.group_index == 1) {
            found_save_end = true;
        }
    }
    
    assert(found_save_start && found_save_end);
    
    rift_bytecode_program_free(program);
    printf("Bytecode compilation test passed.\n");
}

int main() {
    test_bytecode_compilation();
    return 0;
}