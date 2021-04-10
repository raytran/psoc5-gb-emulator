#include "instruction_set.h"
#include "memory.h"
#include "rom.h"
static void increment_pc(Cpu* cpu){
    cpu->reg.pc++;
    if (cpu->reg.pc == BIOS_SIZE && cpu->inBios) {
        cpu->inBios = false;
    }
}

static inline void add_to_pc(Cpu* cpu, int8_t offset){
    if (offset < 0){
        cpu->reg.pc -= -offset;
    }else{
        cpu->reg.pc += offset;
    }
}

static inline uint8_t fetch_and_increment_pc(Cpu* cpu){
    uint8_t data = fetch(&cpu->mem, cpu->reg.pc, cpu->inBios);
    increment_pc(cpu);
    return data;
}

static inline uint16_t fetch_and_increment_pc_twice(Cpu* cpu){
    uint8_t lower = fetch_and_increment_pc(cpu);
    uint8_t upper = fetch_and_increment_pc(cpu);
    return (upper << 8) ^ lower;
}

static inline bool carry_on_subtraction_u8(uint8_t first_num, uint8_t second_num){
    return first_num > second_num;
}
static inline bool carry_on_subtraction_u16(uint16_t first_num, uint16_t second_num){
    return first_num > second_num;
}
static inline bool carry_on_addition_u8(uint8_t first_num, uint8_t second_num){
    return (int) first_num + (int) second_num > 0xFF;
}

static inline bool carry_on_addition_u16(uint16_t first_num, uint16_t second_num){
    return (int) first_num + (int) second_num > 0xFFFF;
}

static inline bool half_carry_addition_u8(uint8_t first_num, uint8_t second_num)
{
    return (((first_num & 0x0F) + (second_num & 0x0F)) & 0x10) == 0x10;
}

static inline bool half_carry_addition_u16(uint16_t first_num, uint16_t second_num)
{
    return (((first_num & 0x00FF) + (second_num & 0x00FF)) & 0x0100) == 0x0100;
}

static inline bool half_carry_subtration_u8(uint8_t first_num, uint8_t second_num)
{
    return (int)(first_num & 0x0F) - (int)(second_num & 0x0F) < 0;
}

static inline bool half_carry_subtration_u16(uint16_t first_num, uint16_t second_num)
{
    return (int)(first_num & 0x00FF) - (int)(second_num & 0x00FF) < 0;
}

static inline void adc_a_b(Cpu* cpu, uint8_t b){
    set_subtraction_flag(&cpu->reg, false);
    uint8_t second_num = b + get_carry_flag(&cpu->reg);
    set_half_carry_flag(&cpu->reg, half_carry_addition_u8(cpu->reg.a, second_num));
    set_carry_flag(&cpu->reg, carry_on_addition_u8(cpu->reg.a, second_num));
    cpu->reg.a += second_num;
    set_zero_flag(&cpu->reg, cpu->reg.a == 0);
}

uint8_t adc_a_r8(Cpu* cpu, uint8_t* reg){
    adc_a_b(cpu, *reg);
    return 1;
}
uint8_t adc_a_mhl(Cpu* cpu){
    adc_a_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    return 2;
}
uint8_t adc_a_n8(Cpu* cpu){
    adc_a_b(cpu, fetch_and_increment_pc(cpu));
    return 2;
}

