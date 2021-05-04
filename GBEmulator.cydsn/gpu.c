#include "gpu.h"
#include "tft.h"
#include "emumode.h"
#include "stdio.h"
#include "stdlib.h"
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


#define LINE_SPI_DMA_BUFFER_SIZE 320    // Each pixel is 2 bytes => 160 * 2 = 320
uint8_t line_spi_dma_buffer[LINE_SPI_DMA_BUFFER_SIZE];   // dma data transfer buffer

static inline int8_t safe_convert(uint8_t x) {
    return x < 128 ? x : x - 256;
}
// Maps pixel index to its actual color index through the color register
// color registers come from either the BGP register @0xFF47
// or OBP0/OBP1 for sprites
static inline uint8_t color_index_from_pxindex(uint8_t color_reg, uint8_t pxindex){
    switch(pxindex){
        case 0:
            return color_reg & 0b11;
        case 1:
            return (color_reg >> 2) & 0b11;
        case 2:
            return (color_reg >> 4) & 0b11;
        case 3:
            return (color_reg >> 6) & 0b11;
        default:
            return 0;
    }
}
void setup_gpu(Gpu* gpu, Memory* mem){
    if (!DEBUG_MODE){
        // Initialize DMA here
        setupDma(&line_spi_dma_buffer[0], LINE_SPI_DMA_BUFFER_SIZE);
    }
    gpu->mem = mem;
}


static inline void write_colorindx_to_dma_buff(uint8_t colorindex, int x_coord){
    switch (colorindex){
        case 0:
            line_spi_dma_buffer[2 * x_coord] = 0xFF;
            line_spi_dma_buffer[2 * x_coord + 1] = 0xFF;
        break;
        case 1:
            line_spi_dma_buffer[2 * x_coord] = 0xC6;
            line_spi_dma_buffer[2 * x_coord + 1] = 0x18;
        break;
        case 2:
            line_spi_dma_buffer[2 * x_coord] = 0x7B;
            line_spi_dma_buffer[2 * x_coord + 1] = 0xEF;
        break;
        case 3:
            line_spi_dma_buffer[2 * x_coord] = 0x00;
            line_spi_dma_buffer[2 * x_coord + 1] = 0x00;
        break;
    }
}

// Renders one of the 40 sprites in OAM on the current scan line, assuming it is possible
// i.e. requires (sprite_y <= mem->current_scan_line && (sprite_y + 8) > mem->current_scan_line) in 8x8 mode
//            or (sprite_y <= mem->current_scan_line && (sprite_y + 16) > mem->current_scan_line) in 8x16 mode
static void render_sprite_on_scanline(Gpu* gpu, Memory* mem, uint8_t sprite_num, bool obj_size8x16){
    int sprite_offset = sprite_num * 4; //4 bytes per sprite
    int sprite_y = (int) mem->oam[sprite_offset] - 16;     // first byte is y  + 16
    int sprite_x = (int) mem->oam[sprite_offset + 1] - 8; //second byte is x + 8
    int sprite_tile_indx = mem->oam[sprite_offset + 2]; //third byte is tile index
    int sprite_attributes_flag = mem->oam[sprite_offset + 3];
    
    // Bit7   BG and Window over OBJ (0=No, 1=BG and Window colors 1-3 over the OBJ)
    // Bit6   Y flip                 (0=Normal, 1=Vertically mirrored)
    // Bit5   X flip                 (0=Normal, 1=Horizontally mirrored)
    // Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
    bool bg_has_priority = sprite_attributes_flag & 0x80;
    bool y_flip = sprite_attributes_flag & 0x40;
    bool x_flip = sprite_attributes_flag & 0x20;
    bool pallete_obp1 = sprite_attributes_flag & 0x10;
  
    
    // Ignore 0th bit for 8x16 sized sprites
    if (obj_size8x16){
        sprite_tile_indx &= ~0b1;
    }

    
    
    int tile_start_addr = sprite_tile_indx * 16; //each tile is 16 bytes
    int tile_y_offset = mem->current_scan_line - sprite_y;
    if (y_flip) tile_y_offset = (obj_size8x16 ? 15 : 7) - tile_y_offset;
    // finally index at the correct 2 bytes for this row
    
    uint8_t low = mem->vram[tile_start_addr + 2 * tile_y_offset];  
    uint8_t high = mem->vram[tile_start_addr + 2 * tile_y_offset + 1];
    
    
    uint8_t sprite_color_palette = pallete_obp1 ? mem->obp1 : mem->obp0;
    int x = sprite_x;
    int j;
    for (j=7;j>=0;j--){
        if (x >= 0 && x < DISPLAY_WIDTH) {
            int8_t pxindex = ((high >> j & 0x1) << 1) | ((low >> j) & 0x1);
            if (x_flip){
                pxindex = ((high >> (7-j) & 0x1) << 1) | ((low >> (7-j)) & 0x1);
            }
            
            if (pxindex != 0  //0 for sprites means the pixel is transparent
                // check background vs sprite priority
                && (!bg_has_priority || (bg_has_priority && gpu->line_bg_px_indx_buffer[x] == 0))){
                    
                // finally draw the pixel
                uint8_t color_index = color_index_from_pxindex(sprite_color_palette, pxindex);
                write_colorindx_to_dma_buff(color_index, x);    
            }  
        }
        x++;   
    }
}


