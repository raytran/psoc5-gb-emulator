#ifndef Gpu_H
#define Gpu_H
#include "memory.h"
#include "stdint.h"    
#define DISPLAY_WIDTH 160
#define DISPLAY_HEIGHT 144

extern const uint16_t COLORS[4]; 
typedef struct Gpu {
    Memory* mem;
    uint32_t mode_clock;
    uint8_t mode;       // current mode of the CPU; 0 1 2 or 3
    // holds background data for current scanline; used for sprite priority
    uint8_t line_bg_px_indx_buffer[DISPLAY_WIDTH];  
    uint8_t window_ly;  // the window maintains its own internal ly that is only incremented when it is displayd
} Gpu;
// Initializes GPU
void setup_gpu(Gpu* gpu, Memory* mem);
// processes the next tick of the GPU
// Takes in the # of machine cycles that elapsed
void tick_gpu(Gpu* gpu, uint8_t delta_machine_cycles);
// Renders the current line at mem->current_scan_line
void renderLine(Gpu* gpu, Memory* mem);

#endif