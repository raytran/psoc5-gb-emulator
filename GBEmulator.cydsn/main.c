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

Cpu cpu;
unsigned long total_cycles = 0;
unsigned long total_instrs = 0;
char buffer[50];

int seconds = 0;
CY_ISR(Timer_1_Handler){
    sprintf(buffer, "On-time (sec): %d \n"
        "Instrs/second: %lu \n"
        "Cycles/second: %lu \n"
        "Machine Cycles/second:\n %lu ", seconds, total_instrs/4, total_cycles, total_cycles/4);
    total_cycles = 0;
    total_instrs = 0;
    
    GUI_DispStringAt(buffer, 0, 0); 
    seconds += 4;
    Timer_1_ReadStatusRegister(); //Clear timer register to leave interrupt
}

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    SPIM_1_Start(); 
    
    
    GUI_Init();
    GUI_Clear();
    GUI_SetFont(&GUI_Font8x16);
    
    reset_cpu(&cpu);
    cpu.inBios = false;
    
    Timer_1_interrupt_StartEx(Timer_1_Handler);
    Timer_1_Start();

    for(;;)
    {
        
        int cycles_taken = tick(&cpu);
        total_cycles += cycles_taken;
        total_instrs++;
    }
}

/* [] END OF FILE */
