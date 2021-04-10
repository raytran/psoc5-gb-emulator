#include "memory.h"
#include "rom.h"
#include "registers.h"
#include "cpu.h"

int8_t safe_convert(uint8_t x) {
    return x < 128 ? x : x - 256;
}

void reset_cpu(Cpu* cpu) {
    cpu->inBios = true;
    reset_memory(&cpu->mem);
    reset_registers(&cpu->reg);
}

static void increment_pc(Cpu* cpu){
    cpu->reg.pc++;
    if (cpu->reg.pc == BIOS_SIZE && cpu->inBios) {
        cpu->inBios = false;
    }
}

static void add_to_pc(Cpu* cpu, int8_t offset){
    if (offset < 0){
        cpu->reg.pc -= -offset;
    }else{
        cpu->reg.pc += offset;
    }
}

static uint8_t fetch_and_increment_pc(Cpu* cpu){
    uint8_t data = fetch(&cpu->mem, cpu->reg.pc, cpu->inBios);
    increment_pc(cpu);
    return data;
}

static uint16_t fetch_and_increment_pc_twice(Cpu* cpu){
    uint8_t lower = fetch_and_increment_pc(cpu);
    uint8_t upper = fetch_and_increment_pc(cpu);
    return (upper << 8) ^ lower;
}

// Returns the incremented value, adjusting flags if applicable
static inline uint8_t inc_8t(Cpu* cpu, uint8_t value){
    return value++;
}
static inline uint8_t dec_8t(Cpu* cpu, uint8_t value){
    return value--;
}

static inline uint8_t add_8t(Cpu* cpu, uint8_t a, uint8_t b){
    return a+b;
}

static inline uint8_t add_8t_with_carry(Cpu* cpu, uint8_t a, uint8_t b){
    return a + b;
}

static inline uint8_t sub_8t(Cpu* cpu, uint8_t a, uint8_t b){
    return a-b;
}

static inline uint8_t sub_8t_with_carry(Cpu* cpu, uint8_t a, uint8_t b){
    return a - b;
}

static inline uint8_t and_8t(Cpu* cpu, uint8_t a, uint8_t b){
    return a & b;
}

static inline uint8_t xor_8t(Cpu* cpu, uint8_t a, uint8_t b){
    return a ^ b;
}

static inline uint8_t or_8t(Cpu* cpu, uint8_t a, uint8_t b){
    return a | b;
}

static inline uint16_t add_16t(Cpu* cpu, uint16_t a, uint16_t b){
    return a + b;
}

