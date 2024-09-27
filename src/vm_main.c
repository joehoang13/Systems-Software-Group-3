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
        vm_run(&vm);
    } else {
        fprintf(stderr, "Invalid arguments.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
