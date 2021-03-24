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
/** @file PCIeMini_CAN_FD_test.cpp : This file contains the 'main' function. Program execution begins and ends there. */
//
// Maintenance Log
//---------------------------------------------------------------------
// v1.0		7/23/2020	phf	Written
//---------------------------------------------------------------------

#include <iostream>
#include "CanFdTest.h"

using namespace std;

int main(int argc, char* argv[])
{
	int verbose = 0;
	int brdNbr = 0;
	bool executeLoopback = false;
	int i;
	CanFdTest *tst = CanFdTest::getInstance();

	for (i = 1; i < argc; i++) {
		char c = argv[i][0];
		switch (c) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			brdNbr = argv[1][0] - '0';
			break;
		case 'v':
			verbose = 1;
			break;
		case 't':
			executeLoopback = true;
			break;
		case '?':
			std::cout << endl << "Possible options: <brd nbr>, v: verbose, t: starts the test" << endl;
			exit(0);
		}
	}

	std::cout << "Starting test" << endl;
	std::cout << "=============" << endl;
	tst->verbose = verbose; 
	tst->dut->verbose = verbose;

	tst->mainTest(brdNbr, executeLoopback);


	return 0;

}

/*
struct TxMsg {
	uint32_t txRate;
	TCAN4x5x_MCAN_TX_Header msgHeader;
	int numBytes;
	uint8_t dataPayload[100];
	uint8_t port;
};
TxMsg txData;
*/

int CanFdTest::sendCanMesg()
{
	uint8_t portNumber;
	TCAN4x5x_MCAN_TX_Header header = { 0 };			// Remember to initialize to 0, or you'll get random garbage!
	uint32_t rate;
	char line[256];
	uint64_t now = GetTickCount64();

	uint8_t data[64] = { 0 };						// Define the data payload
	header.DLCode = MCAN_DLC_4B;					// Set the DLC to be equal to or less than the data payload (it is ok to pass a 64 byte data array into the WriteTXFIFO function if your DLC is 8 bytes, only the first 8 bytes will be read)
	header.ID = 280;								// Set the ID
	header.EFC = 0;
	header.MM = 0;
	header.RTR = 0;
	header.XTD = 0;									// We are not using an extended ID in this example
	header.ESI = 0;									// Error state indicator

	if (isCanFd) {
		header.FDF = 1;									// CAN FD frame enabled
		header.BRS = 1;									// Bit rate switch enabled
	}
	else {
		header.FDF = 0;									// CAN FD frame disabled
		header.BRS = 0;									// Bit rate switch disabled
	}

	int j, k;
	char c;
	printf("rate in mS (0 for once): ");
	while (fgets(line, sizeof line, stdin) == NULL);
	sscanf(line, "%d", &j);
	rate = j;

	printf("id (hexadecimal): 0x");
	while (fgets(line, sizeof line, stdin) == NULL);
	sscanf(line, "%x", &j);
	header.ID = j;

	printf("channel number: ");
	while (fgets(line, sizeof line, stdin) == NULL);
	sscanf(line, "%d", &j);
	if (j > 3 || j < 0 ) j = 0;
	cyclical[j] = rate;

	portNumber = j;
	printf("will this be a remote transmit message ? ( N/y)\n");
	while (fgets(line, sizeof line, stdin) == NULL);
	sscanf(line, " %c", &c);
	if (c == 'y' || c == 'Y')
		header.RTR = 1;
	else
		header.RTR = 0;

	printf("will this be an extended frame message ? ( Y/n)\n");
	while (fgets(line, sizeof line, stdin) == NULL);
	sscanf(line, " %c", &c);
	if (c != 'y' && c != 'Y')
		header.XTD = 1;

	printf("number of bytes (0..8): ");
	while (fgets(line, sizeof line, stdin) == NULL);
	sscanf(line, "%x", &j);
	if (j > 8) j = 8;
	if (j < 0) j = 0;
	header.DLCode = j;
	if (!header.RTR)
		for (int i = 1; i <= j; i++) {
			printf("byte #%d (hexadecimal): 0x", i);
			while (fgets(line, sizeof line, stdin) == NULL);
			sscanf(line, "%x", &k);
			data[i - 1] = k;
		}
	printf("Msg sent:");
	printTxMsg(&header, data);
	dut->can[portNumber]->MCAN_WriteTXBuffer(0, &header, data);	
	dut->can[portNumber]->can->AHB_WRITE_32(REG_MCAN_TXBAR, 1);
	lastMsgTs[portNumber] = now;
	return 0;
}

void CanFdTest::printChannelStatus(uint8_t channelNbr)
{
	printf("________________________________\n");
	printf("Channel #%d status\n", channelNbr);
	printf("________________________________\n");
	dut->can[channelNbr]->printStatusRegister();
	dut->can[channelNbr]->printControlRegister();
	// check SPI access
	printf("Testing the SPI access:\n");
	int errNbr = testSpiRead32(channelNbr);
	if (errNbr == 0) {
		checkSpiErrorBit(dut->can[channelNbr], 1);
	}
}

