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
/** @file AlphiBoard.h
* @brief Base PCIe board class with Jungo driver and Altera PCIe hardware
*/

// Maintenance Log
//---------------------------------------------------------------------
//---------------------------------------------------------------------

#ifndef _ALPHI_BOARDS_H
#define _ALPHI_BOARDS_H
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include "AlphiDll.h"
#include "PcieCra.h"
#include "AlteraDma.h"

//typedef void * WDC_DEVICE_HANDLE;
#define ErrLog printf
#define TraceLog printf

/** @brief minipcie diagnostics interrupt handler function type */
typedef void (*MINIPCIE_INT_HANDLER)(void *pIntData);

/** @brief Memory Segment Descriptor */
typedef struct LinearAddress {
	void* Address;			///< Linear address.
	size_t   Length;		///< Length of the mapping.
} LinearAddress;

/** @brief Board Hardware identification and version */
class DLL BoardVersion {
public:
	BoardVersion(volatile uint32_t* addr);

	uint32_t getVersion();		//!<  Version, if there is one programmed on the board hardware. Typically 0.
	time_t getTimeStamp();		//!<  Date when the board firmware was compiled.
private:
	volatile uint32_t* address;
};

#define CLOCK_REALTIME 0

//! Base class implementing a PCIe board and the Jungo driver.
class DLL AlphiBoard
{
public:
	LinearAddress bar0;		///< Memory descriptor for the BAR0 in user memory
	LinearAddress bar2;		///< Memory descriptor for the BAR2 in user memory
	LinearAddress bar3;		///< Memory descriptor for the BAR3 in user memory
	PcieCra* cra;			///< PCIe Interface standard component. Used by the software for interrupt control and for DMA.
	BoardVersion* sysid;	///< Board identification. It contains the board type and a time stamp identifying the release

	AlphiBoard(uint16_t vendorId, uint16_t deviceId);

	~AlphiBoard(void);

	virtual PCIeMini_status Open(int board_num);

	/** @brief reset some of the board resources
	 *
	 */
	inline uint32_t reset(void)
	{
		cra->reset();
		return ERRCODE_SUCCESS;
	}

	uint32_t getFpgaID(void);
	time_t getFpgaTimeStamp(void);

	void setVerbose(int verbose);

	PCIeMini_status hookInterruptServiceRoutine(uint32_t mask, MINIPCIE_INT_HANDLER uicr, void* userData);
	PCIeMini_status hookInterruptServiceRoutine(MINIPCIE_INT_HANDLER uicr);

	PCIeMini_status unhookInterruptServiceRoutine(void);

	PCIeMini_status enableInterrupts(uint16_t mask = 0xffff);
	PCIeMini_status disableInterrupts(void);
	void* intThreadLoop(void);

	virtual PCIeMini_status Close(void);

	volatile void* getBar0Address(size_t offset);
	volatile void* getBar2Address(size_t offset);
	volatile void* getBar3Address(size_t offset);

	/** @brief Millisecond Delay Function */
	static inline void MsSleep(int ms)
	{
		usleep(ms * 1000);
	}

	int verbose;			///< Flag used by various functions to determine the amount of messages to generate


private:
	int brdNumber;			///< This board number is assigned by the UIO driver and is system dependent

	static const uint16_t defaultVendorId = 0x13c5;		///< Alphi Technology Corporation
	static const uint16_t defaultDeviceId = 0x0508;		///< PCIe-Mini I/O board
	uint32_t dwVendorId;								///< Actual Vendor Id
	uint32_t dwDeviceId;								///< Actual Device Id

	// Board configuration
	static const uint32_t	sysid_offset = 0x0000;		///< Offset of the Sysid component in BAR 2.
	static const uint32_t	cra_address = 0x0000;		///< Offset in BAR 0

	MINIPCIE_INT_HANDLER funcDiagIntHandler;
	void *intHandlerUserData;

	// UIO specific
	char uioDev[20];					///< UIO device name
	char uioName[20];					///< UIO driver name
	char uioVersion[20];				///< UIO driver version

	static const int nbrOfBars = 8;
	size_t uioBarSizes[nbrOfBars];

	int readConfigString(char *data, int *len, const char *name);
	int readConfigHex(const char *name);
	int readUioResource();

	bool brd_valid;						///< When true, the board should be open and allocated
	int     uiofd;						///< UIO Descriptor for board
	int     configfd;					///< UIO Descriptor for board config
	int		resfd0;						///< UIO Descriptor for BAR #0
	int		resfd2;						///< UIO Descriptor for BAR #0

	pthread_t irqThread;				///< Thread managing the interrupts. Independent from the main driver thread, Beware of concurrency issues.
	int startIntThread();
};

#endif
