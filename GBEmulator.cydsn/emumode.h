/*
This file contains precompiler definitions that change the function of the emulator 
*/
#ifndef EMU_MODE_H
#define EMU_MODE_H

#define GB_SERIAL_PASSTHROUGH false        // whether or not to pass through GB serial 
    
#define DEBUG_MODE false
    
#define DEBUG_MODE_STUB_LY_0x90 false       //stubs the LY register to value 0x90
#define DEBUG_TRACE_THROUGH_SERIAL false    // enable trace mode over serial 
#define DEBUG_TRACE_THROUGH_SERIAL_BREAKPOINT 0x0100   // where to start serial trace
    
#define DEBUG_BREAKPOINT_ON false
#define DEBUG_BREAKPOINT 0x0100
#define DEBUG_MEMORY_DISPLAY_LOC 0x9800
#define DEBUG_SHOW_VRAM_ON_BUTTON false
 
// ROM list
#define TETRIS 0
#define DR_MARIO 14
// CPU Test ROMS
#define TEST_ROM_CPU_INSTRS_1_SPECIAL 1             // Passed
#define TEST_ROM_CPU_INSTRS_2_INTERRUPTS 2          // Fail
#define TEST_ROM_CPU_INSTRS_3_OP_SP_HL 3            // Passed
#define TEST_ROM_CPU_INSTRS_4_OP_R_IMM 4            // Passed
#define TEST_ROM_CPU_INSTRS_5_OP_RP 5               // Passed
#define TEST_ROM_CPU_INSTRS_6_LD_R_R 6              // Passed
#define TEST_ROM_CPU_INSTRS_7_JR_CALL_RET_RST 7     // Passed
#define TEST_ROM_CPU_INSTRS_8_MISC_INSTRS 8         // Passed
#define TEST_ROM_CPU_INSTRS_9_OP_R_R 9              // Passed
#define TEST_ROM_CPU_INSTRS_10_BIT_OPS 10           // Passed
#define TEST_ROM_CPU_INSTRS_11_OP_A_MHL 11          // Passed
// Display Test ROMS
#define TEST_DMG_ACID_2 12                          // Passed
#define TEST_WINDOW_SCROLLING 13
    
#define CUSTOM_TEST 99
// ROM selection
#define ROM TETRIS

    
#define START_IN_BIOS true
#endif