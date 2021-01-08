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
    R_R0 = 0,  // Register R0
    R_R1,  // Register R1
    R_R2,  // Register R2
    R_R3,  // Register R3
    R_R4,  // Register R4
    R_R5,  // Register R5
    R_R6,  // Register R6
    R_R7,  // Register R7
    R_PC,  // Register PC
    R_COND,  // Register COND
    REGISTER_COUNT  // Count of registers
};

// Registers declaration
uint16_t registers[REGISTER_COUNT];

// Enumerator type for opcodes
enum
{
    OP_BR = 0,  // Branch
    OP_ADD,  // Addition
    OP_LD,  // Load
    OP_ST,  // Store
    OP_JSR,  // Jump register
    OP_AND,  // And operator
    OP_LDR,  // Load register
    OP_STR,  // Load effective address
    OP_RTI,
    OP_NOT,  // Not operator
    OP_LDI,  // Load indirect
    OP_STI,  // Store indirect
    OP_JMP,  // Jump
    OP_RES,
    OP_LEA,  // Load Effective address
    OP_TRAP  // Trap
};

// Enumerator type for flags
enum
{
    FL_POS = 1 << 0,  // Positive flag
    FL_ZRO = 1 << 1,  // Zero flag
    FL_NEG = 1 << 2  // Negative flag
};

// Trap codes
enum
{
    TRAP_GETC = 0x20,  // Input character
    TRAP_OUT = 0x21,  // Output character
    TRAP_PUTS = 0x22,  // Output word string
    TRAP_IN = 0x23,  // Input character
    TRAP_PUTSP = 0x24,  // Output byte string
    TRAP_HALT = 0x25  // Halt the program
}

// Memory mapped registers
enum
{
    MR_KBSR = 0xFE00,  // Keyboard status register
    MR_KBDR = 0xFE02  // Keyboard data register
}

// Running status
bool running = false;

// Swap function
uint16_t swap16(uint16_t x)
{
    return (x << 8) | (x >> 8);  // Swap byte
}

//Read image file function
uint16_t read_image_file(FILE* file)
{

    uint16_t origin;  // Origin

    fread(&origin, sizeof(origin), 1, file);  // Read origin address from the program file
    origin = swap16(origin);  // Swap bytes

    uint16_t maximum_read = MEMORY - origin;  // Calculate maximum reading size
    uint16_t* pointer = memory[origin];  // Program start pointer
    size_t read = fread(pointer, size_of(uint16_t), maximum_read, file);  // Reading instructions and data

    // Swapping instructions and data
    while(read-- > 0)
    {
        *p = swap16(*p);  // Word swap
        ++p;  // Increment the pointer
    }
}

//Read image function
uint16_t read_image(const char* image_path)
{
    FILE* file = fopen(image_path, "rb");  // Open the program file

    // If the file is not open
    if(!file)
    {
        return false;  // Returning false status
    }

    read_image_file(file);  // Reading file

    fclose(file);  // Close file

    return true;  // Returning true status
}

// Memory write function
uint16_t memory_write(uint16_t address, uint16_t value)
{
    return memory[address] = value;  // Write value to the address of the memory
}

// Memory read function
uint16_t memory_read(uint16_t address)
{
    // Switch of memory mapped registers
    switch(address)
    {
        // Case of keyboard status register
        case MR_KBSR:
            memory[MR_KBSR] = (1 << 15);  // Write keynoard status to memory
            memory[MR_KBDR] = getchar();  // Write character from keyboard to memory

            break;  // Break
    }

    return memory[address];  // Return character
}

// Update flags function
void update_flags(uint16_t register_value)
{
    // If the register value equals to zero
    if (registers[register_value] == 0)
    {
        registers[R_COND] = FL_ZRO;  // Set flag zero
    }
    // If the register has negative value
    else if(registers[register_value] << 15)
    {
        registers[R_COND] = FL_NEG;    // Set flag negative
    }
    // If the register has positive value
    else
    {
        registers[R_COND] = FL_POS;    // Set flag positive
    }
}

// Sign extend function
uint16_t sign_extend(uint16_t value, int bit_count)
{
    // If the value sign is set
    if((value >> (bit_count - 1) & 1))
    {
        value |= (0xFFFF << bit_count);  // Shift sign bit
    }

    return value;  // Return new value
}

