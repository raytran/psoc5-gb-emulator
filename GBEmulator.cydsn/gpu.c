#include "gpu.h"
#include "tft.h"
#include "emumode.h"
const uint16_t COLORS[4]  = {0xF800, 0xF80F, 0x00DD, 0xFFEE};

#define VBLANK_MODE 1
#define HBLANK_MODE 0
#define OAM_MODE 2
#define PIXEL_TRANSFER_MODE 3
#define OAM_READ_TIME_MACHINE_CYCLES 20   //80 clock cycles avg => 80/4 = 20 m cycles
#define VRAM_READ_TIME_MACHINE_CYCLES 43  //172 clock cycles => 43 m cycles
#define HBLANK_TIME_MACHINE_CYCLES 51     //204 clock cycles => 51 m cycles
#define ONE_LINE_TIME_MACHINE_CYCLES 114
#define VBLANK_TIME_MACHINE_CYCLES 1140   //4560 clock cycles => 1140 m cycles

//Called at the end of every PIXEL_TRANSFER_MODE
// Renders the current line
void renderLine(Memory* mem){
    
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
    
    //write8_a0(0x00);                    // send NOP command to end writing process
}


void tick_gpu(Gpu* gpu, uint8_t delta_machine_cycles){
    // TODO Check LCDC register
    Memory* mem = gpu->mem;
    
    // Basic function of the GPU:
    // Mode 0: HBLANK
    // Mode 1: VBLANK
    // Mode 2: Searching OAM
    // Mode 3: Searching VRAM/Pixel transfer
    // Mode 2 -> Mode 3 -> Mode 0  until the last line is reached
    // Then Mode 1 and reset
    
    
    uint8_t original_mode = gpu->mode;
    gpu->mode_clock += delta_machine_cycles;
    
    switch (original_mode) {
        // OAM Read, scanline
        case OAM_MODE:
        if (gpu->mode_clock >= OAM_READ_TIME_MACHINE_CYCLES) {
            // mode switch to VRAM read/pixel transfer (mode 3)
            gpu->mode_clock = 0;
            gpu->mode = PIXEL_TRANSFER_MODE;
        }
        break;
        // VRAM Read, scanline active
        case PIXEL_TRANSFER_MODE:
        if (gpu->mode_clock >= VRAM_READ_TIME_MACHINE_CYCLES){
            // mode switch to HBlank
            gpu->mode_clock = 0;
            gpu->mode = HBLANK_MODE;
            
            //Draw a full line
            if (!DEBUG_MODE)
                renderLine(mem);
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
            }else{
                gpu->mode = OAM_MODE;
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
                
                if (!DEBUG_MODE){
                    write8_a0(0x00);                    // send NOP command to end the last writing process
                    write8_a0(0x2C);                    // send Memory Write command to start a new one
                }
            }
        }
        break;
    }
    
    
    
    
    
   
}