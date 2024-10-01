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
    print_registers(vm);
    print_words(vm);
    print_stack(vm);
    for (int i=0; i<0; i++){
         instr_type * opC = instruction_type(memory.instrs[i]);
            if(opC == 0){
               int binary = memory.instrs[i].comp.func;
                int decim = 0;
                int power = 1;
                    while(binary != 0){
                    decim += (binary % 10) *power;
                    binary /= 10;
                    power *= 2;
                    }
                printf("This is the func in INT: %d",decim);
                switch (decim){
                    case NOP_F:
                        //Literally does nothing.
                    case ADD_F:
                        memory[GPR[Op0->ot] + formOffset(Op0->os)] = memory[GPR[$sp]] + (memory[GPR[s] + formOffset(os)]);
                    case SUB_F:
                        memory[GPR[t] + formOffset(ot)] = memory[GPR[$sp]] − (memory[GPR[s] + formOffset(os)]);
                    case CPW_F:
                        memory[GPR[t] + formOffset(ot)] = memory[GPR[s] + formOffset(os)];
                    case AND_F:
                        umemory[GPR[t] + formOffset(ot)]; = umemory[GPR[$sp]] ∧ (umemory[GPR[s] + formOffset(os)]);
                    case BOR_F:
                        umemory[GPR[t] + formOffset(ot)]; = umemory[GPR[$sp]] ∨ (umemory[GPR[s] + formOffset(os)]);
                    case NOR_F:
                        umemory[GPR[t] + formOffset(ot)];= -(umemory[GPR[$sp]] ∨ (umemory[GPR[s] + formOffset(os)]));
                    case XOR_F:
                        umemory[GPR[t] + formOffset(ot)]; ← umemory[GPR[$sp]] xor (umemory[GPR[s] + formOffset(os)]);
                    case LWR_F:
                        GPR[t] ← memory[GPR[s] + formOffset(os)];
                    case SWR_F:
                        memory[GPR[t] + formOffset(ot)] ← GPR[s];
                    case SCA_F:
                        memory[GPR[t] + formOffset(ot)] ← (GPR[s] + formOffset(os));
                    case LWI_F:
                        memory[GPR[t] + formOffset(ot)] ← memory[memory[GPR[s] + formOffset(os)]];
                    case NEG_F:
                        memory[GPR[t] + formOffset(ot)] ← −memory[GPR[s] + formOffset(os)];
            }
            if(opC == 1){
               int binary = memory.instrs[i].comp.func;
                int decim = 0;
                int power = 1;
                    while(binary != 0){
                    decim += (binary % 10) *power;
                    binary /= 10;
                    power *= 2;
                    }
                    printf("This is the func in INT: %d",decim);

                switch(decim){
                case LIT_F:
                    memory[GPR[t] + formOffset(o)] ← sgnExt(i)
                case ARI_F:
                    GPR[r] ← (GPR[r] + sgnExt(i))
                case SRI_F:
                    GPR[r] ← (GPR[r] − sgnExt(i))
                case MUL_F:
                    [GPR[s] + formOffset(o)]← memory[GPR[$sp]] × (memory[GPR[s] + formOffset(o)])
                case DIV_F:
                    HI ← memory[GPR[$sp]] % (memory[GPR[s] + formOffset(o)]); LO ← memory[GPR[$sp]] / (memory[GPR[s] + formOffset(o)])
                case CFHI_F:
                    memory[GPR[t] + formOffset(o)] ← HI
                case CFLO_F:
                    memory[GPR[t] + formOffset(o)] ← LO
                case SLL_F:
                    umemory[GPR[t] + formOffset(o)] ← umemory[GPR[$sp]] « h
                case SRL_F:
                    umemory[GPR[t] + formOffset(o)] ← umemory[GPR[$sp]] » h
                case JMP_F:
                    PC ← umemory[GPR[s] + formOffset(o)];
                case CSI_F:
                    GPR[$ra] ← PC; PC ← memory[GPR[s] + formOffset(o)]
                case JREL_F:
                    PC ← ((PC − 1) + formOffset(o));
                }
            }
            if(opC == 2){
                int binary = memory.instrs[i].immed.op;
                int decim = 0;
                int power = 1;
                    while(binary != 0){
                    decim += (binary % 10) *power;
                    binary /= 10;
                    power *= 2;
                    }
                    printf("This is the func in INT: %d",decim);
                switch(decim){
                    //
                    case ADDI_O:
                    //ADDI
                            memory.instrs[] + formOffset(OpOther->offset) = (memory.words[OpOther->reg] + formOffset(o)) + sgnExt(i)
                            
                    case ANDI_O:
                    //
                            umemory[GPR[r] + formOffset(o)] ← (umemory[GPR[r] + formOffset(o)]) ∧ zeroExt(i)
                    case BORI_O:
                    //
                            umemory[GPR[r] + formOffset(o)] ← (umemory[GPR[r] + formOffset(o)]) ∨ zeroExt(i)
                    case NORI_O:
                    //
                            umemory[GPR[r] + formOffset(o)] ← ¬(umemory[GPR[r] + formOffset(o)]) ∨ zeroExt(i))
                    case XORI_O:
                    //
                            umemory[GPR[r] + formOffset(o)] ← (umemory[GPR[r] + formOffset(o)]) xor zeroExt(i)
                    case BEQ_O:
                    //
                            if memory[GPR[$sp]] = memory[GPR[r] + formOffset(o)] then PC ← (PC − 1) + formOffset(i)
                    case BGEZ_O:
                    //
                            if memory[GPR[r] + formOffset(o)] ≥ 0   then PC ← (PC − 1) + formOffset(i)
                    case BGTZ_O:
                    //
                            if memory[GPR[r] + formOffset(o)] > 0 then PC ← (PC − 1) + formOffset(i)
                    case BLEZ_O:
                    //
                            if memory[GPR[r] + formOffset(o)] ≤ 0 then PC ← (PC − 1) + formOffset(i)
                    case BLTZ_O:
                    //
                            if memory[GPR[r] + formOffset(o)] < 0 then PC ← (PC − 1) + formOffset(i)
                    case BNE_O:
                    //
                            if memory[GPR[$sp]] != memory[GPR[r] + formOffset(o)] then PC ← (PC − 1) + formOffset(i)
                }
                if (opC == 3){
                int binary = memory.instrs[i].jump.op;
                int decim = 0;
                int power = 1;
                    while(binary != 0){
                    decim += (binary % 10) *power;
                    binary /= 10;
                    power *= 2;
                    }
                    printf("This is the func in INT: %d",decim);
                switch(decim){
                    case JMPA_O:
                    //
                        PC ← formAddress(P C − 1, a)
                    case CALL_O:
                    //
                        GPR[$ra] ← PC; PC ← formAddress(PC − 1, a)
                    case RTN_O:
                    //
                        PC ← GPR[$ra]
                    }
                }
                if (opC == 4){
                int binary = memory.instrs[i].syscall.func;
                int decim = 0;
                int power = 1;
                    while(binary != 0){
                    decim += (binary % 10) *power;
                    binary /= 10;
                    power *= 2;
                    }
                    printf("This is the func in INT: %d",decim);
                }
            default:
                fprintf(stderr, "Unknown instruction: %d at ip=%d\n", instruction, vm->ip - 1);  // Debug: Unknown instruction
                exit(EXIT_FAILURE);
            }
        }
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
