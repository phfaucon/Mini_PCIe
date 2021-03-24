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
/** @file PCIe_mini_ARINC717_IO.h
* @brief Definitition of the PCIe_mini_ARINC717_IO board class.
*/

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		7/23/2020	phf	Written
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
#include "AlteraDma.h"
#include "PcieCra.h"
#include "Arinc3717.h"

//! PCIeMini_CAN_FD controller board object
class DLL PCIe_mini_ARINC717_IO : public AlphiBoard
{
public:
	PCIe_mini_ARINC717_IO();

	PCIeMini_status open(int brdNbr);
	PCIeMini_status close();
	PCIeMini_status reset();

	Arinc3717* arinc;
	AlteraPio* controlRegister;		///< Interface to the board control register
	AlteraPio* ledPio;		///< Interface to the board control register
	ParallelInput* spiStatus;
	AlteraPio* spiControl;
	IrigDecoder* irig;
	AlteraDma* dma;

	volatile uint32_t* dpr;
	volatile uint16_t* mddr;
	static const uint32_t	dpr_offset = 0x4000;
	static const uint32_t	dpr_length = 0x400;

	void hwDMAStart(TransferDesc* tfrDesc);
	bool hwDMAWaitForCompletion(TransferDesc* tfrDesc, bool fPolling);
	bool hwDMAInterruptEnable(MINIPCIE_INT_HANDLER MyDmaIntHandler, void* pDMA);
	void hwDMAInterruptDisable();
	void hwDMAProgram(
		WD_DMA_PAGE* Page,
		DWORD dwPages,
		bool fToDev,
		uint32_t u32LocalAddr,
		TransferDesc* tfrDesc);


private:
	// Board configuration
	static const uint32_t	sysid_offset = 0x0000;		// 0x0000_0007
	static const uint32_t	control_offset = 0x0020;	///< R/W 32-bit	General Purpose Outputs
	static const uint32_t	irig_offset = 0x0040;		///< R   32-bit	General Purpose Inputs
	static const uint32_t	ledPio_offset = 0x0060;		///< R/W   32-bit	LEDs and resets
	static const uint32_t	input0_offset = 0x0080;		///< General Purpose Inputs
	static const uint32_t	input1_offset = 0x00a0;		///< General Purpose Inputs
	static const uint32_t	dma_offset = 0x0200;		///< Block DMA address in Bar2
	static const uint32_t	spi_offset = 0x0400;
	static const uint32_t	spi_control_offset = 0x0480;
	static const uint32_t	spi_status_offset = 0x04a0;

	static const uint32_t	flash_offset = 0x0800;		// 0x0000_0fff
	static const uint32_t	mddr_offset = 0x0000;		// 0x0000_0fff
	static const uint32_t	txs_offset = 0x02000000;	///< Address of the txs in the Avalon bus, BAR2. Typically not reachable from the PC



};
