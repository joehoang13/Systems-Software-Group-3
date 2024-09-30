# Makefile for Simple Stack Machine (SSM) Project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Directories
SRC_DIR = src
OBJ_DIR = obj
PROVIDED_DIR = provided
TEST_DIR = $(PROVIDED_DIR)
ASM = $(PROVIDED_DIR)/asm

# Executable name
EXECUTABLE = vm

# Source and object files
VM_SOURCES = $(SRC_DIR)/vm_main.c $(SRC_DIR)/vm.c
VM_OBJECTS = $(OBJ_DIR)/vm_main.o $(OBJ_DIR)/vm.o\
             $(PROVIDED_DIR)/machine_types.o $(PROVIDED_DIR)/instruction.o $(PROVIDED_DIR)/bof.o \
             $(PROVIDED_DIR)/regname.o $(PROVIDED_DIR)/utilities.o

# Test binary files
TEST_BOF_FILES = $(wildcard $(TEST_DIR)/*.bof)

# Target for compiling the VM
all: $(EXECUTABLE)

# Create the object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Compile the object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link the object files to create the VM executable
$(EXECUTABLE): $(VM_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Run the assembler
asm:
	$(MAKE) -C $(PROVIDED_DIR) asm

# Clean the project
clean:
	rm -rf $(OBJ_DIR) $(EXECUTABLE) $(TEST_DIR)/*.myo $(TEST_DIR)/*.myp

# Run VM on test files to check program listing output (-p flag)
check-lst-outputs: $(EXECUTABLE)
	@for file in $(TEST_BOF_FILES); do \
		echo "Checking listing output for $$file..."; \
		./$(EXECUTABLE) -p $$file > $$file.myp; \
	done

# Run VM on test files to check execution output
check-vm-outputs: $(EXECUTABLE)
	@for file in $(TEST_BOF_FILES); do \
		echo "Checking execution output for $$file..."; \
		./$(EXECUTABLE) $$file > $$file.myo; \
	done

# Run all tests (both listing and execution)
check-outputs: check-lst-outputs check-vm-outputs

# Create submission zip file
submission.zip:
	zip -r submission.zip $(SRC_DIR) Makefile README.md $(PROVIDED_DIR)

