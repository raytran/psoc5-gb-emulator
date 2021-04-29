#ifndef DEBUG_FUNCS_H
#define DEBUG_FUNCS_H
#include "memory.h"
#include "cpu.h"
//Prints the contents of the current tileset to the display
void debug_show_tileset(Memory* mem);

// Formats the state of the CPU into a string returnBuffer
void debug_fmt_cpu_state(char* returnBuffer, Cpu* cpu, Memory* mem, unsigned long total_instrs, unsigned long total_cycles);
#endif