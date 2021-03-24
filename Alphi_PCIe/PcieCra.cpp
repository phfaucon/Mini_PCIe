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
/** @file PcieCra.cpp
* @brief  PCIe interface CRA class 
*/

// Maintenance Log
//---------------------------------------------------------------------
//---------------------------------------------------------------------

#include "PcieCra.h"

/** @brief constructor 
 * 
 * This constructor should be only called when the board is opened.
 * 
 * @param cra_addr Address of the CRA in user space.
 */
PcieCra::PcieCra(volatile void* cra_addr)
{
	cra_address = cra_addr;
	pcieIrqStatus = (volatile uint32_t*)((uint8_t*)cra_addr + pcieIrqStatus_offset);
	pcieIrqEnable = (volatile uint32_t*)((uint8_t*)cra_addr + pcieIrqEnable_offset);
	trEntry = (TransEntry*)((uint8_t*)cra_addr + pcieAddrTrans_offset);
	txs_AvlAddress = 0xcccccccc;
	ttNbrOfEntries = 0;
	ttEntryOffset = 24;
	ttNbrOfEntriesBits = 1;
	ttPageAddressMask = 0x00ffffff;
}

/** @brief Reset the CRA PCIe interface

* Disable interrupts.
*/
void PcieCra::reset()
{
	*pcieIrqEnable = 0;
}

/** @brief return the interrupt status of the local IRQ lines
*
* In order for the PCIe interface to request an interrupt on the PCIe bus, the bit needs
* to be set in this register, and the corresponding bit should be set in the interrupt
* mask register.
*/
uint32_t PcieCra::getIrqStatus()
{
	return *pcieIrqStatus;
}


/** @brief Enable/disable the interrupts
* @param mask bit mask of enabled interrupts
*/
void PcieCra::setIrqEnableMask(uint32_t mask)
{
	*pcieIrqEnable = mask;
}

/** @brief return the interrupt enable mask
*/
uint32_t PcieCra::getIrqEnableMask()
{
	return *pcieIrqEnable;
}

/** @brief Set the local Avalon address for the PCIe txs port
* 
* For example, if the core is configured with an address translation table with the
	following attributes :
- Number of Address Pages—16
	- Size of Address Pages—1 MByte
	- PCI Express Address Size—64 bits
	then the values in Figure 4–12 are :
- N = 20 (due to the 1 MByte page size)
	- Q = 16 (number of pages)
	- M = 24 (20 + 4 bit page selection)
	- P = 64
	In this case, the Avalon address is interpreted as follows :
- Bits[31:24] select the TX slave module port from among other slaves connected to
	the same master by the system interconnect fabric.The decode is based on the base
	addresses assigned in Qsys.
	- Bits[23:20] select the address translation table entry.
	- Bits[63:20] of the address translation table entry become PCI Express address bits
	[63:20].
	- Bits[19:0] are passed throughand become PCI Express address bits[19:0].
	The address translation table can be hardwired or dynamically configured at run
	time.When the IP core is parameterized for dynamic address translation, the address
	translation table is implemented in memoryand can be accessed through the CRA
	slave module.This access mode is useful in a typical PCI Express system where
	address allocation occurs after BIOS initialization.

	- Number of Address Pages—2
	- Size of Address Pages—16 MByte
	- PCI Express Address Size—64 bits
	then the values in Figure 4–12 are :
- N = 24 (due to the 16 MByte page size)
	- Q = 2 (number of pages)
	- M = 25 (24 + 1 bit page selection)
	- P = 64
	In this case, the Avalon address is interpreted as follows :
- Bits[31:24] select the TX slave module port  from among other slaves connected to
	the same master by the system interconnect fabric.The decode is based on the base
	addresses assigned in Qsys.
	- Bits[24] select the address translation table entry.
	- Bits[63:25] of the address translation table entry become PCI Express address bits
	[63:20].
	- Bits[23:0] are passed throughand become PCI Express address bits[19:0].
	The address translation table can be hardwired or dynamically configured at run
	time.When the IP core is parameterized for dynamic address translation, the address
	translation table is implemented in memoryand can be accessed through the CRA
	slave module.This access mode is useful in a typical PCI Express system where
	address allocation occurs after BIOS initialization.

	* @param txs_addr Local Avalon Address of the txs area
	* @param nbrOfEntries Number of table entries used to calculate the bit pattern
	* @param pageSize size of each translation page (this is currently ignored, hard coded)
 */
PCIeMini_status PcieCra::setTxsAvlAddress(uint32_t txs_addr, uint64_t pageSize, uint16_t nbrOfEntries)
{
	txs_AvlAddress = txs_addr;
	ttPageAddressMask = 0x00ffffff;
	ttEntryOffset = 24;
	ttNbrOfEntries = nbrOfEntries;
	ttNbrOfEntriesBits = 1;

	uint64_t i;
	// check for page size, it should be a power of 2, between 10 and 32.
	ttEntryOffset = 12;
	for (i = 0x1000; i <= 0x100000000; i = i * 2)
	{
		if (i == pageSize) {
			break;
		}
		ttEntryOffset++;
	}
	if (i == pageSize) {
		ttPageAddressMask = (uint32_t)i - 1;
	}
	else {			// invalid page size, put default values and return error
		ttPageAddressMask = 0x00ffffff;
		ttEntryOffset = 24;
		return ERRCODE_INVALID_VALUE;
	}
	// check number of entries
	ttNbrOfEntriesBits = 0;
	i = ttNbrOfEntries;
	while (i > 1) {
		i /= 2;
		ttNbrOfEntriesBits++;
	}
	return ERRCODE_NO_ERROR;
}

/** @brief Calculate the DMA address through the txs
 * @param pcieAddress PCIe address of the PC memory
 * @param tableEntry entry number in the translation table
 * @param localAddress Address to program in the DMA to access the txs port of the PCIe controller
 */
PCIeMini_status PcieCra::getMappedAddress(uint64_t pcieAddress, int tableEntry, uint32_t* localAddress)
{
	// compare mapping with the Address Translation Table

	// not found, return error
	// compose local address
	uint32_t la = (uint32_t)pcieAddress & ttPageAddressMask;
	la |= tableEntry << ttEntryOffset;
	la += txs_AvlAddress;
	*localAddress = la;

	return ERRCODE_NO_ERROR;
}

/** @brief program an entry in the translation table 
 *
 * @param entryNbr Index of the entry to set up.
 * @param is64bitAddress True if the address of the target location is a 64-bit address
 * @param pcieAddress Address of the target location
*/

int PcieCra::setTrEntry(int entryNbr, bool is64bitAddress, uint64_t pcieAddress)
{
	trEntry[entryNbr].setEntry((uint32_t)pcieAddress, (uint32_t)(pcieAddress >> 32), is64bitAddress);
	return 0;
}

/** brief initialize a translation table entry */
void PcieCra::TransEntry::setEntry(uint32_t lower, uint32_t upper, bool is64bitAddress)
{
	lower &= 0xfffffffc;
	if (is64bitAddress)
		lower |= 1;
	lower32 = lower;
	upper32 = upper;
	printf("setEntry: 0x%08x 0x%08x\n", upper, lower);
}
