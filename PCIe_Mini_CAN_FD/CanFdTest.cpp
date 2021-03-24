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
/** @file CanFdTest.cpp
* @brief Test methods for the PCIe-Mini-CAN-FD
*/
#include "AlphiDll.h"
#include "AlphiBoard.h"
#include "SccChannel.h"
#include "AlphiErrorCodes.h"
#include "CanFdTest.h"

//#include <msp430.h>

#include "TCAN4550.h"
#include "immintrin.h"

void printIrqStatus(TCAN4x5x_Device_Interrupts* dev_ir);
void PORT1_ISR(void* userData);

CanFdTest* CanFdTest::testInstance = nullptr;

int CanFdTest::testDpr()
{
	int errNbr = 0;
	int passedLocations = 0;
	unsigned int i;

	printf("Test board local memory: ");
	for (i = 0; i < dut->dpr_length / 4; i++) {
		dut->dpr[i] = i * 0x20001;
	}
	for (i = 0; i < dut->dpr_length / 4; i++) {
		if (dut->dpr[i] != i * 0x20001) {
			errNbr++;
			if (errNbr < 5) printf("error @ offset %d: wrote 0x%08x, read 0x%08x\n", i * 4, i * 0x20001, dut->dpr[i]);
		}
		else passedLocations++;
	}
	if (errNbr == 0) printf("Passed\n");
	else printf("Failed %d out of %d locations (%d succeeded)\n", errNbr, dut->dpr_length / 4, passedLocations);
	return errNbr;
}

void printMcanIr(int channel, TCAN4x5x_MCAN_Interrupts dev_ir)
{
	printf("M_CAN irq:\n");
	//! @brief IR[0] RF0N: Rx FIFO 0 new message
	if (dev_ir.RF0N == 1) printf("RF0N: Rx FIFO 0 new message\n");

	//! @brief IR[1] RF0W: Rx FIFO 0 watermark reached
	if (dev_ir.RF0W == 1) printf("RF0N: Rx FIFO 0 watermark reached\n");

	//! @brief IR[2] RF0F: Rx FIFO 0 full
	if (dev_ir.RF0F == 1) printf("RF0F: Rx FIFO 0 full\n");

	//! @brief IR[3] RF0L: Rx FIFO 0 message lost
	if (dev_ir.RF0L == 1) printf("RF0L: Rx FIFO 0 message lost\n");

	//! @brief IR[4] RF1N: Rx FIFO 1 new message
	if (dev_ir.RF1N == 1) printf("RF1N: Rx FIFO 1 new message\n");

	//! @brief IR[5]  RF1W: RX FIFO 1 watermark reached
	if (dev_ir.RF1W == 1) printf("RF1W: RX FIFO 1 watermark reached\n");

	//! @brief IR[6] RF1F: Rx FIFO 1 full
	if (dev_ir.RF1F == 1) printf("RF1F: Rx FIFO 1 full\n");

	//! @brief IR[7] RF1L: Rx FIFO 1 message lost
	if (dev_ir.RF1L == 1) printf("RF1L: Rx FIFO 1 message lost\n");

	//! @brief IR[8] HPM: High priority message
	if (dev_ir.HPM == 1) printf("HPM: High priority message\n");

	//! @brief IR[9] TC: Transmission completed
	if (dev_ir.TC == 1) printf("TC: Transmission completed\n");

	//! @brief IR[10] TCF: Transmission cancellation finished
	if (dev_ir.TCF == 1) printf("TCF: Transmission cancellation finished\n");

	//! @brief IR[11] TFE: Tx FIFO Empty
	if (dev_ir.TFE == 1) printf("TFE: Tx FIFO Empty\n");

	//! @brief IR[12] TEFN: Tx Event FIFO new entry
	if (dev_ir.TEFN == 1) printf("TEFN: Tx Event FIFO new entry\n");

	//! @brief IR[13] TEFW: Tx Event FIFO water mark reached
	if (dev_ir.TEFW == 1) printf("TEFW: Tx Event FIFO 0 watermark reached\n");

	//! @brief IR[14] TEFF: Tx Event FIFO full
	if (dev_ir.TEFF == 1) printf("TEFF: Tx Event FIFO full\n");

	//! @brief IR[15] TEFL: Tx Event FIFO element lost
	if (dev_ir.TEFL == 1) printf("TEFL: Tx Event FIFO element lost\n");

	//! @brief IR[16] TSW: Timestamp wrapped around
	if (dev_ir.TSW == 1) printf("TSW: Timestamp wrapped around\n");

	//! @brief IR[17] MRAF: Message RAM access failure
	if (dev_ir.MRAF == 1) printf("MRAF: Message RAM access failure\n");

	//! @brief IR[18] TOO: Time out occurred
	if (dev_ir.TOO == 1) printf("TOO: Time out occurred\n");

	//! @brief IR[19] DRX: Message stored to dedicated RX buffer
	if (dev_ir.DRX == 1) printf("DRX: Message stored to dedicated RX buffer\n");

	//! @brief IR[20] BEC: MRAM Bit error corrected
	if (dev_ir.BEC == 1) printf("BEC: MRAM Bit error corrected\n");

	//! @brief IR[21] BEU: MRAM Bit error uncorrected
	if (dev_ir.BEU == 1) printf("BEU: MRAM Bit error uncorrected\n");

	//! @brief IR[22] ELO: Error logging overflow
	if (dev_ir.ELO == 1) printf("ELO: Error logging overflow\n");

	//! @brief IR[23] EP: Error_passive status changed
	if (dev_ir.EP == 1) printf("EP: Error_passive status changed\n");

	//! @brief IR[24] EW: Error_warning status changed
	if (dev_ir.EW == 1) printf("EW: Error_warning status changed\n");

	//! @brief IR[25] BO: Bus_off status changed
	if (dev_ir.BO == 1) printf("BO: Bus_off status changed\n");

	//! @brief IR[26] WDI: MRAM Watchdog Interrupt
	if (dev_ir.WDI == 1) printf("WDI: MRAM Watchdog Interrupt\n");

	//! @brief IR[27] PEA Protocol Error in arbitration phase (nominal bit time used)
	if (dev_ir.PEA == 1) printf("PEA Protocol Error in arbitration phase (nominal bit time used)\n");

	//! @brief IR[28] PED: Protocol error in data phase (data bit time is used)
	if (dev_ir.PED == 1) printf("PED: Protocol error in data phase (data bit time is used)\n");

	//! @brief IR[29] ARA: Access to reserved address
	if (dev_ir.ARA == 1) printf("ARA: Access to reserved address\n");

}

