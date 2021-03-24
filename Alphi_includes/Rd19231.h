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
/** @file Rd19231.h 
* @brief Description of the DDC RD19231 controller class
*/

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		7/23/2020	phf	Written
//---------------------------------------------------------------------
#pragma once

#include "AlphiDll.h"
#include "MiniSynchStatusRegister.h"
#include <stdint.h>

/** @brief DDC RD19231 controller class.
* 
* The board hardware reads regularly the position data output to make sure the read position function returns the freshest data possible.
*/
class Rd19231
{
public:

	/** @brief Constructor
	 *
	 * only called by the board open function
	 */
	inline Rd19231(volatile void* rd_addr, StatusRegister* status, ControlRegister* control)
	{
		sync = (volatile uint32_t*)rd_addr;
		statusReg = status;
		controlReg = control;
	}

	/** @brief Read the Synchro raw binary output 
	 *
	 * See getPos() for the output translated in degree
	 * @retval A 16-bit binary number
	 */
	inline uint32_t getRawPos()
	{
		return sync[synchroPos_index];
	}

	/** @brief Get angular position
	 *
	 * @retval Position in degree, from 0 to 360 degrees
	 */
	inline double getPos()
	{
		return (double)(getRawPos() & 0xffff) * 90.0 / 0x4000;
	}

	/** @brief Maintenance only */
	inline uint32_t getCycleTime()
	{
		return sync[cycleTime_index];
	}

	/** @brief Maintenance only */
	inline uint32_t getBusyTime()
	{
		return sync[busyTime_index];
	}

	/** @brief Check if output is valid or if BIT error
	 *
	 * The BIT error is triggered if any of the following conditions exist : ~180 LSBs of
	 *	positive error, ~180 LSBs of negative error, Loss of Signal(LOS), or Loss of
	 *	Reference(LOR) is less than 500 mVp, or a false null occurs when the phase detect
	 *	circuitry causes a BIT and corrects the error.Logic 0 for fault condition.
	 * @retval True if valid read, False if there is a BIT error
	 */
	inline bool isBITnValid()
	{
		return statusReg->isBITnValid();
	}

	/** @brief Get the encoder counter
	 *
	 * Counting the A and B output pulses. Refer to the Synchro manual
	 */
	inline uint32_t getEncoderCounter()
	{
		return sync[EncoderCntr_index];
	}

	/** @brief for compatibility only */
	inline void reset()
	{
	}

private:
	static const uint32_t	synchroPos_index = 0;	///< R   32-bit	Synchro Position Data
	static const uint32_t	busyTime_index = 1;		///< R   32-bit	Converter Busy Time
	static const uint32_t	cycleTime_index = 2;	///< R   32-bit	Converter Cycle Time
	static const uint32_t	EncoderCntr_index = 3;	///< R	 32-bit	Encoder Counter Data

	volatile uint32_t* sync;
	StatusRegister* statusReg;
	AlteraPio* controlReg;

};

