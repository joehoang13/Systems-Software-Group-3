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

static union mem_u {
    word_type words[MEMORY_SIZE_IN_WORDS];
    uword_type uwords[MEMORY_SIZE_IN_WORDS];
    bin_instr_t instrs[MEMORY_SIZE_IN_WORDS];
} memory;

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
    //System used to track which data values we want to use in the output, I.E data that is modified otherwise dont print it.
    for (int i = 0; i < bf_header.data_length; i++) {
        word_type word = bof_read_word(bf_file);
        memory.words[bf_header.data_start_address+i] = word;
        vm->words_index[bf_header.data_start_address+i] = 1;
    }
    memory.words[bf_header.data_start_address + bf_header.data_length] = 0;
    vm->words_index[bf_header.data_start_address + bf_header.data_length] = 1;

    memory.words[vm->registers[1]] = 0;
    vm->words_index[vm->registers[1]] = 1;
}

// Print the loaded program for listing (-p flag)
void vm_print_program(VM *vm) {
    printf("Address Instruction\n");

    for (int i = 0; i < vm->program_size; i++) {
        printf("%6d: %s\n", i, instruction_assembly_form(i, memory.instrs[i]));
    }
    int count = 0;
    for (int i = vm->bf_header.data_start_address; i <= vm->bf_header.data_start_address + vm->bf_header.data_length; i++) {
        if (count % 5 == 0 && count != 0) {
            printf("\n");
        }
        printf("%8d: %d\t",  i, memory.words[i]);
        count++;
    }
    if (count % 5 == 0 && count != 0) {
        printf("\n%11s     \n", "...");
    }
    else if (count == 1) {
        printf("%11s     \n", "...");
    }
    else {
        printf("%11s     \n\n", "...");
    }
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
    printf("==>%7d: %s\n", instruction_number, instruction_assembly_form(1, memory.instrs[instruction_number]));
}

void print_words(VM *vm) {
    int index;
    int count = 0;
    for (index = vm->program_size; index <= vm->bf_header.stack_bottom_addr; index++) {
        if (vm->words_index[index] != 1) {
            continue;
        }
        if (count % 5 == 0 && count != 0) {
            printf("\n");
        }
        printf("%8d: %d\t",  index, memory.words[index]);
        count++;
    }
    printf("\n");
}


