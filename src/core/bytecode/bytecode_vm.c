/**
 * @file bytecode_vm.c
 * @brief Implementation of the bytecode virtual machine for LibRift
 *
 * This file implements the execution engine for compiled bytecode,
 * providing platform-independent regex pattern matching.
 *
 * @copyright Copyright (c) 2025 LibRift Project
 * @license MIT License
 */

#include "core/bytecode/bytecode_vm.h

tmp/librift_analysis/librift-4933472-W/backup/v4_missing/bytecode_vm.h
tmp/librift_analysis/librift-4933472-W/src/core/bytecode/bytecode_vm.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/backup/v4_missing/bytecode_vm.h
tmp/librift_analysis/librift-4e7961d-Milestone_Refatored/src/core/bytecode/bytecode_vm.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/backup/v4_missing/bytecode_vm.h
tmp/librift_analysis/librift-a3c62d2-Milestone_Refatored/src/core/bytecode/bytecode_vm.h"


#define MAX_CAPTURE_GROUPS 100
#define DEFAULT_STACK_CAPACITY 256
#define DEFAULT_MAX_INSTRUCTIONS 10000000 /* Limit for preventing infinite loops */

/**
 * @brief Instruction execution result
 */
typedef enum {
    RESULT_CONTINUE, /* Continue to next instruction */
    RESULT_JUMP,     /* Jump to specified instruction */
    RESULT_MATCH,    /* Match found */
    RESULT_FAIL,     /* Match failed */
    RESULT_ERROR     /* Execution error */
} execution_result_t;

/**
 * @brief Backtracking entry for the VM
 */
typedef struct {
    uint32_t instruction_index; /* Instruction to resume from */
    uint32_t input_position;    /* Input position to restore */
    uint32_t *capture_start;    /* Capture group start positions */
    uint32_t *capture_end;      /* Capture group end positions */
} backtrack_entry_t;

/**
 * @brief Create a new VM for executing bytecode
 *
 * @param program Bytecode program to execute
 * @param input Input string to match against
 * @param input_length Length of input string or (size_t)-1 to use strlen
 * @return New VM instance or NULL on failure
 */
rift_bytecode_vm_t *
rift_bytecode_vm_create(rift_bytecode_program_t *program, const char *input, size_t input_length)
{
    if (!program || !input) {
        return NULL;
    }

    /* Calculate input length if not specified */
    if (input_length == (size_t)-1) {
        input_length = strlen(input);
    }

    /* Allocate VM structure */
    rift_bytecode_vm_t *vm = (rift_bytecode_vm_t *)rift_malloc(sizeof(rift_bytecode_vm_t));
    if (!vm) {
        return NULL;
    }

    /* Initialize VM fields */
    vm->input = input;
    vm->input_length = input_length;
    vm->current_pos = 0;
    vm->stack_size = 0;
    vm->stack_capacity = DEFAULT_STACK_CAPACITY;
    vm->timed_out = false;
    vm->max_instructions = DEFAULT_MAX_INSTRUCTIONS;
    vm->instruction_counter = 0;

    /* Initialize capture groups */
    vm->capture_count = program->group_count + 1; /* +1 for the full match */
    vm->captures = (uint32_t *)rift_malloc(vm->capture_count * 2 * sizeof(uint32_t));
    if (!vm->captures) {
        rift_free(vm);
        return NULL;
    }

    /* Initialize all captures to "not set" (-1) */
    for (uint32_t i = 0; i < vm->capture_count * 2; i++) {
        vm->captures[i] = (uint32_t)-1;
    }

    /* Allocate backtrack stack */
    vm->backtrack_stack = (uint32_t *)rift_malloc(vm->stack_capacity * sizeof(uint32_t) *
                                                  (2 + 2 * vm->capture_count));
    if (!vm->backtrack_stack) {
        rift_free(vm->captures);
        rift_free(vm);
        return NULL;
    }

    return vm;
}

/**
 * @brief Free a bytecode VM
 *
 * @param vm VM to free
 */
void
rift_bytecode_vm_free(rift_bytecode_vm_t *vm)
{
    if (!vm) {
        return;
    }

    if (vm->captures) {
        rift_free(vm->captures);
    }

    if (vm->backtrack_stack) {
        rift_free(vm->backtrack_stack);
    }

    rift_free(vm);
}

/**
 * @brief Push a backtracking entry onto the stack
 *
 * @param vm VM instance
 * @param instruction_index Instruction index to restore to
 * @param input_position Input position to restore to
 * @return true if successful, false on stack overflow
 */