// Main function of the program
int main(int argc, char* argv[])
{
    // If argument count is two
    if(argc == 2)
    {
        // If not read image file
        if(!read_image(argv[j]))
        {
            perror("Failed to load this image! Exiting!");  // Print the error message

            exit(1);  // Exit
        }
    }
    else
    {
        perror("Wrong argument count! Exiting!");  // Print the error message

        exit(1); // Exit
    }

    registers[R_PC] = 0X3000;  // Start program

    running = true;  // Set running status

    // Running loop
    while(running)
    {
        uint16_t instruction = memory_read(registers[R_PC]++);  // Get instruction
        uint16_t operand = instruction >> 12;  // Get operand

        // Operand switch
        switch(operand)
        {
            // Case of ADD operand
            case OP_ADD:
                uint16_t r0 = (instruction >> 9) & 0x7;  // Get the first register
                uint16_t r1 = (instruction >> 6) & 0x7;  // Get the second register
                uint16_t immediate_mode_flag = (instruction >> 5) & 0x1;  // Get the imm value

                // If imm is set
                if(immediate_mode_flag)
                {
                    uint16_t imm5 = sign_extend(instruction & 0x1F, 5);  // Get the imm5 value

                    registers[r0] = registers[r1] + imm5;  // Addition
                }
                // If imm is not set
                else
                {
                    uint16_t r2 = instruction & 0x7;  // Get the third register
                    register[r0] = register[r1] + register[r2];  // Addition
                }

                update_flags(r0);  // Update flags

                break;
            // Case of AND operand
            case OP_AND:
                uint16_t r0 = (instruction >> 9) & 0x7;  // Get the first register
                uint16_t r1 = (instruction >> 6) & 0x7;  // Get the second register
                uint16_t immediate_mode_flag = (instruction >> 5) & 0x1;  // Get the imm value

                // If imm is set
                if(immediate_mode_flag)
                {
                    uint16_t imm5 = sign_extend(instruction & 0x1F, 5);  // Get the imm5 value

                    registers[r0] = registers[r1] & imm5;  // Addition
                }
                else
                {
                    uint16_t r2 = instruction & 0x7;  // Get the third register
                    register[r0] = register[r1] & register[r2];  // Addition
                }

                update_flags(r0);  // Update flags

                break;
            // Case of NOT operand
            case OP_NOT:
                uint16_t r0 = (instruction >> 9) & 0x7;  // Get the the first register
                uint16_t r1 = (instruction >> 6) & 0x7;  // Get the second register

                register[r0] = ~register[r1];  // Take the reciprocal

                update_flags(r0);  // Update flags

                break;  // Break
            // Case of BR operand
            case OP_BR:
                uint16_t offset = sign_extend(instruction & 0X1FF, 9);  // Get program counter offset
                uint16_t conditional_flag = (instruction >> 9) & 0x7;  // Get conditional flag

                // If conditional flag and R_COND are value one
                if(conditional_flag & registers[R_COND])
                {
                    registers[R_PC] += offset;  // Set the program counter as the offset
                }

            break;  // Break
            // Case of JMP operand
            case OP_JMP:
                uint16_t r1 = (instruction >> 6) & 0x7;  // Get the register
                registers[R_PC] = registers[r1];  // Set the program counter as the address

                break;  // Break
            // Case of JSP operand
            case OP_JSP:
                uint16_t long_flag = (instruction >> 11) & 1;  // Get long flag
                reigsters[R_R7] = registers[R_PC];  // Store the address of the program counter

                // If long flag is set
                if(long_flag)
                {
                    uint16_t long_offset = sign_extend(instruction & 0x7FF, 11);  // Get long offset
                    registers[R_PC] += long_offset;  // Set the program counter as the long offset
                }
                // If long flag is not set
                else
                {
                    uint16_t r1 = (instruction >> 6) & 0x7;  // Get the register
                    registers[R_PC] = registers[r1];  // Set the program counter as the address
                }

                break;  // Break
            // Case of LD operand
            case OP_LD:
                uint16_t r0 = (instruction >> 9) & 0x7;  // Get the register
                uint16_t offset = sign_extend(instruction & 0x1FF, 9);  // Get offset

                registers[r0] = memory_read(registers[R_PC] + offset);  // Set the register as the offset

                update_flags(r0);  // Update flags

                break;  // Break
            // Case of LDI operand
            case OP_LDI:
                uint16_t r0 = (instr >> 9) & 0x7;  // Get the register

                uint16_t offset = sign_extend(instruction & 0x1FF, 9);  // Get offset

                registers[r0] = mem_read(memread(reg[R_PC] + offset), registers[r0]);  // Set the register as the offset

                update_flags(r0);  // Update flags

                break;  // Break
            // Case of LDR operand
            case OP_LDR:
                uint16_t r0 = (instruction >> 9) & 0x7;  // Get the first register
                uint16_t r1 = (instruction >> 6) & 0x7;  // Get the second register
                uint16_t offset = sign_extend(instruction & 0x3F, 6);  // Calculate the offset

                registers[r0] = memory_read[r1] + offset;  // Set the register as the offset

                update_flags(r0);  // Update flags

                break;  // Break
            // Case of LEA operand
            case OP_LEA:
                uint16_t r0 = (instruction >> 9) & 0x7;  // Get the first register
                uint16_t offset = sign_extend(instruction & 0x1FF, 9);  // Calculate the offset

                registers[r0] = registers[R_PC] + offset;  // Set the register as the offset

                update_flags(r0);  // Update flags

                break;  // Break
            // Case of ST operand
            case OP_ST:
                uint16_t r0 = (instruction >> 9) & 0x7;  // Get the register
                uint16_t offset = sign_extend(instruction & 0x1FF, 9);  // Calculate the offset

                memory_write(register[R_PC] + offset, reigsters[r0]);  // Set the memory as the value

                break;  // Break
            // Case of STI operand
            case OP_STI:
                uint16_t r0 = (instruction >> 9) & 0x7;  // Get the register
                uint16_t offset = sign_extend(instruction & 0x1FF, 9);  // Calculate the offset

                memory_write(memory_read(registers[R_PC]) + offset);  // Set the memory as the value

                break;  // Break
            // Case of STR operand
            case OP_STR:
                uint16_t r0 = (instruction >> 9) & 0x7;  // Get the first register
                uint16_t r1 = (instruction >> 6) & 0x7;  // Get the second register
                uint16_t offset = sign_extend(instruction & 0x3F, 6);  // Calculate the offset

                memory_write(registers[r1] + offset, registers[r0]);  // Set the memory as the value

                break;  // Break
            // Case of RES operand
            case OP_RES:
            // Case of RTI operand
            case OP_RTI:
                perror("Unused operand code! Exiting!");  // Print error message

                retrun 3;  // Exit
            // Case of TRAP operand
            case OP_TRAP:
                // Switch of trap
                switch(instruction & 0xFF)
                {
                    // Case of TRAP_GETC
                    case TRAP_GETC:
                        registers[R_R0] = (uint16_t)getchar();  // Get the character
                        fflush(stdout);  // Flush standard output

                        break;  // Break
                    // Case of TRAP_OUT
                    case TRAP_OUT:
                        putc((char)registers[R_R0], stdout);  // Print the character

                        break;  // Break
                    // Case of TRAP_PUTS
                    case TRAP_PUTS:
                        uint16_t* character = memory[register[R_R0]];  // Get the string

                        // Print loop
                        while(*character)
                        {
                            putc((char)*character, stdout);  // Print the character
                            ++character;  // Increment character offset
                        }

                        fflush(stdout);  // Flush standard output

                        break;  // Break
                    // Case of TRAP_IN
                    case TRAP_IN:
                        char character = getchar();  // Get the character

                        putc(c, stdout);  // Print the character

                        registers[R_R0] = (uint16_t)c;  // Set the register as the character

                        break;  // Break
                    // Case of TRAP_PUTSP
                    case TRAP_PUTSP:
                        uint16_t* character = memory[register[R_R0]];  // Get the character

                        // Print loop
                        while(*character)
                        {
                            char char_1 = (*character) & 0xFF;  // Get first character
                            putc(char_1, stdout);  // Print first character

                            char char_2 = (*character) >> 8;  // Get second character
                            // If second character is not null
                            if(char2)
                            {
                                putc(char_2, stdout);  // Print second character
                            }

                            ++character;  // Increment character offset
                        }

                        fflush(stdout);  // Flush standard output

                        break;  // Break
                    // Case of TRAP_HALT
                    case TRAP_HALT:
                        fflush(stdout);  // Flush standard output

                        running = false;  // Stop program

                        break;  // Break
                }

                break;  // Break
            default:
                perror("Bad opcode error! Exiting!");  // Print error message

                return 2;  // Exit
        }
    }

        return 0;  // Exit
}