#ifndef MEMORY_H
#define MEMORY_H
#include "stdint.h"
#include "stdbool.h"
#define ECHO_RAM_SIZE 0x1E00
#define ECHO_RAM_START 0xE000
#define ECHO_RAM_END 0xFE00
#define EXTERNAL_RAM_START 0xA000
#define EXTERNAL_RAM_END 0xC000
#define EXTERNAL_RAM_SIZE 0x2000
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
#define VBLANK_INTERRUPT_REG_MASK   0b00001
#define LCD_STAT_INTERRUPT_REG_MASK 0b00010
#define TIMER_INTERRUPT_REG_MASK    0b00100
#define SERIAL_INTERRUPT_REG_MASK   0b01000
#define JOYPAD_INTERRUPT_REG_MASK   0b10000
#define VBLANK_ISR_LOC 0x40
#define LCD_STAT_ISR_LOC 0x48
#define TIMER_ISR_LOC 0x50
#define SERIAL_ISR_LOC 0x58
#define JOYPAD_ISR_LOC 0x60
#define LCDC_LOC 0xFF40
#define LCD_STATUS_LOC 0xFF41
#define SCY_LOC 0xFF42
#define SCX_LOC 0xFF43
#define LY_LOC 0xFF44
#define JOYP_LOC 0xFF00
#define BG_PALETTE_LOC 0xFF47
#define SB_LOC 0xFF01
#define SC_LOC 0xFF02
typedef struct Memory {
    uint8_t wram[WRAM_SIZE];         // work ram
    uint8_t eram[EXTERNAL_RAM_SIZE]; // external ram
    uint8_t vram[VRAM_SIZE];         // video ram

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
    
    // 
    uint8_t sb;  //SB serial transfer data    (located on 0xFF01)
    uint8_t sc;  //SC serial transfer control (located on 0xFF02)
    // Joypad
    uint8_t joyp;              //Joypad register located on 0xFF00;
} Memory;
// Fetch a byte from memory
uint8_t fetch(Memory* memory, uint16_t address, bool inBios);
// Write a byte into memory
void write_mem(Memory* memory, uint16_t address, uint8_t data);
// Reset memory back to 0s
void reset_memory(Memory* memory);

#endif
