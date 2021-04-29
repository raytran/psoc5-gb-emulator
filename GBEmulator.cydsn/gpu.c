#include "gpu.h"
#include "tft.h"

const uint16_t COLORS[4]  = {0xF800, 0xF80F, 0x00DD, 0xFFEE};

#define VBLANK_MODE 1
#define HBLANK_MODE 0
#define OAM_MODE 2
#define VRAM_MODE 3
#define OAM_READ_TIME_MACHINE_CYCLES 20   //80 clock cycles avg => 80/4 = 20 m cycles
#define VRAM_READ_TIME_MACHINE_CYCLES 43  //172 clock cycles => 43 m cycles
#define HBLANK_TIME_MACHINE_CYCLES 51     //204 clock cycles => 51 m cycles
#define ONE_LINE_TIME_MACHINE_CYCLES 114
#define VBLANK_TIME_MACHINE_CYCLES 1140   //4560 clock cycles => 1140 m cycles
void tick_gpu(Gpu* gpu, Memory* mem, uint8_t delta_machine_cycles){
    // TODO Check LCDC register
    
    
    // Basic function of the GPU:
    // Mode 0: HBLANK
    // Mode 1: VBLANK
    // Mode 2: Searching OAM
    // Mode 3: Searching VRAM
    // Mode 2 -> Mode 3 -> Mode 0  until the last line is reached
    // Then Mode 1 and reset
    
    
    uint8_t original_mode = gpu->mode;
    gpu->mode_clock += delta_machine_cycles;
    
    switch (original_mode) {
        // OAM Read, scanline
        case OAM_MODE:
        if (gpu->mode_clock >= OAM_READ_TIME_MACHINE_CYCLES) {
            // mode switch to VRAM read (mode 3)
            gpu->mode_clock = 0;
            gpu->mode = VRAM_MODE;
        }
        break;
        // VRAM Read, scanline active
        case VRAM_MODE:
        if (gpu->mode_clock >= VRAM_READ_TIME_MACHINE_CYCLES){
            // mode switch to HBlank
            gpu->mode_clock = 0;
            gpu->mode = HBLANK_MODE;
        }
        break;
        // HBlank
        case HBLANK_MODE:
        if (gpu->mode_clock >= HBLANK_TIME_MACHINE_CYCLES) {
            gpu->mode_clock = 0;
            mem->current_scan_line++;
            // Last scan line done!
            if (mem->current_scan_line == DISPLAY_HEIGHT){
                // change to vblank
                gpu->mode = VBLANK_MODE;
            }
            // mode 
        }
        break;
        // VBlank
        case VBLANK_MODE:
        if (gpu->mode_clock >= ONE_LINE_TIME_MACHINE_CYCLES) {
            gpu->mode_clock = 0;
            mem->current_scan_line++;
            // Reset to OAM mode
            if (mem->current_scan_line > 153){
                gpu->mode = OAM_MODE;
                mem->current_scan_line = 0;
            }
        }
        break;
    }
    
    
    
    
    
   
}
void renderLine(Gpu* gpu, Memory* mem){
    write8_a0(0x2C);                    // send Memory Write command
    int i;
    for (i=0;i<8;i++){
        
    }
}

void writeLine(){

    int j;
    
    int i;
    for (j=0;j<DISPLAY_HEIGHT;j++){
        for (i=0; i<DISPLAY_WIDTH; i++)               // fill the square with the color orange
    	{
           uint16_t color = 0;
           write8_a1(color & 0xFF00);     // write the color stored in the frame buffer
           write8_a1(color & 0x00FF);
    	}
    
        for (i=0;i<DISPLAY_WIDTH;i++){
           uint16_t color = COLORS[1];
           write8_a1(color & 0xFF00);     // write the color stored in the frame buffer
           write8_a1(color & 0x00FF);
        }
    
    }
    
    write8_a0(0x00);                    // send NOP command to end writing process
}