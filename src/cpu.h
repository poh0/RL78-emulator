#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MEM_SIZE 0x100000 //  1MB address space 
#define RAM_SIZE 512

// Macros to mask program counter to 20 bits
#define PC_MASK        0xFFFFF
#define GET_PC(cpu)    ((cpu)->PC & PC_MASK)
#define SET_PC(cpu,x)  ((cpu)->PC = ((x) & PC_MASK))
#define INC_PC(cpu,n)  ((cpu)->PC = ((cpu)->PC + (n)) & PC_MASK)

typedef union {
    struct {
        uint8_t CY : 1; // Carry flag
        uint8_t ISP0 : 1; // In-service priority flag
        uint8_t ISP1 : 1; // In-service priority flag
        uint8_t unused1 : 1;
        uint8_t AC : 1; // Auxiliary carry flag
        uint8_t unused2 : 1;
        uint8_t Z : 1; // Zero flag
        uint8_t IE : 1; //interrupt enable flag
    };
    uint8_t asByte;
} PSW_u;

typedef union {
    uint8_t R[8];
    uint16_t RP[4];
} GPR_u;

typedef struct {
    uint32_t PC; // Program counter (masked to 20 bits with macros)
    uint16_t SP; // Stack pointer
    uint8_t ES; // Extra segment register
    uint8_t CS;  // Code segment register
    PSW_u PSW; // Program status word
    GPR_u regs;  // 4 x 16-bit general pupose register pairs (8 x 8 bit GPRs)
    uint8_t memory[MEM_SIZE]; // 1 MB address space
    bool ext_addressing; // When opcode 0x11 is encountered, this is set to true. 
} RL78_CPU;

uint8_t read8(RL78_CPU* cpu, uint16_t addr16);
uint8_t read8_indir(RL78_CPU* cpu, uint16_t addr16);
uint8_t read8_saddr(RL78_CPU* cpu, uint8_t saddr);
void write8(RL78_CPU* cpu, uint16_t addr16, uint8_t data);
void write8_indir(RL78_CPU* cpu, uint16_t addr16, uint8_t data);
void write8_saddr(RL78_CPU* cpu, uint8_t saddr, uint8_t data);


uint8_t fetch8(RL78_CPU* cpu);
uint16_t fetch16(RL78_CPU* cpu);
void cpu_init(RL78_CPU* cpu);
void cpu_cycle(RL78_CPU* cpu);
void dump_cpu_state(const RL78_CPU* cpu);