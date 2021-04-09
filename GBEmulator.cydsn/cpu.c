#include "memory.h"
#include "rom.h"
#include "registers.h"
#include "cpu.h"

void reset_cpu(Cpu* cpu) {
    cpu->inBios = true;
    reset_memory(&cpu->mem);
    reset_registers(&cpu->reg);
}

// Executes an instruction
// Returns the number of clock cycles the instruction took
int execute(Cpu* cpu, uint8_t instruction){
    int clock_cycles = 0;
    switch (instruction){
        case 0x00:
        // no op
        break;
        case 0x01:
        break;
    }
    cpu->reg.pc++;
    if (cpu->reg.pc == BIOS_SIZE && cpu->inBios) {
        cpu->inBios = false;
    }
    
    return clock_cycles;
}

int tick(Cpu* cpu){
    // Fetch
    uint8_t instruction = fetch(&cpu->mem, cpu->reg.pc, cpu->inBios);
    
    // Decode/Execute and return # of clock cycles
    return execute(cpu, instruction);
}


