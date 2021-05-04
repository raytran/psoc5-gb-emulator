/*
This struct handles mapping physical hardware on the PSOC to memory locations
*/
#ifndef MMIO_H
#define MMIO_H
#include "memory.h"
typedef struct Mmio {
    Memory* mem;
} Mmio;
void setup_mmio(Mmio* mmio, Memory* mem);
void tick_mmio(Mmio* mmio);
#endif