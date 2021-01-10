/*
 * main.c - Main source file of LC-3
 * Written by Ercan Ersoy.
 */


// Define memory size
#define MEMORY 65536

// Includes
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Memory declaration
uint16_t memory[MEMORY];

// Enumerator type for registers and register count
enum
{
    R_R0 = 0,       // Register R0
    R_R1,           // Register R1
    R_R2,           // Register R2
    R_R3,           // Register R3
    R_R4,           // Register R4
    R_R5,           // Register R5
    R_R6,           // Register R6
    R_R7,           // Register R7
    R_PC,           // Register PC
    R_COND,         // Register COND
    REGISTER_COUNT  // Count of registers
};

// Registers declaration
uint16_t registers[REGISTER_COUNT];

// Enumerator type for opcodes
enum
{
    OP_BR = 0,  // Branch
    OP_ADD,     // Addition
    OP_LD,      // Load
    OP_ST,      // Store
    OP_JSR,     // Jump register
    OP_AND,     // And operator
    OP_LDR,     // Load register
    OP_STR,     // Load effective address
    OP_RTI,     // Return from interrupt
    OP_NOT,     // Not operator
    OP_LDI,     // Load indirect
    OP_STI,     // Store indirect
    OP_JMP,     // Jump
    OP_RES,     // Reserve memory directive
    OP_LEA,     // Load effective address
    OP_TRAP     // Trap
};

// Enumerator type for flags
enum
{
    FL_POS = 1 << 0,  // Positive flag
    FL_ZRO = 1 << 1,  // Zero flag
    FL_NEG = 1 << 2   // Negative flag
};

// Trap codes
enum
{
    TRAP_GETC = 0x20,   // Input a character
    TRAP_OUT = 0x21,    // Output a character
    TRAP_PUTS = 0x22,   // Output a word string
    TRAP_IN = 0x23,     // Input a character
    TRAP_PUTSP = 0x24,  // Output a byte string
    TRAP_HALT = 0x25    // Halt the program
};

// Memory mapped registers
enum
{
    MR_KBSR = 0xFE00,  // Keyboard status register
    MR_KBDR = 0xFE02   // Keyboard data register
};

// Running status
bool running = false;

// Swap word function
uint16_t swap_word(uint16_t x)
{
    // Swap byte
    return (x << 8) | (x >> 8);
}

//Read image file function
uint16_t read_image_file(FILE* file)
{
    // Origin
    uint16_t origin;

    // Read origin address from the program file
    fread(&origin, sizeof(origin), 1, file);
    // Swap bytes
    origin = swap_word(origin);

    // Calculate maximum reading size
    uint16_t maximum_read = MEMORY - origin;
    // Program start pointer
    uint16_t *pointer = memory + origin;
    // Reading instructions and data
    size_t read = fread(pointer, sizeof(uint16_t), maximum_read, file);

    // Swapping instructions and data
    while(read-- > 0)
    {
        // Word swap
        *pointer = swap_word(*pointer);
        // Increment the pointer
        pointer++;
    }
}

// Read image function
uint16_t read_image(const char* image_path)
{
    // Open the program file
    FILE* file = fopen(image_path, "rb");

    // If the file is not open
    if(!file)
    {
        // Returning false status
        return false;
    }

    // Reading file
    read_image_file(file);

    // Close file
    fclose(file);

    // Returning true status
    return true;
}

// Memory write function
uint16_t memory_write(uint16_t address, uint16_t value)
{
    // Write value to the address of the memory
    return memory[address] = value;
}

// Memory read function
uint16_t memory_read(uint16_t address)
{
    // Switch of memory mapped registers
    switch(address)
    {
        // Case of keyboard status register
        case MR_KBSR:
            // Write keynoard status to memory
            memory[MR_KBSR] = (1 << 15);
            // Write character from keyboard to memory
            memory[MR_KBDR] = getchar();

            break;  // Break
    }

    // Return character
    return memory[address];
}

// Update flags function
void update_flags(uint16_t register_value)
{
    // If the register value equals to zero
    if (registers[register_value] == 0)
    {
        // Set flag zero
        registers[R_COND] = FL_ZRO;
    }
    // If the register has negative value
    else if(registers[register_value] >> 15)
    {
        // Set flag negative
        registers[R_COND] = FL_NEG;
    }
    // If the register has positive value
    else
    {
        // Set flag positive
        registers[R_COND] = FL_POS;
    }
}

// Sign extend function
uint16_t sign_extend(uint16_t value, int bit_count)
{
    // If the value sign is set
    if((value >> (bit_count - 1) & 1))
    {
        // Shift sign bit
        value |= (0xFFFF << bit_count);
    }

    // Return new value
    return value;
}

