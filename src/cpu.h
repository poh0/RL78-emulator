#pragma once

#include <stdint.h>

#include "regs.h"

#define MEM_SIZE 0x100000 //  1MB address space 
#define RAM_SIZE 512

// Macros to mask program counter to 20 bits
#define PC_MASK        0xFFFFF
#define GET_PC(cpu)    ((cpu)->PC & PC_MASK)
#define SET_PC(cpu,x)  ((cpu)->PC = ((x) & PC_MASK))
#define INC_PC(cpu,n)  ((cpu)->PC = ((cpu)->PC + (n)) & PC_MASK)

typedef union {
    struct {
        uint8_t CY : 1;
        uint8_t PAD1 : 3;
        uint8_t AC : 1;
        uint8_t PAD2 : 1;
        uint8_t Z : 1;
        uint8_t PAD3 : 1;
    };
    uint8_t asWord;
} PSW_u;

typedef struct {
    uint32_t PC; // Program counter (masked to 20 bits with macros)
    uint16_t SP; // Stack pointer
    PSW_u PSW; // Program status word
    GPR_u regs;  // 4 x 16-bit general pupose register pairs (8 x 8 bit GPRs)
    uint8_t memory[MEM_SIZE]; // 1 MB address space
} RL78_CPU;

uint8_t read8(RL78_CPU* cpu, uint32_t addr);
uint8_t fetch8(RL78_CPU* cpu);
uint16_t fetch16(RL78_CPU* cpu);
void cpu_init(RL78_CPU* cpu);
void cpu_cycle(RL78_CPU* cpu);
void dump_cpu_state(const RL78_CPU* cpu);