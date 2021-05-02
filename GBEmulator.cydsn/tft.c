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
void setDClow(void){
    DC_Write(0x00);
}
void setDChigh(void){
    DC_Write(0x01);
}
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


void setupDma(uint8_t* dma_buff, uint32_t burstLength){
     /* Disable the TX interrupt of SPIM */
    SPIM_1_TX_STATUS_MASK_REG&=(~SPIM_1_INT_ON_TX_EMPTY);
       
    /* Take a copy of SPIM_TX_STATUS_MASK_REG which will be used to disable the TX interrupt using DMA */
    InterruptControl=SPIM_1_TX_STATUS_MASK_REG;
    
   //Init DMA, 1 byte bursts, each burst requires a request
    txChannel = DMA_1_DmaInitialize(DMA_TX_BYTES_PER_BURST, DMA_TX_REQUEST_PER_BURST, HI16(((uint32)&dma_buff[0])), HI16(((uint32)SPIM_1_TXDATA_PTR)));
   
    //Allocate TD to transfer x bytes
    txTD = CyDmaTdAllocate();
   
    //Allocate TD to disable the SPI Master TX interrupt
    InterruptControlTD = CyDmaTdAllocate();
   
    // txTD = From the memory to the SPIM 
    CyDmaTdSetAddress(txTD, LO16(((uint32)&dma_buff[0])), LO16(((uint32) SPIM_1_TXDATA_PTR)));
   
    // Set the source address as variable 'InterruptControl' which stores the value 0 to disable the SPI_INT_ON_TX_EMPTY
	// and the destination is Control_Reg_SPIM_ctrl_reg__CONTROL_REG 
    CyDmaTdSetAddress(InterruptControlTD, LO16((uint32)&InterruptControl), LO16((uint32)&SPIM_1_TX_STATUS_MASK_REG));
   
    // Set TD_tx transfer count as "burstLength" to transfer the data packet
    // Next Td as InterruptControlTD, and auto increment source address after each transaction 
    CyDmaTdSetConfiguration(txTD,burstLength,InterruptControlTD, TD_INC_SRC_ADR );
   
    // Set InterruptControlTD with transfer count 1, next TD as txTD
    // Also enable the Terminal Output . This can be used to monitor whether transfer is complete 
	CyDmaTdSetConfiguration(InterruptControlTD,1,txTD, 0 );
   
    // Terminate the chain of TDs; this clears any pending request to the DMA
    CyDmaChSetRequest(txChannel, CPU_TERM_CHAIN);
    CyDmaChEnable(txChannel,1);
   
    // Set TD_tx as the initial TD associated with channel_tx 
    CyDmaChSetInitialTd(txChannel, txTD); 
   
    // Enable the DMA channel - channel_tx 
    CyDmaChEnable(txChannel,1);
}

void startDmaTransfer(void){
    SPIM_1_TX_STATUS_MASK_REG|=(SPIM_1_INT_ON_TX_EMPTY); 
}


bool isDmaReady(void){
    return (SPIM_1_TX_STATUS_MASK_REG & SPIM_1_INT_ON_TX_EMPTY) == 0;
}

/* [] END OF FILE */