void printDevIr(int channel, TCAN4x5x_Device_Interrupts dev_ir)
{
		printf("Irq reg channel #%d: ", channel);
	//! @brief DEV_IR[0] VTWD: Global Voltage, Temp, or Watchdog (if equipped) Interrupt
	if (dev_ir.VTWD) printf("Voltage or watchdog, ");

	//! @brief DEV_IR[1] M_CAN_INT: There are MCAN interrupts pending
	if (dev_ir.M_CAN_INT) printf("M_CAN_INT, ");

	//! @brief DEV_IR[2] : Selective Wake Error (If equipped)
	if (dev_ir.SWERR) printf("SWERR, ");

	//! @brief DEV_IR[3] : SPI Error
	if (dev_ir.SPIERR) printf("SPIERR, ");

	//! @brief DEV_IR[4] : CBF, CAN Bus Fault
	if (dev_ir.CBF) printf("CAN Bus Fault, ");

	//! @brief DEV_IR[5] : CANERR, CAN Error
	if (dev_ir.CANERR) printf("CAN Error, ");

	//! @brief DEV_IR[6] : WKRQ, Wake Request
	if (dev_ir.WKRQ) printf("Wake request, ");

	//! @brief DEV_IR[7] : GLOBALERR, Global Error. Is the OR output of all interrupts
	if (dev_ir.GLOBALERR) printf("GLOBALERR, ");

	//! @brief DEV_IR[8] : CANDOM, Can bus stuck dominant
	if (dev_ir.CANDOM) printf("CANDOM, ");

	//! @brief DEV_IR[10] : CANSLNT, CAN Silent
	if (dev_ir.CANTO) printf("CAN Silent, ");

	//! @brief DEV_IR[12] : FRAME_OVF, Frame Error Overflow (If Selective Wake is equipped)
	if (dev_ir.FRAME_OVF) printf("FRAME_OVF, ");

	//! @brief DEV_IR[13] : WKERR, Wake Error
	if (dev_ir.WKERR) printf("WKERR, ");

	//! @brief DEV_IR[14] : LWU, Local Wake Up
	if (dev_ir.LWU) printf("LWU, ");

	//! @brief DEV_IR[15] : CANINT, CAN Bus Wake Up Interrupt
	if (dev_ir.CANINT) printf("CANINT, ");

	//! @brief DEV_IR[16] : ECCERR, MRAM ECC Error
	if (dev_ir.ECCERR) printf("ECCERR, ");

	//! @brief DEV_IR[18] : WDTO, Watchdog Time Out
	if (dev_ir.WDTO) printf("Watchdog Timeout, ");

	//! @brief DEV_IR[19] : TSD, Thermal Shut Down
	if (dev_ir.TSD) printf("Thermal shutdown, ");

	//! @brief DEV_IR[20] : PWRON, Power On Interrupt
	if (dev_ir.PWRON) printf("Power on, ");

	//! @brief DEV_IR[21] : UVIO, Undervoltage on UVIO
	if (dev_ir.UVIO) printf("UVIO, ");

	//! @brief DEV_IR[22] : UVSUP, Undervoltage on VSUP and VCCOUT
	if (dev_ir.UVSUP) printf("UVSUP, ");

	//! @brief DEV_IR[23] : SMS, Sleep Mode Status Flag. Set when sleep mode is entered due to WKERR, UVIO, or TSD faults
	if (dev_ir.SMS) printf("SMS, ");

	//! @brief DEV_IR[24] : CANBUSBAT, CAN Shorted to VBAT
	if (dev_ir.CANBUSBAT) printf("CANBUSBAT, ");

	//! @brief DEV_IR[25] : CANBUSGND, CAN Shorted to GND
	if (dev_ir.CANBUSGND) printf("CANBUSGND, ");

	//! @brief DEV_IR[26] : CANBUSOPEN, CAN Open fault
	if (dev_ir.CANBUSOPEN) printf("CANBUSOPEN, ");

	//! @brief DEV_IR[27] : CANLGND, CANL GND
	if (dev_ir.CANLGND) printf("CANLGND, ");

	//! @brief DEV_IR[28] : CANHBAT, CANH to VBAT
	if (dev_ir.CANHBAT) printf("CANHBAT, ");

	//! @brief DEV_IR[29] : CANHCANL, CANH and CANL shorted
	if (dev_ir.CANHCANL) printf("CANHCANL, ");

	//! @brief DEV_IR[30] : CANBUSTERMOPEN, CAN Bus has termination point open
	if (dev_ir.CANBUSTERMOPEN) printf("CANBUSTERMOPEN, ");

	//! @brief DEV_IR[31] : CANBUSNOM, CAN Bus is normal flag
	if (dev_ir.CANBUSNORM) printf("CANBUSNORM, ");
	printf ("\n");
}

int CanFdTest::checkSpiErrorBit(TCAN4550* can, int verbose)
{
	TCAN4x5x_Device_Interrupts dev_ir = { 0 };					// Setup a new MCAN IR object for easy interrupt checking
	can->MCAN_ClearInterruptsAll();

	can->Device_ReadInterrupts(&dev_ir);						// Request that the struct be updated with current DEVICE (not MCAN) interrupt values
	if (dev_ir.word == 0xffffffff) {
		printf("The SPI interface of the channel #%d is hosed. Trying to reset it...\n", can->slaveNbr);
		can->printStatusRegister();
		dut->can[can->slaveNbr]->reset();
		return 1;
	}
	TCAN4x5x_MCAN_Interrupts mcan_ir = { 0 };
	can->MCAN_ReadInterrupts(&mcan_ir);

	if (verbose && dev_ir.word != 0) printDevIr(can->slaveNbr, dev_ir);
	if (verbose && mcan_ir.word != 0) printMcanIr(can->slaveNbr, mcan_ir);
//	if (verbose) can->printStatusRegister();

	if (dev_ir.SPIERR || dev_ir.CANTO) {
//		TCAN4x5x_Device_Interrupts dev_ir = { 0 };					// Setup a new MCAN IR object to reset the SPI irq
//		dev_ir.SPIERR = 1;
		can->Device_ClearInterruptsAll();
		//can->Device_ClearInterrupts(&dev_ir);
		//can->Device_ReadInterrupts(&dev_ir);
		return 1;
	}
	return 0;
}

int CanFdTest::testSpiRead32(uint8_t spiController)
{
	double StartingTime, EndingTime, ElapsedMicroseconds;
	double Frequency;
	TCAN4550* canSpi = dut->can[spiController];
	TcanInterface* can = canSpi->can;
	int errNbr = 0;

	printf("SPI Read access channel #%d: ", spiController);
	if (spiController >= dut->nbrOfCanInterfaces) {
		printf("Failed: invalid channel number!\n");
		return 1;
	}

	uint32_t id[3] = { 0 };
	int nbrOfLoops = 1000;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartingTime);
	for (int i = 0; i < nbrOfLoops; i++) {
		canSpi->Device_ReadDeviceIdent(id);
		if (strcmp((char*)id, "TCAN4550") != 0) {
			if (errNbr < 5) printf("Failed: DeviceIdent read 0x%08x%08x\n", id[0], id[1]);
			errNbr++;
			if (i == 10 && errNbr >= 9) {
				printf("too many failures, aborting!\n");
			}
			can->reset();
		}
	}
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds = EndingTime - StartingTime;
	ElapsedMicroseconds *= 1000000;
	ElapsedMicroseconds /= Frequency;

	printf("read 32-bit: %d accesses in %5.2f milliseconds (%4.2f us/access)\n", nbrOfLoops, ElapsedMicroseconds / 1000, ElapsedMicroseconds / nbrOfLoops);

	if (checkSpiErrorBit(canSpi)) {
		printf("Error: The SPI error flag was set channel #%d.\n", spiController);
		// errNbr++;
	}
	printf("%d errors out of %d loops.\n", errNbr, nbrOfLoops);
	return errNbr;
}

#ifdef DMA_ENABLED
int CanFdTest::testSpiReadMultDMA(uint8_t spiController, int len)
{
	TCAN4550* canSpi = dut->can[spiController];
	TcanInterface* can = canSpi->can;

	// fixed parameters
	uint16_t address = 0x8000;
	len = 8;

	int errNbr = 0;

	// initialize the memory
	can->AHB_WRITE_BURST_START(address, len);
	for (int i = 0; i < len; i++) {
		can->AHB_WRITE_BURST_WRITE(i+10);
	}
	can->AHB_WRITE_BURST_END();


	// Do the DMA
//	Sleep(1);
#define DMA_SPI_READ
#ifdef DMA_SPI_READ
	for (int i = 0; i < len; i++) {
		dut->dpr[i] = 0xcccccccc;
	}
	TransferDesc* tfrDesc = new TransferDesc();
	tfrDesc->src_offset = 0x0240;
	tfrDesc->dest_offset = dut->dpr_offset;
	tfrDesc->tfr_length = (len + 1)* sizeof(uint32_t);
	dut->dma->setControlBit(ALTERA_AVALON_DMA_CONTROL_RCON_MSK);
	dut->dma->launch_bidir(tfrDesc);
//	can->AHB_READ_BURST_START(0x8000, len);
	uint32_t msg = AHB_READ_OPCODE << 24;
	msg |= address << 8;        // Send the 16-bit address
	msg |= len;               // Send the number of words to read

	can->setTxData(msg);

	for (int i = 0; i < len; i++) {
		can->setTxData(0);
	}
	//	Sleep(1);

	printf("Read local RAM back:");
	for (int i = 1; i < len + 1; i++) {
		printf("<0x%x>", dut->dpr[i]);
	}
	dut->dma->print();
#else
	can->AHB_READ_BURST_START(0x8000, len);
	for (int i = 0; i < len; i++) {
		id[i] = can->AHB_READ_BURST_READ();
	}
	printf("Read back:");
	for (int i = 0; i < len; i++) {
		printf("<0x%x>", id[i]);
	}
#endif
	can->AHB_READ_BURST_END();
	printf("\n");
	return errNbr;
}
#endif

