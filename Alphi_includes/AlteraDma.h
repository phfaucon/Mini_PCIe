#pragma once
// Copyright (c) 2020 Alphi Technology Corporation, Inc.  All Rights Reserved
//
// You are hereby granted a copyright license to use, modify and
// distribute this SOFTWARE so long as the entire notice is retained
// without alteration in any modified and/or redistributed versions,
// and that such modified versions are clearly identified as such.
// No licenses are granted by implication, estopple or otherwise under
// any patents or trademarks of Alphi Technology Corporation (Alphi).
//
// The SOFTWARE is provided on an "AS IS" basis and without warranty,
// to the maximum extent permitted by applicable law.
//
// ALPHI DISCLAIMS ALL WARRANTIES WHETHER EXPRESS OR IMPLIED, INCLUDING
// WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
// AND ANY WARRANTY AGAINST INFRINGEMENT WITH REGARD TO THE SOFTWARE
// (INCLUDING ANY MODIFIED VERSIONS THEREOF) AND ANY ACCOMPANYING
// WRITTEN MATERIAL.
//
// To the maximum extent permitted by applicable law, IN NO EVENT SHALL
// ALPHI BE LIABLE FOR ANY DAMAGE WHATSOEVER (INCLUDING WITHOUT LIMITATION,
// DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF
// BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM THE USE
// OR INABILITY TO USE THE SOFTWARE.  GMS assumes no responsibility for
// for the maintenance or support of the SOFTWARE
//
/** @file AlteraDma.h
 * @brief Description of the low-level access routines to the SPI.
 */
 //
 // Maintenance Log
 //---------------------------------------------------------------------
 // v1.0		7/23/2020	phf	Written
 //---------------------------------------------------------------------

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "AlphiDll.h"

typedef void (alt_txchan_done)(void* handle);
typedef void (alt_rxchan_done)(void* handle, void* data);

 /*
  * Flags used to describe the current operating mode of the drivers.
  */

#define ALT_AVALON_DMA_MODE_MSK  (0xf)
#define ALT_AVALON_DMA_MODE_8    (0x0)
#define ALT_AVALON_DMA_MODE_16   (0x1)
#define ALT_AVALON_DMA_MODE_32   (0x3)
#define ALT_AVALON_DMA_MODE_64   (0x7)
#define ALT_AVALON_DMA_MODE_128  (0xf)
#define ALT_AVALON_DMA_TX_STREAM (0x20)
#define ALT_AVALON_DMA_RX_STREAM (0x40)

#define ALT_DMA_TX_STREAM_ON  (0x1)
#define ALT_DMA_TX_STREAM_OFF (0x2)
#define ALT_DMA_RX_STREAM_ON  (0x3)
#define ALT_DMA_RX_STREAM_OFF (0x4)
#define ALT_DMA_SET_MODE_8    (0x5)     ///<    Transfer data in units of 8 bits.
#define ALT_DMA_SET_MODE_16   (0x6)     ///<    Transfer data in units of 16 bits.
#define ALT_DMA_SET_MODE_32   (0x7)     ///<    Transfer data in units of 32 bits.
#define ALT_DMA_SET_MODE_64   (0x8)     ///<    Transfer data in units of 64 bits.
#define ALT_DMA_SET_MODE_128  (0x9)     ///<    Transfer data in units of 128 bits.
#define ALT_DMA_GET_MODE      (0xa)     ///<    Get the current transfer mode.

#define ALTERA_AVALON_DMA_CONTROL_BYTE_MSK             (0x1)
#define ALTERA_AVALON_DMA_CONTROL_HW_MSK               (0x2)
#define ALTERA_AVALON_DMA_CONTROL_WORD_MSK             (0x4)
#define ALTERA_AVALON_DMA_CONTROL_GO_MSK               (0x8)
#define ALTERA_AVALON_DMA_CONTROL_I_EN_MSK             (0x10)
#define ALTERA_AVALON_DMA_CONTROL_REEN_MSK             (0x20)
#define ALTERA_AVALON_DMA_CONTROL_WEEN_MSK             (0x40)
#define ALTERA_AVALON_DMA_CONTROL_LEEN_MSK             (0x80)