static const char* nominalBaudRates[] = { "10k","20k","50k","100k","125k","250k","500k","800k","1000k" }; // baud rate strings
static const char* dataBaudRates[] = { "500k","1000k","2000k","4000k","8000k" }; // baud rate strings

int CanFdTest::mainTest(int brdNbr, bool executeLoopback)
{
	int loopCntr = 0;
	int totalError = 0;
	bool runProgram = true;
	char c;
	int i;
	char buff[20];
	time_t ts;
	uint32_t id;
	char line[256];

	dut->setVerbose(1);
	PCIeMini_status st = dut->open(brdNbr);

	std::cout << "Opening the PCIeMini_CAN-FD: " << getAlphiErrorMsg(st) << endl;
	if (st != ERRCODE_NO_ERROR) {
		std::cout << "Exiting: Press <Enter> to exit." << endl;
		getc(stdin);
		exit(0);
	}

	id = dut->getFpgaID();
	std::cout << "FPGA ID: 0x0" << hex << id << endl;
	if (id == 0xffffffff || id == 0) {
		std::cout << "Cannot read the board!" << endl;
		std::cout << "Exiting: Press <Enter> to exit." << endl;
		getc(stdin);
		exit(0);
	}

	ts = dut->getFpgaTimeStamp();
	std::cout << "FPGA Time Stamp: 0x" << ts << endl;
	strftime(buff, 20, "%m/%d/%Y %H:%M:%S", localtime(&ts));
	printf("%s\n", buff);

	std::cout << endl << "---------------------------------" << endl;
	std::cout << "PCIeMini-CAN_FD Confidence Test" << endl;
	std::cout << "Using board number " << brdNbr << endl;
	if (verbose) std::cout << "verbose mode " << endl;
	std::cout << "---------------------------------" << endl;

	dut->reset();
	Sleep(100);

	// initialize the CAN chips
	for (int i = 0; i < dut->nbrOfCanInterfaces; i++) {
		Init_CAN(dut->can[i], verbose);
		dut->input0->resetIrq();
		if (i == 3) {
			dut->can[i]->setCanTermination(true);
			if (verbose) printf("Termination is on!\n");
		}
		else {
			dut->can[i]->setCanTermination(false);
			if (verbose) printf("Termination is off!\n");
		}
	}
	staticIrqTest();
	testTiLib(3000);
	printf("press <enter> for command menu\n");
	while (runProgram)
	{
		// check for user input
		if (_kbhit() || executeLoopback) {
			// starts the loopback test as needed
			if (executeLoopback) {
				executeLoopback = false;
				c = '6';
			}
			else {
				fgets(line, sizeof line, stdin); // remove the new line

				// enter new message
				printf("Command input mode: \n");
				printf("0: send message\n");
				printf("1: cyclical messages\n");
				printf("2: on-line the board\n");
				printf("3: get status\n");
				printf("4: set baud rate\n");
				//			printf("5: wipe firmware\n");
				printf("6: quickTest\n");
				printf("t: update terminations\n");
				printf("v: toggle verbose mode\n");
				printf("x: exit the application\n");
				printf("any other character to display received messages\n");
				printf("Enter command: >");
			    while(fgets(line, sizeof line, stdin) == NULL); 		// skip the newline
			    c = line[0];
			}
			switch (c) {
				// exit the program
			case 'x':
			case 'X':
				printf("exiting program and clearing cyclic messages...\n");
				runProgram = FALSE;
			case 'v':
			case 'V':
				verbose = !verbose;
				dut->verbose = verbose;
				printf("verbose is now %s\n", verbose ? "on" : "off");
				break;
			case 't':
			case 'T':
				for (int chnNbr = 0; chnNbr < dut->nbrOfCanInterfaces; chnNbr++) {
					printf("Channel #%d termination is %s.\n", chnNbr, dut->can[chnNbr]->isCanTerminationEnabled()?"on":"off");
				}
				printf("Enter the channel number you want to change or return to skip ?\n");
			    while(fgets(line, sizeof line, stdin) == NULL); 		// skip the newline
			    c = line[0];
				printf("%c\n", c);
				i = c - '0';
				if (i < 0 || i > dut->nbrOfCanInterfaces) {
					break;
				}
				else {
					dut->can[i]->setCanTermination(!dut->can[i]->isCanTerminationEnabled());
				}
				for (int chnNbr = 0; chnNbr < dut->nbrOfCanInterfaces; chnNbr++) {
					printf("Channel #%d termination is %s.\n", chnNbr, dut->can[chnNbr]->isCanTerminationEnabled() ? "on" : "off");
				}
				break;
			case '1':		 // remove all cyclic commands
				for (int chnNbr = 0; chnNbr < dut->nbrOfCanInterfaces; chnNbr++) {
					if (cyclical[chnNbr] == 0) continue;
					printf("Channel #%d every %d ms\n", chnNbr, cyclical[chnNbr]);
				}
				printf("Do you want to remove the cyclical messages ? ( y/N)\n");
			    while(fgets(line, sizeof line, stdin) == NULL); 		// skip the newline
			    c = line[0];
				if (c == 'y' || c == 'Y') {
					for (int chnNbr = 0; chnNbr < dut->nbrOfCanInterfaces; chnNbr++) {
						cyclical[chnNbr] = 0;
					}
				}

				break; 
/*
			case '2':
				errorCode = CAN_onLine(&canDevice);
				break;
*/
			case '3':
//				errorCode = CAN_status(&canDevice);
				printf("Enter the channel number you want to display: ");
			    while(fgets(line, sizeof line, stdin) == NULL); 		// skip the newline
			    c = line[0];
				printf("%c\n", c);
				i = c - '0';
				id = dut->getFpgaID();
				std::cout << "FPGA ID: 0x0" << hex << id << endl;
				if (id == 0xffffffff || id == 0) {
					std::cout << "Cannot read the board!" << endl;
					std::cout << "Exiting: Press <Enter> to exit." << endl;
					getc(stdin);
					exit(0);
				}

				ts = dut->getFpgaTimeStamp();
				std::cout << "FPGA Time Stamp: 0x" << ts << endl;
				char buff[20];
				strftime(buff, 20, "%m/%d/%Y %H:%M:%S", localtime(&ts));
				printf("%s\n", buff);
				if (i < 0 || i > dut->nbrOfCanInterfaces) {
					for (int chnNbr = 0; chnNbr < dut->nbrOfCanInterfaces; chnNbr++) {
						printChannelStatus(chnNbr);
					}
				}
				else {
					printChannelStatus(i);
				}
				break;
			case '4':
				for (int i = 0; i < NOMINAL_RATE_PRESETS; i++)
					printf("%d: %s\n", i, nominalBaudRates[i]);
				printf("Please enter the number of the nominal baud rate you want:\n");
				while (fgets(line, sizeof line, stdin) == NULL);
				sscanf(line, "%d", &i);
				nominalSpeed = (MCAN_Nominal_Speed)i;

				for (int i = 0; i < DATA_RATE_PRESETS; i++)
					printf("%d: %s\n", i, dataBaudRates[i]);
				printf("Please enter the number of the data baud rate you want:\n");
				while (fgets(line, sizeof line, stdin) == NULL);
				sscanf(line, "%d", &i);
				dataSpeed = (MCAN_Data_Speed)i;

				printf("Do you want to allow CAN-FD ? ( y/N)\n");
			    while(fgets(line, sizeof line, stdin) == NULL); 		// skip the newline
			    c = line[0];
				if (c == 'y' || c == 'Y') {
					isCanFd = true;
				}
				else {
					isCanFd = false;
				}

				for (int i = 0; i < dut->nbrOfCanInterfaces; i++) {
					Init_CAN(dut->can[i]);
				}

				break;
				// run the default test messages
			case '6':
				printf("Starting the loopback test. The screen should be updated every 10 seconds or less\n");
				printf("Press any key to exit:\n");
				totalError = 0;
				loopCntr = 0;
				while (!_kbhit()) {
					loopCntr++;
					if (testTiLib(10000)) {
						totalError++;
						ts = time(&ts);
						strftime(buff, 20, "%m/%d/%Y %H:%M:%S", localtime(&ts));
						printf("%s - ", buff);

						printf("Test #%d failed: %d failure%s out of %d loops\n", loopCntr, totalError, (totalError<2)?"":"s", loopCntr);
					}
					else {
						ts = time(&ts);
						strftime(buff, 20, "%m/%d/%Y %H:%M:%S", localtime(&ts));
						printf("%s - ", buff);
						printf("Test #%d passed: %d failure%s out of %d loops\n", loopCntr, totalError, (totalError < 2) ? "" : "s", loopCntr);
					}
				}
				break;

				// enter new message
			case '0':
				sendCanMesg();
				break;
			case 'P':
			case 'p':
				testPCIeSpeed();
				break;
			}
		}
		Sleep(1);
		// check for incoming messages
		uint64_t now = GetTickCount64();
//		printf("now %ld\n",now);
		for (int chnNbr = 0; chnNbr < dut->nbrOfCanInterfaces; chnNbr++) {
			TCAN4550* can = dut->can[chnNbr];
			uint8_t numBytes = 0;

			while (!isRxFifo0Empty(can->can)) {
				uint8_t dataPayload[64] = { 0 };
				TCAN4x5x_MCAN_RX_Header MsgHeader = { 0 };		// Initialize to 0 or you'll get garbage
				numBytes = can->MCAN_ReadNextFIFO(RXFIFO0, &MsgHeader, dataPayload);	// This will read the next element in the RX FIFO 0
				printf("Channel #%d: ", chnNbr);
				printRxMsg(&MsgHeader, numBytes, dataPayload);
			}
			// check for outgoing messages
			if (cyclical[chnNbr] == 0) continue;
			if (now >= lastMsgTs[chnNbr] + cyclical[chnNbr]) {
				dut->can[chnNbr]->can->AHB_WRITE_32(REG_MCAN_TXBAR, 1);
				lastMsgTs[chnNbr] = now;
				printf("Msg sent channel %d\n", chnNbr);
			}
		}
	}

	dut->disableInterrupts();
	dut->close();

	return 0;
	 
}
