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
/** @file TcanInterface.h
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
#include "ParallelInput.h"
#include "AlteraPio.h"

// control register

/** @brief This class implements the TCAN4550 SPI interface.
* 
* The interface is responsible for the low level communications with the TCAN4550 chip through the SPI interface.
* Because it can be used to talk to several independent SPI slaves using the slave select lines, it doesn't include
* direct PIO to the state.
*/
class DLL TcanInterface
{
public:
	uint8_t slave;		///< TCAN4550 chip select index

	// control register bit definition
	static const uint32_t control_txDis_mask = 0x001;			///< The transmitter is disabled. the data stays in the transmit FIFO
	static const uint32_t control_irqROE_mask = 0x008;			///< Allows an IRQ when the Rx FIFO overflow error is set
	static const uint32_t control_irqTOE_mask = 0x010;			///< Allows an IRQ when the Tx FIFO overflow error is set
	static const uint32_t control_irqTMT_mask = 0x020;			///< Allows an IRQ when the TMT flag is set
	static const uint32_t control_irqTRDY_mask = 0x040;			///< Allows an IRQ when the TRDY flag is set
	static const uint32_t control_irqRRDY_mask = 0x080;			///< Allows an IRQ when the RRDY flag is set
	static const uint32_t control_irqErr_mask = 0x100;			///< Allows an IRQ when the Error flag is set
	static const uint32_t control_irqEOP_mask = 0x200;			///< Allows an IRQ when the EOP flag is set
	static const uint32_t control_SSO_mask = 0x400;				///< Forces the SPI chip select to stay asserted during the length of the transaction
	static const uint32_t control_resetFifo_mask = 0x800;		///< A transient bit, resets the FIFO and deasserts itself
	static const uint32_t control_purgeRxFifo_mask = 0x1000;	///< Remove the first value from the receive FIFO. Might be faster than reading it.
	static const uint32_t control_resetRxFifo_mask = 0x2000;	///< Disable the receive FIFO	

	// status register bit definition
	static const uint32_t status_rxOvfErr_mask = 0x008;			///< Rx FIFO overflow, or underflow (trying to read from an empty Rx FIFO)
	static const uint32_t status_txOvfErr_mask = 0x010;			///< tx FIFO overflow
	static const uint32_t status_TMT_mask = 0x020;				///< Transmitting
	static const uint32_t status_TRDY_mask = 0x0040;			///< Set to 1 when the transmit FIFO is not full
	static const uint32_t status_RRDY_mask = 0x0080;			///< Set to 1 when the receive FIFO is not empty
	static const uint32_t status_Error_mask = 0x0100;			///< Set to 1 when there is any error flag
	static const uint32_t status_EOP_mask = 0x0200;				///< Set to 1 the most recent word received is an end of packet
	static const uint32_t status_rxFifoUsed_mask = 0x00ff0000;	///< Part of the word containing the rx FIFO level
	static const uint32_t status_rxFifoUsed_bitNbr = 16;		///< bit offset of the rx FIFO level
	static const uint32_t status_txFifoUsed_mask = 0xff000000;	///< Part of the word containing the tx FIFO level
	static const uint32_t status_txFifoUsed_bitNbr = 24;		///< bit offset of the tx FIFO level

	uint32_t maxRxFifoLevel;			///< diagnostic value of FIFO usage during an access
	uint32_t maxTxFifoLevel;			///< diagnostic value of FIFO usage during an access

	/** @brief Constructor
	 *
	 * @param spiAddr Address of the SPI controller
	 * @param rst Address of the PIO controlling the reset line
	 * @param nbr Chip select line number.
	 */
	inline TcanInterface(volatile void* spiAddr, uint8_t nbr)
	{
		slave = nbr;
		base = (volatile uint32_t*)spiAddr;
		controlRegCached = 0;
		maxRxFifoLevel = 0;
		lastRxFifoLevel = 0;
		maxTxFifoLevel = 0;
	}