// Assumes that the pc is already incremented to point to the next instr
int execute_normal(Cpu* cpu, uint8_t instruction){
    int machine_cycles_taken = 0;
    switch (instruction){
        case 0x00:  // NOP
        machine_cycles_taken = 1;
        break;
        case 0x01:  // LD BC,u16
        machine_cycles_taken = 3;
        cpu->reg.c = fetch_and_increment_pc(cpu);
        cpu->reg.b = fetch_and_increment_pc(cpu);
        break;
        case 0x02:  // LD (BC),A 
        machine_cycles_taken = 2;
        write_mem(&cpu->mem, cpu->reg.bc, cpu->reg.a);
        break;
        case 0x03:  // INC BC
        machine_cycles_taken = 2;
        cpu->reg.bc++;  // note this inc does not set flags
        break;
        case 0x04:  // INC B
        machine_cycles_taken = 1;
        cpu->reg.b = inc_8t(cpu, cpu->reg.b);
        break;
        case 0x05:  // DEC B
        machine_cycles_taken = 1;
        cpu->reg.b = dec_8t(cpu, cpu->reg.b);
        break;
        case 0x06:  // LD B,u8
        machine_cycles_taken = 2;
        cpu->reg.b = fetch_and_increment_pc(cpu);
        break;
        case 0x07:  // RLCA - 0x07
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ TODO
        break;
        case 0x08:  // LD (u16),SP
        machine_cycles_taken = 5;
        uint16_t ldu16sp_addr = fetch_and_increment_pc_twice(cpu);
        write_mem(&cpu->mem, ldu16sp_addr, cpu->reg.sp & 0x00FF);
        write_mem(&cpu->mem, ldu16sp_addr + 1, cpu->reg.sp & 0xFF00);
        break;
        case 0x09: // ADD HL,BC
        machine_cycles_taken = 2;
        cpu->reg.hl = add_16t(cpu, cpu->reg.hl, cpu->reg.bc);
        break;
        case 0x0A: // LD A,(BC)
        machine_cycles_taken = 2;
        cpu->reg.a = fetch(&cpu->mem, cpu->reg.bc, cpu->inBios);
        break;     
        case 0x0B: // DEC BC
        machine_cycles_taken = 2;
        cpu->reg.bc--;  
        break;
        case 0x0C: // INC C
        machine_cycles_taken = 1;
        cpu->reg.c = inc_8t(cpu, cpu->reg.c);
        break;
        case 0x0D: // DEC C
        machine_cycles_taken = 1;
        cpu->reg.c = dec_8t(cpu, cpu->reg.c);
        break;
        case 0x0E: // LD C,u8
        machine_cycles_taken = 2;
        cpu->reg.c = fetch_and_increment_pc(cpu);
        break;
        case 0x0F: //RRCA 
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;   
        case 0x10: //STOP
        machine_cycles_taken = 1;
        //don't actually stop..
        break;
        case 0x11: //LD DE,u16
        machine_cycles_taken = 3;
        cpu->reg.e = fetch_and_increment_pc(cpu);
        cpu->reg.d = fetch_and_increment_pc(cpu); 
        break;
        case 0x12:  //LD (DE),A
        machine_cycles_taken = 2;
        write_mem(&cpu->mem, cpu->reg.de, cpu->reg.a);
        break;
        case 0x13:  //INC DE
        machine_cycles_taken = 2;
        cpu->reg.de++;
        break;
        case 0x14:  //INC D
        machine_cycles_taken = 1;
        cpu->reg.d = inc_8t(cpu, cpu->reg.d);
        break;
        case 0x15:  //DEC D
        machine_cycles_taken = 1;
        cpu->reg.d = dec_8t(cpu, cpu->reg.d);
        break;
        case 0x16:  //LD D, u8
        machine_cycles_taken = 2;
        cpu->reg.d = fetch_and_increment_pc(cpu);
        break;
        case 0x17:  //RLA 
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0x18:  //JR i8
        machine_cycles_taken = 3;
        int8_t jri8_offset = safe_convert(fetch_and_increment_pc(cpu));
        add_to_pc(cpu, jri8_offset);
        break;
        case 0x19:  //ADD HL,DE
        machine_cycles_taken = 2;
        cpu->reg.hl = add_16t(cpu, cpu->reg.hl, cpu->reg.de);
        break;
        case 0x1A:  //LD A,(DE)
        machine_cycles_taken = 2;
        cpu->reg.a = fetch(&cpu->mem, cpu->reg.de, cpu->inBios);
        break;
        case 0x1B:  //DEC DE
        machine_cycles_taken = 2;
        cpu->reg.de--;
        break;
        case 0x1C:  //INC E
        machine_cycles_taken = 1;
        cpu->reg.e = inc_8t(cpu, cpu->reg.e);
        break;
        case 0x1D:  //DEC E
        machine_cycles_taken = 1;
        cpu->reg.e = dec_8t(cpu, cpu->reg.e);
        break;
        case 0x1E:  //LD E,u8
        machine_cycles_taken = 2;
        cpu->reg.e = fetch_and_increment_pc(cpu);
        break;
        case 0x1F:  //RRA
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0x20:  //JR NZ,i8
        machine_cycles_taken = 2;
        int8_t jrnz_offset = safe_convert(fetch_and_increment_pc(cpu));
        if (get_zero_flag(&cpu->reg)){
            machine_cycles_taken = 3;
            add_to_pc(cpu, jrnz_offset);
        }
        break;
        case 0x21:  //LD HL,u16
        machine_cycles_taken = 3;
        cpu->reg.l = fetch_and_increment_pc(cpu);
        cpu->reg.h = fetch_and_increment_pc(cpu);
        break;
        case 0x22:  //LD (HL+),A
        machine_cycles_taken = 2;
        write_mem(&cpu->mem, cpu->reg.hl, cpu->reg.a);
        cpu->reg.hl++;
        break;
        case 0x23:  //INC HL
        machine_cycles_taken = 2;
        cpu->reg.hl++;
        break;
        case 0x24:  //INC H
        machine_cycles_taken = 1;
        cpu->reg.h = inc_8t(cpu, cpu->reg.h);
        break;
        case 0x25:  //DEC H
        machine_cycles_taken = 1;
        cpu->reg.h = dec_8t(cpu, cpu->reg.h);
        break;
        case 0x26:  //LD H,u8
        machine_cycles_taken = 2;
        cpu->reg.h = fetch_and_increment_pc(cpu);
        break;
        case 0x27:  //DAA
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0x28:  //JR Z,i8
        machine_cycles_taken = 2;
        int8_t jrzi8_offset = safe_convert(fetch_and_increment_pc(cpu));
        if (get_zero_flag(&cpu->reg)){
            machine_cycles_taken = 3;
            add_to_pc(cpu, jrzi8_offset);
        }
        break;
        case 0x29:  //ADD HL,HL 
        machine_cycles_taken = 2;
        cpu->reg.hl = add_16t(cpu, cpu->reg.hl, cpu->reg.hl);
        break;
        case 0x2A:  //LD A,(HL+)
        machine_cycles_taken = 2;
        cpu->reg.a = fetch(&cpu->mem, cpu->reg.hl, cpu->inBios);
        cpu->reg.hl++;
        break;
        case 0x2B:  //DEC HL
        machine_cycles_taken = 2;
        cpu->reg.hl--;
        break;
        case 0x2C:  //INC L
        machine_cycles_taken = 1;
        cpu->reg.l = inc_8t(cpu, cpu->reg.l);
        break;
        case 0x2D:  //DEC L
        machine_cycles_taken = 1;
        cpu->reg.l = dec_8t(cpu, cpu->reg.l);
        break;
        case 0x2E:  //LD L,u8
        machine_cycles_taken = 2;
        cpu->reg.l = fetch_and_increment_pc(cpu);
        break;
        case 0x2F:  //CPL 
        ///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0x30:  //JR NC,i8
        machine_cycles_taken = 2;
        int8_t jrnci8_offset = safe_convert(fetch_and_increment_pc(cpu));
        if (get_zero_flag(&cpu->reg)){
            machine_cycles_taken = 3;
            add_to_pc(cpu, jrnci8_offset);
        }
        break;
        case 0x31:  // LD SP,u16 
        machine_cycles_taken = 3;
        cpu->reg.sp = fetch_and_increment_pc_twice(cpu);
        break;
        case 0x32:  //LD (HL-),A
        machine_cycles_taken = 2;
        write_mem(&cpu->mem, cpu->reg.hl, cpu->reg.a);
        cpu->reg.hl--;
        break;
        case 0x33:  //INC SP
        machine_cycles_taken = 2;
        cpu->reg.sp++;
        break;
        case 0x34:  //INC (HL)
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0x35:  //DEC (HL)
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0x36:  //LD (HL),u8
        machine_cycles_taken = 3;
        write_mem(&cpu->mem, cpu->reg.hl, fetch_and_increment_pc(cpu));
        break;
        case 0x37:  //SCF 
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0x38:  //JR C,i8
        machine_cycles_taken = 2;
        int8_t jrci8_offset = safe_convert(fetch_and_increment_pc(cpu));
        if (get_carry_flag(&cpu->reg)){
            machine_cycles_taken = 3;
            add_to_pc(cpu, jrci8_offset);
        }
        break;
        case 0x39:  //ADD HL,SP
        machine_cycles_taken = 2;
        cpu->reg.hl = add_16t(cpu, cpu->reg.hl, cpu->reg.sp);
        break;
        case 0x3A:  //LD A,(HL-)
        machine_cycles_taken = 2;
        cpu->reg.a = fetch(&cpu->mem, cpu->reg.hl, cpu->inBios);
        cpu->reg.hl--;
        break;
        case 0x3B:  //DEC SP
        machine_cycles_taken = 2;
        cpu->reg.sp--;
        break;
        case 0x3C:  //INC A
        machine_cycles_taken = 1;
        cpu->reg.a = inc_8t(cpu, cpu->reg.a);
        break;
        case 0x3D:  //DEC A
        machine_cycles_taken = 1;
        cpu->reg.a = dec_8t(cpu, cpu->reg.a);
        break;
        case 0x3E:  //LD A,u8
        machine_cycles_taken = 2;
        cpu->reg.a = fetch_and_increment_pc(cpu);
        break;
        case 0x3F:  //CCF
        ///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0x40:  //LD B,B
        machine_cycles_taken = 1;
        break;
        case 0x41:  //LD B,C
        machine_cycles_taken = 1;
        cpu->reg.b = cpu->reg.c;
        break;
        case 0x42:  //LD B,D 
        machine_cycles_taken = 1;
        cpu->reg.b = cpu->reg.d;
        break;
        case 0x43:  //LD B,E
        machine_cycles_taken = 1;
        cpu->reg.b = cpu->reg.e;
        break;
        case 0x44:  //LD B,H
        machine_cycles_taken = 1;
        cpu->reg.b = cpu->reg.h;
        break;
        case 0x45:  //LD B,L
        machine_cycles_taken = 1;
        cpu->reg.b = cpu->reg.l;
        break;
        case 0x46:  //LD B,(HL)
        machine_cycles_taken = 2;
        cpu->reg.b = fetch(&cpu->mem, cpu->reg.hl, cpu->inBios);
        break;
        case 0x47:  //LD B,A
        machine_cycles_taken = 1;
        cpu->reg.b = cpu->reg.a;
        break;
        case 0x48:  //LD C,B
        machine_cycles_taken = 1;
        cpu->reg.c = cpu->reg.b;
        break;
        case 0x49:  //LD C,C 
        machine_cycles_taken = 1;
        break;
        case 0x4A:  //LD C,D
        machine_cycles_taken = 1;
        cpu->reg.c = cpu->reg.d;
        break;
        case 0x4B:  //LD C,E
        machine_cycles_taken = 1;
        cpu->reg.c = cpu->reg.e;
        break;
        case 0x4C:  //LD C,H 
        machine_cycles_taken = 1;
        cpu->reg.c = cpu->reg.h;
        break;
        case 0x4D:  //LD C,L
        machine_cycles_taken = 1;
        cpu->reg.c = cpu->reg.l;
        break;
        case 0x4E:  //LD C,(HL)
        machine_cycles_taken = 2;
        cpu->reg.c = fetch(&cpu->mem, cpu->reg.hl, cpu->inBios);
        break;
        case 0x4F:  //LD C,A
        machine_cycles_taken = 1;
        cpu->reg.c = cpu->reg.a;
        break;
        case 0x50:  //LD D,B
        machine_cycles_taken = 1;
        cpu->reg.d = cpu->reg.b;
        break;
        case 0x51:  //LD D,C
        machine_cycles_taken = 1;
        cpu->reg.d = cpu->reg.c;
        break;
        case 0x52:  //LD D,D
        machine_cycles_taken = 1;
        break;
        case 0x53:  //LD D,E
        machine_cycles_taken = 1;
        cpu->reg.d = cpu->reg.e;
        break;
        case 0x54:  //LD D,H
        machine_cycles_taken = 1;
        cpu->reg.d = cpu->reg.h;
        break;
        case 0x55:  //LD D,L
        machine_cycles_taken = 1;
        cpu->reg.d = cpu->reg.l;
        break;
        case 0x56:  //LD D,(HL) 
        machine_cycles_taken = 2;
        cpu->reg.d = fetch(&cpu->mem, cpu->reg.hl, cpu->inBios);
        break;
        case 0x57:  //LD D,A
        machine_cycles_taken = 1;
        cpu->reg.d = cpu->reg.a;
        break;
        case 0x58:  //LD E,B
        machine_cycles_taken = 1;
        cpu->reg.e = cpu->reg.b;
        break;
        case 0x59:  //LD E,C
        machine_cycles_taken = 1;
        cpu->reg.e = cpu->reg.c;
        break;
        case 0x5A:  //LD E,D
        machine_cycles_taken = 1;
        cpu->reg.e = cpu->reg.d;
        break;
        case 0x5B:  //LD E,E
        machine_cycles_taken = 1;
        break;
        case 0x5C:  //LD E,H
        machine_cycles_taken = 1;
        cpu->reg.e = cpu->reg.h;
        break;
        case 0x5D:  //LD E,L
        machine_cycles_taken = 1;
        cpu->reg.e = cpu->reg.l;
        break;
        case 0x5E:  //LD E,(HL)
        machine_cycles_taken = 2;
        cpu->reg.e = fetch(&cpu->mem, cpu->reg.hl, cpu->inBios);
        break;
        case 0x5F:  //LD E,A
        machine_cycles_taken = 1;
        cpu->reg.e = cpu->reg.a;
        break;
        case 0x60:  //LD H,B
        machine_cycles_taken = 1;
        cpu->reg.h = cpu->reg.b;
        break;
        case 0x61:  //LD H,C
        machine_cycles_taken = 1;
        cpu->reg.h = cpu->reg.c;
        break;
        case 0x62:  //LD H,D
        machine_cycles_taken = 1;
        cpu->reg.h = cpu->reg.d;
        break;
        case 0x63:  //LD H,E
        machine_cycles_taken = 1;
        cpu->reg.h = cpu->reg.e;
        break;
        case 0x64:  //LD H,H 
        machine_cycles_taken = 1;
        break;
        case 0x65:  //LD H,L
        machine_cycles_taken = 1;
        cpu->reg.h = cpu->reg.l;
        break;
        case 0x66:  //LD H,(HL)
        machine_cycles_taken = 2;
        cpu->reg.h = fetch(&cpu->mem, cpu->reg.hl, cpu->inBios);
        break;
        case 0x67:  //LD H,A
        machine_cycles_taken = 1;
        cpu->reg.h = cpu->reg.a;
        break;
        case 0x68:  //LD L,B
        machine_cycles_taken = 1;
        cpu->reg.l = cpu->reg.b;
        break;
        case 0x69:  //LD L,C 
        machine_cycles_taken = 1;
        cpu->reg.l = cpu->reg.c;
        break;
        case 0x6A:  //LD L,D
        machine_cycles_taken = 1;
        cpu->reg.l = cpu->reg.d;
        break;
        case 0x6B:  //LD L,E
        machine_cycles_taken = 1;
        cpu->reg.l = cpu->reg.e;
        break;
        case 0x6C:  //LD L,H
        machine_cycles_taken = 1;
        cpu->reg.l = cpu->reg.h;
        break;
        case 0x6D:  //LD L,L
        machine_cycles_taken = 1;
        break;
        case 0x6E:  //LD L,(HL)
        machine_cycles_taken = 2;
        cpu->reg.l = fetch(&cpu->mem, cpu->reg.hl, cpu->inBios);
        break;
        case 0x6F:  //LD L,A
        machine_cycles_taken = 1;
        cpu->reg.l = cpu->reg.a;
        break;
        case 0x70:  //LD (HL),B 
        machine_cycles_taken = 2;
        write_mem(&cpu->mem, cpu->reg.hl, cpu->reg.b);
        break;
        case 0x71:  //LD (HL),C
        machine_cycles_taken = 2;
        write_mem(&cpu->mem, cpu->reg.hl, cpu->reg.c);
        break;
        case 0x72:  //LD (HL),D 
        machine_cycles_taken = 2;
        write_mem(&cpu->mem, cpu->reg.hl, cpu->reg.d);
        break;
        case 0x73:  //LD (HL),E
        machine_cycles_taken = 2;
        write_mem(&cpu->mem, cpu->reg.hl, cpu->reg.e);
        break;
        case 0x74:  //LD (HL),H
        machine_cycles_taken = 2;
        write_mem(&cpu->mem, cpu->reg.hl, cpu->reg.h);
        break;
        case 0x75:  //LD (HL),L
        machine_cycles_taken = 2;
        write_mem(&cpu->mem, cpu->reg.hl, cpu->reg.l);
        break;
        case 0x76:  //HALT 
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0x77:  //LD (HL),A
        machine_cycles_taken = 2;
        write_mem(&cpu->mem, cpu->reg.hl, cpu->reg.a);
        break;
        case 0x78:  //LD A,B
        machine_cycles_taken = 1;
        cpu->reg.a = cpu->reg.b;
        break;
        case 0x79:  //LD A,C
        machine_cycles_taken = 1;
        cpu->reg.a = cpu->reg.c;
        break;
        case 0x7A:  //LD A,D
        machine_cycles_taken = 1;
        cpu->reg.a = cpu->reg.d;
        break;
        case 0x7B:  //LD A,E
        machine_cycles_taken = 1;
        cpu->reg.a = cpu->reg.e;
        break;
        case 0x7C:  //LD A,H 
        machine_cycles_taken = 1;
        cpu->reg.a = cpu->reg.h;
        break;
        case 0x7D:  //LD A,L
        machine_cycles_taken = 1;
        cpu->reg.a = cpu->reg.l;
        break;
        case 0x7E:  //LD A,(HL)
        machine_cycles_taken = 2;
        cpu->reg.a = fetch(&cpu->mem, cpu->reg.hl, cpu->inBios);
        break;
        case 0x7F:  //LD A,A 
        machine_cycles_taken = 1;
        break;
        case 0x80:  //ADD A,B
        machine_cycles_taken = 1;
        cpu->reg.a = add_8t(cpu, cpu->reg.a, cpu->reg.b);
        break;
        case 0x81:  //ADD A,C 
        machine_cycles_taken = 1;
        cpu->reg.a = add_8t(cpu, cpu->reg.a, cpu->reg.c);
        break;
        case 0x82:  //ADD A,D
        machine_cycles_taken = 1;
        cpu->reg.a = add_8t(cpu, cpu->reg.a, cpu->reg.d);
        break;
        case 0x83:  //ADD A,E
        machine_cycles_taken = 1;
        cpu->reg.a = add_8t(cpu, cpu->reg.a, cpu->reg.e);
        break;
        case 0x84:  //ADD A,H 
        machine_cycles_taken = 1;
        cpu->reg.a = add_8t(cpu, cpu->reg.a, cpu->reg.h);
        break;
        case 0x85:  //ADD A,L
        machine_cycles_taken = 1;
        cpu->reg.a = add_8t(cpu, cpu->reg.a, cpu->reg.l);
        break;
        case 0x86:  //ADD A,(HL)
        machine_cycles_taken = 2;
        cpu->reg.a = add_8t(cpu, cpu->reg.a, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
        break;
        case 0x87:  //ADD A,A
        machine_cycles_taken = 1;
        cpu->reg.a = add_8t(cpu, cpu->reg.a, cpu->reg.a);
        break;
        case 0x88:  //ADC A,B
        machine_cycles_taken = 1;
        cpu->reg.a = add_8t_with_carry(cpu, cpu->reg.a, cpu->reg.b);
        break;
        case 0x89:  //ADC A,C 
        machine_cycles_taken = 1;
        cpu->reg.a = add_8t_with_carry(cpu, cpu->reg.a, cpu->reg.c);
        break;
        case 0x8A:  //ADC A,D
        machine_cycles_taken = 1;
        cpu->reg.a = add_8t_with_carry(cpu, cpu->reg.a, cpu->reg.d);
        break;
        case 0x8B:  //ADC A,E
        machine_cycles_taken = 1;
        cpu->reg.a = add_8t_with_carry(cpu, cpu->reg.a, cpu->reg.e);
        break;
        case 0x8C:  //ADC A,H
        machine_cycles_taken = 1;
        cpu->reg.a = add_8t_with_carry(cpu, cpu->reg.a, cpu->reg.h);
        break;
        case 0x8D:  //ADC A,L
        machine_cycles_taken = 1;
        cpu->reg.a = add_8t_with_carry(cpu, cpu->reg.a, cpu->reg.l);
        break;
        case 0x8E:  //ADC A,(HL)
        machine_cycles_taken = 2;
        cpu->reg.a = add_8t_with_carry(cpu, cpu->reg.a, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
        break;
        case 0x8F:  //ADC A,A
        machine_cycles_taken = 1;
        cpu->reg.a = add_8t_with_carry(cpu, cpu->reg.a, cpu->reg.a);
        break;
        case 0x90:  //SUB A, B
        machine_cycles_taken = 1;
        cpu->reg.a = sub_8t(cpu, cpu->reg.a, cpu->reg.b);
        break;
        case 0x91:  //SUB A, C
        machine_cycles_taken = 1;
        cpu->reg.a = sub_8t(cpu, cpu->reg.a, cpu->reg.c);
        break;
        case 0x92:  //SUB A, D
        machine_cycles_taken = 1;
        cpu->reg.a = sub_8t(cpu, cpu->reg.a, cpu->reg.d);
        break;
        case 0x93:  //SUB A, E
        machine_cycles_taken = 1;
        cpu->reg.a = sub_8t(cpu, cpu->reg.a, cpu->reg.e);
        break;
        case 0x94:  //SUB A, H
        machine_cycles_taken = 1;
        cpu->reg.a = sub_8t(cpu, cpu->reg.a, cpu->reg.h);
        break;
        case 0x95:  //SUB A, L
        machine_cycles_taken = 1;
        cpu->reg.a = sub_8t(cpu, cpu->reg.a, cpu->reg.l);
        break;
        case 0x96:  //SUB A, (HL)
        machine_cycles_taken = 2;
        cpu->reg.a = sub_8t(cpu, cpu->reg.a, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
        break;
        case 0x97:  //SUB A, A
        machine_cycles_taken = 1;
        cpu->reg.a = sub_8t(cpu, cpu->reg.a, cpu->reg.a);
        break;
        case 0x98:  //SBC A B
        machine_cycles_taken = 1;
        cpu->reg.a = sub_8t_with_carry(cpu, cpu->reg.a, cpu->reg.b);
        break;
        case 0x99:  //SBC A C
        machine_cycles_taken = 1;
        cpu->reg.a = sub_8t_with_carry(cpu, cpu->reg.a, cpu->reg.c);
        break;
        case 0x9A:  //SBC A D
        machine_cycles_taken = 1;
        cpu->reg.a = sub_8t_with_carry(cpu, cpu->reg.a, cpu->reg.d);
        break;
        case 0x9B:  //SBC A E
        machine_cycles_taken = 1;
        cpu->reg.a = sub_8t_with_carry(cpu, cpu->reg.a, cpu->reg.e);
        break;
        case 0x9C:  //SBC A H
        machine_cycles_taken = 1;
        cpu->reg.a = sub_8t_with_carry(cpu, cpu->reg.a, cpu->reg.h);
        break;
        case 0x9D:  //SBC A L
        machine_cycles_taken = 1;
        cpu->reg.a = sub_8t_with_carry(cpu, cpu->reg.a, cpu->reg.l);
        break;
        case 0x9E:  //SBC A HL
        machine_cycles_taken = 2;
        cpu->reg.a = sub_8t_with_carry(cpu, cpu->reg.a, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
        break;
        case 0x9F:  //SBC A A
        machine_cycles_taken = 1;  
        cpu->reg.a = sub_8t_with_carry(cpu, cpu->reg.a, cpu->reg.a);
        break;
        case 0xA0:  //AND A, B
        machine_cycles_taken = 1;
        cpu->reg.a = and_8t(cpu, cpu->reg.a, cpu->reg.b);
        break;
        case 0xA1:  //AND A, C
        machine_cycles_taken = 1;
        cpu->reg.a = and_8t(cpu, cpu->reg.a, cpu->reg.c);
        break;
        case 0xA2:  //AND A, D
        machine_cycles_taken = 1;
        cpu->reg.a = and_8t(cpu, cpu->reg.a, cpu->reg.d);
        break;
        case 0xA3:  //AND A, E
        machine_cycles_taken = 1;
        cpu->reg.a = and_8t(cpu, cpu->reg.a, cpu->reg.e);
        break;
        case 0xA4:  //AND A, H
        machine_cycles_taken = 1;
        cpu->reg.a = and_8t(cpu, cpu->reg.a, cpu->reg.h);
        break;
        case 0xA5:  //AND A, L
        machine_cycles_taken = 1;
        cpu->reg.a = and_8t(cpu, cpu->reg.a, cpu->reg.l);
        break;
        case 0xA6:  //AND A, (HL)
        machine_cycles_taken = 2;
        cpu->reg.a = and_8t(cpu, cpu->reg.a, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
        break;
        case 0xA7:  //AND A, A
        machine_cycles_taken = 1;
        cpu->reg.a = and_8t(cpu, cpu->reg.a, cpu->reg.a);
        break;
        case 0xA8:  //XOR A, B
        machine_cycles_taken = 1;
        cpu->reg.a = xor_8t(cpu, cpu->reg.a, cpu->reg.b);
        break;
        case 0xA9:  //XOR A, C
        machine_cycles_taken = 1;
        cpu->reg.a = xor_8t(cpu, cpu->reg.a, cpu->reg.c);
        break;
        case 0xAA:  //XOR A, D
        machine_cycles_taken = 1;
        cpu->reg.a = xor_8t(cpu, cpu->reg.a, cpu->reg.d);
        break;
        case 0xAB:  //XOR A, E
        machine_cycles_taken = 1;
        cpu->reg.a = xor_8t(cpu, cpu->reg.a, cpu->reg.e);
        break;
        case 0xAC:  //XOR A, H
        machine_cycles_taken = 1;
        cpu->reg.a = xor_8t(cpu, cpu->reg.a, cpu->reg.h);
        break;
        case 0xAD:  //XOR A, L
        machine_cycles_taken = 1;
        cpu->reg.a = xor_8t(cpu, cpu->reg.a, cpu->reg.l);
        break;
        case 0xAE:  //XOR A, (HL)
        machine_cycles_taken = 2;
        cpu->reg.a = xor_8t(cpu, cpu->reg.a, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
        break;
        case 0xAF:  //XOR A, A
        machine_cycles_taken = 1;
        cpu->reg.a = xor_8t(cpu, cpu->reg.a, cpu->reg.a);
        break;
        case 0xB0:  //OR A, B
        machine_cycles_taken = 1;
        cpu->reg.a = or_8t(cpu, cpu->reg.a, cpu->reg.b);
        break;
        case 0xB1:  //OR A, C
        machine_cycles_taken = 1;
        cpu->reg.a = or_8t(cpu, cpu->reg.a, cpu->reg.c);
        break;
        case 0xB2:  //OR A, D
        machine_cycles_taken = 1;
        cpu->reg.a = or_8t(cpu, cpu->reg.a, cpu->reg.d);
        break;
        case 0xB3:  //OR A, E
        machine_cycles_taken = 1;
        cpu->reg.a = or_8t(cpu, cpu->reg.a, cpu->reg.e);
        break;
        case 0xB4:  //OR A, H
        machine_cycles_taken = 1;
        cpu->reg.a = or_8t(cpu, cpu->reg.a, cpu->reg.h);
        break;
        case 0xB5:  //OR A, L
        machine_cycles_taken = 1;
        cpu->reg.a = or_8t(cpu, cpu->reg.a, cpu->reg.l);
        break;
        case 0xB6:  //OR A, (HL)
        machine_cycles_taken = 2;
        cpu->reg.a = or_8t(cpu, cpu->reg.a, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
        break;
        case 0xB7:  //OR A, A
        machine_cycles_taken = 1;
        cpu->reg.a = or_8t(cpu, cpu->reg.a, cpu->reg.a);
        break;
        case 0xB8:  //CP A, B
        machine_cycles_taken = 1;
        sub_8t(cpu, cpu->reg.a, cpu->reg.b);
        break;
        case 0xB9:  //CP A, C
        machine_cycles_taken = 1;
        sub_8t(cpu, cpu->reg.a, cpu->reg.c);
        break;
        case 0xBA:  //CP A, D
        machine_cycles_taken = 1;
        sub_8t(cpu, cpu->reg.a, cpu->reg.d);
        break;
        case 0xBB:  //CP A, E
        machine_cycles_taken = 1;
        sub_8t(cpu, cpu->reg.a, cpu->reg.e);
        break;
        case 0xBC:  //CP A, H
        machine_cycles_taken = 1;
        sub_8t(cpu, cpu->reg.a, cpu->reg.h);
        break;
        case 0xBD:  //CP A, L
        machine_cycles_taken = 1;
        sub_8t(cpu, cpu->reg.a, cpu->reg.l);
        break;
        case 0xBE:  //CP A, (HL)
        machine_cycles_taken = 2;
        sub_8t(cpu, cpu->reg.a, fetch(&cpu->mem, cpu->reg.hl, cpu->inBios));
        break;
        case 0xBF:  //CP A, A
        machine_cycles_taken = 1;
        sub_8t(cpu, cpu->reg.a, cpu->reg.a);
        break;
        case 0xC0:  //RET NZ
        machine_cycles_taken = 5;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xC1:  //POP BC
        machine_cycles_taken = 3;
        // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xC2:  //JP NZ a16
        machine_cycles_taken = 3;
        uint16_t jpnza16_addr = fetch_and_increment_pc_twice(cpu);
        if (!get_zero_flag(&cpu->reg)){
            machine_cycles_taken = 4;
            cpu->reg.pc = jpnza16_addr;
        }
        break;
        case 0xC3:  //JP a16
        machine_cycles_taken = 4;
        uint16_t jpa16_addr = fetch_and_increment_pc_twice(cpu);
        cpu->reg.pc = jpa16_addr;
        break;
        case 0xC4:  //CALL NZ a16
        machine_cycles_taken = 3;
        uint16_t callnza16_addr = fetch_and_increment_pc_twice(cpu);
        if (!get_zero_flag(&cpu->reg)){
            //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        }
        break;
        case 0xC5:  //PUSH BC
        machine_cycles_taken = 4;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xC6:  //ADD A d8
        machine_cycles_taken = 2;
        cpu->reg.a = add_8t(cpu, cpu->reg.a, fetch_and_increment_pc(cpu));
        break;
        case 0xC7:  //RST 00H
        machine_cycles_taken = 4;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xC8:  //RET Z
        machine_cycles_taken = 5;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xC9:  //RET
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xCA:  //JP Z a16
        machine_cycles_taken = 4;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xCB:  //PREFIX
        machine_cycles_taken = 1;
        // handled by caller of this function
        break;
        case 0xCC:  //CALL Z a16
        machine_cycles_taken = 6;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xCD:  //CALL a16
        machine_cycles_taken = 6;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xCE:  //ADC A d8
        machine_cycles_taken = 2;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xCF:  //RST 08H
        machine_cycles_taken = 4;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xD0:  //RET NC
        machine_cycles_taken = 5;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xD1:  //POP DE
        machine_cycles_taken = 3;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xD2:  //JP NC a16
        machine_cycles_taken = 3;
        uint16_t jpnc_addr = fetch_and_increment_pc_twice(cpu);
        if (!get_carry_flag(&cpu->reg)){
            machine_cycles_taken = 4;
            cpu->reg.sp = jpnc_addr;
        }
        break;
        case 0xD3:  //ILLEGAL_D3
        machine_cycles_taken = 1;
        break;
        case 0xD4:  //CALL NC a16
        machine_cycles_taken = 6;
        break;
        case 0xD5:  //PUSH DE
        machine_cycles_taken = 4;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xD6:  //SUB A, d8
        machine_cycles_taken = 2;
        cpu->reg.a = sub_8t(cpu, cpu->reg.a, fetch_and_increment_pc(cpu));
        break;
        case 0xD7:  //RST 10H
        machine_cycles_taken = 4;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xD8:  //RET C
        machine_cycles_taken = 5;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xD9:  //RETI
        machine_cycles_taken = 4;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xDA:  //JP C a16
        machine_cycles_taken = 3;
        uint16_t jpca16_addr = fetch_and_increment_pc_twice(cpu);
        if (get_carry_flag(&cpu->reg)){
            machine_cycles_taken = 4;
            cpu->reg.pc = jpca16_addr;
        }
        break;
        case 0xDB:  //ILLEGAL_DB
        machine_cycles_taken = 1;
        break;
        case 0xDC:  //CALL C a16
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        machine_cycles_taken = 6;
        break;
        case 0xDD:  //ILLEGAL_DD
        machine_cycles_taken = 1;
        break;
        case 0xDE:  //SBC A d8
        machine_cycles_taken = 2;
        cpu->reg.a = sub_8t_with_carry(cpu, cpu->reg.a, fetch_and_increment_pc(cpu));
        break;
        case 0xDF:  //RST 18H
        machine_cycles_taken = 4;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xE0:  //LD (FF00+u8)
        machine_cycles_taken = 3;
        write_mem(&cpu->mem, 0xFF00 + fetch_and_increment_pc(cpu), cpu->reg.a);
        break;
        case 0xE1:  //POP HL
        machine_cycles_taken = 3;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xE2:  //LD (FF00+C),A
        machine_cycles_taken = 2;
        write_mem(&cpu->mem, 0xFF00 + cpu->reg.c, cpu->reg.a);
        break;
        case 0xE3:  //ILLEGAL_E3
        machine_cycles_taken = 1;
        break;
        case 0xE4:  //ILLEGAL_E4
        machine_cycles_taken = 1;
        break;
        case 0xE5:  //PUSH HL
        machine_cycles_taken = 4;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xE6:  //AND A, d8
        machine_cycles_taken = 2;
        cpu->reg.a = and_8t(cpu, cpu->reg.a, fetch_and_increment_pc(cpu));
        break;
        case 0xE7:  //RST 20H
        machine_cycles_taken = 4;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xE8:  //ADD SP,i8
        machine_cycles_taken = 4;
        add_to_pc(cpu, safe_convert(fetch_and_increment_pc(cpu)));
        break;
        case 0xE9:  //JP HL
        machine_cycles_taken = 1; 
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xEA:  //LD a16 A
        machine_cycles_taken = 4;
        write_mem(&cpu->mem, fetch_and_increment_pc_twice(cpu), cpu->reg.a);
        break;
        case 0xEB:  //ILLEGAL_EB
        machine_cycles_taken = 1;
        break;
        case 0xEC:  //ILLEGAL_EC
        machine_cycles_taken = 1;
        break;
        case 0xED:  //ILLEGAL_ED
        machine_cycles_taken = 1;
        break;
        case 0xEE:  //XOR A, d8
        machine_cycles_taken = 2;
        cpu->reg.a = xor_8t(cpu, cpu->reg.a, fetch_and_increment_pc(cpu));
        break;
        case 0xEF:  //RST 28H
        machine_cycles_taken = 4;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xF0:  //LD A,(FF00+u8)
        machine_cycles_taken = 3;
        cpu->reg.a = fetch(&cpu->mem, 0xFF00 + fetch_and_increment_pc(cpu), cpu->inBios);
        break;
        case 0xF1:  //POP AF
        machine_cycles_taken = 3;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xF2:  //LD A,(FF00+C)
        machine_cycles_taken = 2;
        cpu->reg.a = fetch(&cpu->mem, 0xFF00 + cpu->reg.c, cpu->inBios);
        break;
        case 0xF3:  //DI
        machine_cycles_taken = 1;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xF4:  //ILLEGAL_F4
        machine_cycles_taken = 1;
        break;
        case 0xF5:  //PUSH AF
        machine_cycles_taken = 4;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xF6:  //OR A, d8
        machine_cycles_taken = 2;
        cpu->reg.a = or_8t(cpu, cpu->reg.a, fetch_and_increment_pc(cpu));
        break;
        case 0xF7:  //RST 30H
        machine_cycles_taken = 4;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xF8:  //LD HL,SP+i8
        machine_cycles_taken = 3;
        cpu->reg.hl = cpu->reg.sp + safe_convert(fetch_and_increment_pc_twice(cpu));
        break;
        case 0xF9:  //LD SP HL
        machine_cycles_taken = 2;
        cpu->reg.sp = cpu->reg.hl;
        break;
        case 0xFA:  //LD A (u16)
        machine_cycles_taken = 4;
        cpu->reg.a = fetch(&cpu->mem, fetch_and_increment_pc_twice(cpu), cpu->inBios);
        break;
        case 0xFB:  //EI
        machine_cycles_taken = 1;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
        case 0xFC:  //ILLEGAL_FC
        machine_cycles_taken = 1;
        break;
        case 0xFD:  //ILLEGAL_FD
        machine_cycles_taken = 1;
        break;
        case 0xFE:  //CP A, d8
        machine_cycles_taken = 2;
        sub_8t(cpu, cpu->reg.a, fetch_and_increment_pc(cpu));
        break;
        case 0xFF:  //RST 38H
        machine_cycles_taken = 4;
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
    }
    return machine_cycles_taken;
}

int execute_cb_prefix(Cpu* cpu, uint8_t instruction){
    int cycles_taken = 0;
    switch (instruction){
        case 0x0:
        cycles_taken = 2;
        break;
        case 0x01:
        break;
        
    }
    return cycles_taken;
}

// Executes an instruction
// Returns the number of machine cycles the instruction took
int execute(Cpu* cpu, uint8_t instruction){
    int cycles_taken;
    // Handle the pc for most instructions
    increment_pc(cpu);
    // Check for CB-prefixed instructions
    if (instruction == 0xCB) {
        // This is a CB-prefixed instruction! 
        // Have to read the next one
        uint8_t cb_instr = fetch(&cpu->mem, cpu->reg.pc, cpu->inBios);
        increment_pc(cpu);
        cycles_taken = execute_cb_prefix(cpu, cb_instr);
    } else {
        cycles_taken = execute_normal(cpu, instruction);
    }
    return cycles_taken;
}

int tick(Cpu* cpu){
    // Fetch
    uint8_t instruction = fetch(&cpu->mem, cpu->reg.pc, cpu->inBios);
    
    // Decode/Execute and return # of clock cycles
    return execute(cpu, instruction);
}




