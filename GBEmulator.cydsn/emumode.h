/*
This file contains precompiler definitions that change the function of the emulator 
*/
#ifndef EMU_MODE_H
#define EMU_MODE_H
#define DEBUG_MODE false
#define DEBUG_BREAKPOINT_ON true
#define DEBUG_BREAKPOINT 0xFA
#define DEBUG_MEMORY_DISPLAY_LOC 0x9910
    
#define TETRIS 0
#define TEST_ROM_CPU_INSTRS_1 1
    
// ROM selection
#define ROM TEST_ROM_CPU_INSTRS_1
#endif