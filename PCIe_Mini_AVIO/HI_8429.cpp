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
// OR INABILITY TO USE THE SOFTWARE.  ALPHI assumes no responsibility for
// for the maintenance or support of the SOFTWARE
//
/** @file HI_8429.cpp
 * @brief Implementation of a class representing the HI_8429 on the AVIO board.
 */

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		3/3/2021	phf	Written
//---------------------------------------------------------------------

#include "HI_8429.h"

HI_8429::HI_8429(volatile void *spiController, AvioCtrlReg* pioControl)
{
	spi = new SpiOpenCore(spiController);
	ctrlReg = pioControl;
	reset();
}

void HI_8429::reset(void)
{
	spi_init();
	ctrlReg->setDebounceEnable(false);
	ctrlReg->resetHolt();
	setHoltConfigReg(0);		// normal inputs, build-in test disabled
	setHoltThresholdReg(1.0, 4.0, 1.0, 4.0);
}

void HI_8429::spi_init(void)
{
	spi->setSpiControl(SpiOpenCore::SPI_CTRL_ASS | 0x09);	// ass 9 bits
	spi->setSpiDivider(0x05); 								// HI_8429 SPI f_max = 10 MHz.  62.5 MHz / ((5 + 1) * 2 ) = 5.2 MHz
	spi->selectSpiSlave(0xff); 								// we only have 1
}

uint32_t HI_8429::setHoltConfigReg(uint32_t config_val)
{
	uint32_t read_val;
	spi->setTransferWidth(9);
	ctrlReg->setSelect(CFG_REG);
	read_val = spi->rw(config_val);
	return read_val;
}

/** @brief Sets the HI-8429 Threshold Register
 * @param thresh_val Binary value to put in the HI-8429 register
 * @retval Previous value of the threshold register.
 */
uint32_t HI_8429::setHoltThresholdReg(uint32_t thresh_val)
{
	uint32_t read_val;
	spi->setTransferWidth(24);
	ctrlReg->setSelect(THRESH_REG);
	read_val = spi->rw(thresh_val);
	return read_val;
}

/** @brief Sets the HI-8429 Threshold Register
 * @retval Previous value of the threshold register.
 */
uint32_t HI_8429::setHoltThresholdReg(double gl, double gh, double vl, double vh)
{
	uint32_t prev_val;

	spi->setTransferWidth(24);
	ctrlReg->setSelect(THRESH_REG);
	uint32_t thresh_val = calculateThresholds( gl, gh, vl, vh);
	prev_val = spi->rw(thresh_val);
	return prev_val;
}

/** @brief Read the HI-8429 Sense Register
 * @retval Value of the sense register.
 */
uint8_t HI_8429::getHoltSenseReg(void)
{
	uint8_t read_val;
	spi->setTransferWidth(8);
	ctrlReg->setSelect(SENSE_REG);
	read_val = spi->rw(0x00000000);
	return read_val;
}



void HI_8429::print_holt_thresh(uint32_t thresh)
{
	printf("GL %x\n", (int)((thresh >> 18) & 0x3f));
	printf("GH %x\n", (int)((thresh >> 12) & 0x3f));
	printf("VL %x\n", (int)((thresh >>  6) & 0x3f));
	printf("VH %x\n", (int)((thresh >>  0) & 0x3f));
}

