#ifndef VM_H
#define VM_H

#include <stdint.h>
#include "../provided/machine_types.h"
#include "../provided/instruction.h"
#include "../provided/regname.h"

// Word size
#define WORD_IN_BITS 32

#define MEMORY_SIZE_IN_WORDS 32768

// Define the structure of the VM
typedef struct {
    BOFHeader bf_header;        // Loaded BOF Header
    int32_t words_index[MEMORY_SIZE_IN_WORDS]; 
    int32_t pc;
    int32_t HI;
    int32_t LO;
    int32_t registers[NUM_REGISTERS]; 
    int32_t ip;                 // Instruction pointer
    int32_t program[MEMORY_SIZE_IN_WORDS]; // Program memory (loaded instructions)
    int32_t program_size;       // Size of the loaded program
    bool tracing;               
} VM;

// Function declarations
void vm_load_program(VM *vm, const char *filename);
void vm_print_program(VM *vm);
void vm_run(VM *vm, int instruction_number);
void vm_init(VM *vm);
void print_registers(VM *vm);
void print_instruction(VM *vm, int instruction_number);
void print_words(VM *vm);


#endif // VM_H
