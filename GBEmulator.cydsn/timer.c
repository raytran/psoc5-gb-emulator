#include "timer.h"
void setup_timer(Timer* timer, Memory* mem){
    timer->mem = mem;
}
void tick_timer(Timer* timer, uint8_t delta_machine_cycles){
    timer->internal_clock += delta_machine_cycles;
    // Keep ticking the timer until the internal clock < 4
    // It is possible for ticks to take >4 machine cycles when RSTs occur
    while (timer->internal_clock >= 4) {
        // the DIV register is ALWAYS counting!
        timer->internal_clock -= 4;
        timer->divclock++;
        if (timer->divclock == 16){
            // the DIV register is incremented once every 16*4 m-cycles
            timer->mem->timer_divider++;
            timer->divclock = 0;
        }
        
        // Check if timers are enabled
        uint8_t timer_control = timer->mem->timer_control;
        //Bit  2   - Timer Enable
        //Bits 1-0 - Input Clock Select
        //   00: CPU Clock / 1024 =  4096 Hz   = once every 4*64 m-cycles = 64 timer.base_clock s
        //   01: CPU Clock / 16   =  262144 Hz = once every 4 m-cycles =   1 timer.base_clock s
        //   10: CPU Clock / 64   =  65536 Hz  = once every 4*4 m-cycles = 4 timer.base_clock s
        //   11: CPU Clock / 256  =  16384 Hz  = once every 4*16 m-cycles = 16 timer.base_clock s
        
        if (timer_control & 100){
            timer->baseclock++; // the fastest base clock speed is once every 4 m-cycles
            int base_clock_threshold;
            switch (timer_control & 3){
                case 0:
                base_clock_threshold = 64;
                break;
                case 1:
                base_clock_threshold = 1;
                break;
                case 2:
                base_clock_threshold = 4;
                break;
                case 3:
                base_clock_threshold = 16;
                break;
            }
            
            // Time to increment TIMA?
            if (timer->baseclock >= base_clock_threshold) {
                timer->baseclock = 0;
                
                if (timer->mem->timer_counter == 0xFF) {
                    // This increment will cause an overflow; request interrupt
                    timer->mem->interrupt_flag |= INTERRUPT_ENABLE_TIMER_MASK;
                    // And refill wil the timer modulo value
                    timer->mem->timer_counter = timer->mem->timer_modulo;
                } else {
                    timer->mem->timer_counter++;
                }
            }
        }
    }
}