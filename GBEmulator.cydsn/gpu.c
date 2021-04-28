#include "gpu.h"
#include "tft.h"

const uint16_t COLORS[4]  = {0xF800, 0xFF11, 0x00DD, 0xFFEE};

void update_framebuffer(Gpu* gpu, Memory* mem){
    int i;
    for (i=0;i<FRAMEBUFFER_SIZE;i++){
        gpu->framebuffer[i] = 2;
    }
}

void draw(Gpu* gpu){
    write8_a0(0x2C);                    // send Memory Write command
    int i;
    for (i=0; i<FRAMEBUFFER_SIZE; i++)               // fill the square with the color orange
	{
       uint16_t color = COLORS[gpu->framebuffer[i]];
       write8_a1(color & 0xFF00);     // write the color stored in the frame buffer
       write8_a1(color & 0x00FF);
	}
    write8_a0(0x00);                    // send NOP command to end writing process
}