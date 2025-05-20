#include "cpu.h"
#include "instructions.h"
#include <stdio.h>
#include <string.h>

#define GET_LREG(cpu, idx) (cpu->GPR)

// Convert short addresses to absolute
static uint32_t saddr_to_absolute(uint8_t saddr)
{
    return 0xFE20 + saddr; // Map 0x00–0xDF to 0xFE20–0xFEFF
}

uint8_t read8(RL78_CPU* cpu, uint16_t addr16)
{
    // Resolve full 1 MB address if we want ES-prefixed address
    uint32_t full_addr = cpu->ext_addressing ? ((uint32_t)(cpu->ES) << 16) | addr16 : addr16;
    full_addr &= 0xFFFFF;  // Mask to 20-bit address
    return cpu->memory[full_addr];
}

uint8_t read8_indir(RL78_CPU* cpu, uint16_t addr16)
{
    // Resolve full 1 MB address if we want ES-prefixed address
    uint32_t full_addr = cpu->ext_addressing ? ((uint32_t)(cpu->ES) << 16) | addr16 : addr16 | 0xF0000; 
    full_addr &= 0xFFFFF;  // Mask to 20-bit address
    return cpu->memory[full_addr];
}

uint8_t read8_saddr(RL78_CPU* cpu, uint8_t saddr)
{
    return cpu->memory[saddr_to_absolute(saddr)];
}

void write8(RL78_CPU* cpu, uint16_t addr16, uint8_t data)
{
    // Resolve full 1 MB address if we want ES-prefixed address
    uint32_t full_addr = cpu->ext_addressing ? ((uint32_t)(cpu->ES) << 16) | addr16 : addr16;
    full_addr &= 0xFFFFF;  // Mask to 20-bit address
    cpu->memory[full_addr] = data;
}

void write8_indir(RL78_CPU* cpu, uint16_t addr16, uint8_t data)
{
    // Resolve full 1 MB address if we want ES-prefixed address
    uint32_t full_addr = cpu->ext_addressing ? ((uint32_t)(cpu->ES) << 16) | addr16 : addr16 | 0xF0000;
    full_addr &= 0xFFFFF;  // Mask to 20-bit address
    cpu->memory[full_addr] = data;
}

void write8_saddr(RL78_CPU* cpu, uint8_t saddr, uint8_t data)
{
    cpu->memory[saddr_to_absolute(saddr)] = data;
}

// Fetch instruction opcode/operands, increments PC
uint8_t fetch8(RL78_CPU* cpu)
{
    uint8_t byte = cpu->memory[GET_PC(cpu)];
    INC_PC(cpu, 1);
    return byte;
}

uint16_t fetch16(RL78_CPU* cpu)
{
    uint8_t low = fetch8(cpu);
    uint8_t high = fetch8(cpu);
    return (high << 8) | low;
}

uint8_t* get_sfr(RL78_CPU* cpu,  uint8_t code)
{
    switch (code)
    {
    case 0xFA:
        return &cpu->PSW.asByte;
    case 0xFC:
        return &cpu->CS;
    case 0xFD:
        return &cpu->ES;
    default:
        break;
    }
}

void cpu_init(RL78_CPU* cpu)
{
    cpu->PC = 0x0000;
    cpu->SP = 0xf0000;  // "reset signal generation makes the SP contents undefined" manual pg. 11
    cpu->PSW.asByte = 0x06;
    cpu->ES = 0x0F;
    cpu->CS = 0x00;
    cpu->ext_addressing = false;

    memset(cpu->regs.R, 0, sizeof(cpu->regs.R)); // set general purpose registers to 0
    memset(cpu->memory, 0, MEM_SIZE);
}

