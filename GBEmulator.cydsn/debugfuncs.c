#include "debugfuncs.h"
#include "GUI.h"
#include "stdio.h"
#include "tft.h"
#include "gpu.h"
void debug_show_full_vram(Memory* mem) {
    tftStart();    // initialize the TFT display
    uint16 SC = 0;                     
    uint16 EC = 159;
    uint16 SP = 0;
    uint16 EP = 143;
    write8_a0(0x2A);                 	// send Column Address Set command
    write8_a1(SC >> 8);                 // set SC[15:0]
    write8_a1(SC & 0x00FF);
    write8_a1(EC >> 8);                 // set EC[15:0]
    write8_a1(EC & 0x00FF);
    write8_a0(0x2B);                 	// send Page Address Set command
    write8_a1(SP >> 8);                 // set SP[15:0]
    write8_a1(SP & 0x00FF);
    write8_a1(EP >> 8);                 // set EP[15:0]
    write8_a1(EP & 0x00FF);
    mem->current_scan_line = 0;
    
    write8_a0(0x2C);                    // send Memory Write command
    while (mem->current_scan_line != 144){  
        renderLine(mem);
        mem->current_scan_line++;
    }
}

void debug_fmt_cpu_state(char* returnBuffer, Cpu* cpu, Memory* mem, unsigned long total_instrs, unsigned long total_cycles, uint16_t memoffset){
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
        "Z: %d N: %d H: %d C: %d\n"
        "Memory starting @%04X:\n"
        
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
        get_zero_flag(&cpu->reg), get_subtraction_flag(&cpu->reg), get_half_carry_flag(&cpu->reg), get_carry_flag(&cpu->reg),
        memoffset
    );
    
    
    int i=0;
    for (i=0;i<8;i++){
        char buffer[5];
        sprintf(buffer, "%02X ", fetch(mem, memoffset + i, cpu->inBios));
        strcat(returnBuffer, buffer);
    }
    strcat(returnBuffer, "\n");
    for (i=8;i<16;i++){
        char buffer[5];
        sprintf(buffer, "%02X ", fetch(mem, memoffset + i, cpu->inBios));
        strcat(returnBuffer, buffer);
    }
}

