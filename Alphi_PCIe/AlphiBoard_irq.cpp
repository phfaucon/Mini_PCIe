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
/** @file AlphiBoard_irq.cpp
* @brief Implementation of the interrupts for the base PCIe board class with Jungo driver and Altera PCIe hardware
*/

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		7/23/2020	phf	Written
//---------------------------------------------------------------------
#include "AlphiBoard.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>

/* -----------------------------------------------
Interrupts
----------------------------------------------- */
//! Setup the interrupt of the board
/*!
Specify and interrupt service routine and enable the interrupts.
	@param mask board dependent interrupt mask.
	@param uicr pointer to the interrupt service routine.
	@param userData Value sent to the interrupt service routine as parameter.
	@return WD_STATUS_SUCCESS when the operation succeeded
		WD_INVALID_PARAMETER if the board is not opened
		WD_OPERATION_FAILED if the board does not have an interrupt resource
		WD_OPERATION_ALREADY_DONE if there is already an isr active for the interrupt.
*/
PCIeMini_status AlphiBoard::hookInterruptServiceRoutine(uint32_t mask, MINIPCIE_INT_HANDLER uicr, void *userData)
{

	/* Store the diag interrupt handler routine, which will be executed by
	MINIPCIE_IntHandler() when an interrupt is received */
	funcDiagIntHandler = uicr;
	intHandlerUserData = userData;

	return ERRCODE_SUCCESS;
}

//! Set an interrupt handling routine
/*!
		@param	 uicr user callback routine typedef void (__stdcall *UsersIntCompletionRoutine)(void *, uint32_t);
		@return  ERRCODE_NO_ERROR if successful.
*/
PCIeMini_status AlphiBoard::hookInterruptServiceRoutine(MINIPCIE_INT_HANDLER uicr)
{
	PCIeMini_status status;
	status = AlphiBoard::hookInterruptServiceRoutine(
		1, uicr, (void*)this);
	return status;
}

//! Disable the board interrupt
/*!
	@param mask board dependent interrupt mask.
	@param uicr pointer to the interrupt service routine.
	@return WD_STATUS_SUCCESS when the operation succeeded
		WD_INVALID_PARAMETER if the board is not opened
		WD_OPERATION_FAILED if the board does not have an interrupt resource
		WD_OPERATION_ALREADY_DONE if there the interrupt is already disabled.
*/
PCIeMini_status AlphiBoard::unhookInterruptServiceRoutine()
{
	return hookInterruptServiceRoutine(0, NULL, NULL);
}

/** @brief Enable PCIe interrupts
 *
 * Enable the generation of PCIe interrupts by the board's PCIe interface. Enable the reception of PCIe
 * interrupts by the Windows driver.
 * @param mask Optional bit map of which local interrupt line is enabled (board dependent.) If not used, 
 *				default to 0xffff - all local interrupts allowed.
 * @retval Status code
 */
PCIeMini_status AlphiBoard::enableInterrupts(uint16_t mask)
{
	PCIeMini_status dwStatus = ERRCODE_SUCCESS;

	cra->setIrqEnableMask(mask);

	return dwStatus;
}

/** @brief Disable PCIe interrupts
 *
 * Disable the generation of PCIe interrupts by the PCIe interface, and the reception by the Windows driver.
 * @retval Status code
 */
PCIeMini_status AlphiBoard::disableInterrupts()
{
	PCIeMini_status dwStatus = ERRCODE_SUCCESS;

	// Disable the IRQ generation on the PCIe bus
	cra->setIrqEnableMask(0);

	return dwStatus;
}

static void* intThreadEx(void *arg)
{
	AlphiBoard *brd = (AlphiBoard *)arg;
	brd->intThreadLoop();
	return NULL;
}

void* AlphiBoard::intThreadLoop(void)
{

	int i;
	int err;
	unsigned icount;
	unsigned char command_high;


	/* Read and cache command value */
	err = pread(configfd, &command_high, 1, 5);
	if (err != 1) {
		perror("command config read:");
		return NULL;
	}
	command_high &= ~0x4;

	for(i = 0;; ++i) {

		/****************************************/
		/* Here we got an interrupt from the
		   device. Do something about it. */
		/****************************************/

		/* Re-enable interrupts. */
		err = pwrite(configfd, &command_high, 1, 5);
		if (err != 1) {
			perror("config write:");
			break;
		}

		while (funcDiagIntHandler == NULL) Sleep(10);
		/* Wait for next interrupt. */
		err = read(uiofd, &icount, 4);
		if (err != 4) {
			perror("uio read:");
			err = read(uiofd, &icount, 4);
			break;
		}

		//clear_ints(p_uio_board->bar2); //
		funcDiagIntHandler(intHandlerUserData);

	}
	return 0;
}

int AlphiBoard::startIntThread()
{
	int err;

	if (brd_valid) {

		err = pthread_create(&irqThread, NULL, &intThreadEx, (void*)this);
		if (err != 0)
			printf("\ncan't create thread :[%s]", strerror(err));
		else
			printf("\n Interrupt Thread created successfully\n");

	} else
		printf("No valid board object\n");

	return err;
}
