#include "memory.h"
#include "rom.h"
#include "cpu.h"
#include "stdint.h"
#include "stdbool.h"
#define ROM_START 0
#define ROM_END 0x8000

#define VRAM_START 0x8000
#define VRAM_END 0xA000

#define WRAM_START 0xC000
#define WRAM_END 0xE000

void reset_memory(Memory* memory){
    int i;
    for (i=0;i<WRAM_SIZE;i++){
        memory->wram[i] = 0;
    }
    for (i=0;i<VRAM_SIZE;i++){
        memory->vram[i] = 0;
    }
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
    if (ROM_START <= address && address < ROM_END) {
        if (inBios && address < BIOS_SIZE) {
            return bios[address];
        }
        return rom[address];
    } else if (VRAM_START <= address && address < VRAM_END) {
        return memory->vram[address - VRAM_START];
    } else if (WRAM_START <= address && address < WRAM_START) {
        return memory->wram[address - WRAM_START];
    }
    // TODO add other memory access locations...
    return 0;
}