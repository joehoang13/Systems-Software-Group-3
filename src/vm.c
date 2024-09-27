#include "vm.h"
#include <stdio.h>
#include <stdlib.h>

// Initialize the VM with default values
void vm_init(VM *vm) {
    vm->sp = -1; // Stack pointer starts at -1 (empty stack)
    vm->ip = 0;  // Instruction pointer starts at the first instruction
    vm->program_size = 0;  // No program loaded initially
}

// Load the program (instructions) into the VM with debugging
void vm_load_program(VM *vm, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    printf("Loading program from %s:\n", filename);

    // Read the instructions into the program array
    int32_t instruction;
    while (fread(&instruction, sizeof(int32_t), 1, file) == 1) {
        vm->program[vm->program_size] = instruction;
        printf("Instruction %d: %d\n", vm->program_size, instruction);  // Debug: Print the loaded instruction
        vm->program_size++;
    }

    fclose(file);
    printf("Finished loading program. Total instructions: %d\n", vm->program_size);  // Debug: Total program size
}

// Print the loaded program for listing (-p flag)
void vm_print_program(VM *vm) {
    printf("Loaded Program:\n");
    for (int i = 0; i < vm->program_size; i++) {
        printf("%d: %d\n", i, vm->program[i]);
    }
}

// Simple Stack Machine execution with detailed debugging
void vm_run(VM *vm) {
    printf("Starting program execution...\n");

    while (vm->ip < vm->program_size) {
        int instruction = vm->program[vm->ip++]; // Fetch the instruction

        printf("Executing instruction at ip=%d: %d\n", vm->ip - 1, instruction);  // Debug: Current instruction

        switch (instruction) {
            case 1:  // Example: PUSH (next instruction is the value)
                vm->stack[++vm->sp] = vm->program[vm->ip++];
                printf("PUSH %d to stack. sp=%d\n", vm->stack[vm->sp], vm->sp);  // Debug: PUSH operation
                break;

            case 2:  // Example: ADD (pop two values, add, and push result)
                if (vm->sp < 1) {
                    fprintf(stderr, "Stack underflow on ADD\n");
                    exit(EXIT_FAILURE);
                }
                int a = vm->stack[vm->sp--];
                int b = vm->stack[vm->sp--];
                vm->stack[++vm->sp] = a + b;
                printf("ADD %d and %d, result %d. sp=%d\n", a, b, vm->stack[vm->sp], vm->sp);  // Debug: ADD operation
                break;

            case 3:  // Example: PRINT (pop value and print)
                if (vm->sp < 0) {
                    fprintf(stderr, "Stack underflow on PRINT\n");
                    exit(EXIT_FAILURE);
                }
                printf("Output: %d (popped from stack)\n", vm->stack[vm->sp--]);  // Debug: PRINT operation
                break;

            case 0:  // HALT (stop execution)
                printf("HALT. Stopping execution.\n");
                return;

            default:
                fprintf(stderr, "Unknown instruction: %d at ip=%d\n", instruction, vm->ip - 1);  // Debug: Unknown instruction
                exit(EXIT_FAILURE);
        }

        // Debug: Print the current state of the stack after every instruction
        printf("Stack state (sp=%d): [", vm->sp);
        for (int i = 0; i <= vm->sp; i++) {
            printf("%d", vm->stack[i]);
            if (i < vm->sp) printf(", ");
        }
        printf("]\n");
    }
}
