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
#ifndef TFT_H
#define TFT_H
#include "project.h"
#include "stdbool.h"

void write8_a0(uint8 data);
void write8_a1(uint8 data);
void writeM8_a1(uint8 *pData, int N);
uint8 read8_a1(void);
void readM8_a1(uint8 *pData, int N);
void tftStart(void);
void setDClow(void);
void setDChigh(void);

/* DMA Configuration for DMA_TX */
#define DMA_TX_BYTES_PER_BURST      1
#define DMA_TX_REQUEST_PER_BURST    1

/* Variable declarations for DMA_TX*/
uint8 txChannel;
uint8 txTD;

/* Variable declarations for InterruptControl Td*/
uint8_t InterruptControlTD;
uint8 InterruptControl; //this variable stores a copy of the SPI_TX_STATUS_MASK_REG with the SPI_INT_ON_TX_EMPTY bit cleared

// This sets up a DMA chain that transfers data in dma_buff over SPI, 
// then clears the SPI Interrupt on Empty Flag
// To start a "new" DMA transfer use startDMATransfer()  
// burstLength specifies how many bytes to send in one transaction
void setupDma(uint8_t* dma_buff, uint32_t burstLength);

// Returns true if DMA is ready for another round
bool isDmaReady(void);
// Starts a new DMA transfer
void startDmaTransfer(void);

#endif

/* [] END OF FILE */