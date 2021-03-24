//
// Copyright (c) 2020 Alphi Technology Corporation.  All Rights Reserved
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
#pragma once
#include <stdint.h>
#include "AlphiDll.h"
#include "RxFifoData.h"

/** @brief SCC FIFO access class 
 *
 * This class allows resetting and checking the state of the byte oriented FIFOs used by the transmitter and by the receiver.
 */
class DLL SccFifo
{
public:
	const	static int fifoSize = 2048;	///< Total FIFO size in bytes

	SccFifo(uint32_t* ctrlAddress, RxFifoData *fifoOut = NULL);

	int usage();				///< Returns the number of bytes in the FIFO.
	void reset();				///< reset the FIFO.
	bool	isFifoEmpty();
	bool	isFifoAlmostEmpty();
	bool	isFifoAlmostFull();
	bool	isFifoFull();
	int	fifoSpace();	   ///< Returns the space available in the FIFO in bytes.
	uint32_t getCtrlReg();
	uint32_t FifoGetWord();
	int		getByte();
	int		bytesReceived;
private:
	volatile uint32_t* ctrlReg;
	bool readFifo;
	RxFifoData* rxFifoData;

	// FIFO control register
	static const uint32_t fifoUsedMask = 0x00000fff;		///< Mask to 
	static const uint32_t fifoFullMask = 0x00010000;
	static const uint32_t fifoAlmostFullMask = 0x00020000;
	static const uint32_t fifoAlmostEmptyMask = 0x00040000;
	static const uint32_t fifoEmptyMask = 0x00080000;
	static const uint32_t fifoResetMask = 0x80000000;
	static const uint32_t fifoIrqMask = 0x07000000;

	void setCtrlReg(uint32_t val);

	uint32_t  inBuf;				///<	Bytes available for read when in FIFO mode
	uint32_t  inBufValid;			///<	Number of valid bytes in inBuf available for read when in FIFO mode

};