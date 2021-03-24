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
/** @file PcieCra.h
* @brief PCIe interface CRA class 
*/

// Maintenance Log
//---------------------------------------------------------------------
//---------------------------------------------------------------------

#pragma once
#include <stdint.h>
#include <stdio.h>
#include "AlphiDll.h"
#include "AlphiErrorCodes.h"

/** @brief PCIe CRA module controller class
 *
 * This is a limited software interface to the CRA module of the PCIe adapter that: 
 * - allows to enable/disable the interrupt requests to the PCIe bus and check the status
 *			of the local interrupt request lines.
 * - allows some DMA to/from the PC
 * 
 * @param txs_addr Address of the txs interface of the PCIe interface chip, in the Avalon address space.
 *		It is used to program the DMA controller.

 * Avalon-MM to PCI Express Interrupt Enable Register, 0x0050
 *	Bits	Name				Access	Description
 *	[31:24] Reserved			N/A		N/A
 *	[23:16] A2P_MB_IRQ			RW		Enables generation of PCI Express interrupts when a specified mailbox is
 *										written to by an external Avalon-MM master.
 *	[15:0]	AVL_IRQ[15:0]				RW Enables generation of PCI Express interrupts when a specified Avalon-MM
 *										interrupt signal is asserted. Your system may have as many as 16 individual
 *										input interrupt signals
 *
 * Avalon-MM to PCI Express Interrupt Status Register, 0x0040
 *	Bit		Name				Access	Description
 *	[31:24] Reserved			N/A		N/A
 *	[23]	A2P_MAILBOX_INT7	RW1C	Set to 1 when the A2P_MAILBOX7 register is written to
 *	[22]	A2P_MAILBOX_INT6	RW1C	Set to 1 when the A2P_MAILBOX6 register is written to
 *	[21]	A2P_MAILBOX_INT5	RW1C	Set to 1 when the A2P_MAILBOX5 register is written to
 *	[20]	A2P_MAILBOX_INT4	RW1C	Set to 1 when the A2P_MAILBOX4 register is written to
 *	[19]	A2P_MAILBOX_INT3	RW1C	Set to 1 when the A2P_MAILBOX3 register is written to
 *	[18]	A2P_MAILBOX_INT2	RW1C	Set to 1 when the A2P_MAILBOX2 register is written to
 *	[17]	A2P_MAILBOX_INT1	RW1C	Set to 1 when the A2P_MAILBOX1 register is written to
 *	[16]	A2P_MAILBOX_INT0	RW1C	Set to 1 when the A2P_MAILBOX0 register is written to
 *	[15:0]	AVL_IRQ_ASSRTD		RO		Current value of the Avalon-MM interrupt
 *									(IRQ) input ports to the Avalon-MM RX master port:
 *										� 0�Avalon-MM IRQ is not being signaled.
 *										� 1�Avalon-MM IRQ is being signaled.
 *									A PCIe variant may have as many as 16 distinct IRQ input ports.
 *									Each AVL_IRQ_ASSERTED[] bit reflects the value on the
 *									corresponding IRQ input port.
 */
class DLL PcieCra
{
public:
	PcieCra(volatile void* cra_addr);

	void reset();

	uint32_t getIrqStatus();
	void setIrqEnableMask(uint32_t mask);
	uint32_t getIrqEnableMask();

	PCIeMini_status setTxsAvlAddress(uint32_t txs_addr, uint64_t pageSize, uint16_t nbrOfEntries);
	PCIeMini_status getMappedAddress(uint64_t pcieAddress, int tableEntry, uint32_t* localAddress);

	int setTrEntry(int entryNbr, bool is64bitAddress, uint64_t pcieAddress);

private:

	/** @brief class representing a physical entry in the translation table.
	 *
	 * The address of the object is actually the address of the physical table entry.
	 */
	class DLL TransEntry
	{
	public:
		volatile uint32_t lower32;
		volatile uint32_t upper32;

		void setEntry(uint32_t lower, uint32_t upper, bool is64bitAddress);
	};


	volatile void* cra_address;

	uint32_t txs_AvlAddress;
	uint32_t ttPageAddressMask;	///< part of the local address kept in the translation
	uint8_t ttEntryOffset;		///< offset of the entry number
	uint16_t ttNbrOfEntries;
	uint16_t ttNbrOfEntriesBits;

	static const int pcieIrqStatus_offset = 0x40;
	static const int pcieIrqEnable_offset = 0x50;
	static const int pcieAddrTrans_offset = 0x1000;
	static const int trNbrOfTrEntries = 2;		///< Number of address translation entries specified in QSYS
	static const uint32_t trPageSize = 0x01000000;	///< page size specified in QSYS : 16 MB

	volatile uint32_t* pcieIrqStatus;		///< PCIe interface interrupt status
	volatile uint32_t* pcieIrqEnable;		///< PCIe interface interrupt enable
	TransEntry* trEntry;
};