static bool
vm_push_backtrack(rift_bytecode_vm_t *vm, uint32_t instruction_index, uint32_t input_position)
{
    /* Check if we need to expand the stack */
    if (vm->stack_size + 2 + 2 * vm->capture_count > vm->stack_capacity) {
        uint32_t new_capacity = vm->stack_capacity * 2;
        uint32_t *new_stack =
            (uint32_t *)rift_realloc(vm->backtrack_stack, new_capacity * sizeof(uint32_t));

        if (!new_stack) {
            return false;
        }

        vm->backtrack_stack = new_stack;
        vm->stack_capacity = new_capacity;
    }

    /* Push instruction index and input position */
    vm->backtrack_stack[vm->stack_size++] = instruction_index;
    vm->backtrack_stack[vm->stack_size++] = input_position;

    /* Push capture group state */
    for (uint32_t i = 0; i < vm->capture_count; i++) {
        vm->backtrack_stack[vm->stack_size++] = vm->captures[i * 2];     /* Start position */
        vm->backtrack_stack[vm->stack_size++] = vm->captures[i * 2 + 1]; /* End position */
    }

    return true;
}

/**
 * @brief Pop a backtracking entry from the stack
 *
 * @param vm VM instance
 * @param instruction_index Pointer to store instruction index
 * @param input_position Pointer to store input position
 * @return true if successful, false on stack underflow
 */
static bool
vm_pop_backtrack(rift_bytecode_vm_t *vm, uint32_t *instruction_index, uint32_t *input_position)
{
    /* Check if the stack is empty */
    if (vm->stack_size < 2 + 2 * vm->capture_count) {
        return false;
    }

    /* Calculate the base index for the current frame */
    uint32_t base_index = vm->stack_size - (2 + 2 * vm->capture_count);

    /* Pop instruction index and input position */
    *instruction_index = vm->backtrack_stack[base_index];
    *input_position = vm->backtrack_stack[base_index + 1];

    /* Restore capture group state */
    for (uint32_t i = 0; i < vm->capture_count; i++) {
        vm->captures[i * 2] = vm->backtrack_stack[base_index + 2 + i * 2];     /* Start position */
        vm->captures[i * 2 + 1] = vm->backtrack_stack[base_index + 3 + i * 2]; /* End position */
    }

    /* Adjust stack size */
    vm->stack_size -= (2 + 2 * vm->capture_count);

    return true;
}

/**
 * @brief Reset the VM to start from the beginning
 *
 * @param vm VM to reset
 */
void
rift_bytecode_vm_reset(rift_bytecode_vm_t *vm)
{
    if (!vm) {
        return;
    }

    vm->current_pos = 0;
    vm->stack_size = 0;
    vm->timed_out = false;
    vm->instruction_counter = 0;

    /* Reset all captures to "not set" (-1) */
    for (uint32_t i = 0; i < vm->capture_count * 2; i++) {
        vm->captures[i] = (uint32_t)-1;
    }
}

/**
 * @brief Execute a single instruction
 *
 * @param vm VM instance
 * @param program Bytecode program
 * @param instr_ptr Current instruction pointer
 * @param next_instr Pointer to store next instruction index
 * @return Execution result
 */
