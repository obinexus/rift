#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "rift/bytecode.h"

int main(void) {
    rift_bytecode_program_t *program = rift_bytecode_create();
    int64_t result = 0;
    int status;
    assert(program != NULL);
    status = rift_bytecode_emit(program, RIFT_OP_PUSH_I64, 20);
    assert(status == 0);
    status = rift_bytecode_emit(program, RIFT_OP_PUSH_I64, 22);
    assert(status == 0);
    status = rift_bytecode_emit(program, RIFT_OP_ADD, 0);
    assert(status == 0);
    status = rift_bytecode_emit(program, RIFT_OP_HALT, 0);
    assert(status == 0);
    status = rift_bytecode_execute(program, &result);
    assert(status == 0);
    assert(result == 42);
    rift_bytecode_destroy(program);
    puts("test_bytecode: PASS");
    return 0;
}
