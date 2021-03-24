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
/** @file SpiOpenCore.cpp
* @brief Definition of the SpiOpenCore board class.
*/

#include "AlphiDll.h"
#include "SpiOpenCore.h"

void SpiOpenCore::setTransferWidth(uint8_t width)
{
	uint32_t ctrl = getSpiControl();
	ctrl &= ~0x7f;
	width &= 0x7f;
	setSpiControl(ctrl | width);
}

uint8_t SpiOpenCore::getTransferWidth()
{
	uint32_t ctrl = getSpiControl();
	return  ctrl & 0x7f;
}

void SpiOpenCore::startTransfer(void)
{
	while(getSpiControl() & SPI_CTRL_GO); // wait for no busy

	setSpiControl(getSpiControl() | SPI_CTRL_GO); // set busy
}

uint32_t SpiOpenCore::rw(uint32_t data)
{
	setSpiTxData(data); // 0
	startTransfer();
	while(getSpiStatus() & SPI_CTRL_GO); // wait for no busy
	return getSpiRxData();
}




