#include "debugfuncs.h"
#include "GUI.h"
#include "stdio.h"

void debug_show_tileset(Memory* mem) {
    
}

void debug_fmt_cpu_state(char* returnBuffer, Cpu* cpu, Memory* mem, unsigned long total_instrs, unsigned long total_cycles){
    sprintf(returnBuffer, "Next Instr:%02X\nTotal Instrs: %lu\n"
        "Cycles Taken: %lu\n"
        "AF: %04X\n"
        "BC: %04X\n"
        "DE: %04X\n"
        "HL: %04X\n"
        "SP: %04X\n"
        "PC: %04X\n"
        "Stack: \n"
        "%02X@%X\n"
        "%02X@%X\n"
        "%02X@%X\n"
        "%02X@%X\n"
        "%02X@%X\n"
        "%02X@%X\n"
        "Z: %d N: %d H: %d C: %d"
        
        , 
        fetch(mem, cpu->reg.pc, cpu->inBios),
        total_instrs, total_cycles, 
        cpu->reg.af, cpu->reg.bc, cpu->reg.de, cpu->reg.hl, cpu->reg.sp, cpu->reg.pc,
        fetch(mem, 0xFFFE, cpu->inBios), 0xFFFE,
        fetch(mem, 0xFFFD, cpu->inBios), 0xFFFD,
        fetch(mem, 0xFFFC, cpu->inBios), 0xFFFC,
        fetch(mem, 0xFFFB, cpu->inBios), 0xFFFB,
        fetch(mem, 0xFFFA, cpu->inBios), 0xFFFA,
        fetch(mem, 0xFFF9, cpu->inBios), 0xFFF9,
        get_zero_flag(&cpu->reg), get_subtraction_flag(&cpu->reg), get_half_carry_flag(&cpu->reg), get_carry_flag(&cpu->reg)
    );
}

