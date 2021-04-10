#include "memory.h"
#include "rom.h"
#include "registers.h"
#include "cpu.h"
#include "instruction_set.h"

void reset_cpu(Cpu* cpu) {
    cpu->inBios = true;
    reset_memory(&cpu->mem);
    reset_registers(&cpu->reg);
}

void set_ime(Cpu *cpu, bool interrupts_on){
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
}

// Assumes that the pc is already incremented to point to the next instr
int execute_normal(Cpu* cpu, uint8_t instruction){
    switch (instruction){
        case 0x0: return nop(cpu); //NOP
        case 0x1: return ld_r16_n16(cpu, &cpu->reg.bc); //LD BC,u16
        case 0x2: return ld_mr16_a(cpu, &cpu->reg.bc); //LD (BC),A
        case 0x3: return inc_r16(cpu, &cpu->reg.bc); //INC BC
        case 0x4: return inc_r8(cpu, &cpu->reg.b); //INC B
        case 0x5: return dec_r8(cpu, &cpu->reg.b); //DEC B
        case 0x6: return ld_r8_n8(cpu, &cpu->reg.b); //LD B,u8
        case 0x7: return rlca(cpu); //RLCA
        case 0x8: return ld_mn16_sp(cpu); //LD (u16),SP
        case 0x9: return add_hl_r16(cpu, &cpu->reg.bc); //ADD HL,BC
        case 0xa: return ld_a_mr16(cpu, &cpu->reg.bc); //LD A,(BC)
        case 0xb: return dec_r16(cpu, &cpu->reg.bc); //DEC BC
        case 0xc: return inc_r8(cpu, &cpu->reg.c); //INC C
        case 0xd: return dec_r8(cpu, &cpu->reg.c); //DEC C
        case 0xe: return ld_r8_n8(cpu, &cpu->reg.c); //LD C,u8
        case 0xf: return rrca(cpu); //RRCA
        case 0x10: return stop(cpu);  //STOP
        case 0x11: return ld_r16_n16(cpu, &cpu->reg.de); //LD DE,u16
        case 0x12: return ld_mr16_a(cpu, &cpu->reg.de); //LD (DE),A
        case 0x13: return inc_r16(cpu, &cpu->reg.de); //INC DE
        case 0x14: return inc_r8(cpu, &cpu->reg.d); //INC D
        case 0x15: return dec_r8(cpu, &cpu->reg.d); //DEC D
        case 0x16: return ld_r8_n8(cpu, &cpu->reg.d); //LD D,u8
        case 0x17: return rla(cpu); //RLA
        case 0x18: return jr_e8(cpu); //JR i8
        case 0x19: return add_hl_r16(cpu, &cpu->reg.de); //ADD HL,DE
        case 0x1a: return ld_a_mr16(cpu, &cpu->reg.de); //LD A,(DE)
        case 0x1b: return dec_r16(cpu, &cpu->reg.de); //DEC DE
        case 0x1c: return inc_r8(cpu, &cpu->reg.e); //INC E
        case 0x1d: return dec_r8(cpu, &cpu->reg.e); //DEC E
        case 0x1e: return ld_r8_n8(cpu, &cpu->reg.e); //LD E,u8
        case 0x1f: return rra(cpu); //RRA
        case 0x20: return jr_cc_e8(cpu, NZ); //JR NZ,i8
        case 0x21: return ld_r16_n16(cpu, &cpu->reg.hl); //LD HL,u16
        case 0x22: return ld_mr16_a(cpu, &cpu->reg.hl); //LD (HL+),A
        case 0x23: return inc_r16(cpu, &cpu->reg.hl); //INC HL
        case 0x24: return inc_r8(cpu, &cpu->reg.h); //INC H
        case 0x25: return dec_r8(cpu, &cpu->reg.h); //DEC H
        case 0x26: return ld_r8_n8(cpu, &cpu->reg.h); //LD H,u8
        case 0x27: return daa(cpu); //DAA
        case 0x28: return jr_cc_e8(cpu, Z); //JR Z,i8
        case 0x29: return add_hl_r16(cpu, &cpu->reg.hl); //ADD HL,HL
        case 0x2a: return ld_a_mhli(cpu); //LD A,(HL+)
        case 0x2b: return dec_r16(cpu, &cpu->reg.hl); //DEC HL
        case 0x2c: return inc_r8(cpu, &cpu->reg.l); //INC L
        case 0x2d: return dec_r8(cpu, &cpu->reg.l); //DEC L
        case 0x2e: return ld_r8_n8(cpu, &cpu->reg.l); //LD L,u8
        case 0x2f: return cpl(cpu); //CPL
        case 0x30: return jr_cc_e8(cpu, NC); //JR NC,i8
        case 0x31: return ld_sp_n16(cpu); //LD SP,u16
        case 0x32: return ld_mhld_a(cpu); //LD (HL-),A
        case 0x33: return inc_sp(cpu); //INC SP
        case 0x34: return inc_r16(cpu, &cpu->reg.hl); //INC (HL)
        case 0x35: return dec_r16(cpu, &cpu->reg.hl); //DEC (HL)
        case 0x36: return ld_mhl_n8(cpu); //LD (HL),u8
        case 0x37: return scf(cpu); //SCF
        case 0x38: return jr_cc_e8(cpu, C); //JR C,i8
        case 0x39: return add_hl_sp(cpu); //ADD HL,SP
        case 0x3a: return ld_a_mhld(cpu); //LD A,(HL-)
        case 0x3b: return dec_sp(cpu); //DEC SP
        case 0x3c: return inc_r8(cpu, &cpu->reg.a); //INC A
        case 0x3d: return dec_r8(cpu, &cpu->reg.a); //DEC A
        case 0x3e: return ld_r8_n8(cpu, &cpu->reg.a); //LD A,u8
        case 0x3f: return ccf(cpu); //CCF
        case 0x40: return ld_r8_r8(cpu, &cpu->reg.b, &cpu->reg.b); //LD B,B
        case 0x41: return ld_r8_r8(cpu, &cpu->reg.b, &cpu->reg.c); //LD B,C
        case 0x42: return ld_r8_r8(cpu, &cpu->reg.b, &cpu->reg.d); //LD B,D
        case 0x43: return ld_r8_r8(cpu, &cpu->reg.b, &cpu->reg.e); //LD B,E
        case 0x44: return ld_r8_r8(cpu, &cpu->reg.b, &cpu->reg.h); //LD B,H
        case 0x45: return ld_r8_r8(cpu, &cpu->reg.b, &cpu->reg.l); //LD B,L
        case 0x46: return ld_r8_mhl(cpu, &cpu->reg.b); //LD B,(HL)
        case 0x47: return ld_r8_r8(cpu, &cpu->reg.b, &cpu->reg.a); //LD B,A
        case 0x48: return ld_r8_r8(cpu, &cpu->reg.c, &cpu->reg.b); //LD C,B
        case 0x49: return ld_r8_r8(cpu, &cpu->reg.c, &cpu->reg.c); //LD C,C
        case 0x4a: return ld_r8_r8(cpu, &cpu->reg.c, &cpu->reg.d); //LD C,D
        case 0x4b: return ld_r8_r8(cpu, &cpu->reg.c, &cpu->reg.e); //LD C,E
        case 0x4c: return ld_r8_r8(cpu, &cpu->reg.c, &cpu->reg.h); //LD C,H
        case 0x4d: return ld_r8_r8(cpu, &cpu->reg.c, &cpu->reg.l); //LD C,L
        case 0x4e: return ld_r8_mhl(cpu, &cpu->reg.c); //LD C,(HL)
        case 0x4f: return ld_r8_r8(cpu, &cpu->reg.c, &cpu->reg.a); //LD C,A
        case 0x50: return ld_r8_r8(cpu, &cpu->reg.d, &cpu->reg.b); //LD D,B
        case 0x51: return ld_r8_r8(cpu, &cpu->reg.d, &cpu->reg.c); //LD D,C
        case 0x52: return ld_r8_r8(cpu, &cpu->reg.d, &cpu->reg.d); //LD D,D
        case 0x53: return ld_r8_r8(cpu, &cpu->reg.d, &cpu->reg.e); //LD D,E
        case 0x54: return ld_r8_r8(cpu, &cpu->reg.d, &cpu->reg.h); //LD D,H
        case 0x55: return ld_r8_r8(cpu, &cpu->reg.d, &cpu->reg.l); //LD D,L
        case 0x56: return ld_r8_mhl(cpu, &cpu->reg.d); //LD D,(HL)
        case 0x57: return ld_r8_r8(cpu, &cpu->reg.d, &cpu->reg.a); //LD D,A
        case 0x58: return ld_r8_r8(cpu, &cpu->reg.e, &cpu->reg.b); //LD E,B
        case 0x59: return ld_r8_r8(cpu, &cpu->reg.e, &cpu->reg.c); //LD E,C
        case 0x5a: return ld_r8_r8(cpu, &cpu->reg.e, &cpu->reg.d); //LD E,D
        case 0x5b: return ld_r8_r8(cpu, &cpu->reg.e, &cpu->reg.e); //LD E,E
        case 0x5c: return ld_r8_r8(cpu, &cpu->reg.e, &cpu->reg.h); //LD E,H
        case 0x5d: return ld_r8_r8(cpu, &cpu->reg.e, &cpu->reg.l); //LD E,L
        case 0x5e: return ld_r8_mhl(cpu, &cpu->reg.e); //LD E,(HL)
        case 0x5f: return ld_r8_r8(cpu, &cpu->reg.e, &cpu->reg.a); //LD E,A
        case 0x60: return ld_r8_r8(cpu, &cpu->reg.h, &cpu->reg.b); //LD H,B
        case 0x61: return ld_r8_r8(cpu, &cpu->reg.h, &cpu->reg.c); //LD H,C
        case 0x62: return ld_r8_r8(cpu, &cpu->reg.h, &cpu->reg.d); //LD H,D
        case 0x63: return ld_r8_r8(cpu, &cpu->reg.h, &cpu->reg.e); //LD H,E
        case 0x64: return ld_r8_r8(cpu, &cpu->reg.h, &cpu->reg.h); //LD H,H
        case 0x65: return ld_r8_r8(cpu, &cpu->reg.h, &cpu->reg.l); //LD H,L
        case 0x66: return ld_r8_mhl(cpu, &cpu->reg.h); //LD H,(HL)
        case 0x67: return ld_r8_r8(cpu, &cpu->reg.h, &cpu->reg.a); //LD H,A
        case 0x68: return ld_r8_r8(cpu, &cpu->reg.l, &cpu->reg.b); //LD L,B
        case 0x69: return ld_r8_r8(cpu, &cpu->reg.l, &cpu->reg.c); //LD L,C
        case 0x6a: return ld_r8_r8(cpu, &cpu->reg.l, &cpu->reg.d); //LD L,D
        case 0x6b: return ld_r8_r8(cpu, &cpu->reg.l, &cpu->reg.e); //LD L,E
        case 0x6c: return ld_r8_r8(cpu, &cpu->reg.l, &cpu->reg.h); //LD L,H
        case 0x6d: return ld_r8_r8(cpu, &cpu->reg.l, &cpu->reg.l); //LD L,L
        case 0x6e: return ld_r8_mhl(cpu, &cpu->reg.l); //LD L,(HL)
        case 0x6f: return ld_r8_r8(cpu, &cpu->reg.l, &cpu->reg.a); //LD L,A
        case 0x70: return ld_mhl_r8(cpu, &cpu->reg.b); //LD (HL),B
        case 0x71: return ld_mhl_r8(cpu, &cpu->reg.c); //LD (HL),C
        case 0x72: return ld_mhl_r8(cpu, &cpu->reg.d); //LD (HL),D
        case 0x73: return ld_mhl_r8(cpu, &cpu->reg.e); //LD (HL),E
        case 0x74: return ld_mhl_r8(cpu, &cpu->reg.h); //LD (HL),H
        case 0x75: return ld_mhl_r8(cpu, &cpu->reg.l); //LD (HL),L
        case 0x76: return halt(cpu); //HALT
        case 0x77: return ld_mhl_r8(cpu, &cpu->reg.a); //LD (HL),A
        case 0x78: return ld_r8_r8(cpu, &cpu->reg.a, &cpu->reg.b); //LD A,B
        case 0x79: return ld_r8_r8(cpu, &cpu->reg.a, &cpu->reg.c); //LD A,C
        case 0x7a: return ld_r8_r8(cpu, &cpu->reg.a, &cpu->reg.d); //LD A,D
        case 0x7b: return ld_r8_r8(cpu, &cpu->reg.a, &cpu->reg.e); //LD A,E
        case 0x7c: return ld_r8_r8(cpu, &cpu->reg.a, &cpu->reg.h); //LD A,H
        case 0x7d: return ld_r8_r8(cpu, &cpu->reg.a, &cpu->reg.l); //LD A,L
        case 0x7e: return ld_r8_mhl(cpu, &cpu->reg.a); //LD A,(HL)
        case 0x7f: return ld_r8_r8(cpu, &cpu->reg.a, &cpu->reg.a); //LD A,A
        case 0x80: return add_a_r8(cpu, &cpu->reg.b); //ADD A,B
        case 0x81: return add_a_r8(cpu, &cpu->reg.c); //ADD A,C
        case 0x82: return add_a_r8(cpu, &cpu->reg.d); //ADD A,D
        case 0x83: return add_a_r8(cpu, &cpu->reg.e); //ADD A,E
        case 0x84: return add_a_r8(cpu, &cpu->reg.h); //ADD A,H
        case 0x85: return add_a_r8(cpu, &cpu->reg.l); //ADD A,L
        case 0x86: return add_a_mhl(cpu); //ADD A,(HL)
        case 0x87: return add_a_r8(cpu, &cpu->reg.a); //ADD A,A
        case 0x88: return adc_a_r8(cpu, &cpu->reg.b); //ADC A,B
        case 0x89: return adc_a_r8(cpu, &cpu->reg.c); //ADC A,C
        case 0x8a: return adc_a_r8(cpu, &cpu->reg.d); //ADC A,D
        case 0x8b: return adc_a_r8(cpu, &cpu->reg.e); //ADC A,E
        case 0x8c: return adc_a_r8(cpu, &cpu->reg.h); //ADC A,H
        case 0x8d: return adc_a_r8(cpu, &cpu->reg.l); //ADC A,L
        case 0x8e: return adc_a_mhl(cpu); //ADC A,(HL)
        case 0x8f: return adc_a_r8(cpu, &cpu->reg.a); //ADC A,A
        case 0x90: return sub_a_r8(cpu, &cpu->reg.b); //SUB A,B
        case 0x91: return sub_a_r8(cpu, &cpu->reg.c); //SUB A,C
        case 0x92: return sub_a_r8(cpu, &cpu->reg.d); //SUB A,D
        case 0x93: return sub_a_r8(cpu, &cpu->reg.e); //SUB A,E
        case 0x94: return sub_a_r8(cpu, &cpu->reg.h); //SUB A,H
        case 0x95: return sub_a_r8(cpu, &cpu->reg.l); //SUB A,L
        case 0x96: return sub_a_mhl(cpu); //SUB A,(HL)
        case 0x97: return sub_a_r8(cpu, &cpu->reg.a); //SUB A,A
        case 0x98: return sbc_a_r8(cpu, &cpu->reg.b); //SBC A,B
        case 0x99: return sbc_a_r8(cpu, &cpu->reg.c); //SBC A,C
        case 0x9a: return sbc_a_r8(cpu, &cpu->reg.d); //SBC A,D
        case 0x9b: return sbc_a_r8(cpu, &cpu->reg.e); //SBC A,E
        case 0x9c: return sbc_a_r8(cpu, &cpu->reg.h); //SBC A,H
        case 0x9d: return sbc_a_r8(cpu, &cpu->reg.l); //SBC A,L
        case 0x9e: return sbc_a_mhl(cpu); //SBC A,(HL)
        case 0x9f: return sbc_a_r8(cpu, &cpu->reg.a); //SBC A,A
        case 0xa0: return and_a_r8(cpu, &cpu->reg.b); //AND A,B
        case 0xa1: return and_a_r8(cpu, &cpu->reg.c); //AND A,C
        case 0xa2: return and_a_r8(cpu, &cpu->reg.d); //AND A,D
        case 0xa3: return and_a_r8(cpu, &cpu->reg.e); //AND A,E
        case 0xa4: return and_a_r8(cpu, &cpu->reg.h); //AND A,H
        case 0xa5: return and_a_r8(cpu, &cpu->reg.l); //AND A,L
        case 0xa6: return and_a_mhl(cpu); //AND A,(HL)
        case 0xa7: return and_a_r8(cpu, &cpu->reg.a); //AND A,A
        case 0xa8: return xor_a_r8(cpu, &cpu->reg.b); //XOR A,B
        case 0xa9: return xor_a_r8(cpu, &cpu->reg.c); //XOR A,C
        case 0xaa: return xor_a_r8(cpu, &cpu->reg.d); //XOR A,D
        case 0xab: return xor_a_r8(cpu, &cpu->reg.e); //XOR A,E
        case 0xac: return xor_a_r8(cpu, &cpu->reg.h); //XOR A,H
        case 0xad: return xor_a_r8(cpu, &cpu->reg.l); //XOR A,L
        case 0xae: return xor_a_mhl(cpu); //XOR A,(HL)
        case 0xaf: return xor_a_r8(cpu, &cpu->reg.a); //XOR A,A
        case 0xb0: return or_a_r8(cpu, &cpu->reg.b); //OR A,B
        case 0xb1: return or_a_r8(cpu, &cpu->reg.c); //OR A,C
        case 0xb2: return or_a_r8(cpu, &cpu->reg.d); //OR A,D
        case 0xb3: return or_a_r8(cpu, &cpu->reg.e); //OR A,E
        case 0xb4: return or_a_r8(cpu, &cpu->reg.h); //OR A,H
        case 0xb5: return or_a_r8(cpu, &cpu->reg.l); //OR A,L
        case 0xb6: return or_a_mhl(cpu); //OR A,(HL)
        case 0xb7: return or_a_r8(cpu, &cpu->reg.a); //OR A,A
        case 0xb8: return cp_a_r8(cpu, &cpu->reg.b); //CP A,B
        case 0xb9: return cp_a_r8(cpu, &cpu->reg.c); //CP A,C
        case 0xba: return cp_a_r8(cpu, &cpu->reg.d); //CP A,D
        case 0xbb: return cp_a_r8(cpu, &cpu->reg.e); //CP A,E
        case 0xbc: return cp_a_r8(cpu, &cpu->reg.h); //CP A,H
        case 0xbd: return cp_a_r8(cpu, &cpu->reg.l); //CP A,L
        case 0xbe: return cp_a_mhl(cpu); //CP A,(HL)
        case 0xbf: return cp_a_r8(cpu, &cpu->reg.a); //CP A,A
        case 0xc0: return ret_cc(cpu, NZ); //RET NZ
        case 0xc1: return pop_r16(cpu, &cpu->reg.bc); //POP BC
        case 0xc2: return jp_cc_n16(cpu, NZ); //JP NZ,u16
        case 0xc3: return jp_n16(cpu); //JP u16
        case 0xc4: return call_cc_n16(cpu, NZ); //CALL NZ,u16
        case 0xc5: return push_r16(cpu, &cpu->reg.bc); //PUSH BC
        case 0xc6: return add_a_n8(cpu); //ADD A,u8
        case 0xc7: return rst_vec(cpu, 0x00); //RST 00h
        case 0xc8: return ret_cc(cpu, Z); //RET Z
        case 0xc9: return ret(cpu); //RET
        case 0xca: return jp_cc_n16(cpu, Z); //JP Z,u16
        case 0xcb:  //PREFIX CB; handled by caller
        case 0xcc: return call_cc_n16(cpu, Z); //CALL Z,u16
        case 0xcd: return call_n16(cpu); //CALL u16
        case 0xce: return adc_a_n8(cpu); //ADC A,u8
        case 0xcf: return rst_vec(cpu, 0x08); //RST 08h
        case 0xd0: return ret_cc(cpu, NC); //RET NC
        case 0xd1: return pop_r16(cpu, &cpu->reg.de); //POP DE
        case 0xd2: return jp_cc_n16(cpu, NC); //JP NC,u16
        case 0xd3:  //UNUSED
        case 0xd4: return call_cc_n16(cpu, NC); //CALL NC,u16
        case 0xd5: return push_r16(cpu, &cpu->reg.de); //PUSH DE
        case 0xd6: return sub_a_n8(cpu); //SUB A,u8
        case 0xd7: return rst_vec(cpu, 0x10); //RST 10h
        case 0xd8: return ret_cc(cpu, C); //RET C
        case 0xd9: return reti(cpu); //RETI
        case 0xda: return jp_cc_n16(cpu, C); //JP C,u16
        case 0xdb:  //UNUSED
        case 0xdc: return call_cc_n16(cpu, C); //CALL C,u16
        case 0xdd:  //UNUSED
        case 0xde: return sbc_a_n8(cpu); //SBC A,u8
        case 0xdf: return rst_vec(cpu, 0x18); //RST 18h
        case 0xe0: return ldh_mn16_a(cpu); //LD (FF00+u8),A
        case 0xe1: return pop_r16(cpu, &cpu->reg.hl); //POP HL
        case 0xe2: return ldh_mc_a(cpu); //LD (FF00+C),A
        case 0xe3:  //UNUSED
        case 0xe4:  //UNUSED
        case 0xe5: return push_r16(cpu, &cpu->reg.hl); //PUSH HL
        case 0xe6: return and_a_n8(cpu); //AND A,u8
        case 0xe7: return rst_vec(cpu, 0x20); //RST 20h
        case 0xe8: return add_sp_e8(cpu); //ADD SP,i8
        case 0xe9: return jp_hl(cpu); //JP HL
        case 0xea: return ld_mn16_a(cpu); //LD (u16),A
        case 0xeb:  //UNUSED
        case 0xec:  //UNUSED
        case 0xed:  //UNUSED
        case 0xee: return xor_a_n8(cpu); //XOR A,u8
        case 0xef: return rst_vec(cpu, 0x28); //RST 28h
        case 0xf0: return ld_a_mn16(cpu); //LD A,(FF00+u8)
        case 0xf1: return pop_r16(cpu, &cpu->reg.af); //POP AF
        case 0xf2: return ldh_a_mc(cpu); //LD A,(FF00+C)
        case 0xf3: return di(cpu); //DI
        case 0xf4:  //UNUSED
        case 0xf5: return push_r16(cpu, &cpu->reg.af); //PUSH AF
        case 0xf6: return or_a_n8(cpu); //OR A,u8
        case 0xf7: return rst_vec(cpu, 0x30); //RST 30h
        case 0xf8: return ld_hl_sp_e8(cpu); //LD HL,SP+i8
        case 0xf9: return ld_sp_hl(cpu); //LD SP,HL
        case 0xfa: return ld_a_mn16(cpu); //LD A,(u16)
        case 0xfb: return ei(cpu); //EI
        case 0xfc:  //UNUSED
        case 0xfd:  //UNUSED
        case 0xfe: return cp_a_n8(cpu); //CP A,u8
        case 0xff: return rst_vec(cpu, 0x38); //RST 
    }
    return 0;
}

