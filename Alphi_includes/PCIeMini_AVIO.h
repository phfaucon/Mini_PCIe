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
/** @file PCIeMini_AVIO.h
* @brief Definitition of the PCIeMini_AVIO board class.
*/

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		2/23/2021	phf	Written
//---------------------------------------------------------------------

#include <stdint.h>
#include <iostream>
//using namespace std;
#include "AlphiDll.h"
#include "AlphiBoard.h"
#include "AlphiErrorCodes.h"
#include "IrigDecoder.h"
#include "AlteraPio.h"
#include "ParallelInput.h"
#include "PcieCra.h"
#include "SpiOpenCore.h"
#include "HI_8429.h"

// VDD = 12
// Vthresh(G/O) = VDD x (0.126 + D/91.6) Volts
// Vthresh(S/O) = VDD x (0.144 + D/98.9) Volts
// D is a value (0 to 63) programmed into the DAC
// GL5:0 GND/Open Low Threshold 3.608
#define GL_DEFAULT	55
//#define GL_DEFAULT	0x21
// GH5:0 GND/Open High Threshold 7.800
#define GH_DEFAULT	62
//#define GH_DEFAULT	0x21
// VL5:0 Supply/Open Low Threshold 3.669
#define VL_DEFAULT  55
//#define VL_DEFAULT  0x21
// VH5:0 Supply/Open High Threshold 7.552
#define VH_DEFAULT	62
//#define VH_DEFAULT	0x21

//#define Sense_Sel_Hi 1

//! PCIeMini_AVIO controller board object
class DLL PCIeMini_AVIO : public AlphiBoard
{
public:
	PCIeMini_AVIO();
	~PCIeMini_AVIO();

	PCIeMini_status open(int brdNbr);
	PCIeMini_status close();
	PCIeMini_status reset();

	AlteraPio* ledPio;					///< LED control
	ParallelInput* digoutReadback;		///< Readback from the digital outputs
	AlteraPio* digout;					///< Digital outputs
	AvioCtrlReg* ctrlReg;				///< Control register
	AlteraPio* statusReg;				///< Status register
	AlteraPio* avComp;					///< AV comparator
	AlteraPio* rstCtrl;					///< Reset control
	HI_8429* hi8429;					///< SPI interface to the HI-8429

	// Status register
	uint16_t status_fault = 0x000f;
	uint16_t status_irqHi8428 = 0x0010;

	inline uint16_t getStatus(void)
	{
		return statusReg->getData();
	}

	/** @brief Set the digital output values
	 *
	 *
	 */
	inline void setDigout(uint16_t data)
	{
		digout->setData(data);
	}

	/** @brief Get the digital output values readback
	 *
	 *
	 */
	inline uint16_t getDigoutReadback(void)
	{
		return digoutReadback->getData();
	}

	inline uint8_t getDigoutFault(void)
	{
		return getStatus() & 0x0f;
	}

// DAC access: The DAC is used to set the high/low thresholds for the readback of the digital outputs
	static constexpr double DA_LSB = (5.0/((1<<8)-1));		// 0.019607843 V
	static const int nbrOfThesholdChannels = 4;
	volatile uint32_t *thresholds;		///< DAC output used as thresholds

	PCIeMini_status setReadbackThreshold(uint8_t channel, uint8_t thresholdL, uint8_t thresholdH);	// as a raw binary value
	PCIeMini_status setReadbackThreshold(uint8_t channel, double thresholdL, double thresholdH);		// in volts
	PCIeMini_status getReadbackThreshold(uint8_t channel, double *thresholdL, double *thresholdH);		// in volts

private:
	// Board configuration: These are the offsets of the different hardware components within BAR2
	static const uint32_t	ledPio_offset = 0x0010;		///< R/W   32-bit	LEDs
	static const uint32_t	digoutReadback_offset = 0x0020;		///< AV outputs readback
	static const uint32_t	digout_offset = 0x0040;		///< AV outputs
	static const uint32_t	ctrlReg_offset = 0x0050;	///< 32-bit	Control register
	static const uint32_t	statusReg_offset = 0x0060;		///< 32-bit	General Purpose Inputs
	static const uint32_t	avComp_offset = 0x0080;		///< AV comparator
	static const uint32_t	rstCtrl_offset = 0x0070;	///< Reset control
	static const uint32_t	hi_8429_offset = 0x0100;	///< SPI interface to the HI-8429
	static const uint32_t	dac5308_offset = 0x0200;	///< AD5308 digital to analog converter
};
