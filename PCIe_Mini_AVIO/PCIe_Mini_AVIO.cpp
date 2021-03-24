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
/** @file PCIe_Mini_AVIO.cpp
 * @brief Implementation of the PCIeMini_AVIO board class.
 */

 // Maintenance Log
//---------------------------------------------------------------------
// v1.0		2/23/2020	phf	Written
//---------------------------------------------------------------------

#include <stdio.h>
#include "PCIeMini_AVIO.h"

/* command mailbox
*/

/*! The constructor does not take any parameter. The board is not actually
 usable until the open method connects it to real hardware.
 */
PCIeMini_AVIO::PCIeMini_AVIO() :
	AlphiBoard(0x13c5, 0x508)
{
	ledPio = NULL;
	digoutReadback = NULL;
	digout = NULL;
	ctrlReg = NULL;
	statusReg = NULL;
	avComp = NULL;
	rstCtrl = NULL;
	thresholds = NULL;
	hi8429 = NULL;
}

PCIeMini_AVIO::~PCIeMini_AVIO()
{
	close();
}

/** @brief Select the HI-8429 register to access
 *
 * This selects the register being accessed by the SPI read-write operation.
 */
void AvioCtrlReg::setSelect(uint8_t sel)
{
	uint32_t pio_data;
	pio_data = getData();
	pio_data &=  ~selectMask;
	pio_data = pio_data | ((sel & 0x03) << 1);
	setData(pio_data);
}

/** @brief Check which HI-8429 register is selected
 * @retval returns the 2 select bits.
 */
uint8_t AvioCtrlReg::getSelect(void)
{
	uint32_t pio_data;
	pio_data = getData();
	return (pio_data >> 1) & 0x03;
}

/** @brief Activates temporarily the HI-8429 reset line
 *
 * The control register reset bit is activated and then deactivated. The other control register bits are left unchanged.
 */
void AvioCtrlReg::resetHolt(void)
{
	uint32_t pio_data;
	pio_data = getData();
	pio_data = pio_data & ~resetMask;
	setData(pio_data);
	Sleep(1);
	pio_data = pio_data | resetMask;
	setData(pio_data);
}

/** @brief Set the SenseSelect line to the HI-8426s and the HI-8431s
 *
 * This line is used by the output read-back capability. Refers to the chip manual.
 * @param high When true the line is set to high.
 */
void AvioCtrlReg::setSenseSelect(bool high)
{
	uint32_t pio_data;
	pio_data = getData();
	if (high) {
		pio_data = pio_data | SelHiMask;
	}
	else {
		pio_data = pio_data & ~SelHiMask;
	}
	setData(pio_data);
}

/** @brief Set the Debounce Enable line of the HI-8429
 *
 * @param enable When true, the line enable the debounce feature of the HI-8429.
 */
void AvioCtrlReg::setDebounceEnable(bool enable)
{
	uint32_t pio_data;
	pio_data = getData();
	pio_data = pio_data & ~debounceMask;
	if(enable)
		pio_data = pio_data | debounceMask;

	setData(pio_data);
}


/** brief Set the Read-back Sense Threshold as a volt value.
 *
 * @param channel A channel number between 0 and 3.
 * <ul>
 * <li>0 : thresholds for the GND drivers, sense comparator 0 to 3,
 * <li>1 : thresholds for the GND drivers, sense comparator 4 to 7
 * <li>2 : thresholds for the VDRV drivers, sense comparator 8 to 11
 * <li>3 : thresholds for the VDRV drivers, sense comparator 12 to 15
 * </ul>
 * @param thresholdL Value in volt for the low threshold voltage.
 * @param thresholdH Value in volt for the high threshold voltage.
 * @retval ERRCODE_INVALID_CHANNEL_NUM or ERRCODE_NO_ERROR
 */
PCIeMini_status PCIeMini_AVIO::setReadbackThreshold(uint8_t channel, double thresholdL, double thresholdH)
{
	if (channel > nbrOfThesholdChannels) {
		if (verbose)
			printf("setOutputSenseThreshold: channel number invalid (%d)\n", channel);
		return ERRCODE_INVALID_CHANNEL_NUM;
	}
	thresholds[channel * 2] = (unsigned int)((thresholdH/10)/DA_LSB);
	thresholds[channel * 2 + 1] = (unsigned int)((thresholdL/10)/DA_LSB);

	return ERRCODE_NO_ERROR;
}

