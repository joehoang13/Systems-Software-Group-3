#ifndef VM_H
#define VM_H

#include <stdint.h>
#include "../provided/machine_types.h"
#include "../provided/instruction.h"

// Max stack size
#define STACK_SIZE 1024

// Word size
#define WORD_IN_BITS 32

#define MEMORY_SIZE_IN_WORDS 32768

static union mem_u {
word_type words[MEMORY_SIZE_IN_WORDS];
uword_type uwords[MEMORY_SIZE_IN_WORDS];
bin_instr_t instrs[MEMORY_SIZE_IN_WORDS];
} memory;

// Define the structure of the VM
typedef struct {
    BOFHeader bf_header;        // Loaded BOF Header
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
