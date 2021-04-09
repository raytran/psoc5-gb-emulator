#ifndef MEMORY_H
#define MEMORY_H
#include "stdint.h"
#include "stdbool.h"
#define WRAM_SIZE 0x2000
#define VRAM_SIZE 0x1000
typedef struct Memory {
    uint8_t wram[WRAM_SIZE];   // work ram
    uint8_t vram[VRAM_SIZE];   // video ram
} Memory;
// Fetch a byte from memory
uint8_t fetch(Memory* memory, uint16_t address, bool inBios);
// Write a byte into memory
void write_mem(Memory* memory, uint16_t address, uint8_t data);
// Reset memory back to 0s
void reset_memory(Memory* memory);

#endif
