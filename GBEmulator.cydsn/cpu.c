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

// Returns the incremented value, adjusting flags if applicable
static inline uint8_t inc_8t(Cpu* cpu, uint8_t value){
    return value++;
}
static inline uint8_t dec_8t(Cpu* cpu, uint8_t value){
    return value--;
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
        uint8_t ldu16sp_lower = fetch_and_increment_pc(cpu);
        uint8_t ldu16sp_upper = fetch_and_increment_pc(cpu);
        uint16_t ldu16sp_addr = (ldu16sp_upper << 8) & ldu16sp_lower;
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
        uint8_t ldspu16_lower = fetch_and_increment_pc(cpu);
        uint8_t ldspu16_upper = fetch_and_increment_pc(cpu);
        cpu->reg.sp = (ldspu16_upper << 8) ^ ldspu16_lower;
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




