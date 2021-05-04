#ifndef TIMER_H
#define TIMER_H
#include "memory.h"
typedef struct Timer {
    Memory* mem;
    int internal_clock;    //internal clock counting elapsed m-cycles
    int baseclock; // fastest timer speed; increments every 4 m-cycles
    int divclock;  // DIV register increments at 1/16th the rate of a regular increment (4*16 m-cycles)
} Timer;

// Initializes a new timer
void setup_timer(Timer* timer, Memory* mem);

// processes the next tick of the timer
// Takes in the # of machine cycles that elapsed
void tick_timer(Timer* timer, uint8_t delta_machine_cycles);
    
#endif