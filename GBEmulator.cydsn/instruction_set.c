#include "instruction_set.h"
#include "memory.h"
#include "rom.h"

void increment_pc(Cpu* cpu){
    cpu->reg.pc++;
    if (cpu->reg.pc == BIOS_SIZE && cpu->inBios) {
        cpu->inBios = false;
    }
}

static inline void push_stack_u8(Cpu* cpu, uint8_t value){
    cpu->reg.sp--;
    write_mem(&cpu->mem, cpu->reg.sp, value);
} 

static inline uint8_t pop_stack_u8(Cpu* cpu){
    uint8_t result = fetch(&cpu->mem, cpu->reg.sp, cpu->inBios);
    cpu->reg.sp++;
    return result;
}

static inline void push_stack(Cpu* cpu, uint16_t value){
    push_stack_u8(cpu, value & 0xF0);  // push high
    push_stack_u8(cpu, value & 0x0F);  // push low
}

static inline uint16_t pop_stack(Cpu* cpu){
    uint8_t low = pop_stack_u8(cpu);
    uint8_t high = pop_stack_u8(cpu);
    return (high << 8) | low;
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

static inline int8_t safe_convert(uint8_t x) {
    return x < 128 ? x : x - 256;
}

static inline void add_to_pc(Cpu* cpu, int8_t offset){
    if (offset < 0){
        cpu->reg.pc -= -offset;
    }else{
        cpu->reg.pc += offset;
    }
}

static inline void add_to_sp(Cpu* cpu, int8_t offset){
    set_zero_flag(&cpu->reg, false);
    set_subtraction_flag(&cpu->reg, false);
    if (offset < 0){
        set_half_carry_flag(&cpu->reg, half_carry_subtration_u16(cpu->reg.sp, -offset));
        set_carry_flag(&cpu->reg, carry_on_subtraction_u16(cpu->reg.sp, -offset));
        cpu->reg.sp -= -offset;
    }else{
        set_half_carry_flag(&cpu->reg, half_carry_addition_u16(cpu->reg.sp, offset));
        set_carry_flag(&cpu->reg, carry_on_addition_u16(cpu->reg.sp, offset));
        cpu->reg.sp += offset;
    }
}

uint8_t fetch_and_increment_pc(Cpu* cpu){
    uint8_t data = fetch(&cpu->mem, cpu->reg.pc, cpu->inBios);
    increment_pc(cpu);
    return data;
}

uint16_t fetch_and_increment_pc_twice(Cpu* cpu){
    uint8_t lower = fetch_and_increment_pc(cpu);
    uint8_t upper = fetch_and_increment_pc(cpu);
    return (upper << 8) ^ lower;
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

static inline void add_hl_b(Cpu* cpu, uint16_t b){
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, half_carry_addition_u16(cpu->reg.hl, b));
    set_carry_flag(&cpu->reg, carry_on_addition_u16(cpu->reg.hl, b));
    cpu->reg.hl += b;
}
uint8_t add_hl_r16(Cpu* cpu, uint16_t* reg){
    add_hl_b(cpu, *reg);
    return 2;
}
uint8_t dec_r16(Cpu* cpu, uint16_t* reg){
    *reg -= 1;
    return 2;
}
uint8_t inc_r16(Cpu* cpu, uint16_t* reg){
    *reg += 1;
    return 2;
}
static inline void bit_u3_b(Cpu* cpu, uint8_t position, uint8_t to_test){
    set_zero_flag(&cpu->reg, (to_test & (1 << position)) == 0);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, true);
}
uint8_t bit_u3_r8(Cpu* cpu, uint8_t position, uint8_t* reg){
    bit_u3_b(cpu, position, *reg);
    return 2;
}
uint8_t bit_u3_mhl(Cpu* cpu, uint8_t position){
    bit_u3_b(cpu, position, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    return 3;
}
// returns the new value
static inline uint8_t res_u3_b(Cpu* cpu, uint8_t position, uint8_t b){
    return b & ~(1 << position);
}
uint8_t res_u3_r8(Cpu* cpu, uint8_t position, uint8_t* reg){
    *reg = res_u3_b(cpu, position, *reg);
    return 2;
}
uint8_t res_u3_mhl(Cpu* cpu, uint8_t position){
    uint8_t new_val = res_u3_b(cpu, position, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    write_mem(&cpu->mem, cpu->reg.hl, new_val);
    return 4;
}
// Returns the new value
static inline uint8_t set_u3_b(Cpu* cpu, uint8_t position, uint8_t b){
    return b | (1 << position);
}
uint8_t set_u3_r8(Cpu* cpu, uint8_t position, uint8_t* reg){
    *reg = set_u3_b(cpu, position, *reg);
    return 2;
}
uint8_t set_u3_mhl(Cpu* cpu, uint8_t position){
    uint8_t new_val = set_u3_b(cpu, position, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    write_mem(&cpu->mem, cpu->reg.hl, new_val);
    return 4;
}
// Returns new value
static inline uint8_t swap_nibbles(Cpu* cpu, uint8_t val){
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    set_carry_flag(&cpu->reg, false);
    uint8_t result = ((val << 4) | (val >> 4));
    set_zero_flag(&cpu->reg, result == 0);
    return result;
}
uint8_t swap_r8(Cpu* cpu, uint8_t* reg){
    *reg = swap_nibbles(cpu, *reg);
    return 2;
}
uint8_t swap_mhl(Cpu* cpu){
    uint8_t new_val = swap_nibbles(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    write_mem(&cpu->mem, cpu->reg.hl, new_val);
    return 4;
}
static inline uint8_t rl_b(Cpu* cpu, uint8_t b){
    uint8_t old_carry = get_carry_flag(&cpu->reg);
    set_carry_flag(&cpu->reg, (b & 0x8));
    uint8_t result = (b << 1) | old_carry;
    set_zero_flag(&cpu->reg, result == 0);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    return result;
}
uint8_t rl_r8(Cpu* cpu, uint8_t* reg){
    *reg = rl_b(cpu, *reg);
    return 2;
}
uint8_t rl_mhl(Cpu* cpu){
    uint8_t new_val = rl_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    write_mem(&cpu->mem, cpu->reg.hl, new_val);
    return 4;
}
uint8_t rla(Cpu* cpu){
    set_zero_flag(&cpu->reg, false);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    uint8_t old_carry = get_carry_flag(&cpu->reg);
    set_carry_flag(&cpu->reg, (cpu->reg.a & 0x8));
    cpu->reg.a = (cpu->reg.a << 1) | old_carry;
    return 1;
}
static inline uint8_t rlc_b(Cpu* cpu, uint8_t b){
    set_carry_flag(&cpu->reg, (b & 0x8));
    uint8_t result = (b << 1) | (b >> 7);
    set_zero_flag(&cpu->reg, result == 0);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    return result;
}
uint8_t rlc_r8(Cpu* cpu, uint8_t* reg){
    *reg = rlc_b(cpu, *reg);
    return 2;
}
uint8_t rlc_mhl(Cpu* cpu){
    uint8_t new_val = rlc_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    write_mem(&cpu->mem, cpu->reg.hl, new_val);
    return 4;
}
uint8_t rlca(Cpu* cpu){
    set_carry_flag(&cpu->reg, (cpu->reg.a & 0x8));
    cpu->reg.a = (cpu->reg.a << 1) | (cpu->reg.a >> 7);
    set_zero_flag(&cpu->reg, false);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    return 1;
}
static inline uint8_t rr_b(Cpu* cpu, uint8_t b){
    uint8_t old_carry = get_carry_flag(&cpu->reg);
    set_carry_flag(&cpu->reg, (b & 0x1));
    uint8_t result = (b >> 1) | (old_carry << 7);
    set_zero_flag(&cpu->reg, result == 0);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    return result;
}
uint8_t rr_r8(Cpu* cpu, uint8_t* reg){
    *reg = rr_b(cpu, *reg);
    return 2;
}
uint8_t rr_mhl(Cpu* cpu){
    uint8_t new_val = rr_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    write_mem(&cpu->mem, cpu->reg.hl, new_val);
    return 4;
}
uint8_t rra(Cpu* cpu){
    uint8_t old_carry = get_carry_flag(&cpu->reg);
    set_carry_flag(&cpu->reg, (cpu->reg.a & 0x1));
    cpu->reg.a = (cpu->reg.a >> 1) | (old_carry << 7);
    set_zero_flag(&cpu->reg, false);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    return 1;
}
static inline uint8_t rrc_b(Cpu* cpu, uint8_t b){
    set_carry_flag(&cpu->reg, (b & 0x1));
    uint8_t result = (b >> 1) | (b << 7);
    set_zero_flag(&cpu->reg, result == 0);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    return result;
}
uint8_t rrc_r8(Cpu* cpu, uint8_t* reg){
    *reg = rrc_b(cpu, *reg);
    return 2;
}
uint8_t rrc_mhl(Cpu* cpu){
    uint8_t new_val = rrc_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    write_mem(&cpu->mem, cpu->reg.hl, new_val);
    return 4;
}
uint8_t rrca(Cpu* cpu){
    set_carry_flag(&cpu->reg, (cpu->reg.a & 0x1));
    uint8_t result = (cpu->reg.a >> 1) | (cpu->reg.a << 7);
    set_zero_flag(&cpu->reg, false);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    return result;
}
static inline uint8_t sla_b(Cpu* cpu, uint8_t b){
    set_carry_flag(&cpu->reg, b & 0x8);
    uint8_t result = b << 1;
    set_zero_flag(&cpu->reg, result == 0);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    return result;
}
uint8_t sla_r8(Cpu* cpu, uint8_t* reg){
    *reg = sla_b(cpu, *reg);
    return 2;
}
uint8_t sla_mhl(Cpu* cpu){
    uint8_t new_val = sla_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    write_mem(&cpu->mem, cpu->reg.hl, new_val);
    return 4;
}
static inline uint8_t sra_b(Cpu* cpu, uint8_t b){
    set_carry_flag(&cpu->reg, b & 0x1);
    uint8_t result = (b >> 1) | (b & 0x8);
    set_zero_flag(&cpu->reg, result == 0);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    return result;
}
uint8_t sra_r8(Cpu* cpu, uint8_t* reg){
    *reg = sra_b(cpu, *reg);
    return 2;
}
uint8_t sra_mhl(Cpu* cpu){
    uint8_t new_val = sra_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    write_mem(&cpu->mem, cpu->reg.hl, new_val);
    return 4;
}
static inline uint8_t srl_b(Cpu* cpu, uint8_t b){
    set_carry_flag(&cpu->reg, b & 0x1);
    uint8_t result = (b >> 1);
    set_zero_flag(&cpu->reg, result == 0);
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    return result;
}
uint8_t srl_r8(Cpu* cpu, uint8_t* reg){
    *reg = srl_b(cpu, *reg);
    return 2;
}
uint8_t srl_mhl(Cpu* cpu){
    uint8_t new_val = srl_b(cpu, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
    write_mem(&cpu->mem, cpu->reg.hl, new_val);
    return 4;
}
uint8_t ld_r8_r8(Cpu* cpu, uint8_t* a, uint8_t* b){
    *a = *b;
    return 1;
}
uint8_t ld_r8_n8(Cpu* cpu, uint8_t* reg){
    *reg = fetch_and_increment_pc(cpu);
    return 2;
}
uint8_t ld_r16_n16(Cpu* cpu, uint16_t* reg){
    *reg = fetch_and_increment_pc_twice(cpu);
    return 3;
}
uint8_t ld_mhl_r8(Cpu* cpu, uint8_t* reg){
    write_mem(&cpu->mem, cpu->reg.hl, *reg);
    return 2;
}
uint8_t ld_mhl_n8(Cpu* cpu){
    write_mem(&cpu->mem, cpu->reg.hl, fetch_and_increment_pc(cpu));
    return 3;
}
uint8_t ld_r8_mhl(Cpu* cpu, uint8_t* reg){
    *reg = fetch(&cpu->mem, cpu->reg.hl, cpu->inBios);
    return 2;
}
uint8_t ld_mr16_a(Cpu* cpu, uint16_t* reg){
    write_mem(&cpu->mem, *reg, cpu->reg.a);
    return 2;
}
uint8_t ld_mn16_a(Cpu* cpu){
    write_mem(&cpu->mem, fetch_and_increment_pc_twice(cpu), cpu->reg.a);
    return 4;
}
uint8_t ldh_mn16_a(Cpu* cpu){
    write_mem(&cpu->mem, 0xFF00 + fetch_and_increment_pc(cpu), cpu->reg.a);
    return 3;
}
uint8_t ldh_mc_a(Cpu* cpu){
    write_mem(&cpu->mem, 0xFF00 + cpu->reg.c, cpu->reg.a);
    return 2;
}
uint8_t ld_a_mr16(Cpu* cpu, uint16_t* reg){
    cpu->reg.a = fetch(&cpu->mem, *reg, cpu->inBios);
    return 2;
}
uint8_t ld_a_mn16(Cpu* cpu){
    cpu->reg.a = fetch(&cpu->mem, fetch_and_increment_pc_twice(cpu), cpu->inBios);
    return 4;
}
uint8_t ldh_a_mn16(Cpu* cpu){
    cpu->reg.a = fetch(&cpu->mem, 0xFF00 + fetch_and_increment_pc(cpu), cpu->inBios);
    return 3;
}
uint8_t ldh_a_mc(Cpu* cpu){
    cpu->reg.a = fetch(&cpu->mem, 0xFF00 + cpu->reg.c, cpu->inBios);
    return 2;
}
uint8_t ld_mhli_a(Cpu* cpu){
    write_mem(&cpu->mem, cpu->reg.hl, cpu->reg.a);
    cpu->reg.hl++;
    return 2;
}
uint8_t ld_mhld_a(Cpu* cpu){
    write_mem(&cpu->mem, cpu->reg.hl, cpu->reg.a);
    cpu->reg.hl--;
    return 2;
}
uint8_t ld_a_mhli(Cpu* cpu){
    cpu->reg.a = fetch(&cpu->mem, cpu->reg.hl, cpu->inBios);
    cpu->reg.hl++;
    return 2;
}
uint8_t ld_a_mhld(Cpu* cpu){
    cpu->reg.a = fetch(&cpu->mem, cpu->reg.hl, cpu->inBios);
    cpu->reg.hl--;
    return 2;
}
uint8_t call_n16(Cpu* cpu){
    // Push pc after call onto stack
    push_stack(cpu, cpu->reg.pc);
    cpu->reg.pc = fetch_and_increment_pc_twice(cpu);
    return 6;
}
uint8_t call_cc_n16(Cpu* cpu, CC cc){
    switch (cc){
        case Z:
        if (get_zero_flag(&cpu->reg)) return call_n16(cpu);
        break;
        case NZ:
        if (!get_zero_flag(&cpu->reg)) return call_n16(cpu);
        break;
        case C:
        if (get_carry_flag(&cpu->reg)) return call_n16(cpu);
        break;
        case NC:
        if (!get_carry_flag(&cpu->reg)) return call_n16(cpu);
        break;
    }
    return 3;
}
uint8_t jp_hl(Cpu* cpu){
    cpu->reg.pc = cpu->reg.hl;
    return 1;
}
uint8_t jp_n16(Cpu* cpu){
    cpu->reg.pc = fetch_and_increment_pc_twice(cpu);
    return 4;
}
uint8_t jp_cc_n16(Cpu* cpu, CC cc){
    switch (cc){
        case Z:
        if (get_zero_flag(&cpu->reg)) return jp_n16(cpu);
        break;
        case NZ:
        if (!get_zero_flag(&cpu->reg)) return jp_n16(cpu);
        break;
        case C:
        if (get_carry_flag(&cpu->reg)) return jp_n16(cpu);
        break;
        case NC:
        if (!get_carry_flag(&cpu->reg)) return jp_n16(cpu);
        break;
    }
    return 3;
}
uint8_t jr_e8(Cpu* cpu){
    int8_t offset = safe_convert(fetch_and_increment_pc(cpu));
    add_to_pc(cpu, offset);
    return 3;
}
uint8_t jr_cc_e8(Cpu* cpu, CC cc){
    switch (cc){
        case Z:
        if (get_zero_flag(&cpu->reg)) return jr_e8(cpu);
        break;
        case NZ:
        if (!get_zero_flag(&cpu->reg)) return jr_e8(cpu);
        break;
        case C:
        if (get_carry_flag(&cpu->reg)) return jr_e8(cpu);
        break;
        case NC:
        if (!get_carry_flag(&cpu->reg)) return jr_e8(cpu);
        break;
    }
    return 2;
}
uint8_t ret(Cpu* cpu){
    cpu->reg.pc = pop_stack(cpu);
    return 4;
}
uint8_t ret_cc(Cpu* cpu, CC cc){
    switch (cc){
        case Z:
        if (get_zero_flag(&cpu->reg)) return ret(cpu) + 1;
        break;
        case NZ:
        if (!get_zero_flag(&cpu->reg)) return ret(cpu) + 1;
        break;
        case C:
        if (get_carry_flag(&cpu->reg)) return ret(cpu) + 1;
        break;
        case NC:
        if (!get_carry_flag(&cpu->reg)) return ret(cpu) + 1;
        break;
    }
    return 2;
}

uint8_t reti(Cpu* cpu){
    set_ime(cpu, true);
    return ret(cpu);
}
uint8_t rst_vec(Cpu* cpu, uint8_t vec){
    push_stack(cpu, cpu->reg.pc);
    cpu->reg.pc = vec;
    return 4;
}
uint8_t add_hl_sp(Cpu* cpu){
    add_hl_b(cpu, cpu->reg.sp);
    return 2;
}
uint8_t add_sp_e8(Cpu* cpu){
    int8_t offset = safe_convert(fetch_and_increment_pc(cpu));
    add_to_sp(cpu, offset);
    return 4;
}
uint8_t dec_sp(Cpu* cpu){
    cpu->reg.sp--;
    return 2;
}
uint8_t inc_sp(Cpu* cpu){
    cpu->reg.sp++;
    return 2;
}
uint8_t ld_sp_n16(Cpu* cpu){
    cpu->reg.sp = fetch_and_increment_pc_twice(cpu);
    return 3;
}
uint8_t ld_mn16_sp(Cpu* cpu){
    uint16_t addr = fetch_and_increment_pc_twice(cpu);
    write_mem(&cpu->mem, addr, cpu->reg.sp & 0x00FF);
    write_mem(&cpu->mem, addr + 1, cpu->reg.sp & 0xFF00);
    return 5;
}
uint8_t ld_hl_sp_e8(Cpu* cpu){
    uint16_t old_sp = cpu->reg.sp;
    int8_t offset = safe_convert(fetch_and_increment_pc(cpu));
    add_to_sp(cpu, offset);
    cpu->reg.hl = cpu->reg.sp;
    cpu->reg.sp = old_sp;
    return 3;
}
uint8_t ld_sp_hl(Cpu* cpu){
    cpu->reg.sp = cpu->reg.hl;
    return 2;
}
uint8_t pop_af(Cpu* cpu){
    uint16_t popped = pop_stack(cpu);
    set_zero_flag(&cpu->reg, (popped >> 7) & 1);
    set_subtraction_flag(&cpu->reg, (popped >> 6) & 1);
    set_half_carry_flag(&cpu->reg, (popped >> 5) & 1);
    set_carry_flag(&cpu->reg, (popped >> 4) & 1);
    cpu->reg.af = popped;
    return 3;
}
uint8_t pop_r16(Cpu* cpu, uint16_t* reg){
    *reg = pop_stack(cpu);
    return 3;
}
uint8_t push_af(Cpu* cpu){
    push_stack(cpu, cpu->reg.af);
    return 4;
}
uint8_t push_r16(Cpu* cpu, uint16_t* reg){
    push_stack(cpu, *reg);
    return 4;
}
uint8_t ccf(Cpu* cpu){
    set_carry_flag(&cpu->reg, !get_carry_flag(&cpu->reg));
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    return 1;
}
uint8_t cpl(Cpu* cpu){
    cpu->reg.a = ~cpu->reg.a;
    set_subtraction_flag(&cpu->reg, true);
    set_half_carry_flag(&cpu->reg, true);
    return 1;
}
uint8_t daa(Cpu* cpu){
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //TODO
    return 0;
}
uint8_t di(Cpu* cpu){
    set_ime(cpu, false);
    return 1;
}
uint8_t ei(Cpu* cpu){
    set_ime(cpu, true);
    return 1;
}
uint8_t halt(Cpu* cpu){
    ///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    return 0;
}
uint8_t nop(Cpu* cpu){
    return 1;
}
uint8_t scf(Cpu* cpu){
    set_subtraction_flag(&cpu->reg, false);
    set_half_carry_flag(&cpu->reg, false);
    set_carry_flag(&cpu->reg, true);
    return 1;
}
uint8_t stop(Cpu* cpu){
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    return 0;
}