int CanFdTest::testSpiReadMult(uint8_t spiController, int len)
{
	double StartingTime, EndingTime, ElapsedMicroseconds;
	double Frequency;
	TCAN4550* canSpi = dut->can[spiController];
	TcanInterface* can = canSpi->can;
	int errNbr = 0;

	printf("SPI Read %d wd channel #%d: ", len, spiController);
	if (spiController >= dut->nbrOfCanInterfaces) {
		printf("Failed: invalid channel number!\n");
		return 1;
	}
	if (len > 20) {
		printf("Failed: Length is maximum 20!\n");
		return 1;
	}

	uint32_t id[20] = { 0 };
	int nbrOfLoops = 1000;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartingTime);
	for (int j = 0; j < nbrOfLoops; j++) {
		canSpi->can->AHB_READ_BURST_START(0x8000, len);  
		for (int i = 0; i < len; i++) {
			id[i] = canSpi->can->AHB_READ_BURST_READ();
		}
		canSpi->can->AHB_READ_BURST_END();
		if (strcmp((char*)id, "TCAN4550") != 0) {
			if (errNbr < 5) printf("Failed: DeviceIdent read 0x%08x%08x\n", id[0], id[1]);
			errNbr++;
			if (errNbr >= 9) {
				printf("too many failures, aborting!\n");
			}
			can->reset();
		}
	}
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds = EndingTime - StartingTime;
	ElapsedMicroseconds *= 1000000;
	ElapsedMicroseconds /= Frequency;

	double delay = (double)ElapsedMicroseconds;

	printf("read mult %d words: %d accesses in %5.2f milliseconds (%4.2f us/access)\n", len, nbrOfLoops, delay / 1000, delay / nbrOfLoops);

	if (checkSpiErrorBit(canSpi)) {
		printf("Error: The SPI error flag was set channel #%d.\n", spiController);
		// errNbr++;
	}
	printf("%d errors out of %d loops.\n", errNbr, nbrOfLoops);
	return errNbr;
}


int CanFdTest::testSpiWrite(uint8_t spiController)
{
	double StartingTime, EndingTime, ElapsedMicroseconds;
	double Frequency;
	TCAN4550* can = dut->can[spiController];
	int errNbr = 0;
	printf("SPI write access channel #%d: ", spiController);
	if (spiController >= dut->nbrOfCanInterfaces) {
		printf("Failed: invalid channel number!\n");
		return 1;
	}

	int nbrOfLoops = 1000;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartingTime);
	for (int i = 0; i < nbrOfLoops; i++) {
		can->can->AHB_WRITE_32(REG_DEV_TEST_REGISTERS, 0);
	}
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds = EndingTime - StartingTime;
	ElapsedMicroseconds *= 1000000;
	ElapsedMicroseconds /= Frequency;

	double delay = (double)ElapsedMicroseconds;

	printf("write 32-bit: %d accesses in %5.2f milliseconds (%4.2f us/access)\n", nbrOfLoops, delay / 1000, delay / nbrOfLoops);

	if (checkSpiErrorBit(can, errNbr < 5)) {
		printf("Error: The SPI error flag was set channel #%d.\n", spiController);
		// errNbr++;
	}
	printf("%d errors out of %d loops.\n", errNbr, nbrOfLoops);
	return errNbr;
}

int CanFdTest::testSpiReadWrite(int nbrOfLoops)
{
	int spiController;
	const int nbrOfCanInterfaces = 4;// PCIeMini_CAN_FD::nbrOfCanInterfaces;
	TcanInterface* can[nbrOfCanInterfaces];
	for (spiController = 0; spiController < nbrOfCanInterfaces; spiController++) {
		can[spiController] = dut->can[spiController]->can;
		TCAN4x5x_Device_Interrupts dev_ir = { 0 };
		dev_ir.M_CAN_INT = 1;
		dut->can[spiController]->Device_ClearInterrupts(&dev_ir);
	}

	int errNbr = 0;
	int errPerChannel[nbrOfCanInterfaces] = { 0 };
	int spiReadErrs[nbrOfCanInterfaces] = { 0 };
	int spiWriteErrs[nbrOfCanInterfaces] = { 0 };
	uint32_t val = 0;
	printf("SPI Read/write access all channels\n");
	for (int i = 0; i < nbrOfLoops; i++) {
		uint32_t testVal = 0x12345678 + i;
		for (spiController = 0; spiController < nbrOfCanInterfaces; spiController++) {
			can[spiController]->AHB_WRITE_32(REG_DEV_TEST_REGISTERS, testVal + spiController);

			spiWriteErrs[spiController] += checkSpiErrorBit(dut->can[spiController], errNbr<5); // print only the 5 first errors
		}

		for (spiController = 0; spiController < nbrOfCanInterfaces; spiController++) {
			val = can[spiController]->AHB_READ_32(REG_DEV_TEST_REGISTERS);
			spiReadErrs[spiController] += checkSpiErrorBit(dut->can[spiController], errNbr<5); // print only the 5 first errors

			if (val != testVal + spiController) {
				if (errNbr < 10) printf("Failed channel#%d: write 0x%08x read 0x%08x\n", spiController, testVal + spiController, val);
				errNbr++;
				errPerChannel[spiController] += 1;
			}
		}
	}

	if (errNbr == 0){
		printf("SUCCESS! \n");
	}
	else {
		printf("%d errors out of %d loops.\n", errNbr, nbrOfLoops * dut->nbrOfCanInterfaces);
		for (int i = 0; i < nbrOfCanInterfaces; i++) {
			printf("Channel #%d: %d errors out of %d loops\n", i, errPerChannel[i], nbrOfLoops);
		}
	}
	for (int i = 0; i < nbrOfCanInterfaces; i++) {
		if (spiReadErrs[i] > 0) {
			printf("Channel #%d: SPI errors r:%d w:%d out of %d loops\n",
				i, spiReadErrs[i], spiWriteErrs[i], nbrOfLoops);
			errNbr++;
		}
	}

	return errNbr;
}

int CanFdTest::testPCIeSpeed(void)
{
	double StartingTime, EndingTime, ElapsedMicroseconds;
	double Frequency;
	int nbrOfAccesses = 1000000;
	volatile uint32_t* mem = dut->dpr;
	volatile uint64_t* mem64 = (uint64_t * )dut->dpr;

	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartingTime);

	for (int i = 0; i < nbrOfAccesses; i++)
		mem[0] = i;

	// Activity to be timed

	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds = EndingTime - StartingTime;

	//
	// We now have the elapsed number of ticks, along with the
	// number of ticks-per-second. We use these values
	// to convert to the number of elapsed microseconds.
	// To guard against loss-of-precision, we convert
	// to microseconds *before* dividing by ticks-per-second.
	//

	ElapsedMicroseconds *= 1000000;
	ElapsedMicroseconds /= Frequency;

	int delay = (int)ElapsedMicroseconds;
	double bandwidth = delay;
	bandwidth = ((double)nbrOfAccesses * 4) / bandwidth;

	printf("write: %d accesses in %d microseconds (%f MB/s)\n", nbrOfAccesses, delay, bandwidth);

	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartingTime);

	for (int i = 0; i < nbrOfAccesses; i++)
		mem[0] += 1;

	// Activity to be timed

	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds = EndingTime - StartingTime;

	//
	// We now have the elapsed number of ticks, along with the
	// number of ticks-per-second. We use these values
	// to convert to the number of elapsed microseconds.
	// To guard against loss-of-precision, we convert
	// to microseconds *before* dividing by ticks-per-second.
	//

	ElapsedMicroseconds *= 1000000;
	ElapsedMicroseconds /= Frequency;

	delay = (int)ElapsedMicroseconds;
	bandwidth = delay;
	bandwidth = ((double)nbrOfAccesses * 4) / bandwidth;

	printf("R/W: %d accesses in %d microseconds (%f MB/s)\n", nbrOfAccesses, delay, bandwidth);

	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartingTime);

	int j = 0;
	uint64_t j64 = 0;

	for (int i = 0; i < nbrOfAccesses; i++)
		j64 += *mem64;

	// Activity to be timed

	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds = EndingTime - StartingTime;

	//
	// We now have the elapsed number of ticks, along with the
	// number of ticks-per-second. We use these values
	// to convert to the number of elapsed microseconds.
	// To guard against loss-of-precision, we convert
	// to microseconds *before* dividing by ticks-per-second.
	//

	ElapsedMicroseconds *= 1000000;
	ElapsedMicroseconds /= Frequency;

	delay = (int)ElapsedMicroseconds;
	bandwidth = delay;
	bandwidth = ((double)nbrOfAccesses * 8) / bandwidth;

	printf("Read: %d accesses in %d microseconds (%f MB/s)\n", nbrOfAccesses, delay, bandwidth);

	TCAN4550* can = dut->can[0];
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartingTime);

	j = 0;

	for (int i = 0; i < nbrOfAccesses; i++)
		j += can->can->getStatus();

	// Activity to be timed

	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds = EndingTime - StartingTime;

	//
	// We now have the elapsed number of ticks, along with the
	// number of ticks-per-second. We use these values
	// to convert to the number of elapsed microseconds.
	// To guard against loss-of-precision, we convert
	// to microseconds *before* dividing by ticks-per-second.
	//

	ElapsedMicroseconds *= 1000000;
	ElapsedMicroseconds /= Frequency;

	delay = (int)ElapsedMicroseconds;
	bandwidth = delay;
	bandwidth = ((double)nbrOfAccesses * 4) / bandwidth;

	printf("Read: %d accesses in %d microseconds (%f MB/s)\n", nbrOfAccesses, delay, bandwidth);

	return 0;
}