static execution_result_t
execute_instruction(rift_bytecode_vm_t *vm, rift_bytecode_program_t *program, uint32_t instr_ptr,
                    uint32_t *next_instr)
{
    /* Check if we've hit the maximum number of instructions */
    if (vm->instruction_counter++ >= vm->max_instructions) {
        vm->timed_out = true;
        return RESULT_ERROR;
    }

    /* Check if instruction pointer is valid */
    if (instr_ptr >= program->instruction_count) {
        return RESULT_ERROR;
    }

    /* Get the current instruction */
    rift_bytecode_instruction_t *instr = &program->instructions[instr_ptr];

    /* Execute based on opcode */
    switch (instr->opcode) {
    case RIFT_OP_NOP:
        /* No operation, just continue to next instruction */
        *next_instr = instr_ptr + 1;
        return RESULT_CONTINUE;

    case RIFT_OP_MATCH_CHAR:
        /* Match a single character */
        if (vm->current_pos < vm->input_length &&
            vm->input[vm->current_pos] == instr->operand.character) {
            vm->current_pos++;
            *next_instr = instr_ptr + 1;
            return RESULT_CONTINUE;
        }
        return RESULT_FAIL;

    case RIFT_OP_MATCH_CLASS:
        /* Match a character class */
        if (vm->current_pos < vm->input_length) {
            char current_char = vm->input[vm->current_pos];
            const char *class_pattern = instr->operand.char_class.class_pattern;
            uint32_t pattern_length = instr->operand.char_class.pattern_length;

            /* Check if character is in the class */
            for (uint32_t i = 0; i < pattern_length; i++) {
                if (current_char == class_pattern[i]) {
                    vm->current_pos++;
                    *next_instr = instr_ptr + 1;
                    return RESULT_CONTINUE;
                }
            }
        }
        return RESULT_FAIL;

    case RIFT_OP_MATCH_ANY:
        /* Match any character */
        if (vm->current_pos < vm->input_length) {
            vm->current_pos++;
            *next_instr = instr_ptr + 1;
            return RESULT_CONTINUE;
        }
        return RESULT_FAIL;

    case RIFT_OP_JUMP:
        /* Unconditional jump */
        *next_instr = instr->operand.jump_target;
        return RESULT_JUMP;

    case RIFT_OP_SPLIT:
        /* Split execution paths (non-deterministic choice) */
        /* First, save the alternate path as a backtrack point */
        if (!vm_push_backtrack(vm, instr->operand.jump_target, vm->current_pos)) {
            return RESULT_ERROR;
        }

        /* Continue with the primary path */
        *next_instr = instr_ptr + 1;
        return RESULT_CONTINUE;

    case RIFT_OP_SAVE_START:
        /* Save start position for a capture group */
        {
            uint32_t group_index = instr->operand.group_index;
            if (group_index < vm->capture_count) {
                vm->captures[group_index * 2] = vm->current_pos;
                *next_instr = instr_ptr + 1;
                return RESULT_CONTINUE;
            }
            return RESULT_ERROR;
        }

    case RIFT_OP_SAVE_END:
        /* Save end position for a capture group */
        {
            uint32_t group_index = instr->operand.group_index;
            if (group_index < vm->capture_count) {
                vm->captures[group_index * 2 + 1] = vm->current_pos;
                *next_instr = instr_ptr + 1;
                return RESULT_CONTINUE;
            }
            return RESULT_ERROR;
        }

    case RIFT_OP_ACCEPT:
        /* Accept the current input */
        return RESULT_MATCH;

    case RIFT_OP_FAIL:
        /* Fail the current path */
        return RESULT_FAIL;

    case RIFT_OP_REPEAT_START:
        /* Start of a repetition construct */
        {
            uint32_t min = instr->operand.repeat.min;
            uint32_t max = instr->operand.repeat.max;
            bool greedy = instr->operand.repeat.greedy;

            /* Implementation would need to track repetition counts */
            /* This is a simplified placeholder */

            *next_instr = instr_ptr + 1;
            return RESULT_CONTINUE;
        }

    case RIFT_OP_REPEAT_END:
        /* End of a repetition construct */
        *next_instr = instr_ptr + 1;
        return RESULT_CONTINUE;

    case RIFT_OP_BOUNDARY:
        /* Word boundary assertion */
        {
            bool at_boundary = false;

            /* Check if current position is at a word boundary */
            if (vm->current_pos == 0 || vm->current_pos == vm->input_length) {
                /* Start or end of input is a boundary */
                at_boundary = true;
            } else {
                /* Check if transition between word and non-word character */
                char prev = vm->input[vm->current_pos - 1];
                char curr = vm->input[vm->current_pos];
                bool prev_is_word = (prev >= 'a' && prev <= 'z') || (prev >= 'A' && prev <= 'Z') ||
                                    (prev >= '0' && prev <= '9') || (prev == '_');
                bool curr_is_word = (curr >= 'a' && curr <= 'z') || (curr >= 'A' && curr <= 'Z') ||
                                    (curr >= '0' && curr <= '9') || (curr == '_');
                at_boundary = prev_is_word != curr_is_word;
            }

            if (at_boundary) {
                *next_instr = instr_ptr + 1;
                return RESULT_CONTINUE;
            }
            return RESULT_FAIL;
        }

    case RIFT_OP_BACKREF:
        /* Backreference to a previous capture */
        {
            uint32_t group_index = instr->operand.group_index;
            if (group_index >= vm->capture_count) {
                return RESULT_ERROR;
            }

            uint32_t start = vm->captures[group_index * 2];
            uint32_t end = vm->captures[group_index * 2 + 1];

            /* If group hasn't been captured yet, fail */
            if (start == (uint32_t)-1 || end == (uint32_t)-1) {
                return RESULT_FAIL;
            }

            /* Calculate length of captured group */
            uint32_t length = end - start;

            /* Check if there's enough input left */
            if (vm->current_pos + length > vm->input_length) {
                return RESULT_FAIL;
            }

            /* Compare input with captured group */
            if (memcmp(vm->input + vm->current_pos, vm->input + start, length) == 0) {
                vm->current_pos += length;
                *next_instr = instr_ptr + 1;
                return RESULT_CONTINUE;
            }
            return RESULT_FAIL;
        }

    case RIFT_OP_LOOKAHEAD:
    case RIFT_OP_NEG_LOOKAHEAD:
        /* Lookahead assertions would need more complex implementation */
        /* Not fully implemented in this simplified version */
        *next_instr = instr_ptr + 1;
        return RESULT_CONTINUE;

    default:
        /* Unknown opcode */
        return RESULT_ERROR;
    }
}