	/** @brief reset the TCAN4550 chip
	 */
	inline void reset()
	{
		maxRxFifoLevel = 0;
		lastRxFifoLevel = 0;
	}

	inline void resetStatus()
	{
		base[status_Index] = 0;
	}

	/** @brief Get the SPI interface status
	 *
	 * The bits are defined by the AlteraSpi class.
	 * @retval Status register content.
	 */
	inline uint32_t getStatus()
	{
		return base[status_Index];
	}

	/** @brief Get the address of the SPI controller
	 *
	 * @retval Address in user space.
	 */
	inline volatile void* getAddress()
	{
		return base;
	}

	/** @brief Get the content of the receive data register
	 *
	 * @retval Content of the receive data register
	 */
	inline volatile uint32_t getRxData()
	{
		return base[rxData_index];
	}

	/** @brief Add data to the transmit FIFO
	 *
	 * @param data Data word to add to the FIFO
	 */
	inline void setTxData(uint32_t data)
	{
		base[txData_index] = data;
	}

	/** @brief Write in the control register
	* 
	* The method keeps a local copy to avoid having to read back the control register. Only the bits that 
	* will actually stay set in the control register are saved - Reset FIFO and PURGE_RX_FIFO are transient bits that are
	* not saved
	* 
	* @param data Value to write to the control register.
	*/
	inline void setControl(uint32_t data)
	{
		// save a copy of what we wrote
		controlRegCached = data;
		// remove the temporary bits
		controlRegCached &= ~(control_resetFifo_mask | control_purgeRxFifo_mask);

		base[control_index] = data | 1 << (slave + 16);
	}


	inline void setControlBits(uint32_t mask)
	{
		mask |= controlRegCached;
		setControl(mask);
	}

	inline void resetControlBits(uint32_t mask)
	{
		uint32_t data = controlRegCached & ~mask;
		setControl(data);
	}

	inline uint32_t getRxFifoLevel()
	{
		uint32_t level = getStatus() & status_rxFifoUsed_mask;
		level = level >> status_rxFifoUsed_bitNbr;
		if (level > maxRxFifoLevel)
			maxRxFifoLevel = level;
		lastRxFifoLevel = level;
		return level;
	}

	inline uint32_t getRxFifoLevel(uint64_t data64)
	{
		uint32_t level = (data64 >> 32) & status_rxFifoUsed_mask;
		level = level >> status_rxFifoUsed_bitNbr;
		if (level > maxRxFifoLevel)
			maxRxFifoLevel = level;
		lastRxFifoLevel = level;
		return level;
	}

	inline uint32_t getTxFifoLevel()
	{
		uint32_t status = getStatus();
		uint32_t level = status & status_txFifoUsed_mask;
		level = level >> status_txFifoUsed_bitNbr;
		if (level > maxTxFifoLevel)
			maxTxFifoLevel = level;
		return level;
	}

	//------------------------------------------------------------------------
	//							Write Functions
	//------------------------------------------------------------------------

	void AHB_WRITE_32(uint16_t address, uint32_t data);
	void AHB_WRITE_BURST_START(uint16_t address, uint8_t words);
	void AHB_WRITE_BURST_WRITE(uint32_t data);
	void AHB_WRITE_BURST_END(void);


	//--------------------------------------------------------------------------
	//							Read Functions
	//--------------------------------------------------------------------------
	uint32_t AHB_READ_32(uint16_t address);
	void AHB_READ_BURST_START(uint16_t address, uint8_t words);
	uint32_t AHB_READ_BURST_READ(void);
	void AHB_READ_BURST_END(void);

protected:
	volatile uint32_t* base;
	const uint8_t wordSize = 4;
	uint32_t controlRegCached;
	uint32_t lastRxFifoLevel;

	int rxData_index = 16;
	int txData_index = 1;
	int status_Index = 2;
	int control_index = 3;

};

