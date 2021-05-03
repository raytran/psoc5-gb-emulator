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
#define SPRITES_TILE_TABLE_OFFSET 0x8000
#define INTERRUPT_ENABLE_LOC 0xFFFF
#define INTERRUPT_FLAG_LOC 0xFF0F
// Interrupt register mask on INTERRUPT_ENABLE
#define INTERRUPT_ENABLE_VBLANK_MASK   0b00001
#define INTERRUPT_ENABLE_STAT_MASK 0b00010
#define INTERRUPT_ENABLE_TIMER_MASK    0b00100
#define INTERRUPT_ENABLE_SERIAL_MASK   0b01000
#define INTERRUPT_ENABLE_JOYPAD_MASK   0b10000
// LCD status mask on lcd_stat
#define LCD_STAT_LY_LYC_INTERRUPT_REG_MASK 0b1000000
#define LCD_STAT_OAM_INTERRUPT_REG_MASK    0b0100000
#define LCD_STAT_INTERRUPT_ENABLE_VBLANK_MASK 0b0010000
#define LCD_STAT_HBLANK_INTERRUPT_REG_MASK 0b0001000
#define LCD_STAT_LY_LYC_EQ_REG_MASK        0b0000100
    

    
#define VBLANK_ISR_LOC 0x40
#define LCD_STAT_ISR_LOC 0x48
#define TIMER_ISR_LOC 0x50
#define SERIAL_ISR_LOC 0x58
#define JOYPAD_ISR_LOC 0x60
#define LCDC_LOC 0xFF40
#define LCD_STATUS_LOC 0xFF41
#define SCY_LOC 0xFF42               // scroll y 
#define SCX_LOC 0xFF43               // scroll x
#define LY_LOC 0xFF44                // current scanline y value
#define LYC_LOC 0xFF45               // scanline compare register
#define JOYP_LOC 0xFF00              // joypad input
#define BG_PALETTE_LOC 0xFF47        // background palette 
#define OBP0_LOC 0xFF48              // sprite palette 2
#define OBP1_LOC 0xFF49              // sprite palette 1
#define WX_LOC 0xFF4B                  // start of window x
#define WY_LOC 0xFF4A                  // start of window y
#define SB_LOC 0xFF01                // serial buffer
#define SC_LOC 0xFF02                // serial control
#define OAM_DMA_LOC 0xFF46               // OAM DMA start loc
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
    uint8_t lyc;               // LYC (used for ly compare interrupts)
    uint8_t background_palette; // (W located on 0xFF47)
    uint8_t obp0;              // OBP0 object palette 0 (located on 0xFF48) 
    uint8_t obp1;              // OBP1 object palette 1 (located on 0xFF49) 
    uint8_t wx;                // window x position + 7
    uint8_t wy;                // window y position
    
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
