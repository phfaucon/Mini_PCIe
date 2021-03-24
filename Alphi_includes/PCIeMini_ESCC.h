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
#ifndef _PCIE_MINI_ESCC_H
#define _PCIE_MINI_ESCC_H

#include <stdint.h>
#include <iostream>
//using namespace std;
#include "AlphiDll.h"
#include "AlphiBoard.h"
#include "SccChannel.h"
#include "AlphiErrorCodes.h"


//! PCIeMini_ESCC controller board object
class DLL PCIeMini_ESCC : public AlphiBoard
{
public:
	BoardVersion* sysid;					///< Board identification
	volatile uint32_t* ledPio;				///< LED control
	SccChannel* sccDevice_0;				///< Descriptor for first channel
	SccChannel* sccDevice_1;				///< Descriptor for second channel
	UartChannelConfig scc_config[2];
	PcieCra* cra;

	PCIeMini_ESCC();

	PCIeMini_status open(int brdNbr);
	PCIeMini_status close();
	PCIeMini_status reset();

	void setLedPio(uint32_t);
	uint32_t getLedPio();

	static char* getErrorMsg(PCIeMini_status errorNbr);

	SccChannel* getScc(int channelNbr);

	inline void sccRegisterRead(int chan, uint8_t regNbr, volatile uint8_t* val) {
		getScc(chan)->sccRegisterRead(regNbr, val);
	}

	inline void sccRegisterWrite(int chan, uint8_t regNbr, uint8_t val)
	{
		getScc(chan)->sccRegisterWrite(regNbr, val);
	}

	inline void enableRxDma(int chan)
	{
		getScc(chan)->enableRxDma();
	}

	inline void disableRxDma(int chan)
	{
		getScc(chan)->disableRxDma();
	}

	inline void enableTxDma(int chan)
	{
		getScc(chan)->enableTxDma();
	}

	inline void disableTxDma(int chan)
	{
		getScc(chan)->disableTxDma();
	}

	inline void resetChannel(int chan)
	{
		getScc(chan)->resetChannel();
	}

	/** Reset the chip and FIFOs */
	inline void resetChip(int chan)
	{
		getScc(chan)->reset();
	}

	int initChannel(int chan);

private:
	// Board configuration
	static const uint32_t	sysid_offset			= 0x0000;  // 0x0000_0007
	static const uint32_t	led_pio_offset			= 0x0010;  // 0x0000_001f
	static const uint32_t	scc_0_offset			= 0x0020;  // 0x0000_003f
	static const uint32_t	scc_1_offset			= 0x0080;  // 0x0000_009f
	static const uint32_t	flash_offset			= 0x0800;  // 0x0000_0fff

	bool				isOpen;
	uint32_t			*dpr;
	MINIPCIE_INT_HANDLER uicr;
	bool				interruptEnabled;
};

#endif // _PCIE_MINI_ESCC_H