/** Reads from a constant address. When RCON is 0, the read address
increments after every data transfer. This is the mechanism for the DMA
controller to read a range of memory addresses. When RCON is 1, the
read address does not increment. This is the mechanism for the DMA
controller to read from a peripheral at a constant memory address. For
details, see the Addressing and Address Incrementing section.*/
#define ALTERA_AVALON_DMA_CONTROL_RCON_MSK             (0x100)  
#define ALTERA_AVALON_DMA_CONTROL_WCON_MSK             (0x200)
#define ALTERA_AVALON_DMA_CONTROL_DWORD_MSK            (0x400)
#define ALTERA_AVALON_DMA_CONTROL_QWORD_MSK            (0x800)
#define ALTERA_AVALON_DMA_CONTROL_SOFTWARERESET_MSK    (0x1000)

#define ALTERA_AVALON_DMA_STATUS_DONE_MSK           (0x1)
#define ALTERA_AVALON_DMA_STATUS_BUSY_MSK           (0x2)
#define ALTERA_AVALON_DMA_STATUS_REOP_MSK           (0x4)
#define ALTERA_AVALON_DMA_STATUS_WEOP_MSK           (0x8)
#define ALTERA_AVALON_DMA_STATUS_LEN_MSK            (0x10)

/** @brief Structure containing the details of a DMA transaction */
class DLL TransferDesc
{
public:
    // DMA controller specific
    volatile uint32_t   dest_offset;        ///< 32-bit FPGA Avalon bus address of destination
    volatile uint32_t   src_offset;         ///< 32-bit FPGA Avalon bus address of source
    uint32_t            tfr_length;         ///< length of the transfer
    // general configuration
    uint32_t            flags;
    uint32_t            txs_offset;         ///< Offset in the mapping area
    uint32_t            bufLength;          ///< Size of the buffer in bytes
    uint32_t*           userSpaceBuffer;    ///< PC buffer address as an user-space address
    bool                fPolling;           ///< Polling or interrupt for end of transfer (not yet implemented)
};


/** @brief Low level SPI interface to the SPI hardware */
class DLL AlteraDma
{
public:

    /*
     * This is the device driver for the altera Avalon DMA device.
     *
     * The HAL DMA model describes a DMA trasaction as being performed by two
     * drivers: a transmit and a receive channel. In the case of this device these
     * two drivers map onto a single instance of the physical DMA device. This
     * means that for every device instance, a pair of device drivers will be
     * created.
     */

#ifndef ALT_AVALON_DMA_NSLOTS
#define ALT_AVALON_DMA_NSLOTS (4)
#endif

#define ALT_AVALON_DMA_NSLOTS_MSK (ALT_AVALON_DMA_NSLOTS - 1)

    AlteraDma(volatile void* dmaAddress);

 /*   int prepare(
        void* data,
        uint32_t          len,
        alt_rxchan_done* done,
        void* handle);

    int space();

    int send(const void* from,
        uint32_t len,
        alt_txchan_done* done,
        void* handle);

    int tx_ioctl(int req,
        void* arg);

    int rx_ioctl(int req,
        void* arg);

    int ioctl(int req,
        void* arg);
*/
    void launch_bidir(TransferDesc *t);
/*
    void launch_txonly(TransferDesc& t);
    void launch_rxonly(TransferDesc& t);
*/
    inline void reset()
    {
        base[control_index] = ALTERA_AVALON_DMA_CONTROL_SOFTWARERESET_MSK;
        base[control_index] = ALTERA_AVALON_DMA_CONTROL_SOFTWARERESET_MSK;

        /* Set the default mode of the device (32 bit block reads and writes from/to memory). */
        base[control_index] =
            ALTERA_AVALON_DMA_CONTROL_WORD_MSK |
            ALTERA_AVALON_DMA_CONTROL_GO_MSK |
            ALTERA_AVALON_DMA_CONTROL_LEEN_MSK;

        /* Clear any pending interrupts and the DONE flag */
        base[status_index] = 0;
    }