int CanFdTest::checkIrq(int chNumber, int verbose)
{
	TCAN4550* can = dut->can[chNumber];	
	TCAN4x5x_Device_Interrupts dev_ir = { 0 };					// Setup a new MCAN IR object for easy interrupt checking
	uint32_t irqStatus = dut->cra->getIrqStatus();
	uint32_t irqCraMask = dut->cra->getIrqEnableMask();

	uint32_t spiStatus = dut->input0->getData();
	uint32_t spiIrqStatus = dut->input0->getIrqStatus();
	if (verbose) {
		printf("checkIrq channel #%d:\n", chNumber);
		printf("    PCIe IRQ status: 0x%04x\n", irqStatus);
		printf("    PCIe IRQ mask: 0x%04x\n", irqCraMask);
		printf("    PIO status: 0x%04x\n", spiStatus);
		printf("    PIO IRQ status: 0x%04x\n", spiIrqStatus);
		printf("    PIO IRQ enable: 0x%04x\n", dut->input0->getIrqEnable());
		printf("    PIO IRQ edge: 0x%04x\n", dut->input0->base[dut->input0->edgeReg_Index]);
		printf("    PIO IRQ polarity: 0x%04x\n", dut->input0->base[dut->input0->polarity_index]);
	}

	can->Device_ReadInterrupts(&dev_ir);						// Request that the struct be updated with current DEVICE (not MCAN) interrupt values
//	printf("    CAN interrupts: ");
//	printIrqStatus(&dev_ir);

	can->Device_ReadInterrupts(&dev_ir);						// Request that the struct be updated with current DEVICE (not MCAN) interrupt values
	if (dev_ir.SPIERR) {
		printf("SPI error!\n");
		TCAN4x5x_Device_Interrupts dev_ir = { 0 };					// Setup a new MCAN IR object to reset the SPI irq
		dev_ir.SPIERR = 1;
		can->Device_ClearInterrupts(&dev_ir);
		can->Device_ReadInterrupts(&dev_ir);
	}
	return 0;
}

void CanFdTest::printRxMsg(TCAN4x5x_MCAN_RX_Header* MsgHeader, int numBytes, uint8_t* dataPayload)
{
	int lenToDisplay, j = 0;

	printf("%05d\t", (MsgHeader->RXTS));
	printf("0x%08x\t", MsgHeader->ID);
	if (MsgHeader->RTR)
		printf("RTR\t");
	else
		printf("---\t");

	lenToDisplay = MsgHeader->DLCode;

	printf("%d\t", lenToDisplay);
	if (numBytes > 10) {
		printf("msg length=%d, displaying first 10 characters\n", lenToDisplay);
		lenToDisplay = 10;
	}
	if (!MsgHeader->RTR)
		for (j = 0; j < lenToDisplay; j++)
			printf("%02x ", dataPayload[j]);
	else
		for (j = 0; j < lenToDisplay; j++)
			printf("-- ");
	printf("\n");
}

void CanFdTest::printTxMsg(TCAN4x5x_MCAN_TX_Header* MsgHeader, uint8_t* dataPayload)
{
	int lenToDisplay, j = 0;

	printf("0x%08x\t", MsgHeader->ID);
	if (MsgHeader->RTR)
		printf("RTR\t");
	else
		printf("---\t");

	lenToDisplay = MsgHeader->DLCode;
	printf("%d\t", lenToDisplay);
	if (MsgHeader->DLCode > 10) {
		printf("msg length=%d, displaying first 10 characters\n", lenToDisplay);
		lenToDisplay = 10;
	}
	if (!MsgHeader->RTR)
		for (j = 0; j < lenToDisplay; j++)
			printf("%02x ", dataPayload[j]);
	else
		for (j = 0; j < lenToDisplay; j++)
			printf("-- ");
	printf("\n");
}

int CanFdTest::checkMsg(int nbrOfLoops, int chnNumber, TCAN4x5x_MCAN_RX_Header *MsgHeader, int numBytes, uint8_t *dataPayload)
{
	if (numBytes != 4) {
		printf("Incorrect length! Loop Nbr#%d, Channel#%d:", nbrOfLoops, chnNumber);
		printRxMsg(MsgHeader, numBytes, dataPayload);
		return 1;
	}
	int origin = MsgHeader->ID % 10;
	int msgTyp = MsgHeader->ID / 10;
	if (msgTyp == 26) {
		for (int i = 0; i < numBytes; i++) {
			if (dataPayload[i] != (i + 1) * 0x10 + origin)
			{
				printf("Invalid data! Loop Nbr#%d, Channel#%d:", nbrOfLoops, chnNumber);
				printRxMsg(MsgHeader, numBytes, dataPayload);
				return 1;
			}
		}
	}
	else if (msgTyp == 28){
		for (int i = 0; i < numBytes; i++) {
			uint8_t val = (0x50 + origin) + (i * 0x10);
			if (dataPayload[i] != val)
			{
				printf("Invalid data! Loop Nbr#%d, Channel#%d:", nbrOfLoops, chnNumber);
				printRxMsg(MsgHeader, numBytes, dataPayload);
				return 1;
			}
		}
	}
	else
	{
		printf("Invalid ID! Loop Nbr#%d, Channel#%d:", nbrOfLoops, chnNumber);
		printRxMsg(MsgHeader, numBytes, dataPayload);
		return 1;
	}
	return 0;
}

/** brief Check the FIFO 0 status register for message presence
 *
 * @param can SPI interface to the CAN controller
 * @retval true if FIFO is empty, false if there are messages to be read.
 */
bool CanFdTest::isRxFifo0Empty(TcanInterface* can)
{
	/* Status register bit map
	* ________________________________________________________________
	*   bit		name			reset
	*							value
	* ________________________________________________________________
		31:26	RSVD		R	0x0		Reserved
		25		RF0L		R	0		Rx FIFO 0 Message Lost:
										This bit is a copy of interrupt flag IR.RF0L. When IR.RF0L is reset, this bit is also reset.
										0 - No Rx FIFO 0 message lost
										1 - Rx FIFO 0 message lost; also set after write attempt to Rx FIFO 0 of size zero
										Note: Overwriting the oldest message when RXF0C.F0OM = �1� will not set this flag
		24		F0F			R	0		Rx FIFO 0 Full
										0 - Rx FIFO 0 not full
										1 - Rx FIFO 0 full
		23:22	RSVD		R	0x0		Reserved
		21:16	F0PI[5:0]	R	0x0		Rx FIFO 0 Put Index: Rx FIFO 0 write index pointer, range 0 to 63
		15:14	RSVD		R	0x0		Reserved
		13:8	F0GI[5:0]	R	0x0		Rx FIFO 0 Get Index: Rx FIFO 0 read index pointer, range 0 to 63
		7		RSVD		R	0		Reserved
		6:0		F0FL[6:0]	R	0x0		Rx FIFO 0 Fill Level: Number of elements stored in Rx FIFO 0, range 0 to 64.

	*/
	uint32_t readData = can->AHB_READ_32(REG_MCAN_RXF0S);
	if (readData == 0xffffffff) {
		return true;			// the interface is hosed, nothing to read
	}
	if (readData & (1 << 25)) 
		printf("message lost!\n");
	return (readData & 0x7f) == 0;
}

/** brief Check the FIFO 0 status register for message presence
 *
 * @param can SPI interface to the CAN controller
 * @retval true if FIFO is empty, false if there are messages to be read.
 */
