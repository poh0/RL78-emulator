#include "cpu.h"
#include "instructions.h"

#define MEM(x) cpu->memory[x]

#define OPERAND(n) cpu->memory[GET_PC(cpu) + n]

#define LOBYTE(w) ((uint8_t)w)
#define HIBYTE(w) ((uint8_t)(((uint16_t)(w) >> 8) & 0xFF))

#ifdef _DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...) ;
#endif

#define AX cpu->regs.RP[1]

static void solve_add_flags(RL78_CPU* cpu, uint8_t dstval, uint8_t srcval, uint16_t result)
{
    // Set CY (carry out of bit 7)
    cpu->PSW.CY = (result > 0xFF);

    // Set AC (carry from bit 3 to bit 4)
    cpu->PSW.AC = ((dstval & 0x0F) + (srcval & 0x0F)) > 0x0F;

    // Set Z (zero result)
    cpu->PSW.Z = ((uint8_t)result == 0);
}

// MOVE 8-bit immediate to a general purpose register.
// size: 2
// 0x50 ... 0x57, data
// MOV r, #imm8
void mov_r_imm8(RL78_CPU* cpu)
{
    uint16_t instr = fetch16(cpu);
    uint8_t opcode = LOBYTE(instr);
    uint8_t operand = HIBYTE(instr);
    uint8_t reg_idx = opcode - 0x50;
    cpu->regs.R[reg_idx] = operand;
    LOG("Executed MOV R%d, 0x%02X\n", reg_idx, operand);
}

// MOVE contents of r (!=A) to A.
// size: 1
// 0x60, 0x62 ... 67
// MOV A, r
void mov_a_r(RL78_CPU* cpu)
{
    uint8_t opcode = fetch8(cpu);
    uint8_t reg_idx = opcode - 0x60;
    cpu->regs.R[1] = cpu->regs.R[reg_idx];
    LOG("Executed MOV R1, R%d\n", reg_idx);
}

// MOVE contents of a (!=r) to r
// size: 1
// 0x70, 0x72 ... 77
// MOV A, r
void mov_r_a(RL78_CPU* cpu)
{
    uint16_t opcode = fetch8(cpu);
    uint8_t reg_idx = opcode - 0x70;
    cpu->regs.R[reg_idx] = cpu->regs.R[1];
    LOG("Executed MOV R%d, R1\n", reg_idx);
}

void mov_addr16_imm8(RL78_CPU* cpu)
{
    uint16_t addr16 = fetch16(cpu);
    uint8_t data = fetch8(cpu);
    write8(cpu, addr16, data);
}

// Increment a value in a general purpose register by 1
// size: 1
// 0x80 ... 0x87
// INC r
void inc_r(RL78_CPU* cpu)
{
    uint8_t opcode = fetch8(cpu);
    cpu->regs.R[opcode - 0x80]++;
    LOG("Executed INC R%d\n", opcode - 0x80);
}

// Unconditional branch to 16-bit address in AX (RP0) register
// size: 2
// 0x61, 0xCD
// BR AX
void br_ax(RL78_CPU* cpu)
{
    SET_PC(cpu, cpu->regs.RP[0]);
    LOG("Executed BR AX\n");
}

// No operation, increment PC by 1.
// size 1
// 0x00
// NOP
void nop_inst(RL78_CPU* cpu)
{
    INC_PC(cpu, 1);
    LOG("Executed NOP\n");
}

// Exchange values between R1 and other R
// size: 1 OR 2
// 0x08 or 0x61, 0x8A...0x8F
void xch_a_r(RL78_CPU* cpu)
{
    uint8_t opcode = fetch8(cpu);
    if (opcode == 0x08)
    {
        uint16_t val = cpu->regs.RP[0];
        // swap bytes in RP
        cpu->regs.RP[0] = (val >> 8) | (val << 8);
        LOG("Executed XCH A, X");
    }
    else
    {
        uint8_t operand = fetch8(cpu);
        uint8_t reg_idx = operand - 0x8A;
        uint8_t temp = cpu->regs.R[1];
        cpu->regs.R[1] = cpu->regs.R[reg_idx];
        cpu->regs.R[reg_idx] = temp;
        LOG("Executed XCH A, R%d\n", reg_idx);
    }
}

