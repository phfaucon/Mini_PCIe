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
/** @file PCIeMini_Synch.h
 * @brief Main class PCIeMini_Synch definition.
 */

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		7/23/2020	phf	Written
//---------------------------------------------------------------------
#ifndef _PCIE_MINI_ESCC_H
#define _PCIE_MINI_ESCC_H

#include <stdint.h>
#include <iostream>
//using namespace std;
#include "AlphiDll.h"
#include "AlphiBoard.h"
#include "AlphiErrorCodes.h"
#include "Ltc2664_SPI.h"
#include "Ltc2664_av.h"
#include "Rd19231.h"
#include "AlteraPio.h"

/** @brief PCIeMini_Synchro controller board object
 * 
 */
class DLL PCIeMini_Synch : public AlphiBoard
{
public:
	static const uint16_t irq_mask_statusReg = 0x0001;			///< Interrupt mask for the status register (used for BITn)
	static const uint16_t irq_mask_spiInterface = 0x0004;		///< Interrupt mask for the SPI controller

	volatile uint32_t* ledPio;				///< LED control

	// synch
	ControlRegister* controlRegister;		///< Interface to the board control register
	StatusRegister* statusRegister;			///< Interface to the board status register
	Ltc2664_av* da;							///< Interface to the DAs when used as synchro simulator
	Rd19231* sync;							///< Interface to the RD19231 Synchro resolver
	Ltc2664_SPI* spi_da;					///< SPI controller used by the DAs as general purpose DAs

	PCIeMini_Synch();

	PCIeMini_status open(int brdNbr);
	PCIeMini_status close();
	PCIeMini_status reset();

	void setLedPio(uint32_t);
	uint32_t getLedPio();

	static char* getErrorMsg(PCIeMini_status errorNbr);

private:
	// Board configuration
	static const uint32_t	sysid_offset			= 0x0000;  // 0x0000_0007
	static const uint32_t	led_pio_offset			= 0x0010;  // 0x0000_001f
	static const uint32_t	flash_offset			= 0x0800;  // 0x0000_0fff


	static const uint32_t	gpOutput_offset			= 0x0010;	///< R/W 32-bit	General Purpose Outputs
	static const uint32_t	gpInput_offset			= 0x0020;	///< R   32-bit	General Purpose Inputs
	static const uint32_t	sync_offset				= 0x0040;	///< R   32-bit	Synchro Position Data
	static const uint32_t	ltc2664_av_offset		= 0x0080;	///< R/W	32-bit	synchS1 Gain  Control Data
	static const uint32_t	angVelocity_offset		= 0x0090;	///< R/W	32-bit	Ref DDS Speed  Data 
	static const uint32_t	spi_da_offset			= 0x00c0;	///< R/W	32-bit	Ref DDS Speed  Data 

	bool				isOpen;
	MINIPCIE_INT_HANDLER uicr;
};

#endif // _PCIE_MINI_ESCC_H