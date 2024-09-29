#ifndef VM_H
#define VM_H

#include <stdint.h>

// Max stack size
#define STACK_SIZE 1024

// Define the structure of the VM
typedef struct {
    int32_t stack[STACK_SIZE];  // Stack for the machine
    int32_t pc;                 // Program Counter
    int32_t gp;                 // Global pointer
    int32_t sp;                 // Stack pointer
    int32_t fp;                 // Frame pointer
    int32_t ip;                 // Instruction pointer
    int32_t program[STACK_SIZE]; // Program memory (loaded instructions)
    int32_t program_size;       // Size of the loaded program
} VM;

// Function declarations
void vm_load_program(VM *vm, const char *filename);
void vm_print_program(VM *vm);
void vm_run(VM *vm);
void vm_init(VM *vm);

#endif // VM_H