static bool isTxFifoFull(TcanInterface* can)
{
	/* Status register bit map
	* ________________________________________________________________
	*   bit		name		R/W	reset	Description
	*							value
	* ________________________________________________________________
		31:24	RSVD		R	0x0		Reserved
		23:22	RSVD		R	0x0		Reserved
		21		TFQF		R	0		Tx FIFO/Queue Full
										0 - Tx FIFO/Queue not full
										1 - Tx FIFO/Queue full
		20:16	TFQPI[4:0]	R	0x0		Tx FIFO/Queue Put Index
										Tx FIFO/Queue write index pointer, range 0 to 31.
		15:13	RSVD		R	0x0		Reserved
		12:8	TFGI[4:0]	R	0x0		Tx FIFO Get Index
										Tx FIFO read index pointer, range 0 to 31. Read as zero when
										Tx Queue operation is configured (TXBC.TFQM = �1�).
		7:6		RSVD		R	0x0		Reserved
		5:0		TFFL[5:0]	R	0x0		Tx FIFO Free Level
										Number of consecutive free Tx FIFO elements starting from
										TFGI, range 0 to 32. Read as zero when Tx Queue operation is
										configured (TXBC.TFQM = �1�)
										Note: In case of mixed configurations where dedicated Tx
										Buffers are combined with a Tx FIFO or a Tx Queue, the Put
										and Get Indices indicate the number of the Tx Buffer starting
										with the first dedicated Tx Buffers
										Example: For a configuration of 12 dedicated Tx Buffers and a
										Tx FIFO of 20 Buffers a Put Index of 15 points to the fourth
										buffer of the Tx FIFO

	*/
	uint32_t readData = can->AHB_READ_32(REG_MCAN_TXBRP);
	return (readData & 3) != 0;
}

/** @brief loopback test
*
* @param nbrOfPackets Number of loops where packet are sent
* @retval Nbr of errors
*/
int CanFdTest::testTiLib(int nbrOfTestLoops)
{
	int nbrErrors = 0;

	/***********************************
	 * MSP430 Specific Initializations *
	 ***********************************/
	dut->reset();
	Sleep(100);

	/*********************************************
	 * Initialize the 4 interfaces               *
	 *********************************************/
	for (int i = 0; i < dut->nbrOfCanInterfaces; i++) {
		if (verbose) printf("-------------------------------------\n");
		if (verbose) printf("Initializing CAN controller #%d\n", i);
		nbrErrors += Init_CAN(dut->can[i], verbose);
		dut->input0->resetIrq();

	}
	checkIrq(0);

	for (int i = 0; i < dut->nbrOfCanInterfaces; i++) {
		TCAN4550* can = dut->can[i];
		can->enableIrq();
		TCAN4x5x_MCAN_TX_Header header = { 0 };			// Remember to initialize to 0, or you'll get random garbage!

		uint8_t data[4];								// Define the data payload
		data[0] = 0x50 + i;
		data[1] = 0x60 + i;
		data[2] = 0x70 + i;
		data[3] = 0x80 + i;									// Define the data payload
		header.DLCode = MCAN_DLC_4B;					// Set the DLC to be equal to or less than the data payload (it is ok to pass a 64 byte data array into the WriteTXFIFO function if your DLC is 8 bytes, only the first 8 bytes will be read)
		header.ID = 280 + i;								// Set the ID
		if (isCanFd) {
			header.FDF = 1;									// CAN FD frame enabled
			header.BRS = 1;									// Bit rate switch enabled
		}
		else {
			header.FDF = 0;									// CAN FD frame disabled
			header.BRS = 0;									// Bit rate switch disabled
		}
		header.EFC = 0;
		header.MM = 0;
		header.RTR = 0;
		header.XTD = 0;									// We are not using an extended ID in this example
		header.ESI = 0;									// Error state indicator


		can->MCAN_WriteTXBuffer(0, &header, data);	// This function actually writes the header and data payload to the specified TX Fifo number. It returns the bit necessary to write to TXBAR,
														// but does not necessarily require you to use it. In this example, we won't, so that we can send the data queued up at a later point.
		// Let's make another packet
		data[0] = 0x10 + i;
		data[1] = 0x20 + i;
		data[2] = 0x30 + i;
		data[3] = 0x40 + i;									// Define the data payload

		header.DLCode = MCAN_DLC_4B;						// Set the DLC to be equal to or less than the data payload (it is ok to pass a 64 byte data array into the WriteTXFIFO function if your DLC is 8 bytes, only the first 8 bytes will be read)
		header.ID = 260 + i;								// Set the ID
		if (isCanFd) {
			header.FDF = 1;									// CAN FD frame enabled
			header.BRS = 1;									// Bit rate switch enabled
		}
		else {
			header.FDF = 0;									// CAN FD frame disabled
			header.BRS = 0;									// Bit rate switch disabled
		}
		header.EFC = 0;
		header.MM = 0;
		header.RTR = 0;
		header.XTD = 0;									// We are not using an extended ID in this example
		header.ESI = 0;									// Error state indicator

		can->MCAN_WriteTXBuffer(1, &header, data);	// This line writes the data and header to TX FIFO 1
	}

	checkIrq(0);

	int nbrOfLoops;
	int msgTxNbr[4] = { 0 };
	int msgRxNbr[4] = { 0 };
	int packetsPerChannel = nbrOfTestLoops / 2;			// 2 packets per channel per loop

	int rxTarget = (nbrOfTestLoops / 2) * 3;
	QueryPerformanceCounter(&StartingTime);
//	can->MCAN_ClearInterruptsAll();	// Clear any of the interrupt bits that are set.
	double delay = 0;	
	bool txFifoIsFull = false;
	for (nbrOfLoops = 0; nbrOfLoops < nbrOfTestLoops * 2 + 100; nbrOfLoops++)
	{
//		checkIrq(0, 0);
		uint8_t txChannel = nbrOfLoops % 4;
//		int buffNbr = (nbrOfLoops / 4) % 1;

		if (msgTxNbr[txChannel] < packetsPerChannel) {
			if (!isTxFifoFull(dut->can[txChannel]->can)) {
				txFifoIsFull = false;
				//dut->can[txChannel]->MCAN_TransmitBufferContents(3);
				dut->can[txChannel]->can->AHB_WRITE_32(REG_MCAN_TXBAR, 3);
				dut->can[txChannel]->can->AHB_READ_32(REG_MCAN_TXBRP);
				msgTxNbr[txChannel]+=2;
			}
			else {
				if (!txFifoIsFull) {
					if (nbrErrors<5)
						printf("TxFifo full!\n");
					txFifoIsFull = true;
					nbrErrors++;
				}
			}
		}

		for (int chnNbr = 0; chnNbr < dut->nbrOfCanInterfaces; chnNbr++) {
			TCAN4550* can = dut->can[chnNbr];
			uint8_t numBytes = 0;

			while (!isRxFifo0Empty(can->can) && nbrErrors <= 5) {
				uint8_t dataPayload[64] = { 0 };
				TCAN4x5x_MCAN_RX_Header MsgHeader = { 0 };		// Initialize to 0 or you'll get garbage
				numBytes = can->MCAN_ReadNextFIFO(RXFIFO0, &MsgHeader, dataPayload);	// This will read the next element in the RX FIFO 0
				msgRxNbr[chnNbr]++;
				nbrErrors += checkMsg(nbrOfLoops, chnNbr, &MsgHeader, numBytes, dataPayload);
			}

			can->can->AHB_READ_32(REG_MCAN_RXF1S);
			can->can->AHB_READ_32(REG_MCAN_RXF1S);
			can->can->AHB_READ_32(REG_MCAN_RXF1S);
			can->can->AHB_READ_32(REG_MCAN_RXF1S);
			can->can->AHB_READ_32(REG_MCAN_RXF1S);
			can->can->AHB_READ_32(REG_MCAN_RXF1S);

		}

		if (msgRxNbr[0] >= rxTarget && msgRxNbr[1] >= rxTarget
			&& msgRxNbr[2] >= rxTarget && msgRxNbr[3] >= rxTarget)
			break;
		if (nbrErrors > 5) {
			printf("Too many errors, aborting!\n");
			break;
		}
		//		Sleep(1);
	}
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds = EndingTime - StartingTime;
	delay = (double)ElapsedMicroseconds / 1000000.0;

	int totalPackets = 0;
	for (int i = 0; i < 4; i++) {
		totalPackets += msgTxNbr[i] + msgRxNbr[i];

		if (msgTxNbr[i] == packetsPerChannel && msgRxNbr[i] == packetsPerChannel * 3) {
			if (verbose) printf("SUCCESS: Channel #%d: transmitted = %d, received = %d\n", i, msgTxNbr[i], msgRxNbr[i]);
		}
		else {
			nbrErrors++;
			if (msgTxNbr[i] != packetsPerChannel)
				printf("ERROR: Channel #%d: transmitted = %d should be %d\n", i, msgTxNbr[i], packetsPerChannel);
			if (msgRxNbr[i] != packetsPerChannel * 3)
				printf("ERROR: Channel #%d: received = %d should be %d\n", i, msgRxNbr[i], packetsPerChannel * 3);
		}
	}
	if (verbose) printf("%d packets in %7.3f seconds (%7.1f msg/sec, %7.1f microseconds/msg)\n", totalPackets, delay, totalPackets / delay, (delay * 1000000.0) / totalPackets);
	return nbrErrors;
}

