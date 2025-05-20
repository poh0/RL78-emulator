#pragma once

void mov_r_imm8(RL78_CPU* cpu);
void mov_a_r(RL78_CPU* cpu);
void mov_r_a(RL78_CPU* cpu);
void mov_addr16_imm8(RL78_CPU* cpu);
void mov_r_addr16(RL78_CPU* cpu);
void mov_addr16_a(RL78_CPU* cpu);
void mov_a_indir_rp(RL78_CPU* cpu);
void mov_a_indir_rp_offset(RL78_CPU* cpu);
void mov_indir_rp_a(RL78_CPU* cpu);
void mov_indir_rp_offset_a(RL78_CPU* cpu);
void mov_indir_rp_offset_imm8(RL78_CPU* cpu);
void mov_a_indir_hl_plus_r(RL78_CPU* cpu);
void mov_indir_hl_plus_r_a(RL78_CPU* cpu);
void mov_saddr_imm8(RL78_CPU* cpu);
void mov_r_saddr(RL78_CPU* cpu);
void mov_saddr_a(RL78_CPU* cpu);
void mov_based_r_imm8(RL78_CPU* cpu);
void mov_based_bc_imm8(RL78_CPU* cpu);
void mov_sfr_imm8(RL78_CPU* cpu);
void mov_es_imm8(RL78_CPU* cpu);
void mov_a_sfr(RL78_CPU* cpu);
void mov_sfr_a(RL78_CPU* cpu);
void mov_es_saddr(RL78_CPU* cpu);

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