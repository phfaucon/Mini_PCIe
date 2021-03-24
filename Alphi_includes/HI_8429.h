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
// OR INABILITY TO USE THE SOFTWARE.  ALPHI assumes no responsibility for
// for the maintenance or support of the SOFTWARE
//
/** @file HI_8429.h
 * @brief Declaration of a class representing the HI_8429 on the AVIO board.
 */

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		3/3/2021	phf	Written
//---------------------------------------------------------------------

#pragma once

#include "AlphiDll.h"
#include "AlteraPio.h"
#include "AvioCtrlReg.h"
#include "SpiOpenCore.h"

class HI_8429
{
public:
	SpiOpenCore *spi;				///< SPI controller object used to communicate with the HI_8429


	HI_8429(volatile void *spiController, AvioCtrlReg* pioControl);

	void reset(void);

	void spi_init(void);
	uint32_t setHoltConfigReg(uint32_t config_val);

	uint32_t setHoltThresholdReg(uint32_t thresh_val);
	uint32_t setHoltThresholdReg(double gl, double gh, double vl, double vh);

	uint8_t getHoltSenseReg(void);

	/** @brief Set the Debounce Enable line of the HI-8429
	 *
	 * @param enable When true, the line enable the debounce feature of the HI-8429.
	 */
	inline void setDebounceEnable(bool enable) {
		ctrlReg->setDebounceEnable(enable);
	}


	/** @brief Calculate the threshold register content based on the desired voltages
	 * @retval The 24-bit value to put in the HI-8429 threshold register.
	 */
	static inline uint32_t calculateThresholds( double gl, double gh, double vl, double vh) {
		return 	((0x3f &  GL(gl)) << 18) |  // 23:18
                ((0x3f &  GH(gh)) << 12) |  // 17:12
                ((0x3f &  GL(vl)) <<  6) |  // 11: 6
                ((0x3f &  GH(vh)) <<  0);   //  5: 0

	}

	void print_holt_thresh(uint32_t thresh);

private:
	static const uint16_t SPI_BUSY = 0x100;
	static const uint16_t CFG_REG = 0;
	static const uint16_t THRESH_REG = 2;
	static const uint16_t SENSE_REG = 1;

	static inline uint8_t GH(double lvl) {
		return  0.5 + (lvl/12.0 - 0.144) * 98.9;
	}
	static inline uint8_t GL(double lvl) {
		double cvt = 0.5 + (lvl/12.0 - 0.126) * 91.6;
		uint8_t ret = cvt<0 ? 0 : cvt;
		return  ret;
	}

	AvioCtrlReg* ctrlReg;
};