int execute_cb_prefix(Cpu* cpu, uint8_t instruction){
    switch (instruction){
        case 0x0: return rlc_r8(cpu, &cpu->reg.b); //RLC B
        case 0x1: return rlc_r8(cpu, &cpu->reg.c); //RLC C
        case 0x2: return rlc_r8(cpu, &cpu->reg.d); //RLC D
        case 0x3: return rlc_r8(cpu, &cpu->reg.e); //RLC E
        case 0x4: return rlc_r8(cpu, &cpu->reg.h); //RLC H
        case 0x5: return rlc_r8(cpu, &cpu->reg.l); //RLC L
        case 0x6: return rlc_mhl(cpu); //RLC (HL)
        case 0x7: return rlc_r8(cpu, &cpu->reg.a); //RLC A
        case 0x8: return rrc_r8(cpu, &cpu->reg.b); //RRC B
        case 0x9: return rrc_r8(cpu, &cpu->reg.c); //RRC C
        case 0xa: return rrc_r8(cpu, &cpu->reg.d); //RRC D
        case 0xb: return rrc_r8(cpu, &cpu->reg.e); //RRC E
        case 0xc: return rrc_r8(cpu, &cpu->reg.h); //RRC H
        case 0xd: return rrc_r8(cpu, &cpu->reg.l); //RRC L
        case 0xe: return rrc_mhl(cpu); //RRC (HL)
        case 0xf: return rrc_r8(cpu, &cpu->reg.a); //RRC A
        case 0x10: return rl_r8(cpu, &cpu->reg.b); //RL B
        case 0x11: return rl_r8(cpu, &cpu->reg.c); //RL C
        case 0x12: return rl_r8(cpu, &cpu->reg.d); //RL D
        case 0x13: return rl_r8(cpu, &cpu->reg.e); //RL E
        case 0x14: return rl_r8(cpu, &cpu->reg.h); //RL H
        case 0x15: return rl_r8(cpu, &cpu->reg.l); //RL L
        case 0x16: return rl_mhl(cpu); //RL (HL)
        case 0x17: return rl_r8(cpu, &cpu->reg.a); //RL A
        case 0x18: return rr_r8(cpu, &cpu->reg.b); //RR B
        case 0x19: return rr_r8(cpu, &cpu->reg.c); //RR C
        case 0x1a: return rr_r8(cpu, &cpu->reg.d); //RR D
        case 0x1b: return rr_r8(cpu, &cpu->reg.e); //RR E
        case 0x1c: return rr_r8(cpu, &cpu->reg.h); //RR H
        case 0x1d: return rr_r8(cpu, &cpu->reg.l); //RR L
        case 0x1e: return rr_mhl(cpu); //RR (HL)
        case 0x1f: return rr_r8(cpu, &cpu->reg.a); //RR A
        case 0x20: return sla_r8(cpu, &cpu->reg.b); //SLA B
        case 0x21: return sla_r8(cpu, &cpu->reg.c);  //SLA C
        case 0x22: return sla_r8(cpu, &cpu->reg.d);  //SLA D
        case 0x23: return sla_r8(cpu, &cpu->reg.e);  //SLA E
        case 0x24: return sla_r8(cpu, &cpu->reg.h);  //SLA H
        case 0x25: return sla_r8(cpu, &cpu->reg.l);  //SLA L
        case 0x26: return sla_mhl(cpu); //SLA (HL)
        case 0x27: return sla_r8(cpu, &cpu->reg.a);  //SLA A
        case 0x28: return sra_r8(cpu, &cpu->reg.b); //SRA B
        case 0x29: return sra_r8(cpu, &cpu->reg.c); //SRA C
        case 0x2a: return sra_r8(cpu, &cpu->reg.d); //SRA D
        case 0x2b: return sra_r8(cpu, &cpu->reg.e); //SRA E
        case 0x2c: return sra_r8(cpu, &cpu->reg.h); //SRA H
        case 0x2d: return sra_r8(cpu, &cpu->reg.l); //SRA L
        case 0x2e: return sra_mhl(cpu); //SRA (HL)
        case 0x2f: return sra_r8(cpu, &cpu->reg.a); //SRA A
        case 0x30: return swap_r8(cpu, &cpu->reg.b); //SWAP B
        case 0x31: return swap_r8(cpu, &cpu->reg.c); //SWAP C
        case 0x32: return swap_r8(cpu, &cpu->reg.d); //SWAP D
        case 0x33: return swap_r8(cpu, &cpu->reg.e); //SWAP E
        case 0x34: return swap_r8(cpu, &cpu->reg.h); //SWAP H
        case 0x35: return swap_r8(cpu, &cpu->reg.l); //SWAP L
        case 0x36: return swap_mhl(cpu); //SWAP (HL)
        case 0x37: return swap_r8(cpu, &cpu->reg.a); //SWAP A
        case 0x38: return srl_r8(cpu, &cpu->reg.b); //SRL B
        case 0x39: return srl_r8(cpu, &cpu->reg.c); //SRL C
        case 0x3a: return srl_r8(cpu, &cpu->reg.d); //SRL D
        case 0x3b: return srl_r8(cpu, &cpu->reg.e); //SRL E
        case 0x3c: return srl_r8(cpu, &cpu->reg.h); //SRL H
        case 0x3d: return srl_r8(cpu, &cpu->reg.l); //SRL L
        case 0x3e: return srl_mhl(cpu); //SRL (HL)
        case 0x3f: return srl_r8(cpu, &cpu->reg.a); //SRL A
        case 0x40: return bit_u3_r8(cpu, 0, &cpu->reg.b); //BIT 0,B
        case 0x41: return bit_u3_r8(cpu, 0, &cpu->reg.c); //BIT 0,C
        case 0x42: return bit_u3_r8(cpu, 0, &cpu->reg.d); //BIT 0,D
        case 0x43: return bit_u3_r8(cpu, 0, &cpu->reg.e); //BIT 0,E
        case 0x44: return bit_u3_r8(cpu, 0, &cpu->reg.h); //BIT 0,H
        case 0x45: return bit_u3_r8(cpu, 0, &cpu->reg.l); //BIT 0,L
        case 0x46: return bit_u3_mhl(cpu, 0); //BIT 0,(HL)
        case 0x47: return bit_u3_r8(cpu, 0, &cpu->reg.a); //BIT 0,A
        case 0x48: return bit_u3_r8(cpu, 1, &cpu->reg.b); //BIT 1,B
        case 0x49: return bit_u3_r8(cpu, 1, &cpu->reg.c); //BIT 1,C
        case 0x4a: return bit_u3_r8(cpu, 1, &cpu->reg.d); //BIT 1,D
        case 0x4b: return bit_u3_r8(cpu, 1, &cpu->reg.e); //BIT 1,E
        case 0x4c: return bit_u3_r8(cpu, 1, &cpu->reg.h); //BIT 1,H
        case 0x4d: return bit_u3_r8(cpu, 1, &cpu->reg.l); //BIT 1,L
        case 0x4e: return bit_u3_mhl(cpu, 1); //BIT 1,(HL)
        case 0x4f: return bit_u3_r8(cpu, 1, &cpu->reg.a); //BIT 1,A
        case 0x50: return bit_u3_r8(cpu, 2, &cpu->reg.b); //BIT 2,B
        case 0x51: return bit_u3_r8(cpu, 2, &cpu->reg.c); //BIT 2,C
        case 0x52: return bit_u3_r8(cpu, 2, &cpu->reg.d); //BIT 2,D
        case 0x53: return bit_u3_r8(cpu, 2, &cpu->reg.e); //BIT 2,E
        case 0x54: return bit_u3_r8(cpu, 2, &cpu->reg.h); //BIT 2,H
        case 0x55: return bit_u3_r8(cpu, 2, &cpu->reg.l); //BIT 2,L
        case 0x56: return bit_u3_mhl(cpu, 2); //BIT 2,(HL)
        case 0x57: return bit_u3_r8(cpu, 2, &cpu->reg.a); //BIT 2,A
        case 0x58: return bit_u3_r8(cpu, 3, &cpu->reg.b); //BIT 3,B
        case 0x59: return bit_u3_r8(cpu, 3, &cpu->reg.c); //BIT 3,C
        case 0x5a: return bit_u3_r8(cpu, 3, &cpu->reg.d); //BIT 3,D
        case 0x5b: return bit_u3_r8(cpu, 3, &cpu->reg.e); //BIT 3,E
        case 0x5c: return bit_u3_r8(cpu, 3, &cpu->reg.h); //BIT 3,H
        case 0x5d: return bit_u3_r8(cpu, 3, &cpu->reg.l); //BIT 3,L
        case 0x5e: return bit_u3_mhl(cpu, 3); //BIT 3,(HL)
        case 0x5f: return bit_u3_r8(cpu, 3, &cpu->reg.a); //BIT 3,A
        case 0x60: return bit_u3_r8(cpu, 4, &cpu->reg.b); //BIT 4,B
        case 0x61: return bit_u3_r8(cpu, 4, &cpu->reg.c);  //BIT 4,C
        case 0x62: return bit_u3_r8(cpu, 4, &cpu->reg.d);  //BIT 4,D
        case 0x63: return bit_u3_r8(cpu, 4, &cpu->reg.e);  //BIT 4,E
        case 0x64: return bit_u3_r8(cpu, 4, &cpu->reg.h);  //BIT 4,H
        case 0x65: return bit_u3_r8(cpu, 4, &cpu->reg.l);  //BIT 4,L
        case 0x66: return bit_u3_mhl(cpu, 4); //BIT 4,(HL)
        case 0x67: return bit_u3_r8(cpu, 4, &cpu->reg.a);  //BIT 4,A
        case 0x68: return bit_u3_r8(cpu, 5, &cpu->reg.b); //BIT 5,B
        case 0x69: return bit_u3_r8(cpu, 5, &cpu->reg.c); //BIT 5,C
        case 0x6a: return bit_u3_r8(cpu, 5, &cpu->reg.d); //BIT 5,D
        case 0x6b: return bit_u3_r8(cpu, 5, &cpu->reg.e); //BIT 5,E
        case 0x6c: return bit_u3_r8(cpu, 5, &cpu->reg.h); //BIT 5,H
        case 0x6d: return bit_u3_r8(cpu, 5, &cpu->reg.l); //BIT 5,L
        case 0x6e: return bit_u3_mhl(cpu, 5); //BIT 5,(HL)
        case 0x6f: return bit_u3_r8(cpu, 5, &cpu->reg.a); //BIT 5,A
        case 0x70: return bit_u3_r8(cpu, 6, &cpu->reg.b); //BIT 6,B
        case 0x71: return bit_u3_r8(cpu, 6, &cpu->reg.c); //BIT 6,C
        case 0x72: return bit_u3_r8(cpu, 6, &cpu->reg.d); //BIT 6,D
        case 0x73: return bit_u3_r8(cpu, 6, &cpu->reg.e); //BIT 6,E
        case 0x74: return bit_u3_r8(cpu, 6, &cpu->reg.h); //BIT 6,H
        case 0x75: return bit_u3_r8(cpu, 6, &cpu->reg.l); //BIT 6,L
        case 0x76: return bit_u3_mhl(cpu, 6); //BIT 6,(HL)
        case 0x77: return bit_u3_r8(cpu, 6, &cpu->reg.a); //BIT 6,A
        case 0x78: return bit_u3_r8(cpu, 7, &cpu->reg.b); //BIT 7,B
        case 0x79: return bit_u3_r8(cpu, 7, &cpu->reg.c); //BIT 7,C
        case 0x7a: return bit_u3_r8(cpu, 7, &cpu->reg.d); //BIT 7,D
        case 0x7b: return bit_u3_r8(cpu, 7, &cpu->reg.e); //BIT 7,E
        case 0x7c: return bit_u3_r8(cpu, 7, &cpu->reg.h); //BIT 7,H
        case 0x7d: return bit_u3_r8(cpu, 7, &cpu->reg.l); //BIT 7,L
        case 0x7e: return bit_u3_mhl(cpu, 7); //BIT 7,(HL)
        case 0x7f: return bit_u3_r8(cpu, 7, &cpu->reg.a); //BIT 7,A
        case 0x80: return res_u3_r8(cpu, 0, &cpu->reg.b); //RES 0,B
        case 0x81: return res_u3_r8(cpu, 0, &cpu->reg.c); //RES 0,C
        case 0x82: return res_u3_r8(cpu, 0, &cpu->reg.d); //RES 0,D
        case 0x83: return res_u3_r8(cpu, 0, &cpu->reg.e); //RES 0,E
        case 0x84: return res_u3_r8(cpu, 0, &cpu->reg.h); //RES 0,H
        case 0x85: return res_u3_r8(cpu, 0, &cpu->reg.l); //RES 0,L
        case 0x86: return res_u3_mhl(cpu, 0); //RES 0,(HL)
        case 0x87: return res_u3_r8(cpu, 0, &cpu->reg.a); //RES 0,A
        case 0x88: return res_u3_r8(cpu, 1, &cpu->reg.b); //RES 1,B
        case 0x89: return res_u3_r8(cpu, 1, &cpu->reg.c); //RES 1,C
        case 0x8a: return res_u3_r8(cpu, 1, &cpu->reg.d); //RES 1,D
        case 0x8b: return res_u3_r8(cpu, 1, &cpu->reg.e); //RES 1,E
        case 0x8c: return res_u3_r8(cpu, 1, &cpu->reg.h); //RES 1,H
        case 0x8d: return res_u3_r8(cpu, 1, &cpu->reg.l); //RES 1,L
        case 0x8e: return res_u3_mhl(cpu, 1); //RES 1,(HL)
        case 0x8f: return res_u3_r8(cpu, 1, &cpu->reg.a); //RES 1,A
        case 0x90: return res_u3_r8(cpu, 2, &cpu->reg.b); //RES 2,B
        case 0x91: return res_u3_r8(cpu, 2, &cpu->reg.c); //RES 2,C
        case 0x92: return res_u3_r8(cpu, 2, &cpu->reg.d); //RES 2,D
        case 0x93: return res_u3_r8(cpu, 2, &cpu->reg.e); //RES 2,E
        case 0x94: return res_u3_r8(cpu, 2, &cpu->reg.h); //RES 2,H
        case 0x95: return res_u3_r8(cpu, 2, &cpu->reg.l); //RES 2,L
        case 0x96: return res_u3_mhl(cpu, 2); //RES 2,(HL)
        case 0x97: return res_u3_r8(cpu, 2, &cpu->reg.a); //RES 2,A
        case 0x98: return res_u3_r8(cpu, 3, &cpu->reg.b); //RES 3,B
        case 0x99: return res_u3_r8(cpu, 3, &cpu->reg.c); //RES 3,C
        case 0x9a: return res_u3_r8(cpu, 3, &cpu->reg.d); //RES 3,D
        case 0x9b: return res_u3_r8(cpu, 3, &cpu->reg.e); //RES 3,E
        case 0x9c: return res_u3_r8(cpu, 3, &cpu->reg.h); //RES 3,H
        case 0x9d: return res_u3_r8(cpu, 3, &cpu->reg.l); //RES 3,L
        case 0x9e: return res_u3_mhl(cpu, 3); //RES 3,(HL)
        case 0x9f: return res_u3_r8(cpu, 3, &cpu->reg.a); //RES 3,A
        case 0xa0: return res_u3_r8(cpu, 4, &cpu->reg.b); //RES 4,B
        case 0xa1: return res_u3_r8(cpu, 4, &cpu->reg.c); //RES 4,C
        case 0xa2: return res_u3_r8(cpu, 4, &cpu->reg.d); //RES 4,D
        case 0xa3: return res_u3_r8(cpu, 4, &cpu->reg.e); //RES 4,E
        case 0xa4: return res_u3_r8(cpu, 4, &cpu->reg.h); //RES 4,H
        case 0xa5: return res_u3_r8(cpu, 4, &cpu->reg.l); //RES 4,L
        case 0xa6: return res_u3_mhl(cpu, 4); //RES 4,(HL)
        case 0xa7: return res_u3_r8(cpu, 4, &cpu->reg.a); //RES 4,A
        case 0xa8: return res_u3_r8(cpu, 5, &cpu->reg.b); //RES 5,B
        case 0xa9: return res_u3_r8(cpu, 5, &cpu->reg.c); //RES 5,C
        case 0xaa: return res_u3_r8(cpu, 5, &cpu->reg.d); //RES 5,D
        case 0xab: return res_u3_r8(cpu, 5, &cpu->reg.e); //RES 5,E
        case 0xac: return res_u3_r8(cpu, 5, &cpu->reg.h); //RES 5,H
        case 0xad: return res_u3_r8(cpu, 5, &cpu->reg.l); //RES 5,L
        case 0xae: return res_u3_mhl(cpu, 5); //RES 5,(HL)
        case 0xaf: return res_u3_r8(cpu, 5, &cpu->reg.a); //RES 5,A
        case 0xb0: return res_u3_r8(cpu, 6, &cpu->reg.b); //RES 6,B        
        case 0xb1: return res_u3_r8(cpu, 6, &cpu->reg.c); //RES 6,C
        case 0xb2: return res_u3_r8(cpu, 6, &cpu->reg.d); //RES 6,D
        case 0xb3: return res_u3_r8(cpu, 6, &cpu->reg.e); //RES 6,E
        case 0xb4: return res_u3_r8(cpu, 6, &cpu->reg.h); //RES 6,H
        case 0xb5: return res_u3_r8(cpu, 6, &cpu->reg.l); //RES 6,L
        case 0xb6: return res_u3_mhl(cpu, 6); //RES 6,(HL)
        case 0xb7: return res_u3_r8(cpu, 6, &cpu->reg.a); //RES 6,A
        case 0xb8: return res_u3_r8(cpu, 7, &cpu->reg.b); //RES 7,B
        case 0xb9: return res_u3_r8(cpu, 7, &cpu->reg.c); //RES 7,C
        case 0xba: return res_u3_r8(cpu, 7, &cpu->reg.d); //RES 7,D
        case 0xbb: return res_u3_r8(cpu, 7, &cpu->reg.e); //RES 7,E
        case 0xbc: return res_u3_r8(cpu, 7, &cpu->reg.h); //RES 7,H
        case 0xbd: return res_u3_r8(cpu, 7, &cpu->reg.l); //RES 7,L
        case 0xbe: return res_u3_mhl(cpu, 7); //RES 7,(HL)
        case 0xbf: return res_u3_r8(cpu, 7, &cpu->reg.a); //RES 7,A
        case 0xc0: return set_u3_r8(cpu, 0, &cpu->reg.b); //SET 0,B
        case 0xc1: return set_u3_r8(cpu, 0, &cpu->reg.c); //SET 0,C
        case 0xc2: return set_u3_r8(cpu, 0, &cpu->reg.d); //SET 0,D
        case 0xc3: return set_u3_r8(cpu, 0, &cpu->reg.e); //SET 0,E
        case 0xc4: return set_u3_r8(cpu, 0, &cpu->reg.h); //SET 0,H
        case 0xc5: return set_u3_r8(cpu, 0, &cpu->reg.l); //SET 0,L
        case 0xc6: return set_u3_mhl(cpu, 0); //SET 0,(HL)
        case 0xc7: return set_u3_r8(cpu, 0, &cpu->reg.a); //SET 0,A
        case 0xc8: return set_u3_r8(cpu, 1, &cpu->reg.b); //SET 1,B     
        case 0xc9: return set_u3_r8(cpu, 1, &cpu->reg.c); //SET 1,C
        case 0xca: return set_u3_r8(cpu, 1, &cpu->reg.d); //SET 1,D
        case 0xcb: return set_u3_r8(cpu, 1, &cpu->reg.e); //SET 1,E
        case 0xcc: return set_u3_r8(cpu, 1, &cpu->reg.h); //SET 1,H
        case 0xcd: return set_u3_r8(cpu, 1, &cpu->reg.l); //SET 1,L
        case 0xce: return set_u3_mhl(cpu, 1); //SET 1,(HL)
        case 0xcf: return set_u3_r8(cpu, 1, &cpu->reg.a); //SET 1,A
        case 0xd0: return set_u3_r8(cpu, 2, &cpu->reg.b); //SET 2,B      
        case 0xd1: return set_u3_r8(cpu, 2, &cpu->reg.c); //SET 2,C
        case 0xd2: return set_u3_r8(cpu, 2, &cpu->reg.d); //SET 2,D
        case 0xd3: return set_u3_r8(cpu, 2, &cpu->reg.e); //SET 2,E
        case 0xd4: return set_u3_r8(cpu, 2, &cpu->reg.h); //SET 2,H
        case 0xd5: return set_u3_r8(cpu, 2, &cpu->reg.l); //SET 2,L
        case 0xd6: return set_u3_mhl(cpu, 2); //SET 2,(HL)
        case 0xd7: return set_u3_r8(cpu, 2, &cpu->reg.a); //SET 2,A
        case 0xd8: return set_u3_r8(cpu, 3, &cpu->reg.b); //SET 3,B      
        case 0xd9: return set_u3_r8(cpu, 3, &cpu->reg.c); //SET 3,C
        case 0xda: return set_u3_r8(cpu, 3, &cpu->reg.d); //SET 3,D
        case 0xdb: return set_u3_r8(cpu, 3, &cpu->reg.e); //SET 3,E
        case 0xdc: return set_u3_r8(cpu, 3, &cpu->reg.h); //SET 3,H
        case 0xdd: return set_u3_r8(cpu, 3, &cpu->reg.l); //SET 3,L
        case 0xde: return set_u3_mhl(cpu, 3); //SET 3,(HL)
        case 0xdf: return set_u3_r8(cpu, 3, &cpu->reg.a); //SET 3,A
        case 0xe0: return set_u3_r8(cpu, 4, &cpu->reg.b); //SET 4,B
        case 0xe1: return set_u3_r8(cpu, 4, &cpu->reg.c); //SET 4,C
        case 0xe2: return set_u3_r8(cpu, 4, &cpu->reg.d); //SET 4,D
        case 0xe3: return set_u3_r8(cpu, 4, &cpu->reg.e); //SET 4,E
        case 0xe4: return set_u3_r8(cpu, 4, &cpu->reg.h); //SET 4,H
        case 0xe5: return set_u3_r8(cpu, 4, &cpu->reg.l); //SET 4,L
        case 0xe6: return set_u3_mhl(cpu, 4); //SET 4,(HL)
        case 0xe7: return set_u3_r8(cpu, 4, &cpu->reg.a); //SET 4,A
        case 0xe8: return set_u3_r8(cpu, 5, &cpu->reg.b); //SET 5,B
        case 0xe9: return set_u3_r8(cpu, 5, &cpu->reg.c); //SET 5,C
        case 0xea: return set_u3_r8(cpu, 5, &cpu->reg.d); //SET 5,D
        case 0xeb: return set_u3_r8(cpu, 5, &cpu->reg.e); //SET 5,E
        case 0xec: return set_u3_r8(cpu, 5, &cpu->reg.h); //SET 5,H
        case 0xed: return set_u3_r8(cpu, 5, &cpu->reg.l); //SET 5,L
        case 0xee: return set_u3_mhl(cpu, 5); //SET 5,(HL)
        case 0xef: return set_u3_r8(cpu, 5, &cpu->reg.a); //SET 5,A
        case 0xf0: return set_u3_r8(cpu, 6, &cpu->reg.b); //SET 6,B
        case 0xf1: return set_u3_r8(cpu, 6, &cpu->reg.c); //SET 6,C
        case 0xf2: return set_u3_r8(cpu, 6, &cpu->reg.d); //SET 6,D
        case 0xf3: return set_u3_r8(cpu, 6, &cpu->reg.e); //SET 6,E
        case 0xf4: return set_u3_r8(cpu, 6, &cpu->reg.h); //SET 6,H
        case 0xf5: return set_u3_r8(cpu, 6, &cpu->reg.l); //SET 6,L
        case 0xf6: return set_u3_mhl(cpu, 6); //SET 6,(HL)
        case 0xf7: return set_u3_r8(cpu, 6, &cpu->reg.a); //SET 6,A
        case 0xf8: return set_u3_r8(cpu, 7, &cpu->reg.b); //SET 7,B
        case 0xf9: return set_u3_r8(cpu, 7, &cpu->reg.c); //SET 7,C
        case 0xfa: return set_u3_r8(cpu, 7, &cpu->reg.d); //SET 7,D
        case 0xfb: return set_u3_r8(cpu, 7, &cpu->reg.e); //SET 7,E
        case 0xfc: return set_u3_r8(cpu, 7, &cpu->reg.h); //SET 7,H
        case 0xfd: return set_u3_r8(cpu, 7, &cpu->reg.l); //SET 7,L
        case 0xfe: return set_u3_mhl(cpu, 7); //SET 7,(HL)
        case 0xff: return set_u3_r8(cpu, 7, &cpu->reg.a); //SET 7,A
    }
    return 0;
}

int tick(Cpu* cpu){
    // Fetch
    uint8_t instruction = fetch_and_increment_pc(cpu);

    // Check for CB-prefixed instructions
    if (instruction == 0xCB) {
        // This is a CB-prefixed instruction! 
        // Have to read the next one
        uint8_t cb_instr = fetch_and_increment_pc(cpu);
        return execute_cb_prefix(cpu, cb_instr);
    } else {
        // Regular instruction
        return execute_normal(cpu, instruction);
    }
}




