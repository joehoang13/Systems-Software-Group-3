#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include "../provided/bof.h"
#include "../provided/machine_types.h"
#include "../provided/regname.h"
#include "../provided/instruction.h"

// Initialize the VM with default values
void vm_init(VM *vm) {
    vm->ip = 0;  // Instruction pointer starts at the first instruction
    vm->program_size = 0;  // No program loaded initially
    vm->pc = 0;
    vm->tracing = true;
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
    printf("==>%8d: %s\n", instruction_number, instruction_assembly_form(1, memory.instrs[instruction_number]));
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
    printf("\n");
}

// Simple Stack Machine execution with detailed debugging
void vm_run(VM *vm, int instruction_number) {
    instr_type opC = instruction_type(memory.instrs[instruction_number]);
    if(opC == comp_instr_type)
    {
        int func = memory.instrs[instruction_number].comp.func;
        switch (func){
            case NOP_F:
                //Literally does nothing.
            case ADD_F:
            case SUB_F:
            case CPW_F:
            case AND_F:
            case BOR_F:
            case NOR_F:
            case XOR_F:
            case LWR_F:
            case SWR_F:
            case SCA_F:
            case LWI_F:
            case NEG_F:
        }
    }
    if(opC == other_comp_instr_type){
        int func = memory.instrs[instruction_number].comp.func;

        switch(func){
            case LIT_F:
            case ARI_F:
            case SRI_F:
            case MUL_F:
            case DIV_F:
            case CFHI_F:
            case CFLO_F:
            case SLL_F:
            case SRL_F:
            case JMP_F:
            case CSI_F:
            case JREL_F:
        }
    }
    if(opC == immed_instr_type){
        int op = memory.instrs[instruction_number].immed.op;
        switch(op){
            //
            case ADDI_O:
                    
            //ADDI
            case ANDI_O:
            //
            case BORI_O:
            //
            case NORI_O:
            //
            case XORI_O:
            //
            case BEQ_O:
            //
            case BGEZ_O:
            //
            case BGTZ_O:
            //
            case BLEZ_O:
            //
            case BLTZ_O:
            //
            case BNE_O:
            //
        }
    }
    if (opC == jump_instr_type){
        int binary = memory.instrs[instruction_number].jump.op;
        switch(binary){
            case JMPA_O:
            //
            case CALL_O:
            //
            case RTN_O:
            //
        }
    }
    if (opC == syscall_instr_type){
        int binary = memory.instrs[instruction_number].syscall.func;
        switch(binary) {
            case exit_sc:
                vm->tracing = false;
                break;
        }
    }
    vm->pc++;
}