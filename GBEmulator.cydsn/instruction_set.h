#ifndef INSTRUCTION_SET_H
#define INSTRUCTION_SET_H
#include "cpu.h"
#include "stdint.h"
    
typedef enum CC {
    Z, NZ, C, NC
} CC;

void increment_pc(Cpu* cpu);
uint8_t fetch_and_increment_pc(Cpu* cpu);
uint16_t fetch_and_increment_pc_twice(Cpu* cpu);
// all instructions return the # of machine cycles used
// all instructions assume the program counter is pointing at the next instr
uint8_t adc_a_r8(Cpu* cpu, uint8_t* reg);
uint8_t adc_a_mhl(Cpu* cpu);
uint8_t adc_a_n8(Cpu* cpu);
uint8_t add_a_r8(Cpu* cpu, uint8_t* reg);
uint8_t add_a_mhl(Cpu* cpu);
uint8_t add_a_n8(Cpu* cpu);
uint8_t and_a_r8(Cpu* cpu, uint8_t* reg);
uint8_t and_a_mhl(Cpu* cpu);
uint8_t and_a_n8(Cpu* cpu);
uint8_t cp_a_r8(Cpu* cpu, uint8_t* reg);
uint8_t cp_a_mhl(Cpu* cpu);
uint8_t cp_a_n8(Cpu* cpu);
uint8_t dec_r8(Cpu* cpu, uint8_t* reg);
uint8_t dec_mhl(Cpu* cpu);
uint8_t inc_r8(Cpu* cpu, uint8_t* reg);
uint8_t inc_mhl(Cpu* cpu);
uint8_t or_a_r8(Cpu* cpu, uint8_t* reg);
uint8_t or_a_mhl(Cpu* cpu);
uint8_t or_a_n8(Cpu* cpu);
uint8_t sbc_a_r8(Cpu* cpu, uint8_t* reg);
uint8_t sbc_a_mhl(Cpu* cpu);
uint8_t sbc_a_n8(Cpu* cpu);
uint8_t sub_a_r8(Cpu* cpu, uint8_t* reg);
uint8_t sub_a_mhl(Cpu* cpu);
uint8_t sub_a_n8(Cpu* cpu);
uint8_t xor_a_r8(Cpu* cpu, uint8_t* reg);
uint8_t xor_a_mhl(Cpu* cpu);
uint8_t xor_a_n8(Cpu* cpu);
uint8_t add_hl_r16(Cpu* cpu, uint16_t* reg);
uint8_t dec_r16(Cpu* cpu, uint16_t* reg);
uint8_t inc_r16(Cpu* cpu, uint16_t* reg);
uint8_t bit_u3_r8(Cpu* cpu, uint8_t position, uint8_t* reg);
uint8_t bit_u3_mhl(Cpu* cpu, uint8_t position);
uint8_t res_u3_r8(Cpu* cpu, uint8_t position, uint8_t* reg);
uint8_t res_u3_mhl(Cpu* cpu, uint8_t position);
uint8_t set_u3_r8(Cpu* cpu, uint8_t position, uint8_t* reg);
uint8_t set_u3_mhl(Cpu* cpu, uint8_t position);
uint8_t swap_r8(Cpu* cpu, uint8_t* reg);
uint8_t swap_mhl(Cpu* cpu);
uint8_t rl_r8(Cpu* cpu, uint8_t* reg);
uint8_t rl_mhl(Cpu* cpu);
uint8_t rla(Cpu* cpu);
uint8_t rlc_r8(Cpu* cpu, uint8_t* reg);
uint8_t rlc_mhl(Cpu* cpu);
uint8_t rlca(Cpu* cpu);
uint8_t rr_r8(Cpu* cpu, uint8_t* reg);
uint8_t rr_mhl(Cpu* cpu);
uint8_t rra(Cpu* cpu);
uint8_t rrc_r8(Cpu* cpu, uint8_t* reg);
uint8_t rrc_mhl(Cpu* cpu);
uint8_t rrca(Cpu* cpu);
uint8_t sla_r8(Cpu* cpu, uint8_t* reg);
uint8_t sla_mhl(Cpu* cpu);
uint8_t sra_r8(Cpu* cpu, uint8_t* reg);
uint8_t sra_mhl(Cpu* cpu);
uint8_t srl_r8(Cpu* cpu, uint8_t* reg);
uint8_t srl_mhl(Cpu* cpu);
uint8_t ld_r8_r8(Cpu* cpu, uint8_t* a, uint8_t* b);
uint8_t ld_r8_n8(Cpu* cpu, uint8_t* reg);
uint8_t ld_r16_n16(Cpu* cpu, uint16_t* reg);
uint8_t ld_mhl_r8(Cpu* cpu, uint8_t* reg);
uint8_t ld_mhl_n8(Cpu* cpu);
uint8_t ld_r8_mhl(Cpu* cpu, uint8_t* reg);
uint8_t ld_mr16_a(Cpu* cpu, uint16_t* reg);
uint8_t ld_mn16_a(Cpu* cpu);
uint8_t ldh_mn16_a(Cpu* cpu);
uint8_t ldh_mc_a(Cpu* cpu);
uint8_t ld_a_mr16(Cpu* cpu, uint16_t* reg);
uint8_t ld_a_mn16(Cpu* cpu);
uint8_t ldh_a_mn16(Cpu* cpu);
uint8_t ldh_a_mc(Cpu* cpu);
uint8_t ld_mhli_a(Cpu* cpu);
uint8_t ld_mhld_a(Cpu* cpu);
uint8_t ld_a_mhli(Cpu* cpu);
uint8_t ld_a_mhld(Cpu* cpu);
uint8_t call_n16(Cpu* cpu);
uint8_t call_cc_n16(Cpu* cpu,CC cc);
uint8_t jp_hl(Cpu* cpu);
uint8_t jp_n16(Cpu* cpu);
uint8_t jp_cc_n16(Cpu* cpu, CC cc);
uint8_t jr_e8(Cpu* cpu);
uint8_t jr_cc_e8(Cpu* cpu, CC cc);
uint8_t ret_cc(Cpu* cpu, CC cc);
uint8_t ret(Cpu* cpu);
uint8_t reti(Cpu* cpu);
uint8_t rst_vec(Cpu* cpu, uint8_t vec);
uint8_t add_hl_sp(Cpu* cpu);
uint8_t add_sp_e8(Cpu* cpu);
uint8_t dec_sp(Cpu* cpu);
uint8_t inc_sp(Cpu* cpu);
uint8_t ld_sp_n16(Cpu* cpu);
uint8_t ld_mn16_sp(Cpu* cpu);
uint8_t ld_hl_sp_e8(Cpu* cpu);
uint8_t ld_sp_hl(Cpu* cpu);
uint8_t pop_af(Cpu* cpu);
uint8_t pop_r16(Cpu* cpu, uint16_t* reg);
uint8_t push_af(Cpu* cpu);
uint8_t push_r16(Cpu* cpu, uint16_t* reg);
uint8_t ccf(Cpu* cpu);
uint8_t cpl(Cpu* cpu);
uint8_t daa(Cpu* cpu);
uint8_t di(Cpu* cpu);
uint8_t ei(Cpu* cpu);
uint8_t halt(Cpu* cpu);
uint8_t nop(Cpu* cpu);
uint8_t scf(Cpu* cpu);
uint8_t stop(Cpu* cpu);
#endif