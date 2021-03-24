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
/** @file Ltc2664_SPI.h 
 * @brief D/A controller SPI class definition
 */

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		8/23/2020	phf	Written
//---------------------------------------------------------------------
#pragma once

#include "AlphiErrorCodes.h"
#include "AlteraSpi.h"
#include "MiniSynchStatusRegister.h"

/** @brief D/A controller SPI class definition
* 
* We interface to the LTC2664 chip through a standard SPI interface defined in AlteraSpi.
 */
class DLL Ltc2664_SPI : public AlteraSpi
{

	enum COMMAND {
		CODE = 0x00,					// Write Code to n
		CODE_ALL = 0x80,				// Write Code to All
		SPAN = 0x60,					// Write Span to n
		SPAN_ALL = 0xe0,				// Write Span to All
		UPDATE = 0x10,					// Update n(Power Up)
		UPDATE_ALL = 0x90,				// Update All(Power Up)
		CODE_UPDATE = 0x30,			// Write Code to n, Update n(Power Up)
		CODE_UPDATE_ALL = 0x20,		// Write Code to n, Update All(Power Up)
		CODE_ALL_UPDATE_ALL = 0xa0,	// Write Code to All, Update All(Power Up)
		POWER_DOWN_N = 0x40,			// Power Down n
		POWER_DOWN_CHIP = 0x50,		// Power Down Chip(All DACs, Mux and Reference)
		ANALOG_MUX = 0xb0,			// Analog Mux
		TOGGLE_SELECT = 0xc0,			// Toggle Select
		TOGGLE_GLOBAL = 0xd0,			// Global Toggle
		CONFIG = 0x70,					// Config
		NOP = 0xf0					// No Operation
	};

public:
	/** @brief Definition of channel number possible parameters
	*
	* The commands to the LTC2664 have typically as an option the channel number, or all the channels, or
	* if we want to disable part of a command, no channel.
	*/
	enum ChannelNbr : uint8_t {
		CHANNEL_0 = 0,			///< Channel #0, first channel
		CHANNEL_1 = 1,			///< Channel #1, second channel
		CHANNEL_2 = 2,			///< Channel #2, third channel
		CHANNEL_3 = 3,			///< Channel #3, fourth channel
		CHANNEL_ALL = 15,		///< All the channels are selected
		CHANNEL_NONE = 14		///< No channel, option disabled
	};

	/** @brief Constructor
	* 
	* Only called by the board open function.
	* 
	* @param addr Pointer to the SPI controller in user space
	* @param control Pointer to the Control Register object
	*/
	inline Ltc2664_SPI(volatile void* addr, ControlRegister* control)
		: AlteraSpi(addr)
	{
		controlReg = control;
		reset();
	}

	/** @brief Check that a channel is a valid selection
	* @param c Channel number to check
	* @retval True if the channel is valid.
	*/
	inline bool isChannelValid(ChannelNbr c)
	{
		return (c == CHANNEL_0) || (c == CHANNEL_1) || (c == CHANNEL_2)
			|| (c == CHANNEL_3) || (c == CHANNEL_ALL) || (c == CHANNEL_NONE);
	}

	/** @brief Reset the LTC2664 chip
	*/
	inline void reset()
	{
		for (int i = 0; i < 4; i++)
		{
			buff_out[i] = 0;
			buff_in[i] = 0;
		}

		uint32_t prevValue = controlReg->getData() & ~(controlReg->CTRL_DA_CLEAR_mask);

		controlReg->setData(prevValue | controlReg->CTRL_DA_CLEAR_mask);
		Sleep(20);
		controlReg->setData(prevValue);
	}

	PCIeMini_status setCode(int16_t code, ChannelNbr channel, ChannelNbr update = CHANNEL_NONE, bool echo = false);

	uint32_t buff_out[4];		///< output buffer, could be checked for verification
	uint32_t buff_in[4];		///< input buffer, could be checked for verification

	ControlRegister* controlReg;

};