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

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		7/23/2020	phf	Written
//---------------------------------------------------------------------

#pragma once

#include "stdint.h"
#include "PCIeMini_CAN_FD.h"
#include "TestProgram.h"

enum eTX_Baud_Rates
{
	BAUD_10K,
	BAUD_20K,
	BAUD_50K,
	BAUD_100K,
	BAUD_125K,
	BAUD_250K,
	BAUD_500K,
	BAUD_800K,
	BAUD_1000K
};
//static const char* nominalBaudRates[] = { "10k","20k","50k","100k","125k","250k","500k","800k","1000k" }; // baud rate strings
#define NOMINAL_RATE_PRESETS 9

enum MCAN_Nominal_Speed {
	NOMINAL_SPEED_10K,
	NOMINAL_SPEED_20K,
	NOMINAL_SPEED_50K,
	NOMINAL_SPEED_100K,
	NOMINAL_SPEED_125K,
	NOMINAL_SPEED_250K,
	NOMINAL_SPEED_500K,
	NOMINAL_SPEED_800K,
	NOMINAL_SPEED_1000K
};

//static const char* dataBaudRates[] = { "500k","1000k","2000k","4000k","8000k" }; // baud rate strings
#define DATA_RATE_PRESETS 5
enum MCAN_Data_Speed {
	DATA_SPEED_500K,
	DATA_SPEED_1000K,
	DATA_SPEED_2000K,
	DATA_SPEED_4000K,
	DATA_SPEED_8000K
};

class CanFdTest : public TestProgram
{
public:
	PCIeMini_CAN_FD* dut;
	double StartingTime, EndingTime, ElapsedMicroseconds;
	double Frequency;
	volatile uint8_t TCAN_Int_Cnt = 0;					// A variable used to keep track of interrupts on P1.3 (MCAN Interrupt pin)
	MCAN_Nominal_Speed nominalSpeed;
	bool isCanFd;
	MCAN_Data_Speed dataSpeed;

	static CanFdTest* getInstance()
	{
		if (testInstance == NULL) {
			testInstance = new CanFdTest();
		}
		return testInstance;
	}

	int mainTest(int brdNbr, bool executeLoopback);

	int checkSpiErrorBit(TCAN4550* can, int verbose = 1);

	int testTiLibIrq(int nbrOfPackets = 10000);
	int testTiLib(int nbrOfPackets = 10000);
	int testDpr();
	int Init_CAN(TCAN4550* can, bool verbose = true);
	int checkIrq(int chNumber, int verbose = 0);
	int testSpiRead32(uint8_t spiController);
	int testSpiReadMult(uint8_t spiController, int len);
	int testSpiReadMultDMA(uint8_t spiController, int len);
	int testSpiWrite(uint8_t spiController);
	int testSpiReadWrite(int nbrOfLoops);
	int testPCIeSpeed();
	int testLocalBlockDma(uint32_t tfrLengthWord);
	int testPCIeDma();
	int testPCIeToBrdDma(TransferDesc* tfrDesc);
	int testBrdToPCIeDma(TransferDesc* tfrDesc);
	int staticIrqTest();

	// utilities
	bool isRxFifo0Empty(TcanInterface* can);
	void printTxMsg(TCAN4x5x_MCAN_TX_Header* MsgHeader, uint8_t* dataPayload);
	void printRxMsg(TCAN4x5x_MCAN_RX_Header* MsgHeader, int numBytes, uint8_t* dataPayload);
	void printChannelStatus(uint8_t channelNbr);
	bool setNominalTiming(TCAN4550* can, MCAN_Nominal_Speed nominalSpeed, bool verbose);
	bool setDataTiming(TCAN4550* can, MCAN_Data_Speed dataSpeed, bool verbose);
	int checkMsg(int nbrOfLoops, int chnNumber, TCAN4x5x_MCAN_RX_Header* MsgHeader, int numBytes, uint8_t* dataPayload);
	int sendCanMesg();
	int canFdNiosTest();

private:
	uint32_t cyclical[PCIeMini_CAN_FD::nbrOfCanInterfaces];
	uint64_t lastMsgTs[PCIeMini_CAN_FD::nbrOfCanInterfaces];
	inline CanFdTest()
	{
		dut = new PCIeMini_CAN_FD();
//		ElapsedMicroseconds = { 0 };
//		StartingTime = { 0 };
//		EndingTime = { 0 };
//		Frequency = { 0 };
		nominalSpeed = NOMINAL_SPEED_500K;
		dataSpeed = DATA_SPEED_2000K;
		isCanFd = false;

		for (int i = 0; i < PCIeMini_CAN_FD::nbrOfCanInterfaces; i++) {
			cyclical[i] = 0;
			lastMsgTs[i] = 0;
		}
	}
	static CanFdTest* testInstance;

};
