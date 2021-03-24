/**
 * @file TCAN4x5x_SPI.cpp
 * @brief This file is responsible for abstracting the lower-level microcontroller SPI read and write functions
 */
/*
 * Created on: October 1, 2017
 *     Author: Texas Instruments
 *
 *
 * Copyright (c) 2017 Texas Instruments Incorporated.  All rights reserved.
 * Software License Agreement
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdint.h>
#include "TCAN4550.h"

 //    if (status & ALTERA_AVALON_SPI_CONTROL_IE_MSK) resetStatus(); 

#define  WAIT_FOR_IDLE()   while (getTxFifoLevel() > 0);

/**
 * @brief Single word write
 *
 * @param address A 16-bit address of the destination register
 * @param data A 32-bit word of data to write to the destination register
 */
void
TcanInterface::AHB_WRITE_32(uint16_t address, uint32_t data)
{
    uint32_t msg;
    uint8_t words = 1;

    //keep the CS low during the transaction
    setControl(control_SSO_mask | control_resetFifo_mask);

    msg = AHB_WRITE_OPCODE << 24;
    msg |= address << 8;    // Send the 16-bit address
    msg |= words;           // Send the number of words to read

    setTxData(msg);
    setTxData(data);
    getRxData();
    getRxData();

    setControl(0);  // reset SSO

}


/**
 * @brief Single word read
 *
 * @param address A 16-bit address of the source register
 *
 * @return Returns 32-bit word of data from source register
 */
uint32_t
TcanInterface::AHB_READ_32(uint16_t address)
{
    uint8_t words = 1;
    uint32_t returnData;

    uint32_t msg;

    msg = AHB_READ_OPCODE << 24;
    msg |= address << 8;        // Send the 16-bit address
    msg |= words;               // Send the number of words to read

    //set the CS low andmake sure that the FIFOs are clear to start the transaction
    setControl(control_SSO_mask | control_resetFifo_mask);

    setTxData(msg);
    setTxData(0);
    getRxData();        // this the first word, a 0, discarded
    returnData = getRxData();
    setControl(control_resetFifo_mask);  // reset SSO

    return returnData;

}

/**
 * @brief Burst write start
 *
 * The SPI transaction contains 3 parts: the header (start), the payload, and the end of data (end)
 * This function is the start, where the register address and number of words are transmitted
 *
 * @param address A 16-bit address of the destination register
 * @param words The number of 4-byte words that will be transferred. 0 = 256 words
 */
void
TcanInterface::AHB_WRITE_BURST_START(uint16_t address, uint8_t words)
{
    uint32_t msg;

    //set the CS low to start the transaction
    setControl(ALTERA_AVALON_SPI_CONTROL_SSO_MSK | control_resetRxFifo_mask);

    msg = AHB_WRITE_OPCODE << 24;
    msg |= address << 8;    // Send the 16-bit address
    msg |= words;           // Send the number of words to read

    setTxData(msg);

}


/**
 * @brief Burst write
 *
 * The SPI transaction contains 3 parts: the header (start), the payload, and the end of data (end)
 * This function writes a single word at a time
 *
 * @param data A 32-bit word of data to write to the destination register
 */
void
TcanInterface::AHB_WRITE_BURST_WRITE(uint32_t data)
{
    setTxData(data);
}


/**
 * @brief Burst write end
 *
 * The SPI transaction contains 3 parts: the header (start), the payload, and the end of data (end)
 * This function ends the burst transaction by pulling nCS high
 */
void
TcanInterface::AHB_WRITE_BURST_END(void)
{
    /* Wait until the interface has finished transmitting */
    WAIT_FOR_IDLE();

    // Clear SSO (release chipselect) and empty the receive FIFO
    setControl(0);
}

/************************************************************************************************/
/**
 * @brief Burst read start
 *
 * The SPI transaction contains 3 parts: the header (start), the payload, and the end of data (end)
 * This function is the start, where the register address and number of words are transmitted
 *
 * @param address A 16-bit start address to begin the burst read
 * @param words The number of 4-byte words that will be transferred. 0 = 256 words
 */
void
TcanInterface::AHB_READ_BURST_START(uint16_t address, uint8_t words)
{
    uint32_t msg;

    //    WAIT_FOR_IDLE();
        //set the CS low to start the transaction
    setControl(ALTERA_AVALON_SPI_CONTROL_SSO_MSK | control_resetFifo_mask);
    lastRxFifoLevel = 0;

    /*
     * Discard any stale data present in the RXDATA register, in case
     * previous communication was interrupted and stale data was left
     * behind.
     */
     //    getRxData();

    msg = AHB_READ_OPCODE << 24;
    msg |= address << 8;        // Send the 16-bit address
    msg |= words;               // Send the number of words to read

    setTxData(msg);

    for (int i = 0; i < words; i++) {
        setTxData(0);
    }
    getRxData();
}

/**
 * @brief Burst read start
 *
 * The SPI transaction contains 3 parts: the header (start), the payload, and the end of data (end)
 * This function where each word of data is read from the TCAN4x5x
 *
 * @return A 32-bit single data word that is read at a time
 */
uint32_t
TcanInterface::AHB_READ_BURST_READ(void)
{
    uint32_t returnData;

    returnData = getRxData();
    return returnData;
}


/**
 * @brief Burst write end
 *
 * The SPI transaction contains 3 parts: the header (start), the payload, and the end of data (end)
 * This function ends the burst transaction by pulling nCS high. We have already received everything so we don't
 * need to wait.
 */
void
TcanInterface::AHB_READ_BURST_END(void)
{
    // Clear SSO (release chipselect) and empty the receive FIFO
    setControl(control_resetFifo_mask);

    //   printf("%d\n", maxRxFifoLevel);
}

