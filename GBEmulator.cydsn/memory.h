#ifndef MEMORY_H
#define MEMORY_H
#include "stdint.h"
#include "stdbool.h"
#define WRAM_SIZE 0x2000
#define VRAM_SIZE 0x2000
#define OAM_SIZE 0xA0
#define OAM_START 0xFE00
#define OAM_END 0xFEA0
#define ROM_START 0
#define ROM_END 0x8000
#define VRAM_START 0x8000
#define VRAM_END 0xA000
#define WRAM_START 0xC000
#define WRAM_END 0xE000
#define ZERO_PAGE_START 0xFF80
#define ZERO_PAGE_END 0xFFFF
#define ZERO_PAGE_SIZE 0x7F
#define INTERRUPT_ENABLE_LOC 0xFFFF
#define INTERRUPT_FLAG_LOC 0xFF0F
#define LCDC_LOC 0xFF40
#define LCD_STATUS_LOC 0xFF41
#define SCY_LOC 0xFF42
#define SCX_LOC 0xFF43
#define LY_LOC 0xFF44
#define BG_PALETTE_LOC 0xFF47
typedef struct Memory {
    uint8_t wram[WRAM_SIZE];   // work ram
    uint8_t vram[VRAM_SIZE];   // video ram
    uint8_t oam[OAM_SIZE];     // Sprite attribute table (OAM)
    uint8_t zero_page[ZERO_PAGE_SIZE];       // High address RAM (stack here)
    uint8_t interrupt_enable;  // interrupt enable (located on 0xFFFF)
    uint8_t interrupt_flag;    // interrupt flag (located on 0xFF0F)
    
    // GPU registers
    uint8_t lcdc;              // LCD Control (R/W located on 0xFF40)
    uint8_t lcdstatus;         // LCD Status  (R/Wlocated on 0xFF41)
    uint8_t scroll_y;          // SCY (R/W located on 0xFF42)
    uint8_t scroll_x;          // SCX (R/W located on 0xFF43)
    uint8_t current_scan_line; // LY  (R located on 0xFF44)
    uint8_t background_palette; // (W located on 0xFF47)
} Memory;
// Fetch a byte from memory
uint8_t fetch(Memory* memory, uint16_t address, bool inBios);
// Write a byte into memory
void write_mem(Memory* memory, uint16_t address, uint8_t data);
// Reset memory back to 0s
void reset_memory(Memory* memory);

#endif
