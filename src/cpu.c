#include "cpu.h"
#include "instructions.h"
#include <stdio.h>

#define GET_LREG(cpu, idx) (cpu->GPR)


uint8_t read8(RL78_CPU* cpu, uint32_t addr)
{
    if (addr > 0xFFFF) {
        printf("Out of bounds read\n");
        return 0;
    }
    return cpu->memory[addr];
}

uint8_t fetch8(RL78_CPU* cpu)
{
    uint8_t byte = read8(cpu, GET_PC(cpu));
    INC_PC(cpu, 1);
    return byte;
}

uint16_t fetch16(RL78_CPU* cpu)
{
    uint8_t low = fetch8(cpu);
    uint8_t high = fetch8(cpu);
    return (high << 8) | low;
}

void cpu_init(RL78_CPU* cpu)
{
    cpu->PC = 0x0000;
    cpu->SP = 0xf0000;  // "reset signal generation makes the SP contents undefined" manual pg. 11
    // cpu->PSW.AC = 0,
    cpu->PSW.asWord = 0;
    memset(cpu->regs.R, 0, sizeof(cpu->regs.R)); // set general purpose registers to 0
    memset(cpu->memory, 0, MEM_SIZE);
}

void cpu_cycle(RL78_CPU* cpu)
{
    uint8_t opcode_1st = read8(cpu, GET_PC(cpu));
    uint8_t opcode_2nd = read8(cpu, GET_PC(cpu) + 1);
    switch (opcode_1st) {
        case 0x00: nop_inst(cpu); break;
        case 0x0C: add_a_imm8(cpu); break;
        case 0x12: movw_rp_ax(cpu); break;
        case 0x13: movw_ax_rp(cpu); break;
        case 0x14: movw_rp_ax(cpu); break;
        case 0x15: movw_ax_rp(cpu); break;
        case 0x16: movw_rp_ax(cpu); break;
        case 0x17: movw_ax_rp(cpu); break;

        case 0x30: movw_rp_imm16(cpu); break;
        case 0x32: movw_rp_imm16(cpu); break;
        case 0x33: xchw_ax_rp(cpu); break;
        case 0x34: movw_rp_imm16(cpu); break;
        case 0x35: xchw_ax_rp(cpu); break;
        case 0x36: movw_rp_imm16(cpu); break;
        case 0x37: xchw_ax_rp(cpu); break;

        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57: mov_r_imm8(cpu); break;
        
        case 0x61:
            switch (opcode_2nd) {

                case 0x08: add_a_r(cpu); break;
                case 0x0A:
                case 0x0B:
                case 0x0C:
                case 0x0D:
                case 0x0E:
                case 0x0F: add_a_r(cpu); break;

                case 0xCD: br_ax(cpu); break;
                case 0x8A:
                case 0x8B:
                case 0x8C:
                case 0x8D:
                case 0x8E:
                case 0x8F: xch_a_r(cpu); break;
            }
            break;

        case 0x60:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67: mov_a_r(cpu); break;


        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87: inc_r(cpu); break;

        case 0xE0:
        case 0xE1:
        case 0xE2:
        case 0xE3: oneb_r(cpu); break;

        case 0xE6:
        case 0xE7: onew_rp(cpu); break;

        case 0xF0:
        case 0xF1:
        case 0xF2:
        case 0xF3: clrb_r(cpu); break;

        case 0xF6:
        case 0xF7: clrw_rp(cpu); break;

        default:
            printf("Unknown opcode: 0x%02X at PC=0x%04X\n", opcode_1st, GET_PC(cpu));
            break;
    }
    dump_cpu_state(cpu);
}

void dump_cpu_state(const RL78_CPU* cpu)
{
    printf("\nCPU State:\n");
    printf("PC:     0x%04X\n", cpu->PC);
    printf("SP:     0x%04X\n", cpu->SP);
    printf("PSW:    0x%02X\n", cpu->PSW.asWord);
    // general purpose regs
    for (int i = 0; i < 8; i++) {
        printf("R%d:    0x%02X\n", i, cpu->regs.R[i]);
    }
    // register pairs
    for (int i = 0; i < 4; i++) {
        printf("RP%d:   0x%04X\n", i, cpu->regs.RP[i]);
    }
    printf("----------------------\n");
}