//Called at the end of every PIXEL_TRANSFER_MODE
// Renders the current line

// the following arrays are used to sort sprites (handles priority)
int sprite_nums_to_display[10];  // used to hold sprites for sorting
int sprite_nums_to_x_coord[40];

// Comparator that gives correct priority sorting
int compare_sprite_nums_priority(const void * a, const void * b)
{
  int sprite_num_1 = *(int *)a, sprite_num_2 = *(int *)b;
  if(sprite_nums_to_x_coord[sprite_num_1] > sprite_nums_to_x_coord[sprite_num_2]) // a < b
    return -1;
  else if(sprite_nums_to_x_coord[sprite_num_1] == sprite_nums_to_x_coord[sprite_num_2]) // a == b
    // break ties by choosing the sprite that comes first in oam
    return sprite_num_2 - sprite_num_1;
  else 
    return 1;  // a > b
}

void renderLine(Gpu* gpu, Memory* mem){
    
    bool lcd_enable = (mem->lcdc &          0b10000000) != 0; // 7 LCD and PPU enable 	0=Off, 1=On
    bool window_map_area_1 = (mem->lcdc &   0b01000000) != 0; // Window tile map area 	0=9800-9BFF, 1=9C00-9FFF
    bool window_enable = (mem->lcdc &       0b00100000) != 0; // Window enable 	0=Off, 1=On 
    bool data_area_1 = (mem->lcdc &         0b00010000) != 0; // BG and Window tile data area 	0=8800-97FF, 1=8000-8FFF
    bool bg_map_area_1 = (mem->lcdc &       0b00001000) != 0; // BG tile map area 	0=9800-9BFF, 1=9C00-9FFF
    bool obj_size8x16 = (mem->lcdc &        0b00000100) != 0; // OBJ size 	0=8x8, 1=8x16
    bool obj_enable = (mem->lcdc &          0b00000010) != 0; // OBJ enable 	0=Off, 1=On
    bool bg_window_enable = (mem->lcdc &    0b00000001) != 0; // BG and Window enable/priority 	0=Off, 1=On
    
    if (!lcd_enable) return;
    
    // wait until we can modify the line buffer
    while (!isDmaReady()){};
    
    // BACKGROUND AND WINDOW
    if (bg_window_enable){
        // The two background maps are located at 9800h-9BFFh and 9C00h-9FFFh
        // Each bg map is 32x32 tiles, for a total of 256x256 pixels
        // We can access this as either          vram[0x1800:] or vram[0x1C00:]
        int bg_map_offset = bg_map_area_1 ? 0x1C00 : 0x1800;

        // True y adjusted for scrolling
        int bg_y = mem->current_scan_line + mem->scroll_y;
        // Each "y" is worth 8 pixels, so divide by 8 to get num tiles
        // Then multiply by 32 since we have 32 tiles per row
        int bgmap_row_start = bg_map_offset + ((bg_y/8) % 32) * 32;
        int bg_tile_line = bg_y % 8; //y % 8 gives us the specific line in the tile to show
     
        
        int i;
        int x = 0;
        for (i=0;i<DISPLAY_WIDTH/8;i++){    //DISPLAY_WIDTH/8 = 20 total tiles on the line
            // Which tile are we on?
            uint8_t tile_id_u8 = mem->vram[bgmap_row_start + ((i + mem->scroll_x/8) & 0x1F)];
            int tile_id = (int32_t) tile_id_u8;
            if (!data_area_1){ //bit 4 in LCDC specifies whether or not to use 0x8000 base addr unsigned
                // when bit 4 is low, 9000 is the base pointer and the index is signed
                if (tile_id < 128) tile_id  += 256;
            }
           

            int tile_start_addr = tile_id * 16; //each tile is 16 bytes            
            // finally index at the correct 2 bytes for this row
            uint8_t low = mem->vram[tile_start_addr + 2 * bg_tile_line];  
            uint8_t high = mem->vram[tile_start_addr + 2 * bg_tile_line + 1];
            // Finally print the line
            int j;
            for (j=7;j>=0;j--){
                uint8_t pxindex = ((high >> j & 0x1) << 1) | ((low >> j) & 0x1);
                // Save the background pixel index info for later use in sprite priority
                gpu->line_bg_px_indx_buffer[x] = pxindex;
                uint8_t color_index = color_index_from_pxindex(mem->background_palette, pxindex);
                write_colorindx_to_dma_buff(color_index, x);
                x++;   
            }
        }
        
        // WINDOW DRAWING
        if (window_enable && mem->wy <= mem->current_scan_line && mem->wx - 7 < DISPLAY_WIDTH){
            int window_map_offset = window_map_area_1 ? 0x1C00 : 0x1800;        
            int windowmap_row_start = window_map_offset + (((gpu->window_ly)/8) % 32) * 32;
            int window_tile_line = (gpu->window_ly) % 8;
      
            
       
            x = mem->wx - 7;
           
            while (x < DISPLAY_WIDTH) {
                // Which tile are we on?
                uint8_t tile_id_u8 = mem->vram[windowmap_row_start + ((x - (mem->wx - 7))/8)];
                int tile_id = (int32_t) tile_id_u8;
                if (!data_area_1){ //bit 4 in LCDC specifies whether or not to use 0x8000 base addr unsigned
                    // when bit 4 is low, 9000 is the base pointer and the index is signed
                    if (tile_id < 128) tile_id  += 256;
                }
               

                int tile_start_addr = tile_id * 16; //each tile is 16 bytes            
                // finally index at the correct 2 bytes for this row
                uint8_t low = mem->vram[tile_start_addr + 2 * window_tile_line];  
                uint8_t high = mem->vram[tile_start_addr + 2 * window_tile_line + 1];
                int j;
                for (j=7;j>=0;j--){
                    if (x >= 0 && x < DISPLAY_WIDTH) {
                        int8_t pxindex = ((high >> j & 0x1) << 1) | ((low >> j) & 0x1);
                        // Save the background pixel index info for later use in sprite priority
                        gpu->line_bg_px_indx_buffer[x] = pxindex;
                        // finally draw the pixel
                        uint8_t color_index = color_index_from_pxindex(mem->background_palette, pxindex);
                        write_colorindx_to_dma_buff(color_index, x);    
                        
                    }
                    x++;   
                }
            }

            // the window maintains its own internal ly that is only incremented when it is drawn
            gpu->window_ly++;
        }
    }
 
    
    
    //////////// SPRITES
    if (obj_enable) {
        // Search through the 40 sprites to see which ones we can render
        // (valid x and y)
        int sprite_display_count = 0;
        int sprite_num;
        for (sprite_num=0;sprite_num<40;sprite_num++){
            int sprite_offset = sprite_num * 4; //4 bytes per sprite
            int sprite_y = (int) mem->oam[sprite_offset] - 16;     // first byte is y  + 16
            int sprite_x = (int) mem->oam[sprite_offset + 1] - 8; //second byte is x + 8
            sprite_nums_to_x_coord[sprite_num] = sprite_x;
            // Only render sprites that lie on the scan line
            if (sprite_y <= mem->current_scan_line && (sprite_y + (obj_size8x16 ? 16 : 8)) > mem->current_scan_line){
                sprite_nums_to_display[sprite_display_count] = sprite_num;
                sprite_display_count++;
                if (sprite_display_count >= 10) break;         // can only draw 10 sprites per line       
            }
        }
        
        // Sort the sprite nums in order of increasing x value (priority)
        qsort(sprite_nums_to_display, sprite_display_count, sizeof(int), compare_sprite_nums_priority);
        
        int i;
        for (i = 0; i < sprite_display_count; i++){
            render_sprite_on_scanline(gpu, mem, sprite_nums_to_display[i], obj_size8x16);   
        }
        
    }

    
    // Finally send the line buffer over SPI
    startDmaTransfer();
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
                renderLine(gpu, mem);
        }
        break;
        // HBlank
        case HBLANK_MODE:
        if (gpu->mode_clock >= HBLANK_TIME_MACHINE_CYCLES) {
            
            
            gpu->mode_clock = 0;
            mem->current_scan_line++;
            // Check for LY==LYC 
            if (mem->current_scan_line == mem->lyc){
                // Check for LY==LYC interrupts
                if(mem->lcdstatus & LCD_STAT_LY_LYC_INTERRUPT_REG_MASK){
                    mem->interrupt_flag |= INTERRUPT_ENABLE_STAT_MASK;
                }
                mem->lcdstatus |= LCD_STAT_LY_LYC_EQ_REG_MASK;
            } else {
                mem->lcdstatus &= ~LCD_STAT_LY_LYC_EQ_REG_MASK;
            }
            
            // Last scan line done!
            if (mem->current_scan_line == DISPLAY_HEIGHT){
                
                // change to vblank
                gpu->mode = VBLANK_MODE;
                // request interrupt
                mem->interrupt_flag |= INTERRUPT_ENABLE_VBLANK_MASK;
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
                gpu->window_ly = 0;
                
                if (!DEBUG_MODE){
                    while (!isDmaReady()){};            // wait for DMA stuff to finish
                    write8_a0(0x00);                    // send NOP command to end the last writing process
                    write8_a0(0x2C);                    // send Memory Write command to start a new one
                    setDChigh();                        // set DC line high to start sending data instead of cmds
                }
            }
        }
        break;
    }
}