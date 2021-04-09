#ifndef ROM_H
#define ROM_H
#include "stdint.h"
#define BIOS_SIZE 256
extern const uint8_t bios[256];
extern const uint8_t rom[0x8000];
#endif