void cpu_cycle(RL78_CPU* cpu)
{
    uint8_t opcode_1st = read8(cpu, GET_PC(cpu));
    uint8_t opcode_2nd = read8(cpu, GET_PC(cpu) + 1);

    // Handle instructions with ES:
    // Note: 
    // - using the ES: prefix adds EXACTLY ONE additional cycle to the base instruction's execution time
    if (opcode_1st == 0x11) {
        INC_PC(cpu, 1);
        opcode_1st = read8(cpu, GET_PC(cpu));
        opcode_2nd = read8(cpu, GET_PC(cpu) + 1);
        cpu->ext_addressing = true;
    }

    switch (opcode_1st) {
        case 0x00: nop_inst(cpu); break;
        case 0x0C: add_a_imm8(cpu); break;
        // 0x11: Extended addressing
        case 0x12: movw_rp_ax(cpu); break;
        case 0x13: movw_ax_rp(cpu); break;
        case 0x14: movw_rp_ax(cpu); break;
        case 0x15: movw_ax_rp(cpu); break;
        case 0x16: movw_rp_ax(cpu); break;
        case 0x17: movw_ax_rp(cpu); break;
        case 0x19: mov_based_r_imm8(cpu); break;
        case 0x30: movw_rp_imm16(cpu); break;
        case 0x32: movw_rp_imm16(cpu); break;
        case 0x33: xchw_ax_rp(cpu); break;
        case 0x34: movw_rp_imm16(cpu); break;
        case 0x35: xchw_ax_rp(cpu); break;
        case 0x36: movw_rp_imm16(cpu); break;
        case 0x37: xchw_ax_rp(cpu); break;
        case 0x38: mov_based_r_imm8(cpu); break;
        case 0x39: mov_based_bc_imm8(cpu); break;

        case 0x41: mov_es_imm8(cpu);

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

                case 0xCB: br_ax(cpu); break;
                case 0xC9: mov_a_indir_hl_plus_r(cpu); break;
                case 0xD9: mov_indir_hl_plus_r_a(cpu); break;
                case 0xE9: mov_a_indir_hl_plus_r(cpu); break;
                case 0xF9: mov_indir_hl_plus_r_a(cpu); break;
                case 0x8A:
                case 0x8B:
                case 0x8C:
                case 0x8D:
                case 0x8E:
                case 0x8F: xch_a_r(cpu); break;

                case 0xB8: mov_es_saddr(cpu); break;
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
        
        case 0x89: mov_a_indir_rp(cpu); break;
        case 0x8A: mov_a_indir_rp_offset(cpu); break;
        case 0x8B: mov_a_indir_rp(cpu); break;
        case 0x8C: mov_a_indir_rp_offset(cpu); break;

        case 0x8D: mov_r_saddr(cpu); break;


        // MOV A, sfr
        case 0x8E: mov_a_sfr(cpu); break;
        
        case 0x8F: mov_r_addr16(cpu); break;
        
        case 0x99: mov_indir_rp_a(cpu); break;
        case 0x9A: mov_indir_rp_offset_a(cpu); break;
        case 0x9B: mov_indir_rp_a(cpu); break;
        case 0x9C: mov_indir_rp_offset_a(cpu); break;
        case 0x9D: mov_saddr_a(cpu); break;
        case 0x9E: mov_sfr_a(cpu);

        case 0xCA: mov_indir_rp_offset_imm8(cpu); break;
        case 0xCC: mov_indir_rp_offset_imm8(cpu); break;
        case 0xCD: mov_saddr_imm8(cpu); break;
        case 0xCE: mov_sfr_imm8(cpu); break;
        case 0xCF: mov_addr16_imm8(cpu); break;
        
        case 0xD8: mov_r_saddr(cpu); break;
        case 0xD9: mov_r_addr16(cpu); break;

        case 0xE0:
        case 0xE1:
        case 0xE2:
        case 0xE3: oneb_r(cpu); break;

        case 0xE6:
        case 0xE7: onew_rp(cpu); break;
        case 0xE8: mov_r_saddr(cpu); break;
        case 0xE9: mov_r_addr16(cpu); break;

        case 0xF0:
        case 0xF1:
        case 0xF2:
        case 0xF3: clrb_r(cpu); break;

        case 0xF6:
        case 0xF7: clrw_rp(cpu); break;
        case 0xF8: mov_r_saddr(cpu); break;
        case 0xF9: mov_r_addr16(cpu); break;

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
    printf("PSW:    0x%02X\n", cpu->PSW.asByte);
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