// Simple Stack Machine execution with detailed debugging
void vm_run(VM *vm, int instruction_number) {
    bin_instr_t instr = memory.instrs[instruction_number];
    instr_type opC = instruction_type(memory.instrs[instruction_number]);
    if(opC == comp_instr_type)
    {
        int func = instr.comp.func;
        switch (func){
            case NOP_F:
                //Literally does nothing.
                vm->pc++;
                break;
            case ADD_F:
                // OP 0/Func 1
                memory.words[vm->registers[instr.comp.rt] + machine_types_formOffset(instr.comp.ot)] = memory.words[vm->registers[1]] + (memory.words[vm->registers[instr.comp.rs] + machine_types_formOffset(instr.comp.os)]);
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.comp.rt] + machine_types_formOffset(instr.comp.ot)] = 1;
                vm->words_index[vm->registers[instr.comp.rs] + machine_types_formOffset(instr.comp.os)] = 1;
                vm->pc++;
                break;
            case SUB_F:
                // OP 0/Func 2
                memory.words[vm->registers[instr.comp.rt] + machine_types_formOffset(instr.comp.ot)] = memory.words[vm->registers[1]] - (memory.words[vm->registers[instr.comp.rs] + machine_types_formOffset(instr.comp.os)]);
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.comp.rt] + machine_types_formOffset(instr.comp.ot)] = 1;
                vm->words_index[vm->registers[instr.comp.rs] + machine_types_formOffset(instr.comp.os)] = 1;
                vm->pc++;
                break;
            case CPW_F:
                // OP 0/Func 3
                memory.words[vm->registers[instr.comp.rt] + machine_types_formOffset(instr.comp.ot)] = memory.words[vm->registers[instr.comp.rs] + machine_types_formOffset(instr.comp.ot)];
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.comp.rt] + machine_types_formOffset(instr.comp.ot)] = 1;
                vm->words_index[vm->registers[instr.comp.rs] + machine_types_formOffset(instr.comp.ot)] = 1;
                vm->pc++;
                break;
            case AND_F:
                // OP 0/Func 5
                memory.words[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]] = memory.words[vm->registers[1]] & memory.words[vm->registers[instr.comp.rs + machine_types_formOffset(instr.comp.os)]];
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]] = 1;
                vm->words_index[vm->registers[instr.comp.rs + machine_types_formOffset(instr.comp.os)]] = 1;
                vm->pc++;
                break;
            case BOR_F:
                // OP 0/Func 6
                memory.words[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]] = memory.words[vm->registers[1]] | memory.words[vm->registers[instr.comp.rs + machine_types_formOffset(instr.comp.os)]];
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]] = 1;
                vm->words_index[vm->registers[instr.comp.rs + machine_types_formOffset(instr.comp.os)]] = 1;
                vm->pc++;
                break;
            case NOR_F:
                // OP 0/Func 7
                memory.words[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]] = ~(memory.words[vm->registers[1]] | memory.words[vm->registers[instr.comp.rs + machine_types_formOffset(instr.comp.os)]]);
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]] = 1;
                vm->words_index[vm->registers[instr.comp.rs + machine_types_formOffset(instr.comp.os)]] = 1;
                vm->pc++;
                break;
            case XOR_F: 
                // OP 0/Func 8
                memory.words[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]] = memory.words[vm->registers[1]] ^ memory.words[vm->registers[instr.comp.rs + machine_types_formOffset(instr.comp.os)]];
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.comp.rt + machine_types_formOffset(instr.comp.ot)]] = 1;
                vm->words_index[vm->registers[instr.comp.rs + machine_types_formOffset(instr.comp.os)]] = 1;
                vm->pc++;
                break;
            case LWR_F:
                // OP 0/Func 9
                vm->registers[instr.comp.rt] = memory.words[vm->registers[instr.comp.rs] + machine_types_formOffset(instr.comp.os)];
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.comp.rs] + machine_types_formOffset(instr.comp.os)] = 1;
                vm->pc++;
                break;
            case SWR_F:
                // OP 0/Func 10
                memory.words[vm->registers[instr.comp.rt] + machine_types_formOffset(instr.comp.ot)] = vm->registers[instr.comp.rs];
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.comp.rt] + machine_types_formOffset(instr.comp.ot)] = 1;
                vm->pc++;
                break;
            case SCA_F:
                // OP 0/Func 11
                memory.words[vm->registers[instr.comp.rt] + machine_types_formOffset(instr.comp.ot)] = (vm->registers[instr.comp.rs] + machine_types_formOffset(instr.comp.os));
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.comp.rt] + machine_types_formOffset(instr.comp.ot)] = 1;
                vm->pc++;
                break;
            case LWI_F:
                // OP 0/Func 12
                memory.words[vm->registers[instr.comp.rt] + machine_types_formOffset(instr.comp.ot)] = memory.words[memory.words[vm->registers[instr.comp.rs] + machine_types_formOffset(instr.comp.rs)]];
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.comp.rt] + machine_types_formOffset(instr.comp.ot)] = 1;
                vm->words_index[memory.words[vm->registers[instr.comp.rs] + machine_types_formOffset(instr.comp.rs)]] = 1;
                vm->pc++;
                break;
            case NEG_F:
                // OP 0/Func 13
                memory.words[vm->registers[instr.comp.rt] + machine_types_formOffset(instr.comp.ot)] = -memory.words[vm->registers[instr.comp.rs] + machine_types_formOffset(instr.comp.os)];
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.comp.rt] + machine_types_formOffset(instr.comp.ot)] = 1;
                vm->words_index[vm->registers[instr.comp.rs] + machine_types_formOffset(instr.comp.os)] = 1;
                vm->pc++;
                break;
        }
    }
    if(opC == other_comp_instr_type){
        int func = instr.comp.func;

        switch(func){
            case LIT_F:
                // OP 1/Func 1
                memory.words[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)] = machine_types_sgnExt(instr.othc.arg);
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)] = 1;
                vm->pc++;
                break;
            case ARI_F:
                // OP 1/Func 2
                vm->registers[instr.othc.reg] = (vm->registers[instr.othc.reg] + machine_types_sgnExt(instr.othc.arg));
                vm->pc++;
                break;
            case SRI_F:
                // OP 1/Func 3
                vm->registers[instr.othc.reg] = (vm->registers[instr.othc.reg] - machine_types_sgnExt(instr.othc.arg));
                vm->pc++;
                break;
            case MUL_F:
                // OP 1/Func 4
                memory.words[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)] = memory.words[vm->registers[1]] * (memory.words[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)]);
                vm->words_index[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)] = 1;
                vm->pc++;
                break;
            case DIV_F:
                // OP 1/Func 5
                vm->HI = memory.words[vm->registers[1]] % (memory.words[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)]);
                vm->LO = memory.words[vm->registers[1]] / (memory.words[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)]);
                vm->pc++;
                break;
            case CFHI_F:
                // OP 1/Func 6
                memory.words[memory.words[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)] = vm->HI;
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[memory.words[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)] = 1;
                vm->pc++;
                break;
            case CFLO_F:
                // OP 1/Func 7
                memory.words[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)] = vm->LO;
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)] = 1;
                vm->pc++;
                break;
            case SLL_F:
                // OP 1/Func 8
                memory.words[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)] = memory.words[vm->registers[1]] << instr.othc.arg;
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)] = 1;
                vm->pc++;
                break;
            case SRL_F:
                // OP 1/Func 9
                memory.words[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)] = memory.words[vm->registers[1]] >> instr.othc.arg;
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)] = 1;
                vm->pc++;
                break;
            case JMP_F:
                // OP 1/Func 10
                vm->pc = memory.words[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)];
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)] = 1;
                break;
            case CSI_F:
                // OP 1/Func 11
                vm->registers[7] = vm->pc;
                vm->pc = memory.words[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)];
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[instr.othc.reg] + machine_types_formOffset(instr.othc.offset)] = 1;
                break;
            case JREL_F:
                // OP 1/Func 15
                vm->pc = ((vm->pc - 1) + machine_types_formOffset(instr.othc.offset));
                break;
        }
    }
    if(opC == immed_instr_type){
        immed_instr_t immed = instr.immed;
        int op = immed.op;
        switch(op){
            case ADDI_O:
                //OP 2
                memory.words[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] += machine_types_sgnExt(immed.immed);
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] = 1;
                vm->pc++;
                break;
            case ANDI_O:
                //OP 3
                memory.words[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] &= machine_types_zeroExt(immed.immed);
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] = 1;
                vm->pc++;
                break;
            case BORI_O:
                //OP 4
                memory.words[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] |= machine_types_zeroExt(immed.immed);
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] = 1;
                vm->pc++;
                break;
            case NORI_O:
                //OP 5
                memory.words[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] =  ~(machine_types_zeroExt(immed.immed) | (memory.words[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)]));
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] = 1;
                vm->pc++;
                break;
            case XORI_O:
                //OP 6
                memory.words[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] ^= machine_types_zeroExt(immed.immed);
                //Code Below used to store the index that we want to print in the output.
                vm->words_index[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] = 1;
                vm->pc++;
                break;
            case BEQ_O:
                if (memory.words[vm->registers[1]] == memory.words[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)]) {
                    vm->pc--;
                    vm->pc += machine_types_formOffset(immed.immed);
                }
                else {
                    vm->pc++;
                }
                break;
            case BGEZ_O:
                if (memory.words[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] >= 0) {
                    vm->pc--;
                    vm->pc += machine_types_formOffset(immed.immed); 
                }
                else {
                    vm->pc++;
                }
                break;
            case BGTZ_O:
                if (memory.words[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] > 0) {
                    vm->pc--;
                    vm->pc += machine_types_formOffset(immed.immed); 
                }
                else {
                    vm->pc++;
                }
                break;
            case BLEZ_O:
                if (memory.words[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] <= 0) {
                    vm->pc--;
                    vm->pc += machine_types_formOffset(immed.immed); 
                }
                else {
                    vm->pc++;
                }
                break; 
            case BLTZ_O:
                if (memory.words[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)] < 0) {
                    vm->pc--;
                    vm->pc += machine_types_formOffset(immed.immed); 
                }
                else {
                    vm->pc++;
                }
                break;
            case BNE_O:
                if (memory.words[vm->registers[1]] != memory.words[vm->registers[immed.reg] + machine_types_formOffset(immed.offset)]) {
                    vm->pc--;
                    vm->pc += machine_types_formOffset(immed.immed);
                }
                else {
                    vm->pc++;
                }
                break;
        }
    }
    if (opC == jump_instr_type){
        int binary = instr.jump.op;
        switch(binary){
            case JMPA_O:
                //OP 13
                vm->pc = machine_types_formAddress(vm->pc - 1, instr.jump.addr);
                break;
            case CALL_O:
                //OP 14
                vm->registers[7] = vm->pc;
                vm->pc = machine_types_formAddress(vm->pc - 1, instr.jump.addr);
                break; 
            case RTN_O:
                //OP 15
                vm->pc = vm->registers[7];
                break;
        }
    }
    if (opC == syscall_instr_type) {
        int code = instr.syscall.code;
        int reg = instr.syscall.reg;
        int offset = instr.syscall.offset;
        //All of these Op's should be OP 1 FUNC 15.
        switch(code){
            case 1: 
                printf("GFDJGDOFHGDFHGDFH");
                print_instruction(vm, instruction_number);
                exit(machine_types_sgnExt(offset));
                vm->pc++;
                break;

            case 2: 
                char* str = (char*)&memory.words[vm->registers[reg] + machine_types_formOffset(offset)];
                memory.words[vm->registers[1]] = printf("%s", str);
                vm->words_index[vm->registers[1]] = 1;
                vm->pc++;
                break;
            case 3:
                int integer = (int)memory.words[vm->registers[reg] + machine_types_formOffset(offset)];
                memory.words[vm->registers[1]] = printf("%d", integer);
                vm->words_index[vm->registers[1]] = 1;
                vm->pc++;
                break;

            case 4: 
                memory.words[vm->registers[1]] = fputc(memory.words[vm->registers[reg] + machine_types_formOffset(offset)], stdout);
                vm->words_index[vm->registers[1]] = 1;
                vm->pc++;
                break;

            case 5:
                vm->registers[reg + machine_types_formOffset(offset)] = getc(stdin);
                vm->pc++;
                break;
            case 2046:
                vm->tracing = true;
                vm->pc++;
                break;
            case 2047:
                vm->tracing = false;
                vm->pc++;
                break;
        }
    }
}