static inline void add_a_b(Cpu* cpu, uint8_t b){
    set_subtraction_flag(&cpu->reg, false);
    set_carry_flag(&cpu->reg, carry_on_addition_u8(cpu->reg.a, b));
    set_half_carry_flag(&cpu->reg, half_carry_addition_u8(cpu->reg.a, b));
    cpu->reg.a += b;
    set_zero_flag(&cpu->reg, cpu->reg.a == 0);
}
uint8_t add_a_r8(Cpu* cpu, uint8_t* reg){
    add_a_b(cpu, *reg);
    return 1;
}
uint8_t add_a_mhl(Cpu* cpu){
    add_a_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    return 2;
}
uint8_t add_a_n8(Cpu* cpu){
    adc_a_b(cpu, fetch_and_increment_pc(cpu));
    return 2;
}
static inline void and_a_b(Cpu* cpu, uint8_t b){
    cpu->reg.a &= b;
    set_zero_flag(&cpu->reg, cpu->reg.a == 0);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, true);
    set_carry_flag(&cpu->reg, false);
}
uint8_t and_a_r8(Cpu* cpu, uint8_t* reg){
    and_a_b(cpu, *reg);
    return 1;
}
uint8_t and_a_mhl(Cpu* cpu){
    and_a_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    return 2;
}
uint8_t and_a_n8(Cpu* cpu){
    and_a_b(cpu, fetch_and_increment_pc(cpu));
    return 2;
}
static inline void cp_a_b(Cpu* cpu, uint8_t b){
    set_zero_flag(&cpu->reg, cpu->reg.a - b == 0);
    set_subtraction_flag(&cpu->reg, true);
    set_half_carry_flag(&cpu->reg, half_carry_subtration_u8(cpu->reg.a, b));
    set_carry_flag(&cpu->reg, b > cpu->reg.a);
}
uint8_t cp_a_r8(Cpu* cpu, uint8_t* reg){
    cp_a_b(cpu, *reg);
    return 1;
}
uint8_t cp_a_mhl(Cpu* cpu){
    cp_a_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    return 2;
}
uint8_t cp_a_n8(Cpu* cpu){
    cp_a_b(cpu, fetch_and_increment_pc(cpu));
    return 2;
}
static inline uint8_t dec_u8(Cpu* cpu, uint8_t num){
    bool bit_4_set = num & 0b00001000;
    num -= 1;
    set_zero_flag(&cpu->reg, num == 0);
    set_subtraction_flag(&cpu->reg, true);
    set_half_carry_flag(&cpu->reg, bit_4_set && !(num & 0b00001000));
    return num;
}
uint8_t dec_r8(Cpu* cpu, uint8_t* reg){
    *reg = dec_u8(cpu, *reg);
    return 1;
}
uint8_t dec_mhl(Cpu* cpu){
    uint8_t num = fetch(&cpu->mem, cpu->reg.hl, cpu->inBios);
    write_mem(&cpu->mem, cpu->reg.hl, dec_u8(cpu, num));
    return 3;
}
static inline uint8_t inc_u8(Cpu* cpu, uint8_t num){
    bool bit_3_set = num & 0b00000100;
    num += 1;
    set_zero_flag(&cpu->reg, num == 0);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, bit_3_set && !(num & 0b00000100));
    return num;
}
uint8_t inc_r8(Cpu* cpu, uint8_t* reg){
    *reg = inc_u8(cpu, *reg);
    return 1;
}
uint8_t inc_mhl(Cpu* cpu){
    uint8_t num = fetch(&cpu->mem, cpu->reg.hl, cpu->inBios);
    write_mem(&cpu->mem, cpu->reg.hl, inc_u8(cpu, num));
    return 3;
}
static inline void or_a_b(Cpu* cpu, uint8_t b){
    cpu->reg.a |= b;
    set_zero_flag(&cpu->reg, cpu->reg.a == 0);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    set_carry_flag(&cpu->reg, false);
}
uint8_t or_a_r8(Cpu* cpu, uint8_t* reg){
    or_a_b(cpu, *reg);
    return 1;
}
uint8_t or_a_mhl(Cpu* cpu){
    or_a_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    return 2;
}
uint8_t or_a_n8(Cpu* cpu){
    or_a_b(cpu, fetch_and_increment_pc(cpu));
    return 2;
}
static inline void sbc_a_b(Cpu* cpu, uint8_t b){
    set_subtraction_flag(&cpu->reg, true);
    uint8_t second_num = b + get_carry_flag(&cpu->reg);
    set_half_carry_flag(&cpu->reg, half_carry_subtration_u8(cpu->reg.a, second_num));
    set_carry_flag(&cpu->reg, carry_on_subtraction_u8(cpu->reg.a, second_num));
    cpu->reg.a -= second_num;
    set_zero_flag(&cpu->reg, cpu->reg.a == 0);
}
uint8_t sbc_a_r8(Cpu* cpu, uint8_t* reg){
    sbc_a_b(cpu, *reg);
    return 1;
}
uint8_t sbc_a_mhl(Cpu* cpu){
    sbc_a_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    return 2;
}
uint8_t sbc_a_n8(Cpu* cpu){
    sbc_a_b(cpu, fetch_and_increment_pc(cpu));
    return 2;
}
static inline void sub_a_b(Cpu* cpu, uint8_t b){
    set_subtraction_flag(&cpu->reg, true);
    set_half_carry_flag(&cpu->reg, half_carry_subtration_u8(cpu->reg.a, b));
    set_carry_flag(&cpu->reg, carry_on_subtraction_u8(cpu->reg.a, b));
    cpu->reg.a -= b;
    set_zero_flag(&cpu->reg, cpu->reg.a == 0);
}
uint8_t sub_a_r8(Cpu* cpu, uint8_t* reg){
    sub_a_b(cpu, *reg);
    return 1;
}
uint8_t sub_a_mhl(Cpu* cpu){
    sub_a_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    return 2;
}
uint8_t sub_a_n8(Cpu* cpu){
    sub_a_b(cpu, fetch_and_increment_pc(cpu));
    return 2;
}
static inline void xor_a_b(Cpu* cpu, uint8_t b){
    cpu->reg.a ^= b;
    set_zero_flag(&cpu->reg, cpu->reg.a == 0);
    set_subtraction_flag(&cpu->reg, false);
    set_carry_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
}
uint8_t xor_a_r8(Cpu* cpu, uint8_t* reg){
    xor_a_b(cpu, *reg);
    return 1;
}
uint8_t xor_a_mhl(Cpu* cpu){
    xor_a_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    return 2;
}
uint8_t xor_a_n8(Cpu* cpu){
    xor_a_b(cpu, fetch_and_increment_pc(cpu));
    return 2;
}
uint8_t add_hl_r16(Cpu* cpu, uint16_t* reg);
uint8_t dec_r16(Cpu* cpu, uint16_t* reg);
uint8_t inc_r16(Cpu* cpu, uint16_t* reg);
uint8_t bit_u3_r8(Cpu* cpu, uint8_t* reg);
uint8_t bit_u3_mhl(Cpu* cpu);
uint8_t res_u3_r8(Cpu* cpu, uint8_t* reg);
uint8_t res_u3_mhl(Cpu* cpu);
uint8_t set_u3_r8(Cpu* cpu, uint8_t* reg);
uint8_t set_u3_mhl(Cpu* cpu);
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
uint8_t ld_mr16_a(Cpu* cpu);
uint8_t ld_mn16_a(Cpu* cpu);
uint8_t ldh_mn16_a(Cpu* cpu);
uint8_t ldh_mc_a(Cpu* cpu);
uint8_t ld_a_mr16(Cpu* cpu);
uint8_t ld_a_mn16(Cpu* cpu);
uint8_t ldh_a_mn16(Cpu* cpu);
uint8_t ldh_a_mc(Cpu* cpu);
uint8_t ld_mhli_a(Cpu* cpu);
uint8_t ld_mhld_a(Cpu* cpu);
uint8_t ld_a_mhli(Cpu* cpu);
uint8_t ld_a_mhld(Cpu* cpu);
uint8_t call_n16(Cpu* cpu);
uint8_t call_cc_n16(Cpu* cpu, CC cc);
uint8_t jp_hl(Cpu* cpu);
uint8_t jp_n16(Cpu* cpu);
uint8_t jp_cc_n16(Cpu* cpu, CC cc);
uint8_t jr_e8(Cpu* cpu);
uint8_t jr_cc_e8(Cpu* cpu, CC cc);
uint8_t ret_cc(Cpu* cpu, CC cc);
uint8_t ret(Cpu* cpu);
uint8_t reti(Cpu* cpu);
uint8_t rst_vec(Cpu* cpu);
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