    void irq(void* context, uint32_t id);

    inline uint32_t getStatus()
    {
        return base[status_index];
    }

    inline uint32_t getLength()
    {
        return base[length_index];
    }

    inline uint32_t setControlBit(uint32_t mask)
    {
        base[control_index] |= mask;
        return base[control_index];
    }

    inline char* statusToString(char* buffer)
    {
        buffer[0] = 0;
        uint32_t status = base[status_index];
        if (status & ALTERA_AVALON_DMA_STATUS_DONE_MSK)
            strcat(buffer, "DONE ");
        if (status & ALTERA_AVALON_DMA_STATUS_BUSY_MSK)
            strcat(buffer, "BUSY ");
        if (status & ALTERA_AVALON_DMA_STATUS_REOP_MSK)
            strcat(buffer, "RD end ");
        if (status & ALTERA_AVALON_DMA_STATUS_WEOP_MSK)
            strcat(buffer, "WR end ");
        if (status & ALTERA_AVALON_DMA_STATUS_LEN_MSK)
            strcat(buffer, "LEN=0 ");
        return buffer;
    }

    inline char* controlToString(char* buffer)
    {
        buffer[0] = 0;
        uint32_t control = base[control_index];
        if (control & ALTERA_AVALON_DMA_CONTROL_BYTE_MSK)
            strcat(buffer, "8-bit ");
        if (control & ALTERA_AVALON_DMA_CONTROL_HW_MSK)
            strcat(buffer, "16-bit ");
        if (control & ALTERA_AVALON_DMA_CONTROL_WORD_MSK)
            strcat(buffer, "32-bit ");
        if (control & ALTERA_AVALON_DMA_CONTROL_GO_MSK)
            strcat(buffer, "GO ");
        if (control & ALTERA_AVALON_DMA_CONTROL_I_EN_MSK)
            strcat(buffer, "INT_EN ");
        if (control & ALTERA_AVALON_DMA_CONTROL_REEN_MSK)
            strcat(buffer, "RE_EN ");
        if (control & ALTERA_AVALON_DMA_CONTROL_WEEN_MSK)
            strcat(buffer, "WE_EN ");
        if (control & ALTERA_AVALON_DMA_CONTROL_LEEN_MSK)
            strcat(buffer, "Length_EN ");
        if (control & ALTERA_AVALON_DMA_CONTROL_RCON_MSK)
            strcat(buffer, "RCON ");
        if (control & ALTERA_AVALON_DMA_CONTROL_WCON_MSK)
            strcat(buffer, "WCON ");
        if (control & ALTERA_AVALON_DMA_CONTROL_DWORD_MSK)
            strcat(buffer, "64-bit ");
        if (control & ALTERA_AVALON_DMA_CONTROL_QWORD_MSK)
            strcat(buffer, "128-bit ");
        return buffer;
    }

    inline void print(const char* title = 0)
    {
        char buffer[300];
        if (title)
            printf("\n%s\n", title);
        printf("Status:     %s\n", statusToString(buffer));
        printf("RdAddress:  0x%08x\n", base[rdAddress_index]);
        printf("WrAddress:  0x%08x\n", base[wrAddress_index]);
        printf("Length:     0x%08x\n", base[length_index]);
        printf("Control:    %s\n", controlToString(buffer));
    }

private:
        volatile uint32_t* base;
 //       alt_avalon_dma_txslot  tx_buf[ALT_AVALON_DMA_NSLOTS];
 //       alt_avalon_dma_rxslot  rx_buf[ALT_AVALON_DMA_NSLOTS];

        // indices
        static const uint8_t status_index = 0;
        static const uint8_t rdAddress_index = 1;
        static const uint8_t wrAddress_index = 2;
        static const uint8_t length_index = 3;
        static const uint8_t control_index = 6;
};
