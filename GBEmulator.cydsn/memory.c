#include "memory.h" 
#include <project.h>
#include "rom.h"
#include "cpu.h"
#include "stdint.h"
#include "stdbool.h"
#include "emumode.h"

// Emulates a dma transfer
static void start_dma(Memory* mem, uint8_t xx){
    // Source:      $XX00-$XX9F   ;XX = $00 to $DF
    // Destination: $FE00-$FE9F
    uint16_t source = xx << 8;
    int i;  // copy 160 bytes
    for (i=0;i<160;i++){
        mem->oam[i] = fetch(mem, source + i, false);
    }
}

void reset_memory(Memory* memory){
    int i;
    for (i=0;i<WRAM_SIZE;i++){
        memory->wram[i] = 0xFF;
    }
    for (i=0;i<VRAM_SIZE;i++){
        memory->vram[i] = 0xFF;
    }
    //@@@TODO remove this once you add joypad
    memory->joyp = 0xFF;
}



// 	FFFF 	Interrupt Enable Flag
//	FF80-FFFE 	Zero Page - 127 bytes
//	FF00-FF7F 	Hardware I/O Registers
//	FEA0-FEFF 	Unusable Memory
//	FE00-FE9F 	OAM - Object Attribute Memory
//	E000-FDFF 	Echo RAM - Reserved, Do Not Use
//	D000-DFFF 	Internal RAM - Bank 1-7 (switchable - CGB only)
//	C000-CFFF 	Internal RAM - Bank 0 (fixed)
//	A000-BFFF 	Cartridge RAM (If Available)
//	9C00-9FFF 	BG Map Data 2
//	9800-9BFF 	BG Map Data 1
//	8000-97FF 	Character RAM
//	4000-7FFF 	Cartridge ROM - Switchable Banks 1-xx
//	0150-3FFF 	Cartridge ROM - Bank 0 (fixed)
//	0100-014F 	Cartridge Header Area
//  0000-00FF 	Restart and Interrupt Vectorss
uint8_t fetch(Memory* memory, uint16_t address, bool inBios){
    if (DEBUG_MODE && DEBUG_MODE_STUB_LY_0x90 && address == LY_LOC){
        return 0x90;
    }
    
    if (ROM_START <= address && address < ROM_END) {
        if (inBios && address < BIOS_SIZE) {
            return bios[address];
        }
        return rom[address];
    } else if (VRAM_START <= address && address < VRAM_END) {
        return memory->vram[address - VRAM_START];
    } else if (EXTERNAL_RAM_START <= address && address < EXTERNAL_RAM_END){
        return memory->eram[address - EXTERNAL_RAM_START];
    } else if (WRAM_START <= address && address < WRAM_END) {
        return memory->wram[address - WRAM_START];
    } else if (ECHO_RAM_START <= address && address < ECHO_RAM_END){
        return memory->wram[address - ECHO_RAM_START];
    } else if (OAM_START <= address && address < OAM_END){
        return memory->oam[address - OAM_START];
    } else if (ZERO_PAGE_START <= address && address < ZERO_PAGE_END){
        return memory->zero_page[address - ZERO_PAGE_START];
    }else {
        switch (address){
            case INTERRUPT_ENABLE_LOC:
                return memory->interrupt_enable;
            case INTERRUPT_FLAG_LOC:
                return memory->interrupt_flag;    
            case LCDC_LOC:
                return memory->lcdc;
            case LCD_STATUS_LOC:
                return memory->lcdstatus;
            case SCX_LOC:
                return memory->scroll_x;
            case SCY_LOC:
                return memory->scroll_y;
            case LY_LOC:
                return memory->current_scan_line;
            case LYC_LOC:
                return memory->lyc;
            case JOYP_LOC:
                return memory->joyp;
            case SB_LOC:
                return memory->sb;
            case SC_LOC:
                return memory->sc;
            case BG_PALETTE_LOC:
                return memory->background_palette;
            case OBP0_LOC:
                return memory->obp0;
            case OBP1_LOC:
                return memory->obp1;
            case WX_LOC:
                return memory->wx;
            break;
            case WY_LOC:
                return memory->wy;
            break;
            default: break;
        }
    
    }
    // TODO add other memory access locations...
    return 0;
}


void write_mem(Memory* memory, uint16_t address, uint8_t data) {
    if (GB_SERIAL_PASSTHROUGH && address == 0xFF02 && data == 0x81) {
        // Pass through GB serial in debug mode
        UART_1_PutChar(fetch(memory, 0xFF01, false));
    }   
    
    if (ROM_START <= address && address < ROM_END) {
        // Nothing to do... can't write to ROM
    } else if (VRAM_START <= address && address < VRAM_END) {
        memory->vram[address - VRAM_START] = data;
    } else if (EXTERNAL_RAM_START <= address && address < EXTERNAL_RAM_END){
        memory->eram[address - EXTERNAL_RAM_START] = data;
    } else if (WRAM_START <= address && address < WRAM_END) {
        memory->wram[address - WRAM_START] = data;
    } else if (ECHO_RAM_START <= address && address < ECHO_RAM_END){
        memory->wram[address - ECHO_RAM_START] = data;
    } else if (OAM_START <= address && address < OAM_END){
        memory->oam[address - OAM_START] = data;
    } else if (ZERO_PAGE_START <= address && address < ZERO_PAGE_END){
        memory->zero_page[address - ZERO_PAGE_START] = data;
    }else {
        switch (address){
            case INTERRUPT_ENABLE_LOC:
            memory->interrupt_enable = data;
            break;
            case INTERRUPT_FLAG_LOC:
            memory->interrupt_flag = data;
            break;
            case LCDC_LOC:
            memory->lcdc = data;
            break;
            case LYC_LOC:
            memory->lyc = data;
            break;
            case LCD_STATUS_LOC:
            memory->lcdstatus = data;
            break;
            case SCX_LOC:
            memory->scroll_x = data;
            break;
            case SCY_LOC:
            memory->scroll_y = data;
            break;
            case SB_LOC:
            memory->sb = data;
            break;
            case SC_LOC:
            memory->sc = data;
            break;
            case BG_PALETTE_LOC:
            memory->background_palette = data;
            break;
            case OBP0_LOC:
            memory->obp0 = data;
            break;
            case OBP1_LOC:
            memory->obp1 = data;
            break;
            case WX_LOC:
            memory->wx = data;
            break;
            case WY_LOC:
            memory->wy = data;
            break;
            case OAM_DMA_LOC:
            start_dma(memory, data);
            break;
            case JOYP_LOC:
            memory->joyp = data;
            break;
            default: break;
        }
    
    } 
}