#include "memory.h"
#include "rom.h"
#include "registers.h"
#include "cpu.h"

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