void oneb_r(RL78_CPU* cpu)
{
    uint8_t reg_idx = fetch8(cpu) - 0xE0;
    cpu->regs.R[reg_idx] = 0x01;
    LOG("Executed ONEB R%d\n", reg_idx);
}

void clrb_r(RL78_CPU* cpu)
{
    uint8_t reg_idx = fetch8(cpu) - 0xF0;
    cpu->regs.R[reg_idx] = 0x00;
    LOG("Executed CLRB R%d\n", reg_idx);
}

void movw_rp_imm16(RL78_CPU* cpu)
{
    uint8_t rp_idx = (fetch8(cpu) - 0x30)/2;
    uint16_t data = fetch16(cpu);
    cpu->regs.RP[rp_idx] = data;
    LOG("Executed MOVW RP%d, 0x%04X", rp_idx, data);
}

void movw_ax_rp(RL78_CPU* cpu)
{
    uint8_t rp_idx = (fetch8(cpu) - 0x13) / 2;
    cpu->regs.RP[0] = cpu->regs.RP[rp_idx];
    LOG("Executed MOVW AX, RP%d\n", rp_idx);
}

void movw_rp_ax(RL78_CPU* cpu)
{
    uint8_t rp_idx =1+ (fetch8(cpu) - 0x12) / 2;
    cpu->regs.RP[rp_idx] = cpu->regs.RP[0];
    LOG("Executed MOVW RP%d, AX\n", rp_idx);
}

void xchw_ax_rp(RL78_CPU* cpu)
{
    uint8_t rp_idx = (fetch8(cpu) - 0x33) / 2;
    uint16_t temp = cpu->regs.RP[1];
    AX = cpu->regs.RP[rp_idx];
    cpu->regs.RP[rp_idx] = temp;
    LOG("Executed XCHW AX, RP%d, AX\n", rp_idx);
}

void onew_rp(RL78_CPU* cpu)
{
    uint8_t rp_idx = fetch8(cpu) - 0xE6;
    cpu->regs.RP[rp_idx] = 0x0001;
    LOG("Executed ONEW RP%d\n", rp_idx);
}

void clrw_rp(RL78_CPU* cpu)
{
    uint8_t rp_idx = fetch8(cpu) - 0xF6;
    cpu->regs.RP[rp_idx] = 0x0000;
    LOG("Executed CLRW RP%d\n", rp_idx);
}

void add_a_imm8(RL78_CPU* cpu)
{
    INC_PC(cpu, 1);
    uint8_t val = fetch8(cpu);
    uint16_t result = val + cpu->regs.R[1];
    solve_add_flags(cpu, cpu->regs.R[1], val, result);
    cpu->regs.R[1] = (uint8_t)result;
    LOG("Executed ADD A, 0x%02X\n", val);
}

void add_a_r(RL78_CPU* cpu)
{
    INC_PC(cpu, 1);
    uint8_t operand = fetch8(cpu);
    uint8_t val;
    if (operand == 0x08)
    {
        val = cpu->regs.R[0];
    }
    else
    {
        val = cpu->regs.R[operand - 0x0A];
    }

    uint16_t result = val + cpu->regs.R[1];
    solve_add_flags(cpu, cpu->regs.R[1], val, result);
    cpu->regs.R[1] = (uint8_t)result;

    LOG("Executed ADD A, R%d\n", operand - 0x0A);
}

void add_r_a(RL78_CPU* cpu)
{
    INC_PC(cpu, 1);
    uint8_t operand = fetch8(cpu);
    uint8_t val = cpu->regs.R[operand - 0x00];

    uint16_t result = val + cpu->regs.R[1];
    solve_add_flags(cpu, cpu->regs.R[1], val, result);
    cpu->regs.R[1] = (uint8_t)result;

    LOG("Executed ADD R%d, A\n", operand - 0x00);
}


