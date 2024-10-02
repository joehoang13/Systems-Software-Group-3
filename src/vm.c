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
    int count = 0;
    for (int i = 1024; i <= vm->bf_header.stack_bottom_addr; i++) {
        if (count % 5 == 0 && count != 0) {
            printf("\n");
        }
        if (vm->stack_indexes[i] == 0) {
            if (!dots_printed && count > 0) {
                printf("...");
                dots_printed = true;
            }
            continue;
        }
        printf("%8d: %d\t", i, vm->stack[i]);
        count++;
    }
    printf("\n\n");
}

// Simple Stack Machine execution with detailed debugging
void vm_run(VM *vm, int instruction_number) {
    bin_instr_t instr = memory.instrs[instruction_number];
    bin_instr_t instr = memory.instrs[instruction_number];
    instr_type opC = instruction_type(memory.instrs[instruction_number]);
    if(opC == comp_instr_type)
    {
        int func = instr.comp.func;
        switch (func){
            case NOP_F:
                //Literally does nothing.
            case ADD_F:
                vm->stack[instr.comp.rt + machine_types_formOffset(instr.comp.ot)] = vm->registers[1] + (vm->stack[instr.comp.rs + machine_types_formOffset(instr.comp.ot)]);
                break;
            case SUB_F:
                vm->stack[instr.comp.rt + machine_types_formOffset(instr.comp.ot)] = vm->registers[1] - (vm->stack[instr.comp.rs + machine_types_formOffset(instr.comp.ot)]);
                break;
            case CPW_F:
                vm->stack[instr.comp.rt + machine_types_formOffset(instr.comp.ot)] = vm->stack[instr.comp.rs + machine_types_formOffset(instr.comp.ot)];
                break;
            case AND_F:
                vm->stack[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]] = vm->stack[vm->registers[1]] & vm->stack[vm->registers[instr.comp.rs + machine_types_formOffset(instr.comp.os)]];
                vm->stack_indexes[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]];
                break;
            case BOR_F:
                vm->stack[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]] = vm->stack[vm->registers[1]] | vm->stack[vm->registers[instr.comp.rs + machine_types_formOffset(instr.comp.os)]];
                vm->stack_indexes[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]];
                break;
            case NOR_F:
                vm->stack[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]] = ~(vm->stack[vm->registers[1]] | vm->stack[vm->registers[instr.comp.rs + machine_types_formOffset(instr.comp.os)]]);
                vm->stack_indexes[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]];
                break;
            case XOR_F: 
                vm->stack[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]] = vm->stack[vm->registers[1]] ^ vm->stack[vm->registers[instr.comp.rs + machine_types_formOffset(instr.comp.os)]];
                vm->stack_indexes[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]];
                break;
            case LWR_F:
                instr.comp.rt = vm->stack[instr.comp.rs + machine_types_formOffset(instr.comp.os)];
                break;
            case SWR_F:
                vm->stack[instr.comp.rt + machine_types_formOffset(instr.comp.ot)] = instr.comp.rs;
                break;
            case SCA_F:
                vm->stack[instr.comp.rt + machine_types_formOffset(instr.comp.ot)] = (instr.comp.rs + machine_types_formOffset(instr.comp.os));
                break;
            case LWI_F:
                vm->stack[instr.comp.rt + machine_types_formOffset(instr.comp.ot)] = vm->stack[vm->stack[instr.comp.rs + machine_types_formOffset(instr.comp.rs)]];
                break;
            case NEG_F:
                vm->stack[instr.comp.rt + machine_types_formOffset(instr.comp.ot)] = -vm->stack[instr.comp.rs + machine_types_formOffset(instr.comp.os)];
                break;
        }
    }
    if(opC == other_comp_instr_type){
        int func = instr.comp.func;

        switch(func){
            case LIT_F:
                vm->stack[instr.othc.reg + machine_types_formOffset(instr.othc.offset)] = machine_types_sgnExt(instr.othc.arg);
                break;
            case ARI_F:
                instr.othc.reg = (instr.othc.reg + machine_types_sgnExt(instr.othc.arg));
                break;
            case SRI_F:
                instr.othc.reg = (instr.othc.reg - machine_types_sgnExt(instr.othc.arg));
                break;
            case MUL_F:
                vm->stack[instr.othc.reg + machine_types_formOffset(instr.othc.offset)] = vm->stack[vm->registers[1]] * (vm->stack[instr.othc.reg + machine_types_formOffset(instr.othc.offset)]);
                break;
            case DIV_F:
                vm->HI = vm->stack[vm->registers[1]] % (vm->stack[instr.othc.reg + machine_types_formOffset(instr.othc.offset)]); vm->LO = vm->stack[vm->registers[1]] / (vm->stack[instr.othc.reg + machine_types_formOffset(instr.othc.offset)]);
                break;
            case CFHI_F:
                vm->stack[instr.othc.reg + machine_types_formOffset(instr.othc.offset)] = vm->HI;
                break;
            case CFLO_F:
                vm->stack[instr.othc.reg + machine_types_formOffset(instr.othc.offset)] = vm->LO;
                break;
            case SLL_F:
                vm->stack[instr.othc.reg + machine_types_formOffset(instr.othc.offset)] = vm->stack[vm->registers[1]] << instr.othc.arg;
                break;
            case SRL_F:
                vm->stack[instr.othc.reg + machine_types_formOffset(instr.othc.offset)] = vm->stack[vm->registers[1]] >> instr.othc.arg;
                break;
            case JMP_F:
                vm->pc = vm->stack[instr.othc.reg + machine_types_formOffset(instr.othc.offset)];
                break;
            case CSI_F:
                vm->registers[7] = vm->pc;  vm->pc = vm->stack[instr.othc.reg + machine_types_formOffset(instr.othc.offset)];
                break;
            case JREL_F:
                vm->pc = ((vm->pc - 1) + machine_types_formOffset(instr.othc.offset));
                break;
            case 15:
                int sys = memory.instrs[instruction_number].syscall.code;
                switch(sys){
                    case exit_sc:

                    case print_str_sc:

                    case print_char_sc:

                    case read_char_sc:

                    case start_tracing_sc:

                    case stop_tracing_sc:
                    
                }
        }
    }
    if(opC == immed_instr_type){
        immed_instr_t immed = instr.immed;
        int op = immed.op;
        switch(op){
            case ADDI_O:
                vm->stack[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] += machine_types_sgnExt(immed.immed);
                vm->stack_indexes[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] = 1;
                vm->pc++;
                break;
            case ANDI_O:
                vm->stack[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] &= machine_types_zeroExt(immed.immed);
                vm->stack_indexes[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] = 1;
                vm->pc++;
                break;
            case BORI_O:
                vm->stack[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] |= machine_types_zeroExt(immed.immed);
                vm->stack_indexes[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] = 1;
                vm->pc++;
                break;
            case NORI_O:
                vm->stack[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] =  ~(machine_types_zeroExt(immed.immed) | (vm->stack[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)]));
                vm->stack_indexes[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] = 1;
                vm->pc++;
                break;
            case XORI_O:
                vm->stack[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] ^= machine_types_zeroExt(immed.immed);
                vm->stack_indexes[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] = 1;
                vm->pc++;
                break;
            case BEQ_O:
                if (vm->registers[1] == vm->registers[immed.reg] + machine_types_formOffset(immed.offset)) {
                    vm->pc--;
                    vm->pc += machine_types_formOffset(immed.immed);
                }
                break;
            case BGEZ_O:
                if (vm->registers[immed.reg] + machine_types_formOffset(immed.offset) >= 0) {
                    vm->pc--;
                    vm->pc += machine_types_formOffset(immed.immed); 
                }
                break;
            case BGTZ_O:
                if (vm->registers[immed.reg] + machine_types_formOffset(immed.offset) > 0) {
                    vm->pc--;
                    vm->pc += machine_types_formOffset(immed.immed); 
                }
                break;
            case BLEZ_O:
                if (vm->registers[immed.reg] + machine_types_formOffset(immed.offset) <= 0) {
                    vm->pc--;
                    vm->pc += machine_types_formOffset(immed.immed); 
                }
                break; 
            case BLTZ_O:
                if (vm->registers[immed.reg] + machine_types_formOffset(immed.offset) < 0) {
                    vm->pc--;
                    vm->pc += machine_types_formOffset(immed.immed); 
                }
                break;
            case BNE_O:
                if (vm->registers[1] != vm->registers[immed.reg] + machine_types_formOffset(immed.offset)) {
                    vm->pc--;
                    vm->pc += machine_types_formOffset(immed.immed);
                }
                break;
        }
    }
    if (opC == jump_instr_type){
        int binary = instr.jump.op;
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
        int binary = instr.syscall.func;
        switch(binary) {
            case exit_sc:
                vm->tracing = false;
                break;
        }
    }
}