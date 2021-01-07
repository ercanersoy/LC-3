/*
 * main.c - Main source file of LC-3
 * Written by Ercan Ersoy.
 */


// Define memory size
#define MEMORY 65536

// Includes
#include <stdint.h>

// Memory declaration
uint16_t memory[MEMORY];

// Enumerator type for registers and register count
enum
{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,
    R_COND,
    REGISTER_COUNT
};

// Registers declaration
uint16_t reg[REGISTER_COUNT];

// Enumerator type for opcodes
enum
{
    OP_BR = 0,
    OP_ADD,
    OP_LD,
    OP_ST,
    OP_JSR,
    OP_AND,
    OP_LDR,
    OP_STR,
    OP_RTI,
    OP_STI,
    OP_JMP,
    OP_RES,
    OP_LEA,
    OP_TRAP
};

// Enumerator type for flags
enum
{
    FL_POS = 1 << 0,
    FL_ZERO = 1 << 1,
    FL_NEG = 1 << 2
};

// Main function of the program
int main(int argc, char* argv[])
{
    return 0;
}