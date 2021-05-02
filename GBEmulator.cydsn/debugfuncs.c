#include "debugfuncs.h"
#include "GUI.h"
#include "stdio.h"
#include "tft.h"
#include "gpu.h"

void debug_renderLine(Memory* mem){
    
    // The two background maps are located at 9800h-9BFFh and 9C00h-9FFFh
    // Each bg map is 32x32 tiles, for a total of 256x256 pixels
    // We can access this as either          vram[0x1800:] or vram[0x1C00:]
    int mapOffset = 0x1800;
    
    
    int y = mem->current_scan_line + mem->scroll_y;
    
    // Each "y" is worth 8 pixels, so divide by 8 to get num tiles
    // Then multiply by 32 since we have 32 tiles per row
    int mapRowStart = mapOffset + (y/8) * 32; 
    
    
    int tileLine = y % 8; //y % 8 gives us the specific line in the tile to show
    
    //write8_a0(0x2C);                    // send Memory Write command
    int i;
    for (i=0;i<DISPLAY_WIDTH/8;i++){    //DISPLAY_WIDTH/8 = 20 total tiles on the line
        // Which tile are we on?
        uint8_t tileId = mem->vram[mapRowStart + i];
        
        int tileStartAddr = tileId * 16; //each tile is 16 bytes
        // finally index at the correct 2 bytes for this row
        uint8_t low = mem->vram[tileStartAddr + 2 * tileLine];  
        uint8_t high = mem->vram[tileStartAddr + 2 * tileLine + 1];
        
        
        
        // Finally print the line
        int j;
        for (j=7;j>=0;j--){
            uint8_t pxcolor = ((high >> j & 0x1) << 1) | ((low >> j) & 0x1);
            
            
            switch (pxcolor){
                case 0:
                    write8_a1(0xFF);
                    write8_a1(0xFF);
                break;
                case 1:
                    write8_a1(0xC6);
                    write8_a1(0x18);
                break;
                case 2:
                    write8_a1(0x7B);
                    write8_a1(0xEF);
                break;
                case 3:
                    write8_a1(0x00);
                    write8_a1(0x00);
                break;
            }
            
        }
    }
    
}

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
        debug_renderLine(mem);
        mem->current_scan_line++;
    }
}

void debug_fmt_cpu_trace(char* returnBuffer, Cpu* cpu, Memory* mem, unsigned long total_instrs, unsigned long total_cycles){
    sprintf(returnBuffer, "A: %02X F: %02X B: %02X C: %02X D: %02X E: %02X H: %02X L: %02X SP: %04X PC: 00:%04X (%02X %02X %02X %02X)\n\r",
    cpu->reg.a, cpu->reg.f, cpu->reg.b, cpu->reg.c, cpu->reg.d, cpu->reg.e, cpu->reg.h, cpu->reg.l, cpu->reg.sp, cpu->reg.pc, fetch(mem, cpu->reg.pc, cpu->inBios), fetch(mem, cpu->reg.pc + 1, cpu->inBios), fetch(mem, cpu->reg.pc + 2, cpu->inBios), fetch(mem, cpu->reg.pc + 3, cpu->inBios)
    );
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
        "%04X@%X\n"
        "%04X@%X\n"
        "%04X@%X\n"
        "%04X@%X\n"
        "%04X@%X\n"
        "%04X@%X\n"
        "Z: %d N: %d H: %d C: %d\n"
        "Memory starting @%04X:\n"
        
        , 
        fetch(mem, cpu->reg.pc, cpu->inBios),
        total_instrs, total_cycles, 
        cpu->reg.af, cpu->reg.bc, cpu->reg.de, cpu->reg.hl, cpu->reg.sp, cpu->reg.pc,
        (fetch(mem, cpu->reg.sp + 11, cpu->inBios) << 8) | fetch(mem, cpu->reg.sp + 10, cpu->inBios), cpu->reg.sp + 10, 
        (fetch(mem, cpu->reg.sp + 9, cpu->inBios) << 8) | fetch(mem, cpu->reg.sp + 8, cpu->inBios), cpu->reg.sp + 8, 
        (fetch(mem, cpu->reg.sp + 7, cpu->inBios) << 8) | fetch(mem, cpu->reg.sp + 6, cpu->inBios), cpu->reg.sp + 6, 
        (fetch(mem, cpu->reg.sp + 5, cpu->inBios) << 8) | fetch(mem, cpu->reg.sp + 4, cpu->inBios), cpu->reg.sp + 4, 
        (fetch(mem, cpu->reg.sp + 3, cpu->inBios) << 8) | fetch(mem, cpu->reg.sp + 2, cpu->inBios), cpu->reg.sp + 2, 
        (fetch(mem, cpu->reg.sp + 1, cpu->inBios) << 8) | fetch(mem, cpu->reg.sp, cpu->inBios), cpu->reg.sp, 
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

