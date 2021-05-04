/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include "cpu.h"
#include "stdio.h"

#include "rom.h"
#include "GUI.h"
#include "gpu.h"
#include "tft.h"
#include "debugfuncs.h"
#include "emumode.h"
#include "mmio.h"


Cpu cpu;
Gpu gpu;
Memory mem;
Mmio mmio;
unsigned long total_cycles = 0;
unsigned long total_instrs = 0;
char buffer[500];

double seconds = 0;


CY_ISR(Timer_1_Handler){
    /*
    sprintf(buffer, "On-time (sec): %d  \n"
        "Instrs/second: %lu \n"
        "Cycles/second: %lu \n"
        "Machine Cycles/second:\n %lu\n PC: %x", (int) seconds, total_instrs/4, total_cycles, total_cycles/4, cpu.reg.pc);
    total_cycles = 0;
    total_instrs = 0;
    
    GUI_DispStringAt(buffer, 0, 0); 
    seconds += 4;
    Timer_1_ReadStatusRegister(); //Clear timer register to leave interrupt
    */
}

bool debug_trace_through_serial_on = false;
void tick_all(){
    if (DEBUG_MODE && DEBUG_TRACE_THROUGH_SERIAL){
        if (debug_trace_through_serial_on || cpu.reg.pc >= DEBUG_TRACE_THROUGH_SERIAL_BREAKPOINT){
            debug_trace_through_serial_on = true;
            debug_fmt_cpu_trace(buffer, &cpu, &mem, total_instrs, total_cycles);
            UART_1_PutString(buffer);
        }
    }
    int cycles_taken = tick(&cpu);
    if (cpu.inBios){
        cpu.inBios = (fetch(&mem, 0xFF50, cpu.inBios) == 0);
    }
    tick_mmio(&mmio);
    tick_gpu(&gpu, cycles_taken);
    total_cycles += cycles_taken;
    total_instrs++;
}
CY_ISR(button_press_1_handler){
    if (DEBUG_MODE){
        if (DEBUG_SHOW_VRAM_ON_BUTTON){
            debug_show_full_vram(&mem);
        } else {
            tick_all(); 
            debug_fmt_cpu_state(buffer, &cpu, &mem, total_instrs, total_cycles, DEBUG_MEMORY_DISPLAY_LOC);
            GUI_DispStringAt(buffer, 0, 0);
        }
    }
}


int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

                
        
    button_press_1_StartEx(button_press_1_handler);
    
    SPIM_1_Start(); 
    SPIM_1_ClearFIFO();
    UART_1_Start();
    
    
    ADC_JOY_X_Start();
    ADC_JOY_Y_Start();
    ADC_JOY_Y_StartConvert();
    ADC_JOY_X_StartConvert();
    
    CyDelay(500);
    UART_1_PutString("Hello from the PSOC GB Emulator\r\n");
    if (DEBUG_MODE){
        // Only use emWin to print text in debug mode
        GUI_Init();
        GUI_Clear();
        GUI_SetFont(&GUI_Font8x16);
        
      
        /*
        // Setup timer interrupt for performance timing
        Timer_1_interrupt_StartEx(Timer_1_Handler);
        Timer_1_Start();
        */
    } else {
        tftStart();    // initialize the TFT display
        uint16 SC = 0;                     
        uint16 EC = 159;
        uint16 SP = 0;
        uint16 EP = 143;
        write8_a0(0x2A);                 	// send Column Address Set command
        write8_a1(SC >> 8);                 // set SC[15:0]
        write8_a1(SC & 0x00FF);
        write8_a1(EC >> 8);                 // set EC[15:0]
        write8_a1(EC & 0x00FF);
        write8_a0(0x2B);                 	// send Page Address Set command
        write8_a1(SP >> 8);                 // set SP[15:0]
        write8_a1(SP & 0x00FF);
        write8_a1(EP >> 8);                 // set EP[15:0]
        write8_a1(EP & 0x00FF);

    }
    
    cpu.mem = &mem;
    setup_mmio(&mmio, &mem);
    setup_gpu(&gpu, &mem);
  
    reset_cpu(&cpu);
    reset_memory(&mem);
    cpu.inBios = START_IN_BIOS;
    

    if (DEBUG_MODE){
        for (;;){
            if (!DEBUG_BREAKPOINT_ON || cpu.reg.pc == DEBUG_BREAKPOINT){
                debug_fmt_cpu_state(buffer, &cpu, &mem, total_instrs, total_cycles, DEBUG_MEMORY_DISPLAY_LOC);
                GUI_DispStringAt(buffer, 0, 0);
                break;
            }
            tick_all();
        }
    } else {
        
        for(;;)
        {
            tick_all();
        }
       
    }


}

/* [] END OF FILE */
