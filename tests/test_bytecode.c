#include "rift_check.h"
#include <stdint.h>
#include <stdio.h>
#include "rift/bytecode.h"

int main(void) {
    rift_bytecode_program_t *program = rift_bytecode_create();
    int64_t result = 0;
    int status;
    RIFT_CHECK(program != NULL);
    status = rift_bytecode_emit(program, RIFT_OP_PUSH_I64, 20);
    RIFT_CHECK(status == 0);
    status = rift_bytecode_emit(program, RIFT_OP_PUSH_I64, 22);
    RIFT_CHECK(status == 0);
    status = rift_bytecode_emit(program, RIFT_OP_ADD, 0);
    RIFT_CHECK(status == 0);
    status = rift_bytecode_emit(program, RIFT_OP_HALT, 0);
    RIFT_CHECK(status == 0);
    status = rift_bytecode_execute(program, &result);
    RIFT_CHECK(status == 0);
    RIFT_CHECK(result == 42);
    rift_bytecode_destroy(program);
    puts("test_bytecode: PASS");
    return 0;
}
