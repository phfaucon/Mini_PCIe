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
/** @file AlteraSpi.h 
 * @brief Description of the low-level access routines to the SPI. 
 */
//
// Maintenance Log
//---------------------------------------------------------------------
// v1.0		7/23/2020	phf	Written
//---------------------------------------------------------------------

#include <stddef.h>
#include <stdint.h>
#include "AlphiDll.h"

/** @brief Low level SPI interface to the SPI hardware */
class DLL AlteraSpi
{
public:
    /*
     * Use this function to perform one SPI access on your target.  'base' should
     * be the base address of your SPI peripheral, while 'slave' indicates which
     * bit in the slave select register should be set.
     */

     /* If you need to make multiple accesses to the same slave then you should
      * set the merge bit in the flags for all of them except the first.
      */
#define ALT_AVALON_SPI_COMMAND_MERGE (0x01)

      /** @brief option: toggle the SS line between words
       * 
       * If you need the slave select line to be toggled between words then you
       * should set the toggle bit in the flag.
       */
#define ALT_AVALON_SPI_COMMAND_TOGGLE_SS_N (0x02)

    /** @brief Receive - overrun error
     *
     * The ROE bit is set to 1 if new data is received while the rxdata register is full(that is, while the RRDY
     * bit is 1).In this case, the new data overwrites the old. Writing to the status register clears the ROE bit to 0.
     */
    static const uint32_t status_ROE_mask = 0x0008;

    /** @brief Transmitter-overrun error
     * 
     * The TOE bit is set to 1 if new data is written to the txdata register while it is still full (that is, while the
     * TRDY bit is 0). In this case, the new data is ignored. Writing to the status register clears the TOE bit to 0.
     */
    static const uint32_t status_TOE_mask = 0x0010;

    /** @brief Transmitter shift-register empty
     * 
     * In master mode, the TMT bit is set to 0 when a transaction is in progress and set to 1 when the shift
     * register is empty.
     */
    static const uint32_t status_TMT_mask = 0x0020;

    /** @brief Transmitter ready
     *
     * The TRDY bit is set to 1 when the txdata register is empty.
     */
    static const uint32_t status_TRDY_mask = 0x0040;

    /** @brief Receiver ready
     * 
     * The RRDY bit is set to 1 when the rxdata register is full.
     */
    static const uint32_t status_RRDY_mask = 0x0080;


#define ALTERA_AVALON_SPI_STATUS_E_MSK                (0x100)
#define ALTERA_AVALON_SPI_STATUS_E_OFST               (8)

#define ALTERA_AVALON_SPI_CONTROL_IROE_MSK            (0x8)
#define ALTERA_AVALON_SPI_CONTROL_IROE_OFST           (3)
#define ALTERA_AVALON_SPI_CONTROL_ITOE_MSK            (0x10)
#define ALTERA_AVALON_SPI_CONTROL_ITOE_OFST           (4)
#define ALTERA_AVALON_SPI_CONTROL_ITRDY_MSK           (0x40)
#define ALTERA_AVALON_SPI_CONTROL_ITRDY_OFS           (6)
#define ALTERA_AVALON_SPI_CONTROL_IRRDY_MSK           (0x80)
#define ALTERA_AVALON_SPI_CONTROL_IRRDY_OFS           (7)
#define ALTERA_AVALON_SPI_CONTROL_IE_MSK              (0x100)
#define ALTERA_AVALON_SPI_CONTROL_IE_OFST             (8)
#define ALTERA_AVALON_SPI_CONTROL_SSO_MSK             (0x400)
#define ALTERA_AVALON_SPI_CONTROL_SSO_OFST            (10)

    AlteraSpi(volatile void* addr, uint8_t width = 1);

    int sendSpiCommand(uint32_t slave,
        uint32_t write_length, const uint32_t* write_data,
        uint32_t read_length, uint32_t* read_data,
        uint32_t flags);

    /** @brief Get the content of the receive data register
     *
     * @retval Content of the receive data register
     */
    inline volatile uint32_t getRxData()
    {
        return base[rxData_index];
    }

    inline volatile uint32_t getStatus()
    {
        return base[status_Index];
    }

    inline void resetStatus()
    {
        base[status_Index] = 0;
    }

    inline void setTxData(uint32_t data)
    {
        base[txData_index] = data;
    }

    inline void setControl(uint32_t data)
    {
        base[control_index] = data;
    }

    inline uint32_t getControl()
    {
        return base[control_index];
    }

    inline void selectSlave(volatile uint32_t data)
    {
        base[slaveSelect_index] = data;
    }

protected:
    volatile uint32_t* base;
    uint8_t wordSize;

    int rxData_index = 0;
    int txData_index = 1;
    int status_Index = 2;
    int control_index = 3;
    int slaveSelect_index = 5;

};