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
#include "AvioTest.h"
#include "AvioAddressSpace.h"
#include "hostbug.h"

AvioTest* AvioTest::testInstance = NULL;

uint32_t holt_thresh_glob = ((0x3f &  GL_DEFAULT) << 18) |  // 23:18
		                    ((0x3f &  GH_DEFAULT) << 12) |  // 17:12
		                    ((0x3f &  VL_DEFAULT) <<  6) |  // 11: 6
		                    ((0x3f &  VH_DEFAULT) <<  0);   //  5: 0
uint32_t holt_config_glob = 0x00; // 4 high / 4 low '1' = GND/Open


void int_function(void* p_uio_board)
{
	uint8_t val = 0;

	printf("Int!! %x\n", val & 0x3f);

	return;
}

void print_thresh_volts(uint32_t thresh)
{
	uint8_t gl, gh, vl, vh;
	gl = ((thresh >> 18) & 0x3f);
	gh = ((thresh >> 12) & 0x3f);
	vl = ((thresh >>  6) & 0x3f);
	vh = ((thresh >>  0) & 0x3f);

	printf("GL %x\n", (int)(gl));
	printf("GH %x\n", (int)(gh));
	printf("VL %x\n", (int)(vl));
	printf("VH %x\n", (int)(vh));

	printf("GL %f Volts\n", 12 * (0.126 + gl/91.6));
	printf("GH %f Volts\n", 12 * (0.144 + gh/98.9));
	printf("VL %f Volts\n", 12 * (0.126 + vl/91.6));
	printf("VH %f Volts\n", 12 * (0.144 + vh/98.9));

}

int AvioTest::spiTest()
{
	HI_8429 *hi = dut->hi8429;
	SpiOpenCore *spi = hi->spi;
	uint32_t errNbr = 0;
	uint32_t err = 0;

	uint32_t ctrl = spi->getSpiControl();
	for (int i = 0; i<0x7f; i++) {
		uint32_t data = 0x2000 + i;
		spi->setSpiControl(data);
		uint32_t readback = spi->getSpiControl();
		if (readback != data) {
			if (err < 5)
				printf("Error: SPI Controller Control Register: wrote 0x%04x, read 0x%04x\n", data, readback);
			err++;
		}
	}
	spi->setSpiControl(ctrl);
	if (err == 0) {
		printf("Success: SPI Controller Control Register test passed.\n");
	}
	else {
		printf("Failed: SPI Controller Control Register test.\n");
		errNbr += err;
	}

	uint32_t divider = spi->getSpiDivider();
	err = 0;
	for (uint32_t i = 0; i<0x7f; i++) {
		//hi->spi_width(i);
		spi->setSpiDivider(i);
//		Sleep(1);
		uint32_t readback = spi->getSpiDivider();
		if (i != readback) {
			if (err < 5)
				printf("error: SPI Controller Divider register wrote 0x%04x, read 0x%04x\n", i, spi->getSpiDivider());
			err++;
		}
	}
	spi->setSpiDivider(divider);
	if (err == 0) {
		printf("Success: SPI Controller Divider register test passed.\n");
	}
	else {
		printf("Failed: SPI Controller Divider Register test.\n");
		errNbr += err;
	}

	return errNbr;
}

int AvioTest::check8429ThresholdRegister(uint32_t newVal, uint32_t oldVal)
{
	uint32_t readBack;
	int errNbr = 0;

	readBack = dut->hi8429->setHoltThresholdReg(newVal);
	if (readBack == oldVal) {
		printf("Success: ");
	}
	else {
		printf("Failure: ");
		errNbr++;
	}
    printf("read 0x%08x, wrote 0x%08x, spi_control=0x%04x\n", readBack, newVal, dut->hi8429->spi->getSpiControl());
    return errNbr;
}

int AvioTest::check8429ControlRegisterBIT(bool verbose, uint32_t newVal, uint32_t oldVal)
{
	uint32_t readBack;
	int errNbr = 0;

	readBack = dut->hi8429->setHoltConfigReg(0x100 + newVal);
	uint8_t sense = dut->hi8429->getHoltSenseReg();
	if ((readBack == 0x100 + oldVal) && sense == newVal) {
		if (!verbose) {
			return errNbr;
		}
		printf("Success: ");
	}
	else {
		printf("Failure: ");
		errNbr++;
	}
	printf("Config Reg = 0x%04x, Readback config reg 0x%04x, Sense inputs register 0x%04x,\n", 0x100 + newVal, readBack, sense);
    return errNbr;
}