// Main function of the program
int main(int argc, char* argv[])
{
    // If argument count is two
    if(argc == 2)
    {
        // If not read image file
        if(!read_image(argv[1]))
        {
            // Print the error message
            fprintf(stderr, "Failed to load this image! Exiting!");

            // Exit with "failed to load this image" error
            return 2;
        }
    }
    else
    {
        // Print the error message
        fprintf(stderr, "Wrong argument count! Exiting!");

        // Exit with "wrong argument count" error
        return 1;
    }

    // Start program
    registers[R_PC] = 0X3000;

    // Set running status
    running = true;

    // Running loop
    while(running)
    {
        // Get instruction
        uint16_t instruction = memory_read(registers[R_PC]++);
        // Get operand
        uint16_t operand = instruction >> 12;

        // Operand switch
        switch(operand)
        {
            // Case of ADD operand
            case OP_ADD:
            {
                // Get the first register
                uint16_t r0 = (instruction >> 9) & 0x7;
                // Get the second register
                uint16_t r1 = (instruction >> 6) & 0x7;
                // Get the imm value
                uint16_t immediate_mode_flag = (instruction >> 5) & 0x1;

                // If imm is set
                if(immediate_mode_flag)
                {
                    // Get the imm5 value
                    uint16_t imm5 = sign_extend(instruction & 0x1F, 5);

                    // Addition
                    registers[r0] = registers[r1] + imm5;
                }
                // If imm is not set
                else
                {
                    // Get the third register
                    uint16_t r2 = instruction & 0x7;
                    // Addition
                    registers[r0] = registers[r1] + registers[r2];
                }

                // Update flags
                update_flags(r0);

                // Break
                break;
            }
            // Case of AND operand
            case OP_AND:
            {
                // Get the first register
                uint16_t r0 = (instruction >> 9) & 0x7;
                // Get the second register
                uint16_t r1 = (instruction >> 6) & 0x7;
                // Get the imm value
                uint16_t immediate_mode_flag = (instruction >> 5) & 0x1;

                // If imm is set
                if(immediate_mode_flag)
                {
                    // Get the imm5 value
                    uint16_t imm5 = sign_extend(instruction & 0x1F, 5);

                    // Addition
                    registers[r0] = registers[r1] & imm5;
                }
                // If imm is not set
                else
                {
                    // Get the third register
                    uint16_t r2 = instruction & 0x7;
                    // Addition
                    registers[r0] = registers[r1] & registers[r2];
                }

                // Update flags
                update_flags(r0);

                // Break
                break;
            }
            // Case of NOT operand
            case OP_NOT:
            {
                // Get the the first register
                uint16_t r0 = (instruction >> 9) & 0x7;
                // Get the second register
                uint16_t r1 = (instruction >> 6) & 0x7;

                // Take the reciprocal
                registers[r0] = ~registers[r1];

                // Update flags
                update_flags(r0);

                // Break
                break;
            }
            // Case of BR operand
            case OP_BR:
            {
                // Get program counter offset
                uint16_t offset = sign_extend(instruction & 0x1FF, 9);
                // Get conditional flag
                uint16_t conditional_flag = (instruction >> 9) & 0x7;

                // If conditional flag and R_COND are value one
                if(conditional_flag & registers[R_COND])
                {
                    // Set the program counter as the offset
                    registers[R_PC] += offset;
                }

                // Break
                break;
            }
            // Case of JMP operand
            case OP_JMP:
            {
                // Get the register
                uint16_t r1 = (instruction >> 6) & 0x7;
                 // Set the program counter as the address
                registers[R_PC] = registers[r1];

                // Break
                break;
            }
            // Case of JSR operand
            case OP_JSR:
            {
                // Get long flag
                uint16_t long_flag = (instruction >> 11) & 1;
                // Store the address of the program counter
                registers[R_R7] = registers[R_PC];

                // If long flag is set
                if(long_flag)
                {
                    // Get long offset
                    uint16_t long_offset = sign_extend(instruction & 0x7FF, 11);
                    // Set the program counter as the long offset
                    registers[R_PC] += long_offset;
                }
                // If long flag is not set
                else
                {
                    // Get the register
                    uint16_t r1 = (instruction >> 6) & 0x7;
                    // Set the program counter as the address
                    registers[R_PC] = registers[r1];
                }

                // Break
                break;
            }
            // Case of LD operand
            case OP_LD:
            {
                // Get the register
                uint16_t r0 = (instruction >> 9) & 0x7;
                // Get offset
                uint16_t offset = sign_extend(instruction & 0x1FF, 9);

                // Set the register as the offset
                registers[r0] = memory_read(registers[R_PC] + offset);

                // Update flags
                update_flags(r0);

                // Break
                break;
            }
            // Case of LDI operand
            case OP_LDI:
            {
                // Get the register
                uint16_t r0 = (instruction >> 9) & 0x7;

                // Get offset
                uint16_t offset = sign_extend(instruction & 0x1FF, 9);

                // Set the register as the offset
                registers[r0] = memory_read(memory_read(registers[R_PC] + offset));

                // Update flags
                update_flags(r0);

                // Break
                break;
            }
            // Case of LDR operand
            case OP_LDR:
            {
                // Get the first register
                uint16_t r0 = (instruction >> 9) & 0x7;
                // Get the second register
                uint16_t r1 = (instruction >> 6) & 0x7;
                // Calculate the offset
                uint16_t offset = sign_extend(instruction & 0x3F, 6);

                // Set the register as the offset
                registers[r0] = memory_read(r1) + offset;

                // Update flags
                update_flags(r0);

                // Break
                break;
            }
            // Case of LEA operand
            case OP_LEA:
            {
                // Get the first register
                uint16_t r0 = (instruction >> 9) & 0x7;
                // Calculate the offset
                uint16_t offset = sign_extend(instruction & 0x1FF, 9);

                // Set the register as the offset
                registers[r0] = registers[R_PC] + offset;

                // Update flags
                update_flags(r0);

                // Break
                break;
            }
            // Case of ST operand
            case OP_ST:
            {
                // Get the register
                uint16_t r0 = (instruction >> 9) & 0x7;
                // Calculate the offset
                uint16_t offset = sign_extend(instruction & 0x1FF, 9);

                // Set the memory as the value
                memory_write(registers[R_PC] + offset, registers[r0]);

                // Break
                break;
            }
            // Case of STI operand
            case OP_STI:
            {
                // Get the register
                uint16_t r0 = (instruction >> 9) & 0x7;
                // Calculate the offset
                uint16_t offset = sign_extend(instruction & 0x1FF, 9);

                // Set the memory as the value
                memory_write(memory_read(registers[R_PC] + offset), registers[r0]);

                // Break
                break;
            }
            // Case of STR operand
            case OP_STR:
            {
                // Get the first register
                uint16_t r0 = (instruction >> 9) & 0x7;
                // Get the second register
                uint16_t r1 = (instruction >> 6) & 0x7;
                // Calculate the offset
                uint16_t offset = sign_extend(instruction & 0x3F, 6);

                // Set the memory as the value
                memory_write(registers[r1] + offset, registers[r0]);

                // Break
                break;
            }
            // Case of RES operand
            case OP_RES:
            // Case of RTI operand
            case OP_RTI:
            {
                // Print error message
                fprintf(stderr, "Unused operand code! Exiting!");

                // Exit with "unused operand code" error
                return 4;
            }
            // Case of TRAP operand
            case OP_TRAP:
            {
                // Switch of trap
                switch(instruction & 0xFF)
                {
                    // Case of TRAP_GETC
                    case TRAP_GETC:
                    {
                        // Get the character
                        registers[R_R0] = (uint16_t)getchar();
                        // Flush standard output
                        fflush(stdout);

                        // Break
                        break;
                    }
                    // Case of TRAP_OUT
                    case TRAP_OUT:
                    {
                        // Print the character
                        putc((char)registers[R_R0], stdout);

                        // Break
                        break;
                    }
                    // Case of TRAP_PUTS
                    case TRAP_PUTS:
                    {
                        // Get the string
                        uint16_t* character = memory + registers[R_R0];

                        // Print loop
                        while(*character)
                        {
                            // Print the character
                            putc((char)*character, stdout);
                            // Increment character offset
                            character++;
                        }

                        // Flush standard output
                        fflush(stdout);

                        // Break
                        break;
                    }
                    // Case of TRAP_IN
                    case TRAP_IN:
                    {
                        // Get the character
                        char character = getchar();

                        // Print the character
                        putc(character, stdout);

                        // Set the register as the character
                        registers[R_R0] = (uint16_t)character;

                        // Break
                        break;
                    }
                    // Case of TRAP_PUTSP
                    case TRAP_PUTSP:
                    {
                        // Get the character
                        uint16_t* character = memory + registers[R_R0];

                        // Print loop
                        while(*character)
                        {
                            // Get first character
                            char char_1 = (*character) & 0xFF;
                            // Print first character
                            putc(char_1, stdout);

                            // Get second character
                            char char_2 = (*character) >> 8;

                            // If second character is not null
                            if(char_2)
                            {
                                // Print second character
                                putc(char_2, stdout);
                            }

                            // Increment character offset
                            character++;
                        }

                        // Flush standard output
                        fflush(stdout);

                        // Break
                        break;
                    }
                    // Case of TRAP_HALT
                    case TRAP_HALT:
                    {
                        // Flush standard output
                        fflush(stdout);

                        // Stop program
                        running = false;

                        // Print error message
                        fprintf(stderr, "The program has been ended! Exiting!");

                        // Break
                        break;
                    }
                }

                // Break
                break;
            }
            default:
            {
                // Print error message
                fprintf(stderr, "Bad opcode error! Exiting!");

                // Exit with "bad opcode" error
                return 3;
            }
        }
    }

    // Exit with normal status
    return 0;
}
