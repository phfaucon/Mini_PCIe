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

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		7/23/2020	phf	Written
//---------------------------------------------------------------------
//
#ifndef _VME_SCC_CHANNEL_H
#define _VME_SCC_CHANNEL_H

#include "AlphiDll.h"
#include "stdint.h"
#include "escc_defines.h"
#include "LTC2872.h"
#include "SccFifo.h"
#include "RxFifoData.h"
#include "AlphiErrorCodes.h"

#undef NIOS
#define PCIeMINI_ESCC

#ifdef PCIeMINI_ESCC
#undef D16_SWAP
#undef D32_SWAP
#endif



#define SCC_BUSY		-1

static const uint8_t R0_PTR = 0;
static const uint8_t R1 = 1;
static const uint8_t R2_IVR = 2;
static const uint8_t R3 = 3;
static const uint8_t R4 = 4;
static const uint8_t R5 = 5;
static const uint8_t R6 = 6;
static const uint8_t R7 = 7;
static const uint8_t R8 = 8;
static const uint8_t R9 = 9;
static const uint8_t R10 = 10;
static const uint8_t R11 = 11;
static const uint8_t R12_BRGL = 12;
static const uint8_t R13_BRGH = 13;
static const uint8_t R14 = 14;
static const uint8_t R15 = 15;

//! @brief UART channel configuration structure
/*!
 * This structure contains the configuration values for the UART channel.
 *
*/
class DLL UartChannelConfig
{
public:
	int baudRate;			///< 300 to 38400 bps
	uint8_t dataBits;		///< 7 or 8 data bits
	uint8_t stopBits;		///< 1 or 2 stop bits
	bool parityEnable;		///< 0 for parity disabled, 1 for parity enabled
	uint8_t parity;			///< 0 for even parity, 1 for odd parity
	uint8_t eolChar;		///< byte value used as an end of line for the Scc_gets_s function. Default is 0x0a. If eolChar contains 0xff, the feature is disabled.
	int bufferConfig;		///< buffer selection
	bool useRxFifo;			///< use external FIFO to store received data
	bool useTxFifo;			///< use external FIFO to store data to transmit
	bool localLoopbackMode;

	UartChannelConfig();
};

//! Low-level SCC access class
/*!
 * This class contains low-level functions to access an SCC controller with a FIFO input and a FIFO output.
 * It has no provision for DMA however the data can be either read or written directly to the SCC data registers,
 * or to/from a FIFO that will send the data to the SCC using the SCC DMA mechanism.
 * Going through the FIFO allows 32-bit read and write operations.
 *
*/
class DLL SccChannel {
public:
	static const int SCC_clocksource = 16000000;					///< PCLK frequency used for baud rate calculations
	static const int RTxcFrequency = 14745600;					///< TxC input, should be 14.745600 MHz. Used to calculate the value for the divider register.
	volatile void	*baseAddress;		///< Pointer to the channel hardware
	UartChannelConfig scc_config;		///< Copy of the SCC configuration parameters
	LTC2872* buffer;					///< Pointer to the buffer control
	SccFifo* rxFifo;					///< Receiver FIFO
	SccFifo* txFifo;					///< Transmitter FIFO

	SccChannel(volatile void *baseAddress);
	PCIeMini_status	reset();
	void	resetChannel();
	void	set_serialBuffers(uint32_t val);

	int32_t	sccRegisterRead(uint8_t regNbr, volatile uint8_t *val);
	int32_t	sccRegisterWrite(uint8_t regNbr, uint8_t val);

	int32_t	sccDataRead(uint8_t *val);
	int32_t	sccDataWrite(uint8_t val);

	int		channelLoad(uint8_t *rtable);

	/*! \brief Change an SCC channel configuration.
	 *
	 * Scc_Config allows setting new UART parameters.
	 * placed in pSizeRead.
	 * @param  config New channel configuration.
	 * @return If successful, Scc_Config returns 0. If the function fails, it returns a nonzero value.
	*/
	PCIeMini_status config(
		UartChannelConfig* config
	);

	// SCC control register
	static const uint32_t sccControlRxFifoIrqMask = 0x040000;			///< status of receive FIFO interrupt request
	static const uint32_t sccControlTxFifoIrqMask = 0x020000;			///< status of transmit FIFO interrupt request
	static const uint32_t sccControlSccIrqMask = 0x010000;				///< status of the interrupt requests from the ESCC
	static const uint32_t sccControlMainResetMask = 0x008000;			///< When set, do a hard reset on the SCC logic. Must be manually set to 0 to allow operations.
	static const uint32_t sccControlFilterOnMask = 0x002000;			///< Filter the escaped bisync characters
	static const uint32_t sccControlTxDmaEnableMask = 0x001000;			///< Use the FIFO to transmit characters
	static const uint32_t sccControlRxDmaEnableMask = 0x000800;			///< Use the FIFO to receive characters
	static const uint32_t sccControlIrqEnableMask = 0x000400;			///< General interrupt enable
	static const uint32_t sccControlStripcharRegMask = 0x00ff;			///< Bisync character to strip

	uint32_t setSccControlRegister(uint32_t mask);
	uint32_t resetSccControlRegister(uint32_t mask);
	uint32_t getSccControlRegister();

	int32_t	enableRxDma();
	int32_t	disableRxDma();
	int32_t	enableTxDma();
	int32_t	disableTxDma();

	int32_t	enableRTS();
	int32_t	disableRTS();

	void	outch(uint8_t val);
	int		outchnw(uint8_t val);

	int 	inchnw();
	int 	inch();
    int 	inch(int pollDelay);

	char *gets_s(char* buffer, size_t n);
	int puts(const char* str);

	uint8_t brgDivLow(int SCC_clocksource, int div, int baudRate);

	PCIeMini_status enableIntSCC(uint8_t mask);
	PCIeMini_status disableIntSCC(uint8_t mask);

	void dump_regs();
	uint8_t	cachedRegs[32];

private:
	static const int sccCtrlRegOffset = 0x00;
	static const int sccDataRegOffset = 0x04;
	static const int chanCtrlRegOffset = 0x08;
	static const int rxFifoCtrlOffset = 0x0c;
	static const int txFifoCtrlOffset = 0x10;
	static const int rxFifoDataOffset = 0x20;
	static const int rxFifoValidOffset = 0x24;
	static const int txFifoDataOffset = 0x28;
	static const int bufferCtrlOffset = 0x30;

	int pollingDelay;			///< Delay used when polling for characters to receive or transmit in milliseconds. Should be calculated in relation with a message transmission time.

	volatile uint8_t* txFiFoData8bit;
	volatile uint8_t* sccCtrlReg;
	volatile uint8_t* sccDataReg;
	volatile uint32_t* controlRegister;
//	volatile uint32_t* rxFifoData;
//	volatile uint32_t* rxFifoDataStatus;
	RxFifoData* rxFifoData;

	void	outch_FIFO(uint8_t val);
	int		outchnw_FIFO(uint8_t val);
	int 	inchnw_FIFO();
	int 	inch_FIFO();

	bool	useFifoRcv;			///<	Receive FIFO use is enabled
	bool	useFifoTrmt;		///<	Transmit FIFO use is enabled

};


#endif //_VME_SCC_CHANNEL_H