int AvioTest::mainTestLoop()
{
	uint32_t sys_id;
	uint32_t timestamp;
	int brdNbr = 0;
	uint32_t holt_thresh;
	double low,high;
	AvioCtrlReg *ctrlReg;
	int err;
	int errNbr = 0;

	dut->open(brdNbr);
	ctrlReg = dut->ctrlReg;
    printf("CtrlReg: 0x%04x\n", ctrlReg->getData());

	sys_id = dut->getFpgaID();
	printf("System ID %x\n", sys_id);

	timestamp = dut->getFpgaTimeStamp();
	printf("Timestamp %x\n", timestamp);
	time_t ts = (time_t)timestamp;
	char buff[20];
	strftime(buff, 20, "%m/%d/%Y %H:%M:%S", localtime(&ts));
    printf("%s\n", buff);

//    AvioAddressSpace *CurrentAs = new AvioAddressSpace("BAR #2", dut->bar2.Address, dut->bar2.Length);
//	hostbug_main((AddrSpace*)CurrentAs);

    holt_thresh = dut->hi8429->setHoltThresholdReg(8.717240, 9.250750, 8.717240, 9.250750);
    holt_thresh = dut->hi8429->setHoltThresholdReg(8.717240, 9.250750, 8.717240, 9.250750);
    printf("Programmed thresholds: ");
    print_thresh_volts(holt_thresh);

//    holt_thresh = 	HI_8429::calculateThresholds( 8.717240, 9.250750, 8.717240, 9.250750);
    holt_thresh = dut->hi8429->setHoltThresholdReg(3.0, 4.0, 3.0, 4.0);
    holt_thresh = dut->hi8429->setHoltThresholdReg(3.0, 4.0, 3.0, 4.0);
    printf("Calculated thresholds: ");
    print_thresh_volts(holt_thresh);

    spiTest();

    err = 0;
    dut->hi8429->setHoltThresholdReg(0x555555);
    err += check8429ThresholdRegister(0x555555, 0x555555);
    err += check8429ThresholdRegister(0xabcdef, 0x555555);
    err += check8429ThresholdRegister(0xaaaaaa, 0xabcdef);
    err += check8429ThresholdRegister(HI_8429::calculateThresholds(3.0, 4.0, 3.0, 4.0), 0xaaaaaa);
    if (err == 0) {
    	printf("Success: HI-8429 Threshold Register test passed\n");
    }
    else {
    	printf("Failure: HI-8429 Threshold Register test failed\n");
    	errNbr += err;
    }

    Sleep(100);
    uint8_t oldVal = 0;
    dut->hi8429->setHoltConfigReg(0x100);
    err = 0;
    for (uint32_t channel = 1; channel < 0x100; channel *= 2) {
    	err += check8429ControlRegisterBIT(false, channel, oldVal);
    	oldVal = channel;
    }
    if (err == 0) {
    	printf("Success: HI-8429 Control Register BIT test passed\n");
    }
    else {
    	printf("Failure: HI-8429 Control Register BIT test failed\n");
    	errNbr += err;
    }

    dut->hi8429->setHoltConfigReg(0);

#if 0
    printf("_________________________________________________\n");
    printf("Ground voltages\n");
    ctrlReg->setSenseSelect(false); // Gnd / Open
    dut->setDigout(0xaaaa);
    double gol = 0;
    for (double v = 0.0; v < 5.0; v += 0.1) {
    	for (int i = 0; i<4; i++) {
    		dut->setReadbackThreshold(i, v, v+1);
    		Sleep(1);
    	}
    	dut->getReadbackThreshold(0, &low, &high);
    	uint16_t rb = dut->digoutReadback->getData();
		printf("Requested read-back threshold of %.3f and %.3f volts, actual %.3f and %.3f volts\n", v, v+1, low, high);
		printf("Digital out: 0x%04x, Read-backs: %04x, Inputs %02x\n", 0xaaaa, rb, dut->getSenseInputsRegister());
    	if ((rb & 0xff) == 0xaa) {
			printf("Requested read-back threshold of %.3f and %.3f volts, actual %.3f and %.3f volts\n", v, v+1, low, high);
			printf("Digital out: 0x%04x, Read-backs: %04x, Inputs %02x\n", 0xaaaa, rb, dut->getSenseInputsRegister());
			gol = v;
			break;
    	}
    }


    double goh;
	for (double v = 5.0; v > gol; v -= 0.1) {
		for (int i = 0; i<4; i++) {
			dut->setReadbackThreshold(i, gol, v);
			Sleep(1);
		}
		dut->getReadbackThreshold(0, &low, &high);
		uint16_t rb = dut->digoutReadback->getData();
		printf("Requested read-back threshold of %.3f and %.3f volts, actual %.3f and %.3f volts\n", gol, v, low, high);
		printf("Digital out: 0x%04x, Read-backs: %04x, Inputs %02x\n", 0xaaaa, dut->digoutReadback->getData(), dut->getSenseInputsRegister());
		if ((rb & 0xff) == 0xaa) {
			printf("Requested read-back threshold of %.3f and %.3f volts, actual %.3f and %.3f volts\n", gol, v, low, high);
			printf("Digital out: 0x%04x, Read-backs: %04x, Inputs %02x\n", 0xaaaa, dut->digoutReadback->getData(), dut->getSenseInputsRegister());
			goh = v;
			break;
		}
	}
#endif

    printf("_________________________________________________\n");
    printf("Power voltages\n");
    double poh;
    double pol = 0;

    ctrlReg->setSenseSelect(false); // Gnd / Open
    for (double v = 5.0; v > 1.0; v -= 0.1) {
    	for (int i = 2; i<3; i++) {
    		dut->setReadbackThreshold(i, v-1, v);
    		Sleep(1);
    	}
    	dut->getReadbackThreshold(2, &low, &high);
    	uint16_t rb = dut->digoutReadback->getData();
		printf("Requested read-back threshold of %.3f and %.3f volts, actual %.3f and %.3f volts\n", v-1, v, low, high);
		printf("Digital out: 0x%04x, Read-backs: %04x, Inputs %02x\n", 0x66aa, rb, dut->hi8429->getHoltSenseReg());
    	if ((rb & 0xff00) == 0x6600) {
			printf("Requested read-back threshold of %.3f and %.3f volts, actual %.3f and %.3f volts\n", v, v+1, low, high);
			printf("Digital out: 0x%04x, Read-backs: %04x, Inputs %02x\n", 0x66aa, rb, dut->hi8429->getHoltSenseReg());
			poh = v;
			break;
    	}
    }

	for (double v = 0.0; v < poh; v += 0.1) {
		for (int i = 2; i<3; i++) {
			dut->setReadbackThreshold(i, pol, v);
			Sleep(1);
		}
		dut->getReadbackThreshold(2, &low, &high);
		uint16_t rb = dut->digoutReadback->getData();
		printf("Requested read-back threshold of %.3f and %.3f volts, actual %.3f and %.3f volts\n", pol, v, low, high);
		printf("Digital out: 0x%04x, Read-backs: %04x, Inputs %02x\n", 0xaaaa, dut->digoutReadback->getData(), dut->hi8429->getHoltSenseReg());
		if ((rb & 0xff00) == 0xaa) {
			printf("Requested read-back threshold of %.3f and %.3f volts, actual %.3f and %.3f volts\n", pol, v, low, high);
			printf("Digital out: 0x%04x, Read-backs: %04x, Inputs %02x\n", 0xaaaa, dut->digoutReadback->getData(), dut->hi8429->getHoltSenseReg());
			poh = v;
			break;
		}
	}
#define GOL		3.0
#define GOH		4.0
#define POL		4.0
#define POH		4.5

    dut->setReadbackThreshold(0, GOL, GOH);
    dut->setReadbackThreshold(1, GOL, GOH);
    dut->setReadbackThreshold(2, POL, POH);
    dut->setReadbackThreshold(3, POL, POH);
	Sleep(1);
    dut->getReadbackThreshold(0, &low, &high);
    printf("Requested read-back threshold of %f and %f volts, actual %f and %f volts\n", GOL, GOH, low, high);

    dut->hi8429->reset();
	dut->hi8429->setHoltThresholdReg(2.0, 4.0, 2.0, 4.0);
    ctrlReg->setSenseSelect(false); // Gnd / Open
    printf ("Ctrl register: 0x%04x\n", ctrlReg->getData());
	while(1){

		for (uint32_t channel = 1; channel < 0x10000; channel = channel * 2) {
			dut->setDigout(channel);
		    ctrlReg->setSenseSelect(false); // Gnd / Open
		    dut->hi8429->setHoltConfigReg(0);
		    Sleep(10);
			printf("Digital out: 0x%04x, Read-backs: %04x, Inputs(low) %02x\n", channel, dut->digoutReadback->getData(), dut->hi8429->getHoltSenseReg());

		    ctrlReg->setSenseSelect(true); // Gnd / Open
		    dut->hi8429->setHoltConfigReg(0);
		    Sleep(10);
			printf("Digital out: 0x%04x, Read-backs: %04x, Inputs(high) %02x\n", channel, dut->digoutReadback->getData(), dut->hi8429->getHoltSenseReg());
		}

	}

	dut->close();

	return 0;

}

int main()
{
	AvioTest::getInstance()->mainTestLoop();
	return 0;
}