/**
 * @brief Execute bytecode program
 *
 * @param program Bytecode program to execute
 * @param vm VM state for execution
 * @param match Match information to fill (can be NULL)
 * @return true if the program matched, false otherwise
 */
bool
rift_bytecode_execute(rift_bytecode_program_t *program, rift_bytecode_vm_t *vm,
                      rift_regex_match_t *match)
{
    if (!program || !vm) {
        return false;
    }

    /* Reset VM state */
    rift_bytecode_vm_reset(vm);

    /* Initialize captures for full match (group 0) */
    vm->captures[0] = vm->current_pos; /* Start at current position */
    vm->captures[1] = (uint32_t)-1;    /* End not determined yet */

    uint32_t instr_ptr = 0;
    uint32_t next_instr = 0;

    while (true) {
        /* Execute the current instruction */
        execution_result_t result = execute_instruction(vm, program, instr_ptr, &next_instr);

        switch (result) {
        case RESULT_CONTINUE:
            /* Continue to next instruction */
            instr_ptr = next_instr;
            break;

        case RESULT_JUMP:
            /* Jump to specified instruction */
            instr_ptr = next_instr;
            break;

        case RESULT_MATCH:
            /* Match found - set the end position for group 0 */
            vm->captures[1] = vm->current_pos;

            /* Fill match information if provided */
            if (match) {
                match->start_pos = vm->captures[0];
                match->end_pos = vm->captures[1];
                match->group_count = vm->capture_count;

                /* Match groups would be copied here in a complete implementation */
            }

            return true;

        case RESULT_FAIL:
            /* Match failed - try backtracking */
            if (vm_pop_backtrack(vm, &instr_ptr, &vm->current_pos)) {
                /* Successfully backtracked */
                continue;
            }

            /* No more backtracking options, match failed */
            return false;

        case RESULT_ERROR:
            /* Execution error */
            return false;
        }
    }

    /* Should never reach here */
    return false;
}

/**
 * @brief Create a VM instance with custom settings
 *
 * @param program Bytecode program to execute
 * @param input Input string to match against
 * @param input_length Length of input string or (size_t)-1 to use strlen
 * @param max_instructions Maximum number of instructions to execute
 * @return New VM instance or NULL on failure
 */
rift_bytecode_vm_t *
rift_bytecode_vm_create_with_options(rift_bytecode_program_t *program, const char *input,
                                     size_t input_length, uint64_t max_instructions)
{
    rift_bytecode_vm_t *vm = rift_bytecode_vm_create(program, input, input_length);
    if (vm) {
        vm->max_instructions = max_instructions;
    }
    return vm;
}

/**
 * @brief Check if VM timed out during execution
 *
 * @param vm VM instance
 * @return true if timed out, false otherwise
 */
bool
rift_bytecode_vm_timed_out(const rift_bytecode_vm_t *vm)
{
    return vm ? vm->timed_out : false;
}

/**
 * @brief Get captured group from VM
 *
 * @param vm VM instance
 * @param group_index Group index (0 for the whole match)
 * @param start Pointer to store start position
 * @param end Pointer to store end position
 * @return true if group exists, false otherwise
 */
bool
rift_bytecode_vm_get_group(const rift_bytecode_vm_t *vm, uint32_t group_index, uint32_t *start,
                           uint32_t *end)
{
    if (!vm || !start || !end || group_index >= vm->capture_count) {
        return false;
    }

    uint32_t start_pos = vm->captures[group_index * 2];
    uint32_t end_pos = vm->captures[group_index * 2 + 1];

    /* Check if group was captured */
    if (start_pos == (uint32_t)-1 || end_pos == (uint32_t)-1) {
        return false;
    }

    *start = start_pos;
    *end = end_pos;
    return true;
}


