#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s [-p] <program.bof>\n", argv[0]);
        return EXIT_FAILURE;
    }

    VM vm;
    vm_init(&vm);
    // Check if the -p flag is present
    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        vm_load_program(&vm, argv[2]);
        vm_print_program(&vm);
    } else if (argc == 2) {
        vm_load_program(&vm, argv[1]);
        print_registers(&vm);
        print_words(&vm);
        for (int i = 0; i < vm.program_size; i++) {
            if(vm.tracing) {
                print_instruction(&vm, vm.pc);
            }
            vm_run(&vm, vm.pc);
            if (vm.tracing) {
                print_registers(&vm);
                print_words(&vm);
            }
        }
    } else {
        fprintf(stderr, "Invalid arguments.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
