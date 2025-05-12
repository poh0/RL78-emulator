#pragma once

void mov_r_imm8(RL78_CPU* cpu);
void mov_a_r(RL78_CPU* cpu);
void mov_r_a(RL78_CPU* cpu);

void inc_r(RL78_CPU* cpu);
void br_ax(RL78_CPU* cpu);

void nop_inst(RL78_CPU* cpu);

void xch_a_r(RL78_CPU* cpu);

void oneb_r(RL78_CPU* cpu);

void clrb_r(RL78_CPU* cpu);

void movw_rp_imm16(RL78_CPU* cpu);
void movw_ax_rp(RL78_CPU* cpu);
void movw_rp_ax(RL78_CPU* cpu);

void xchw_ax_rp(RL78_CPU* cpu);
void onew_rp(RL78_CPU* cpu);
void clrw_rp(RL78_CPU* cpu);

void add_a_imm8(RL78_CPU* cpu);
void add_a_r(RL78_CPU* cpu);
void add_r_a(RL78_CPU* cpu);