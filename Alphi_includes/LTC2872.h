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

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		7/23/2020	phf	Written
//---------------------------------------------------------------------
#pragma once
#include <stdint.h>

/** @brief Implementation of the LTC2872 buffer 
 *
 * The SCC use a transceiver to transform the serial interface signals to the proper voltages and format. This class 
 * allows to control the transceivers.
 */
class LTC2872
{
public:
	/*
	  assign clk_sel = iobits[9];
	  assign dcd_sel = iobits[8];
	  assign de_mode = iobits[7:6];
	  assign TERM  = iobits[5];
	  assign LB    = iobits[4];
	  assign FAST  = iobits[3];
	  assign RS485 = iobits[2];
	  assign HFN   = iobits[1];
	  assign REN   = iobits[0];

	  assign DE = ((de_mode == 2'b00) & 1'b0)
				| ((de_mode == 2'b01) & 1'b1)
				| ((de_mode == 2'b10) & RTSPIN)
				| ((de_mode == 2'b11) & ~RTSPIN);

	  assign readdata[9:0] = iobits;
	*/

	/** @brief  Receiver Disable 
	 * A logic high disables RS232 and RS485 receivers in transceiver #1. A logic low enables the RS232 or RS485
	 * receivers in the transceiver #1, depending on the state of the Interface Select Input 485/232_1.
	 */
	static const uint16_t recvDisable = 0x0001;

	/** @brief S485 Half-duplex Select Input
	 *
	 * A logic low is used for full duplex operation where pins A and B are the receiver inputs and pins Y and Z are the driver outputs. A logic high is used
	 * for half duplex operation where pins Y and Z are both the receiver inputs and driver outputs and pins A and B do
	 * not serve as the receiver inputs. The impedance on A and B and state of differential termination between A and B is
	 * independent of the state of H/F. The H/F pin has no effect on RS232 operation.
	 */
	static const uint16_t halfDuplexEnable = 0x0002;

	/** @brief Interface Select
	 *
	 * A logic low enables RS232 mode and a high enables RS485 mode. The mode determines which transceiver
	 * inputs and outputs are accessible at the LTC2872 pins as well as which is controlled by the driver and receiver
	 * enable pins.
	 */
	static const uint16_t rs485Mode = 0x0004;

	/** @brief Fast mode enable
	 *
	 * A logic high enable continuous voltage generation inside the buffer hardware. When the bit is 0 the DC/DC converters are
	 * on as needed which slows down slightly the performance, in exchange for a small gain in power consumption. This bit should 
	 * be kept set.
	 */
	static const uint16_t fastMode = 0x0008;

	/** @brief Loopback Enable
	 * 
	 * A logic high enables Logic Loopback diagnostic mode,
	 * internally routing the driver input logic levels to the receiver output pins within the same transceiver. This applies to
	 * both RS232 channels as well as the RS485 driver/receiver.
	 * The targeted receiver must be enabled for the loopback signal to be available on its output. A logic low disables
	 * loopback mode. In loopback mode, signals are not inverted from driver inputs to receiver outputs.
	 */
	static const uint16_t loopbackEnable = 0x0010;

	/** @brief RS485 Termination Enable for Transceiver
	 * 
	 * A logic high enables a 120? resistor between
	 * pins A1 and B1. If DZ1 is also high, a 120 Ohm resistor is enabled between pins Y1 and Z1. A logic low on TE485_1
	 * opens the resistors, leaving A1/B1 and Y1/Z1 unterminated, independent of DZ1. The differential termination resistors
	 * are never enabled in RS232 mode.
	 */
	static const uint16_t terminationEnable = 0x0020;

	/** @brief Drivers Enable
	 * 
	 * A logic low disables the RS232 and RS485 drivers, leaving their
	 * outputs in a Hi-Z state. A logic high enables the RS232 or RS485 drivers in transceiver #1, depending on the state
	 * of the Interface Select Input 485/232_1.
	 */
	static const uint16_t  driverModeMask = 0x00c0;    	

	/** @brief DCD Input Disable
	 *
	 * A logic high (1) force the DCD input of the ESCC to go low (active). When this bit is low the DCD input of the chip is the DCD input from the board interface.
	 */
	static const uint16_t dcdInputDisable = 0x0100;

	/** @brief SCC clock selection 
	 * 
	 * when 1 the transmission clock is the local 14.7456MHz, when 0, it is the user provided clock.
	*/
	static const uint16_t clockSelect = 0x0200;

	static const uint16_t RS_232_buffers = 0x0348;			///< Set the output buffers to RS-232, Fast mode, driver 1 enable, DCD disable.
	static const uint16_t RS_422_buffers = 0x036c;			///< Set the output buffers to RS-422/485, Fast mode, driver 1 enable, DCD disable.
	static const uint16_t RS_422_localLoopback = 0x035c;	///< Set the output buffers to RS-422/485, Fast mode, driver 1 enable. Local Loop back mode, DCD disable
	static const uint16_t RS_422_pullups_buffers = 0x37c;	///< Set the output buffers to RS-422/485, Fast mode, pull-ups enabled, driver 1 enable, DCD disable.

	volatile uint32_t* addr;		///< Address of the buffer

	/** @brief Constructor
	 * 
	 * To be used only by the SccChannel constructor.
	 * @param devAddr Pointer to the device mapped in user memory.
	 */
	inline LTC2872(volatile void* devAddr)
	{
		addr = (uint32_t * )devAddr;
	}

	/** @brief Write the buffer configuration 
	 * @param settings A 16-bit unsigned containing the bit mapped settings for the buffer.
	 */
	inline void setBuffer(uint32_t settings)
	{
		*addr = settings;
	}

	/** @brief Read the buffer configuration
	 * @retval A 16-bit unsigned containing the bit mapped settings for the buffer.
	 */
	inline uint32_t getBuffer()
	{
		return *addr;
	}
};