/** @brief loopback test
*
* @param nbrOfPackets Number of loops where packet are sent
* @retval Nbr of errors
*/
int CanFdTest::testTiLibIrq(int nbrOfTestLoops)
{
	int nbrErrors = 0;

	/***********************************
	 * MSP430 Specific Initializations *
	 ***********************************/
	dut->reset();
	Sleep(100);

	/*********************************************
	 * Initialize the 4 interfaces               *
	 *********************************************/
	for (int i = 0; i < dut->nbrOfCanInterfaces; i++) {
		printf("-------------------------------------\n");
		printf("Initializing CAN controller #%d\n", i);
		nbrErrors += Init_CAN(dut->can[i]);
		dut->input0->resetIrq();
		printf("Finishing CAN initialization!\n");
	}
	checkIrq(0);

	dut->hookInterruptServiceRoutine(0xffff, (MINIPCIE_INT_HANDLER)PORT1_ISR, (void*)this);
	dut->enableInterrupts();
	for (int i = 0; i < dut->nbrOfCanInterfaces; i++) {
		TCAN4550* can = dut->can[i];
		can->enableIrq();
		TCAN4x5x_MCAN_TX_Header header = { 0 };			// Remember to initialize to 0, or you'll get random garbage!

		uint8_t data[4];								// Define the data payload
		data[0] = 0x50 + i;
		data[1] = 0x60 + i;
		data[2] = 0x70 + i;
		data[3] = 0x80 + i;									// Define the data payload
		header.DLCode = MCAN_DLC_4B;					// Set the DLC to be equal to or less than the data payload (it is ok to pass a 64 byte data array into the WriteTXFIFO function if your DLC is 8 bytes, only the first 8 bytes will be read)
		header.ID = 280 + i;								// Set the ID
		header.FDF = 1;									// CAN FD frame enabled
		header.BRS = 1;									// Bit rate switch enabled
		header.EFC = 0;
		header.MM = 0;
		header.RTR = 0;
		header.XTD = 0;									// We are not using an extended ID in this example
		header.ESI = 0;									// Error state indicator


		can->MCAN_WriteTXBuffer(0, &header, data);	// This function actually writes the header and data payload to the specified TX Fifo number. It returns the bit necessary to write to TXBAR,
														// but does not necessarily require you to use it. In this example, we won't, so that we can send the data queued up at a later point.
		// Let's make another packet
		data[0] = 0x10 + i;
		data[1] = 0x20 + i;
		data[2] = 0x30 + i;
		data[3] = 0x40 + i;									// Define the data payload

		header.DLCode = MCAN_DLC_4B;						// Set the DLC to be equal to or less than the data payload (it is ok to pass a 64 byte data array into the WriteTXFIFO function if your DLC is 8 bytes, only the first 8 bytes will be read)
		header.ID = 260 + i;								// Set the ID
		header.FDF = 1;									// CAN FD frame enabled
		header.BRS = 1;									// Bit rate switch enabled
		header.EFC = 0;
		header.MM = 0;
		header.RTR = 0;
		header.XTD = 0;									// We are not using an extended ID in this example
		header.ESI = 0;									// Error state indicator

		can->MCAN_WriteTXBuffer(1, &header, data);	// This line writes the data and header to TX FIFO 1
	}

	checkIrq(0);

	int nbrOfLoops;
	int msgTxNbr[4] = { 0 };
	int msgRxNbr[4] = { 0 };
	QueryPerformanceCounter(&StartingTime);

	for (nbrOfLoops = 0; nbrOfLoops < nbrOfTestLoops+100; nbrOfLoops++)
	{
		checkIrq(0, 0);
		if (nbrOfLoops < nbrOfTestLoops) {
			dut->can[nbrOfLoops % 4]->MCAN_TransmitBufferContents(0);		// Request that TX Buffer 1 be transmitted
			msgTxNbr[nbrOfLoops % 4]++;
			dut->can[nbrOfLoops % 4]->MCAN_TransmitBufferContents(1);		// Request that TX Buffer 1 be transmitted
			msgTxNbr[nbrOfLoops % 4]++;
		}

		checkIrq(0, 0);
		dut->cra->setIrqEnableMask(0);
		dut->cra->setIrqEnableMask(0xffff);
		TCAN_Int_Cnt++;
		if (TCAN_Int_Cnt > 0)
		{
			TCAN_Int_Cnt--;
			for (int chnNbr = 0; chnNbr < dut->nbrOfCanInterfaces; chnNbr++) {
				TCAN4550* can = dut->can[chnNbr];
				TCAN4x5x_MCAN_RX_Header MsgHeader = { 0 };		// Initialize to 0 or you'll get garbage
				uint8_t numBytes = 0;
				uint8_t dataPayload[64] = { 0 };

				can->MCAN_ClearInterruptsAll();	// Clear any of the interrupt bits that are set.
				while (!isRxFifo0Empty(can->can) && nbrErrors <= 5) {
					numBytes = can->MCAN_ReadNextFIFO(RXFIFO0, &MsgHeader, dataPayload);	// This will read the next element in the RX FIFO 0
					msgRxNbr[chnNbr]++;
					nbrErrors += checkMsg(nbrOfLoops, chnNbr, &MsgHeader, numBytes, dataPayload);
				}
			}
		}
		if (nbrErrors > 5) {
			printf("Too many errors, aborting!\n");
			break;
		}

	}
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds = EndingTime - StartingTime;
	double delay = (double)ElapsedMicroseconds / 1000000.0;

	int totalPackets = 0;
	for (int i = 0; i < 4; i++) {
		int packetsPerChannel = nbrOfTestLoops / 2;			// 2 packets per channel per loop
		totalPackets += msgTxNbr[i] + msgRxNbr[i];

		if (msgTxNbr[i] == packetsPerChannel && msgRxNbr[i] == packetsPerChannel * 3)
			printf("SUCCESS: Channel #%d: transmitted = %d, received = %d\n", i, msgTxNbr[i], msgRxNbr[i]);
		else {
			nbrErrors++;
			if (msgTxNbr[i] != packetsPerChannel)
				printf("ERROR: Channel #%d: transmitted = %d should be %d\n", i, msgTxNbr[i], packetsPerChannel);
			if (msgRxNbr[i] != packetsPerChannel * 3)
				printf("ERROR: Channel #%d: received = %d should be %d\n", i, msgRxNbr[i], packetsPerChannel * 3);
		}
	}
	printf("%d packets in %7.3f seconds (%7.1f msg/sec, %7.1f microsecs/msg)\n", totalPackets, delay, totalPackets / delay, (delay * 1000000.0) / totalPackets);
	return nbrErrors;
}

