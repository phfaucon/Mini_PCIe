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
/** @file AvioCtrlReg.h
* @brief Definitition of the PCIeMini_CAN_FD board class.
*/

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		3/20/2021	phf	Written
//---------------------------------------------------------------------

#include "AlphiDll.h"
#include "AlteraPio.h"

class AvioCtrlReg : public AlteraPio
{
public:
	inline AvioCtrlReg(volatile void* addr)
		: AlteraPio(addr, AlteraPio::CAP_OUTPUT)
	{
		setData(0x10);
	}

	void setSelect(uint8_t sel);
	uint8_t getSelect(void);
	void resetHolt(void);
	void setDebounceEnable(bool enable);
	void setSenseSelect(bool high);

private:
	static const uint8_t SelHiMask = 0x01;
	static const uint8_t selectMask = 0x06;
	static const uint8_t debounceMask = 0x08;
	static const uint8_t resetMask = 0x10;
};

