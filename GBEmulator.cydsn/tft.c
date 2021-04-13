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
#include "tft.h"

//==============================================================
// write8_a0()
// writes an 8-bit value to the TFT with the D/C line low
//
// Arguments:
//      data - 8-bit value
//==============================================================
void write8_a0(uint8 data)
{
	DC_Write(0x00); 						        // set DC line low
    SPIM_1_WriteTxData(data);         		        // send data to transmit buffer
    while (!(SPIM_1_ReadTxStatus() & 0x01)){};	    // wait for data to be sent
}

//==============================================================
// write8_a1()
// writes an 8-bit value to the TFT with the D/C line high
//
// Arguments:
//      data - 8-bit value
//==============================================================
void write8_a1(uint8 data)
{
	DC_Write(0x01); 						        // set DC line high
    SPIM_1_WriteTxData(data);                       // send data to transmit buffer
    while (!(SPIM_1_ReadTxStatus() & 0x01)){};	    // wait for data to be sent
}

//==============================================================
// writeM8_a1()
// writes multiple bytes to the TFT with the D/C line high
//
// Arguments:
//      pData - pointer to an array of 8-bit data values
//      N - the size of the array *pData
//==============================================================
void writeM8_a1(uint8 *pData, int N)
{
	DC_Write(0x01); 						        // set DC line high
    int i;
    for (i=0; i<N; i++)
    {
        SPIM_1_WriteTxData(pData[i]);               // send data to transmit buffer
        while (!(SPIM_1_ReadTxStatus() & 0x01)){};  // wait for data to be sent
    }
}

//==============================================================
// read8_a1()
// reads an 8-bit value to the TFT with the D/C line high
//==============================================================
uint8 read8_a1(void)
{
	for (;;) {}                                     // read function not implemented
};

//==============================================================
// readM8_a1()
// reads multple 8-bit values from the TFT with the D/C line high
//
// Arguments:
//      pData - an array where the read values we be stored
//      N - the number of values that will be read (also size of 
//          the array pData)
//==============================================================
void readM8_a1(uint8 *pData, int N)
{
	for (;;) {}                                     // read function not implemented
}

//==============================================================
// tftStart()
// this function must be called to initializes the TFT
//==============================================================
void tftStart(void)
{
    write8_a0(0x01);         			// send Software Reset Command (must wait at least 5ms after command)
    CyDelay(10);
    write8_a0(0x36);         			// send Memory Access Control command
	write8_a1(0x88);
    write8_a0(0x3A);         			// send COLMOD: Pixel Format Set command
    write8_a1(0x55);
    write8_a0(0x11);         			// send Sleep Out command
    write8_a0(0x29);         			// send Display ON command
    CyDelay(250);            			// delay to allow all changes to take effect	
}


/* [] END OF FILE */