/** @brief set the nominal timing */
bool CanFdTest::setNominalTiming(TCAN4550* can, MCAN_Nominal_Speed nominalSpeed, bool verbose)
{
	TCAN4x5x_MCAN_Nominal_Timing_Simple TCANNomTiming = { 0 };
	bool st;

	// Configure the CAN bus speeds
	switch (nominalSpeed) {
	case NOMINAL_SPEED_10K:
		// 250k arbitration with a 40 MHz crystal ((40E6 / 100) / (32 + 8) = 10E3)
		TCANNomTiming.NominalBitRatePrescaler = 100;
		TCANNomTiming.NominalTqBeforeSamplePoint = 32;
		TCANNomTiming.NominalTqAfterSamplePoint = 8;
		if (verbose) printf("Nominal speed 10 kbps\n");
		break;
	case NOMINAL_SPEED_20K:
		// 250k arbitration with a 40 MHz crystal ((40E6 / 50) / (32 + 8) = 20E3)
		TCANNomTiming.NominalBitRatePrescaler = 50;
		TCANNomTiming.NominalTqBeforeSamplePoint = 32;
		TCANNomTiming.NominalTqAfterSamplePoint = 8;
		if (verbose) printf("Nominal speed 20 kbps\n");
		break;
	case NOMINAL_SPEED_50K:
		// 250k arbitration with a 40 MHz crystal ((40E6 / 20) / (32 + 8) = 50E3)
		TCANNomTiming.NominalBitRatePrescaler = 20;
		TCANNomTiming.NominalTqBeforeSamplePoint = 32;
		TCANNomTiming.NominalTqAfterSamplePoint = 8;
		if (verbose) printf("Nominal speed 50 kbps\n");
		break;
	case NOMINAL_SPEED_100K:
		// 250k arbitration with a 40 MHz crystal ((40E6 / 10) / (32 + 8) = 100E3)
		TCANNomTiming.NominalBitRatePrescaler = 10;
		TCANNomTiming.NominalTqBeforeSamplePoint = 32;
		TCANNomTiming.NominalTqAfterSamplePoint = 8;
		if (verbose) printf("Nominal speed 100 kbps\n");
		break;
	case NOMINAL_SPEED_125K:
		// 250k arbitration with a 40 MHz crystal ((40E6 / 8) / (32 + 8) = 125E3)
		TCANNomTiming.NominalBitRatePrescaler = 8;
		TCANNomTiming.NominalTqBeforeSamplePoint = 32;
		TCANNomTiming.NominalTqAfterSamplePoint = 8;
		if (verbose) printf("Nominal speed 125 kbps\n");
		break;
	case NOMINAL_SPEED_250K:
		// 250k arbitration with a 40 MHz crystal ((40E6 / 4) / (32 + 8) = 250E3)
		TCANNomTiming.NominalBitRatePrescaler = 4;
		TCANNomTiming.NominalTqBeforeSamplePoint = 32;
		TCANNomTiming.NominalTqAfterSamplePoint = 8;
		if (verbose) printf("Nominal speed 250 kbps\n");
		break;
	case NOMINAL_SPEED_500K:
		// 500k arbitration with a 40 MHz crystal ((40E6 / 2) / (32 + 8) = 500E3)
		TCANNomTiming.NominalBitRatePrescaler = 2;
		TCANNomTiming.NominalTqBeforeSamplePoint = 32;
		TCANNomTiming.NominalTqAfterSamplePoint = 8;
		if (verbose) printf("Nominal speed 500 kbps\n");
		break;
	case NOMINAL_SPEED_800K:
		// 500k arbitration with a 40 MHz crystal ((40E6 / 2) / (20 + 5) = 800E3)
		TCANNomTiming.NominalBitRatePrescaler = 2;
		TCANNomTiming.NominalTqBeforeSamplePoint = 20;
		TCANNomTiming.NominalTqAfterSamplePoint = 5;
		if (verbose) printf("Nominal speed 800 kbps\n");
		break;
	case NOMINAL_SPEED_1000K:
		// 500k arbitration with a 40 MHz crystal ((40E6 / 2) / (16 + 4) = 1000E3)
		TCANNomTiming.NominalBitRatePrescaler = 2;
		TCANNomTiming.NominalTqBeforeSamplePoint = 16;
		TCANNomTiming.NominalTqAfterSamplePoint = 4;
		if (verbose) printf("Nominal speed 1 Mbps\n");
		break;
	default:
		return false;
	}

	st = can->MCAN_ConfigureNominalTiming_Simple(&TCANNomTiming);	// Setup nominal/arbitration bit timing
	return st;
}

/** @brief set the data timing */
bool CanFdTest::setDataTiming(TCAN4550* can, MCAN_Data_Speed dataSpeed, bool verbose)
{
	TCAN4x5x_MCAN_Data_Timing_Simple TCANDataTiming = { 0 };
	bool st;

	switch (dataSpeed) {
	case DATA_SPEED_500K:
		// 500k CAN FD with a 40 MHz crystal ((40E6 / 2) / (32 + 8) = 500E3)
		TCANDataTiming.DataBitRatePrescaler = 2;
		TCANDataTiming.DataTqBeforeSamplePoint = 32;
		TCANDataTiming.DataTqAfterSamplePoint = 8;
		if (verbose) printf("Data speed 500 kbps\n");
		break;
	case DATA_SPEED_1000K:
		// 500k CAN FD with a 40 MHz crystal (40E6 / (32 + 8) = 1E6)
		TCANDataTiming.DataBitRatePrescaler = 2;
		TCANDataTiming.DataTqBeforeSamplePoint = 32;
		TCANDataTiming.DataTqAfterSamplePoint = 8;
		if (verbose) printf("Data speed 1 Mbps\n");
		break;
	case DATA_SPEED_2000K:
		// 2 Mbps CAN FD with a 40 MHz crystal (40E6 / (15 + 5) = 2E6)
		TCANDataTiming.DataBitRatePrescaler = 1;
		TCANDataTiming.DataTqBeforeSamplePoint = 15;
		TCANDataTiming.DataTqAfterSamplePoint = 5;
		if (verbose) printf("Data speed 2 Mbps\n");
		break;
	default:
	case DATA_SPEED_4000K:
		// 4 Mbps CAN FD with a 40 MHz crystal (40E6 / (8 + 2) = 4E6)
		TCANDataTiming.DataBitRatePrescaler = 1;
		TCANDataTiming.DataTqBeforeSamplePoint = 8;
		TCANDataTiming.DataTqAfterSamplePoint = 2;
		if (verbose) printf("Data speed 4 Mbps\n");
		break;
	case DATA_SPEED_8000K:
		// 8 Mbps CAN FD with a 40 MHz crystal (40E6 / (4 + 1) = 8E6)
		TCANDataTiming.DataBitRatePrescaler = 1;
		TCANDataTiming.DataTqBeforeSamplePoint = 4;
		TCANDataTiming.DataTqAfterSamplePoint = 1;
		if (verbose) printf("Data speed 8 Mbps\n");
		break;
	}

	st = can->MCAN_ConfigureDataTiming_Simple(&TCANDataTiming);		// Setup CAN FD timing
	return st;
}

/** @brief Configure the TCAN4550
 */
