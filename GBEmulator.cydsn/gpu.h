#ifndef Gpu_H
#define Gpu_H
#include "memory.h"
#include "stdint.h"    
#define FRAMEBUFFER_SIZE 23040  //Game Boy had a 160x144 display = 23040 total pixels

extern const uint16_t COLORS[4]; 
typedef struct Gpu {
    Memory* mem;
    uint8_t framebuffer[FRAMEBUFFER_SIZE];   // each item in the framebuffer is an index in COLORS
} Gpu;

// Updates the frame buffer by looking at VRAM
void update_framebuffer(Gpu* gpu, Memory* mem);
// Draws the contents of the frame buffer to the display
void draw(Gpu* gpu);
#endif