#pragma once

//
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
/** @file SpiOpenCore.h
* @brief Class describing an Open Core SPI interface.
*/

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		2/23/2021	phf	Written
//---------------------------------------------------------------------


#include "AlphiDll.h"


/** @brief Class describing an Open Core SPI interface.
*/
class SpiOpenCore
{
public:
	//
	// Control register bit position
	//
	static const int SPI_CTRL_ASS = 0x2000;				///<
	static const int SPI_CTRL_IE = 0x1000;				///< Interrupt enabled
	static const int SPI_CTRL_LSB = 0x0800;				///< LSB first on the SPI data line
	static const int SPI_CTRL_TX_NEGEDGE = 0x0400;		///< Transmit data valid on the SPI clock falling edge
	static const int SPI_CTRL_RX_NEGEDGE = 0x0200;		///< Receive data valid on the SPI clock falling edge
	static const int SPI_CTRL_GO = 0x0100;				///< SPI transmitting
	static const int SPI_CTRL_RES_1 = 0x0080;
	static const int SPI_CTRL_CHAR_LEN = 0x007f;		///< Transmission length

	/** @brief constructor
	 *
	 * @param spiController Address of the hardware structure in user space
	 */
	inline SpiOpenCore(volatile void *spiController)
	{
		base = (volatile uint32_t *)spiController;
	}

	/** @brief Reading the received data
	 *
	 * @retval 32-bit received data
	 */
    inline volatile uint32_t getSpiRxData(void)
    {
        return base[rxData_index];
    }

	/** @brief Read control/status register
	 *
	 * @retval 13-bit register value
	 */
    inline volatile uint32_t getSpiStatus(void)
    {
        return base[control_index];
    }

	/** @brief Write the data to transmit
	 *
	 * @param data Data to transmit
	 */
    inline void setSpiTxData(uint32_t data)
    {
        base[txData_index] = data;
    }

	/** @brief Write data to the control register
	 *
	 * @param data Data to write
	 */
    inline void setSpiControl(uint32_t data)
    {
        base[control_index] = data;
    }

	/** @brief Read control/status register
	 *
	 * Control and status are the same register.
	 * @retval 13-bit register value
	 */
    inline uint32_t getSpiControl(void)
    {
        return base[control_index];
    }

	/** @brief Select the SPI slave
	 *
	 * @param Slave to select.
	 */
    inline void selectSpiSlave(volatile uint32_t slave)
    {
        base[slaveSelect_index] = slave;
    }

	/** @brief Set the clock divider
	 *
	 * Specifies the divider between the module clock and the SCLK frequency for the slave module.
	 * @param divider Divider value: board dependent.
	 */
    inline void setSpiDivider(uint32_t divider)
    {
        base[divider_index] = divider;
    }

	/** @brief Get the clock divider
	 *
	 * Specifies the divider between the module clock and the SCLK frequency for the slave module.
	 * @retval Divider value: board dependent.
	 */
    inline uint32_t getSpiDivider(void)
    {
        return base[divider_index];
    }

	void setTransferWidth(uint8_t width);
	uint8_t getTransferWidth(void);
	void startTransfer(void);

	uint32_t rw(uint32_t data);


private:
	volatile uint32_t *base;

    int rxData_index = 0;
    int txData_index = 0;
    int control_index = 4;
    int divider_index = 5;
    int slaveSelect_index = 6;

};