int CanFdTest::Init_CAN(TCAN4550* can, bool verbose)
{
	int nbrErrors = 0;
	bool st;

	uint32_t id[3] = { 0 };
	can->Device_ReadDeviceIdent(id);
	if (verbose) printf("Device nbr #%d, offset = 0x%4p\n", can->can->slave, can->can->getAddress());
//	printf("Device id: %s\n", (char *)id);
//	printf("Device version 0x%04x\n", can->Device_ReadDeviceVersion());

	if (strcmp((char*)id, "TCAN4550") != 0) {
		printf("Device nbr #%d, Cannot read the device!\n", can->can->slave);
		return 1;
	}

	// Step one attempt to clear all interrupts
	TCAN4x5x_Device_Interrupt_Enable dev_ie = {0};				// Initialize to 0 to all bits are set to 0.
	st = can->Device_ConfigureInterruptEnable(&dev_ie);	// Disable all non-MCAN related interrupts for simplicity
	if (!st) {
		printf("Device nbr #%d, Device_ConfigureInterruptEnable failed!\n", can->can->slave);
		nbrErrors++;
	}
	else {
		if (verbose)
			printf("Device_ConfigureInterruptEnable succeeded!\n");
	}

	TCAN4x5x_Device_Interrupts dev_ir = {0};					// Setup a new MCAN IR object for easy interrupt checking
	can->Device_ReadInterrupts(&dev_ir);						// Request that the struct be updated with current DEVICE (not MCAN) interrupt values

	if (dev_ir.PWRON) {
		can->Device_ClearInterrupts(&dev_ir);
	}

	// Configure the MCAN core settings
	TCAN4x5x_MCAN_CCCR_Config cccrConfig = {0};					// Remember to initialize to 0, or you'll get random garbage!
	if (isCanFd) {
		cccrConfig.FDOE = 1;										// CAN FD mode enable
		cccrConfig.BRSE = 1;										// CAN FD Bit rate switch enable
		if (verbose) printf("CAN FD enabled\n");
	}
	else {
		cccrConfig.FDOE = 0;										// CAN FD mode disable
		cccrConfig.BRSE = 0;										// CAN FD Bit rate switch disable
		if (verbose) printf("CAN FD disabled\n");
	}
	cccrConfig.DAR = 1;											// Disable automatic transmission

	/* ************************************************************************
	 * In the next configuration block, we will set the MCAN core up to have:
	 *   - 1 SID filter element
	 *   - 1 XID Filter element
	 *   - 5 RX FIFO 0 elements
	 *   - RX FIFO 0 supports data payloads up to 64 bytes
	 *   - RX FIFO 1 and RX Buffer will not have any elements, but we still set their data payload sizes, even though it's not required
	 *   - No TX Event FIFOs
	 *   - 2 Transmit buffers supporting up to 64 bytes of data payload
	 */
	TCAN4x5x_MRAM_Config MRAMConfiguration = {0};
	MRAMConfiguration.SIDNumElements = 1;						// Standard ID number of elements
	MRAMConfiguration.XIDNumElements = 1;						// Extended ID number of elements
	MRAMConfiguration.Rx0NumElements = 5;						// RX0 Number of elements
	MRAMConfiguration.Rx0ElementSize = MRAM_64_Byte_Data;		// RX0 data payload size
	MRAMConfiguration.Rx1NumElements = 0;						// RX1 number of elements
	MRAMConfiguration.Rx1ElementSize = MRAM_64_Byte_Data;		// RX1 data payload size
	MRAMConfiguration.RxBufNumElements = 0;						// RX buffer number of elements
	MRAMConfiguration.RxBufElementSize = MRAM_64_Byte_Data;		// RX buffer data payload size
	MRAMConfiguration.TxEventFIFONumElements = 0;				// TX Event FIFO number of elements
	MRAMConfiguration.TxBufferNumElements = 2;					// TX buffer number of elements
	MRAMConfiguration.TxBufferElementSize = MRAM_64_Byte_Data;	// TX buffer data payload size


	// Configure the MCAN core with the settings above, these changes in this block all are protected write registers, 
	// so we just knock them out at once
	st = can->MCAN_EnableProtectedRegisters();						// Start by making protected registers accessible
	if (!st) {
		printf("Device nbr #%d, MCAN_EnableProtectedRegisters failed!\n", can->can->slave);
		nbrErrors++;
	}
	else {
		if (verbose)
			printf("MCAN_EnableProtectedRegisters succeeded!\n");
	}

	st = can->MCAN_ConfigureCCCRRegister(&cccrConfig);				// Enable FD mode and Bit rate switching
	if (!st) {
		printf("Device nbr #%d, MCAN_ConfigureCCCRRegister failed!\n", can->can->slave);
		nbrErrors++;
	}

	st = setNominalTiming(can, nominalSpeed, verbose);	// Setup nominal/arbitration bit timing
	if (!st) {
		printf("Device nbr #%d, MCAN_ConfigureNominalTiming_Simple failed!\n", can->can->slave);
		nbrErrors++;
	}

	st = setDataTiming(can, dataSpeed, verbose);	// Setup nominal/arbitration bit timing
	if (!st) {
		printf("Device nbr #%d, MCAN_ConfigureDataTiming_Simple failed!\n", can->can->slave);
		nbrErrors++;
	}


	if (!st) {
		printf("Device nbr #%d, MCAN_ConfigureDataTiming_Simple failed!\n", can->can->slave);
		nbrErrors++;
	}

	can->MRAM_Clear();												// Clear all of MRAM (Writes 0's to all of it)
	st = can->MRAM_Configure(&MRAMConfiguration);					// Set up the applicable registers related to MRAM configuration
	if (!st) {
		printf("Device nbr #%d, MRAM_Configure failed!\n", can->can->slave);
		nbrErrors++;
	}

	st = can->MCAN_DisableProtectedRegisters();						// Disable protected write and take device out of INIT mode
	if (!st) {
		printf("Device nbr #%d, MCAN_DisableProtectedRegisters failed!\n", can->can->slave);
		nbrErrors++;
	}

	// Set the interrupts we want to enable for MCAN
	TCAN4x5x_MCAN_Interrupt_Enable mcan_ie = {0};				// Remember to initialize to 0, or you'll get random garbage!
	mcan_ie.RF0NE = 1;											// RX FIFO 0 new message enable

	can->MCAN_ConfigureInterruptEnable(&mcan_ie);				// Enable the appropriate registers


	// Setup filters, this filter will mark any message with ID 0x055 as a priority message
	TCAN4x5x_MCAN_SID_Filter SID_ID = {0};
	SID_ID.SFT = TCAN4x5x_SID_SFT_CLASSIC;						// SFT: Standard filter type. Configured as a classic filter
	SID_ID.SFEC = TCAN4x5x_SID_SFEC_PRIORITYSTORERX0;			// Standard filter element configuration, store it in RX fifo 0 as a priority message
	SID_ID.SFID1 = 0x055;										// SFID1 (Classic mode Filter)
	SID_ID.SFID2 = 0x7FF;										// SFID2 (Classic mode Mask)
	st = can->MCAN_WriteSIDFilter(0, &SID_ID);					// Write to the MRAM
	if (!st) {
		printf("Device nbr #%d, MCAN_WriteSIDFilter failed!\n", can->can->slave);
		nbrErrors++;
	}
	else {
		if (verbose)
			printf("MCAN_WriteSIDFilter succeeded!\n");
	}

	// Store ID 0x12345678 as a priority message
	TCAN4x5x_MCAN_XID_Filter XID_ID = {0};
	XID_ID.EFT = TCAN4x5x_XID_EFT_CLASSIC;						// EFT
	XID_ID.EFEC = TCAN4x5x_XID_EFEC_PRIORITYSTORERX0;			// EFEC
	XID_ID.EFID1 = 0x12345678;									// EFID1 (Classic mode filter)
	XID_ID.EFID2 = 0x1FFFFFFF;									// EFID2 (Classic mode mask)
	st = can->MCAN_WriteXIDFilter(0, &XID_ID);					// Write to the MRAM
	if (!st) {
		printf("Device nbr #%d, MCAN_WriteXIDFilter failed!\n", can->can->slave);
		nbrErrors++;
	}
	else {
		if (verbose)
			printf("MCAN_WriteXIDFilter succeeded!\n");
	}

	st = can->Device_SetMode(TCAN4x5x_DEVICE_MODE_NORMAL);				// Set to normal mode, since configuration is done. This line turns on the transceiver
	if (!st) {
		printf("Device nbr #%d, Device_SetMode failed!\n", can->can->slave);
		nbrErrors++;
	}
	else {
		if (verbose)
			printf("Device_SetMode succeeded!\n");
	}

	can->MCAN_ClearInterruptsAll();					// Resets all MCAN interrupts
	return nbrErrors;
}

/** @brief PORT1 Interrupt Service Routine
 * 
 * Handles Interrupt from the TCAN4550 on P1.5
 */
void PORT1_ISR(void *userData)
{
	CanFdTest* dut = (CanFdTest *)userData;
	dut->TCAN_Int_Cnt++; 
}

void printIrqStatus(TCAN4x5x_Device_Interrupts *dev_ir)
{ 
	if (dev_ir->VTWD) printf("VTWD ");
	if (dev_ir->M_CAN_INT) printf("M_CAN_INT ");
	if (dev_ir->SWERR) printf("SWERR ");
	if (dev_ir->SPIERR) printf("SPIERR ");
	if (dev_ir->CBF) printf("CBF ");
	if (dev_ir->CANERR) printf("CANERR ");
	if (dev_ir->WKRQ) printf("WKRQ ");
	if (dev_ir->GLOBALERR) printf("GLOBALERR ");
	if (dev_ir->CANDOM) printf("CANDOM ");
	if (dev_ir->RESERVED) printf("RESERVED ");
	if (dev_ir->CANTO) printf("CANTO ");
	if (dev_ir->RESERVED2) printf("RESERVED2 ");
	if (dev_ir->FRAME_OVF) printf("FRAME_OVF ");
	if (dev_ir->WKERR) printf("WKERR ");
	if (dev_ir->LWU) printf("LWU ");
	if (dev_ir->CANINT) printf("CANINT ");
	if (dev_ir->ECCERR) printf("ECCERR ");
	if (dev_ir->RESERVED3) printf("RESERVED3 ");
	if (dev_ir->WDTO) printf("WDTO ");
	if (dev_ir->TSD) printf("TSD ");
	if (dev_ir->PWRON) printf("PWRON ");
	if (dev_ir->UVIO) printf("UVIO ");
	if (dev_ir->UVSUP) printf("UVSUP ");
	if (dev_ir->SMS) printf("SMS ");
	if (dev_ir->CANBUSBAT) printf("CANBUSBAT ");
	if (dev_ir->CANBUSGND) printf("CANBUSGND ");
	if (dev_ir->CANBUSOPEN) printf("CANBUSOPEN ");
	if (dev_ir->CANLGND) printf("CANLGND ");
	if (dev_ir->CANHBAT) printf("CANHBAT ");
	if (dev_ir->CANHCANL) printf("CANHCANL ");
	if (dev_ir->CANBUSTERMOPEN) printf("CANBUSTERMOPEN ");
	if (dev_ir->CANBUSNORM) printf("CANBUSNORM ");
	printf("\n");
}
