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

#include "rom.h"

Cpu cpu;
int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    reset_cpu(&cpu);
    int total_cycles = 0;
    for(;;)
    {
        int cycles_taken = tick(&cpu);
        total_cycles += cycles_taken;
    }
}

/* [] END OF FILE */
