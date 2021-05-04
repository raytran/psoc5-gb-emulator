#include "mmio.h"
#include <project.h>
#include "stdio.h"
#include "stdbool.h"

void setup_mmio(Mmio* mmio, Memory* mem){
    mmio->mem = mem;
}
void tick_mmio(Mmio* mmio) {
    // Do stuff here
    int joyx = ADC_JOY_X_GetResult16();
    int joyy = ADC_JOY_Y_GetResult16();
    uint8_t buttons = Button_Status_Read();
    bool button1 = !(buttons & 0b0001);
    bool button2 = !(buttons & 0b0010);
    bool joy_sw = !(buttons &  0b0100);
    bool button3 = !(buttons & 0b1000);
    
    //char buff[100];
    //sprintf(buff, "JOYX: %d, JOYY:%d, BUTTON1: %d, BUTTON2: %d, JOYSW: %d, BUTTON3: %d\n\r", joyx, joyy, button1, button2, joy_sw, button3);
    //UART_1_PutString(buff);
    
    
    // Map inputs to gameboy inputs
    bool left_pushed = joyx > 3700;
    bool right_pushed = joyx < 500;
    bool up_pushed = joyy < 500;
    bool down_pushed = joyy > 3700;
    bool a_pushed = button1;
    bool b_pushed = button2;
    bool start_pushed = joy_sw;
    bool select_pushed = button3;
    
    // Write to memory location 0xFF00
    //Bit 7 - Not used
    //Bit 6 - Not used
    //Bit 5 - P15 Select Action buttons    (0=Select)
    //Bit 4 - P14 Select Direction buttons (0=Select)
    //Bit 3 - P13 Input: Down  or Start    (0=Pressed) (Read Only)
    //Bit 2 - P12 Input: Up    or Select   (0=Pressed) (Read Only)
    //Bit 1 - P11 Input: Left  or B        (0=Pressed) (Read Only)
    //Bit 0 - P10 Input: Right or A        (0=Pressed) (Read Only)
    //uint8_t selected = fetch(mmio->mem, 0xFF00, false);
    uint8_t selected = mmio->mem->joyp;
    uint8_t to_write = 0;
    if (((selected >> 5) & 0b1) == 0) {
        // action button selected
        // direction button selected
        to_write |= !a_pushed;
        to_write |= (!b_pushed) << 1;
        to_write |= (!select_pushed) << 2;
        to_write |= (!start_pushed) << 3;
        //write_mem(mmio->mem, 0xFF00, to_write);
        mmio->mem->joyp = to_write;
    } else if (((selected >> 4) & 0b1) == 0) {
        // direction button selected
        to_write |= !right_pushed;
        to_write |= (!left_pushed) << 1;
        to_write |= (!up_pushed) << 2;
        to_write |= (!down_pushed) << 3;
        //write_mem(mmio->mem, 0xFF00, to_write);
        mmio->mem->joyp = to_write;
    } else {
        //write_mem(mmio->mem, 0xFF00, 0xFF);
        mmio->mem->joyp = to_write;
    }
}