#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include "../provided/bof.h"
#include "../provided/machine_types.h"
#include "../provided/regname.h"

// Initialize the VM with default values
void vm_init(VM *vm) {
    vm->ip = 0;  // Instruction pointer starts at the first instruction
    vm->program_size = 0;  // No program loaded initially
    vm->pc = 0;
    for (int i = 0; i < NUM_REGISTERS; i++) {
        vm->registers[i] = 0; 
    }
}

// Load the program (instructions) into the VM with debugging
void vm_load_program(VM *vm, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read the instructions into the program array
    BOFFILE bf_file = bof_read_open(filename);
    BOFHeader bf_header = bof_read_header(bf_file);
    vm->bf_header = bf_header; 

    vm->pc = bf_header.text_start_address;
    vm->registers[0] = bf_header.data_start_address;
    vm->registers[1] = bf_header.stack_bottom_addr;
    vm->registers[2] = bf_header.stack_bottom_addr;

    vm->program_size = bf_header.text_length;

    for (int i = 0; i < vm->program_size; i++) {
        memory.instrs[i] = instruction_read(bf_file);
    }

    for (int i = 0; i < bf_header.data_length; i++) {
        word_type word = bof_read_word(bf_file);
        memory.words[vm->program_size + i] = word;
    }
    memory.words[vm->program_size + bf_header.data_length] = 0;

    vm->stack[vm->registers[1]] = 0;
    vm->stack_indexes[vm->registers[1]] = 1;
}

// Print the loaded program for listing (-p flag)
void vm_print_program(VM *vm) {
    printf("Address Instruction\n");

    for (int i = 0; i < vm->program_size; i++) {
        printf("%6d: %s\n", i, instruction_assembly_form(i, memory.instrs[i]));
    }
    print_words(vm);
}

void print_registers(VM *vm) {
    printf("%8s: %d\t", "PC", vm->pc);
    for (int i = 0; i < NUM_REGISTERS; i++ ) {
        if (i % 5 == 0) {
            printf("\n");
        }
        char buffer[100];
        sprintf(buffer, "GPR[%-3s]", regname_get(i));
        printf("%8s: %d\t", buffer, vm->registers[i]);
    }
    printf("\n"); 
}

void print_instruction(VM *vm, int instruction_number) {
    printf("==>%8d: %s", instruction_number, instruction_assembly_form(1, memory.instrs[instruction_number]));
}

void print_words(VM *vm) {
    int index;

    for (index = 0; index < vm->bf_header.data_length; index++) {
        if (index % 5 == 0 && index != 0) {
            printf("\n");
        }
        printf("%8d: %d\t", vm->registers[0]+index, memory.words[vm->program_size + index]);
    }
    printf("%8d: %d\t", vm->registers[0]+vm->bf_header.data_length, memory.words[vm->program_size + vm->bf_header.data_length]);
    if ((index+1) % 5 == 0) {
        printf("\n%11s     \n", "...");
    }
    else if ((index+1) % 4 == 0) {
        printf("%11s     \n\n", "...");
    }
    else if ((index+1) % 3 == 0) {
        printf("%11s     \n\n", "...");
    }
    else {
        printf("%11s     \n", "...");
    }
}

void print_stack(VM *vm) {
    bool dots_printed = false;
    for (int i = vm->registers[1]; i <= vm->bf_header.stack_bottom_addr; i++) {
        if (i % 5 == 0) {
            printf("\n");
        }
        if (vm->stack_indexes[i] == 0) {
            if (!dots_printed) {
                printf("...");
                dots_printed = true;
            }
            continue;
        }
        printf("%8d: %d\t", i, vm->stack[i]);
    }
}

// Simple Stack Machine execution with detailed debugging
void vm_run(VM *vm) {
    printf("Starting program execution...\n");
    print_registers(vm);
    print_words(vm);
    print_stack(vm);

    while (vm->ip < vm->program_size) {
        int instruction = vm->program[vm->ip++]; // Fetch the instruction

        printf("Executing instruction at ip=%d: %d\n", vm->ip - 1, instruction);  // Debug: Current instruction

        switch (instruction) {
            case 1:  // Example: PUSH (next instruction is the value)
                vm->stack[++vm->registers[1]] = vm->program[vm->ip++];
                printf("PUSH %d to stack. sp=%d\n", vm->stack[vm->registers[1]], vm->registers[1]);  // Debug: PUSH operation
                break;

            case 2:  // Example: ADD (pop two values, add, and push result)
                if (vm->registers[1] < 1) {
                    fprintf(stderr, "Stack underflow on ADD\n");
                    exit(EXIT_FAILURE);
                }
                int a = vm->stack[vm->registers[1]--];
                int b = vm->stack[vm->registers[1]--];
                vm->stack[++vm->registers[1]] = a + b;
                printf("ADD %d and %d, result %d. sp=%d\n", a, b, vm->stack[vm->registers[1]], vm->registers[1]);  // Debug: ADD operation
                break;

            case 3:  // Example: PRINT (pop value and print)
                if (vm->registers[1] < 0) {
                    fprintf(stderr, "Stack underflow on PRINT\n");
                    exit(EXIT_FAILURE);
                }
                printf("Output: %d (popped from stack)\n", vm->stack[vm->registers[1]--]);  // Debug: PRINT operation
                break;

            case 0:  // HALT (stop execution)
                printf("HALT. Stopping execution.\n");
                return;

            default:
                fprintf(stderr, "Unknown instruction: %d at ip=%d\n", instruction, vm->ip - 1);  // Debug: Unknown instruction
                exit(EXIT_FAILURE);
        }

        // Debug: Print the current state of the stack after every instruction
        printf("Stack state (sp=%d): [", vm->registers[1]);
        for (int i = 0; i <= vm->registers[1]; i++) {
            printf("%d", vm->stack[i]);
            if (i < vm->registers[1]) printf(", ");
        }
        printf("]\n");
    }
}