/** brief Read the actual Read-back Sense Threshold as a volt value.
 *
 * @param channel A channel number between 0 and 3.
 * <ul>
 * <li>0 : thresholds for the GND drivers, sense comparator 0 to 3,
 * <li>1 : thresholds for the GND drivers, sense comparator 4 to 7
 * <li>2 : thresholds for the VDRV drivers, sense comparator 8 to 11
 * <li>3 : thresholds for the VDRV drivers, sense comparator 12 to 15
 * </ul>
 * @param thresholdL Pointer to a value in volt for the low threshold voltage.
 * @param thresholdH Pointer to a value in volt for the high threshold voltage.
 * @retval ERRCODE_INVALID_CHANNEL_NUM or ERRCODE_NO_ERROR
 */
PCIeMini_status PCIeMini_AVIO::getReadbackThreshold(uint8_t channel, double *thresholdL, double *thresholdH)
{
	if (channel > nbrOfThesholdChannels) {
		if (verbose)
			printf("setOutputSenseThreshold: channel number invalid (%d)\n", channel);
		return ERRCODE_INVALID_CHANNEL_NUM;
	}
	*thresholdH = (thresholds[channel * 2] * 10.0) * DA_LSB;
	*thresholdL = (thresholds[channel * 2 + 1] * 10.0) * DA_LSB;

	return ERRCODE_NO_ERROR;
}

/** brief Set the Output Sense Threshold as a raw DAC value
 *
 * @param channel A channel number between 0 and 3.
 * <ul>
 * <li>0 : thresholds for the GND drivers, sense comparator 0 to 3,
 * <li>1 : thresholds for the GND drivers, sense comparator 4 to 7
 * <li>2 : thresholds for the VDRV drivers, sense comparator 8 to 11
 * <li>3 : thresholds for the VDRV drivers, sense comparator 12 to 15
 * </ul>
 * @param thresholdL Value for the low threshold voltage, as the 8-bit value to write in the DAC register.
 * @param thresholdH Value for the high threshold voltage, as the 8-bit value to write in the DAC register.
 * @retval ERRCODE_INVALID_CHANNEL_NUM or ERRCODE_NO_ERROR
 */
PCIeMini_status PCIeMini_AVIO::setReadbackThreshold(uint8_t channel, uint8_t thresholdL, uint8_t thresholdH)
{
	if (channel > nbrOfThesholdChannels) {
		if (verbose)
			printf("setOutputSenseThreshold: channel number invalid (%d)\n", channel);
		return ERRCODE_INVALID_CHANNEL_NUM;
	}
	thresholds[channel * 2] = thresholdH;
	thresholds[channel * 2 + 1] = thresholdL;

	return ERRCODE_NO_ERROR;
}

//! Open: connect to an actual board
/*!
	\param brdNbr The board number is actually system dependent but if you have only one board, it should be 0.
	\return  ERRCODE_NO_ERROR if successful.
*/
PCIeMini_status PCIeMini_AVIO::open(int brdNbr)
{
	if (brdNbr < 0 || brdNbr>9)
		return ERRCODE_INVALID_BOARD_NUM;
	HRESULT result = AlphiBoard::Open(brdNbr);
	if (result != ERRCODE_SUCCESS) {
		return result;
	}

	// create the device objects

	ledPio = new AlteraPio(getBar2Address(ledPio_offset), AlteraPio::CAP_OUTPUT);
	digoutReadback = new ParallelInput(getBar2Address(digoutReadback_offset));
	digout = new AlteraPio(getBar2Address(digout_offset), AlteraPio::CAP_OUTPUT);
	ctrlReg = new AvioCtrlReg(getBar2Address(ctrlReg_offset));
	statusReg = new AlteraPio(getBar2Address(statusReg_offset), AlteraPio::CAP_INPUT);
	avComp = new AlteraPio(getBar2Address(avComp_offset), AlteraPio::CAP_INPUT);
	rstCtrl = new AlteraPio(getBar2Address(ledPio_offset), AlteraPio::CAP_OUTPUT);
	thresholds = (volatile uint32_t*)getBar2Address(dac5308_offset);		///< DAC output value array
	hi8429 = new HI_8429 (getBar2Address(hi_8429_offset), ctrlReg);

	return ERRCODE_NO_ERROR;
}

//! Close the connection to a board object and free the resources
/*!
 *		@retval  ERRCODE_NO_ERROR if successful.
 */
PCIeMini_status PCIeMini_AVIO::close()
{
	PCIeMini_status st;

	st = AlphiBoard::Close();
	if (st != ERRCODE_NO_ERROR)
		return st;

	delete 	ledPio;
	delete	digoutReadback;
	delete	digout;
	delete	ctrlReg;
	delete	statusReg;
	delete	avComp;
	delete	rstCtrl;
	delete	hi8429;

	ledPio = NULL;
	digoutReadback = NULL;
	digout = NULL;
	ctrlReg = NULL;
	statusReg = NULL;
	avComp = NULL;
	rstCtrl = NULL;
	thresholds = NULL;
	hi8429 = NULL;

	return ERRCODE_NO_ERROR;
}

