#ifndef CPU_H
#define CPU_H
#include "stdbool.h"
#include "registers.h"
#include "stdint.h"
#include "memory.h"
typedef struct Cpu {
    Registers reg;
    Memory mem;
    bool inBios;
} Cpu;

// Handles one round of fetch/decode/execute
// Returns the number of machine cycles taken
// 4 clock cycles == 1 machine cycle
int tick(Cpu* cpu);
// Resets the cpu to the starting state, clearing all registers etc
void reset_cpu(Cpu *cpu);
#endif