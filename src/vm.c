#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include "../provided/bof.h"
#include "../provided/machine_types.h"
#include "../provided/instruction.h"

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

    BOFFILE bf_file = bof_read_open(filename);
    BOFHeader bf_header = bof_read_header(bf_file);
    vm->bf_header = bf_header; 

    vm->pc = bf_header.text_start_address;
    vm->gp = bf_header.data_start_address;
    vm->fp = bf_header.stack_bottom_addr;
    vm->sp = bf_header.stack_bottom_addr;

    for (int i = 0; i < bf_header.text_length; i++) {
        bin_instr_t instr = instruction_read(bf_file);
        memory.instrs[i] = instr;
    }

    for (int i = 0; i < bf_header.data_length; i++) {
        word_type word = bof_read_word(bf_file);
        memory.words[i] = word;
    }
}

// Print the loaded program for listing (-p flag)
void vm_print_program(VM *vm) {
    printf("PC: %d\n GP: %d\n FP:%d\n", vm->pc, vm->gp, vm->fp); //Debug: Get Header Values
    printf("Text Length:%d\n", vm->bf_header.text_length); //Debug: Num Instructions

    for (int i = 0; i < vm->bf_header.text_length; i++) {
        char* ins = instruction_assembly_form(i, memory.instrs[i]);
        printf("Instruction: %s\n", ins);
    }
    
    for (int i = 0; i < vm->bf_header.data_length; i++) {
        printf("Word: %d\n", memory.words[i]);
    }
}

// Simple Stack Machine execution with detailed debugging
void vm_run(VM *vm , comp_instr_t *Op0, other_comp_instr_t *Op1, syscall_instr_t *OpOther) {
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
               int binary = memory.instrs[i].comp.func;
                int decim = 0;
                int power = 1;
                    while(binary != 0){
                    decim += (binary % 10) *power;
                    binary /= 10;
                    power *= 2;
                    }
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
                if(opC == 3){
                    int binary = memory.instrs[i].comp.func;
                    int decim = 0;
                    int power = 1;
                        while(binary != 0){
                        decim += (binary % 10) *power;
                        binary /= 10;
                        power *= 2;
                        }
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
