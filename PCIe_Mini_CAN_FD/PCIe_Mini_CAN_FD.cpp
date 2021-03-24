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
/** @file PCIe_Mini_CAN_FD.cpp
 * @brief Implementation of the PCIeMini_CAN_FD board class.
 */

 // Maintenance Log
//---------------------------------------------------------------------
// v1.0		7/23/2020	phf	Written
//---------------------------------------------------------------------

#include <stdio.h>
#include "PCIeMini_CAN_FD.h"

/* command mailbox
*/

/*! The constructor does not take any parameter. The board is not actually
 usable until the open method connects it to real hardware.
 */
PCIeMini_CAN_FD::PCIeMini_CAN_FD() :
	AlphiBoard(0x13c5, 0x508)
{
	controlRegister = NULL;
	irig = NULL;
	can[0] = NULL;
	can[1] = NULL;
	can[2] = NULL;
	can[3] = NULL;
	input0 = NULL;
	input1 = NULL;
	dpr = NULL;
	dma = NULL;
	ledPio = NULL;
	mddr = NULL;
}

//! Open: connect to an actual board
/*!
	\param brdNbr The board number is actually system dependent but if you have only one board, it should be 0.
	\return  ERRCODE_NO_ERROR if successful.
*/
PCIeMini_status PCIeMini_CAN_FD::open(int brdNbr)
{
	if (brdNbr < 0 || brdNbr>9)
		return ERRCODE_INVALID_BOARD_NUM;
	HRESULT result = AlphiBoard::Open(brdNbr);
	if (result != ERRCODE_SUCCESS) {
		return result;
	}

	if (verbose) {
		std::cout << "bar #0 address = 0x" << std::hex << bar0.Address << std::endl;
		std::cout << "bar #2 address = 0x" << std::hex << bar2.Address << std::endl;
		std::cout << "bar #3 address = 0x" << std::hex << bar3.Address << std::endl;
	}

	// create the device objects
	cra = new PcieCra(getBar0Address(0));
	cra->setTxsAvlAddress(txs_offset, 0x1000000, 2);

	controlRegister = new AlteraPio(getBar2Address(control_offset), AlteraPio::CAP_OUTPUT);
	ledPio = new AlteraPio(getBar2Address(ledPio_offset), AlteraPio::CAP_OUTPUT);
	irig = new IrigDecoder(getBar2Address(irig_offset));
#ifdef DMA_ENABLED
	dma = new AlteraDma(getBar2Address(dma_offset));
#endif
	input0 = new ParallelInput(getBar2Address(input0_offset));
	input1 = new ParallelInput(getBar2Address(input1_offset));

	for (uint8_t i = 0; i < nbrOfCanInterfaces; i++)
	{
//		can[i] = new TCAN4550(getBar2Address((size_t)(spi_offset + (size_t)(i) * 0x40)), ledPio, i);
		new ParallelInput(getBar2Address(input1_offset));
		can[i] = new TCAN4550(getBar2Address(spi_offset), 
			new AlteraPio(getBar2Address((size_t)tcans_offset + i * tcans_size + tcans_ctrl_offset), AlteraPio::CAP_OUTPUT),
			new ParallelInput(getBar2Address((size_t)tcans_offset + i* tcans_size + tcans_st_offset)),
			i);
	}

	dpr = (volatile uint32_t*)getBar2Address(dpr_offset);
	mddr = (volatile uint16_t*)getBar3Address(mddr_offset);

	return ERRCODE_NO_ERROR;
}

//! Close the connection to a board object and free the resources
/*!
		\return  ERRCODE_NO_ERROR if successful.
*/
PCIeMini_status PCIeMini_CAN_FD::close()
{
	AlphiBoard::Close();

	delete controlRegister;
	controlRegister = NULL;
	for (int i = 0; i < 4; i++)
	{
		delete can[i];
		can[i] = NULL;
	}
	delete input0;
	delete input1;
	input0 = NULL;
	input1 = NULL;

	dpr = NULL;

	return ERRCODE_NO_ERROR;
}

//! Reset the board controllers
/*!
		\return  ERRCODE_NO_ERROR if successful.
*/
PCIeMini_status PCIeMini_CAN_FD::reset()
{
	if (controlRegister == NULL)
		return ERRCODE_INVALID_HANDLE;
	controlRegister->reset();

	input0->reset();
	input1->reset();

	for (int i = 0; i < 4; i++)
	{
		can[i]->reset();
	}

	AlphiBoard::reset();
	return ERRCODE_NO_ERROR;
}

#ifdef DMA_ENABLED

void PCIeMini_CAN_FD::hwDMAStart(TransferDesc* tfrDesc)
{
	dma->launch_bidir(tfrDesc);
}

bool PCIeMini_CAN_FD::hwDMAWaitForCompletion(TransferDesc* tfrDesc, bool fPolling)
{
	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;

	QueryPerformanceCounter(&StartingTime);
	while (dma->getLength() != 0)
	{
		QueryPerformanceCounter(&EndingTime);
		ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
		int delay = (int)ElapsedMicroseconds.QuadPart;
		if (delay > 10000) return false;
	}
	return true;
}

bool PCIeMini_CAN_FD::hwDMAInterruptEnable(MINIPCIE_INT_HANDLER MyDmaIntHandler, void* pDMA) { return false; }

void PCIeMini_CAN_FD::hwDMAInterruptDisable() {}

/** @brief program the local devices (DMA and CRA) for the DMA
 @param fToDev When true DMA to device, when false DMA from device.

 */
void PCIeMini_CAN_FD::hwDMAProgram(
	WD_DMA_PAGE* Page, 
	DWORD dwPages, 
	bool fToDev, 
	uint32_t u32LocalAddr,
	TransferDesc *tfrDesc)
{
	uint64_t pcieAddress = Page[0].pPhysicalAddr; // Physical sync of page.
	uint32_t txsLocalAddress;
	;

	// program the CRA
	cra->setTrEntry(0, true, pcieAddress);

	// calculate the local address in the txs
	cra->getMappedAddress(pcieAddress, 0, &txsLocalAddress);

	tfrDesc->tfr_length = 0;
	tfrDesc->txs_offset = txsLocalAddress;

	if (fToDev) {		// DMA to device
		tfrDesc->src_offset = txsLocalAddress;
		tfrDesc->dest_offset = u32LocalAddr;
	}
	else {				// DMA from device
		tfrDesc->src_offset = u32LocalAddr;
		tfrDesc->dest_offset = txsLocalAddress;
	}

	Sleep(10);

}
#endif

