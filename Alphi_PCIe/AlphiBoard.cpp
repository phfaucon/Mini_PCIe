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
/** @file AlphiBoard.cpp
* @brief Implementation of the base PCIe board class using Linux UIO
*/
// Maintenance Log
//---------------------------------------------------------------------
// v1.0		7/23/2020	phf	Written
//---------------------------------------------------------------------

//#include "utils.h"
#include "AlphiErrorCodes.h"
#include "AlphiBoard.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <iostream>

using namespace std;

/*************************************************************
Internal definitions
*************************************************************/

/** @brief Constructor for the AlphiBoard object
 *
 * This mostly reset everything. Actual values will be filled up when the board is opened.
 * @param vendorId PCI vendor identification. Alphi is 0x13c5.
 * @param deviceId PCI device identification. The PCIe-Mini all use 0x0508.
 */
AlphiBoard::AlphiBoard(uint16_t vendorId, uint16_t deviceId)
{
	dwVendorId = vendorId;
	dwDeviceId = deviceId; 
	/* Initialize the minipcie_arinc429 library */
	verbose = false;

	bar0.Address = NULL;
	bar0.Length = 0;
	bar2.Address = NULL;
	bar2.Length = 0;

	brdNumber = 0;
	brd_valid = false;
	cra = NULL;
	sysid = NULL;
	intHandlerUserData = NULL;
	funcDiagIntHandler = NULL;
	irqThread = 0;

	resfd2 = -1;
	resfd0 = -1;
	configfd = -1;
	uiofd = -1;
}

//! Destructor
/*!
	Will close the connection to the board if needed.
*/
AlphiBoard::~AlphiBoard(void)
{
	Close();
}

/** @brief Utility function to read an UIO config file containing a string
 *
 * @retval return an error number or 0.
 */
int AlphiBoard::readConfigString(char *data, int *len, const char *name)
{
	int infoFn;
	int l;
	char filename[100];

	sprintf(filename, "/sys/class/uio/uio%d/%s", brdNumber, name);
	infoFn = open(filename, O_RDWR);
	if (infoFn < 0) {
		perror("Cannot get uio dev info:");
		return errno;
	}
	l = read(infoFn, data, *len - 1);
	data[l] = 0;
	*len = l;
	return close(infoFn);
}

int AlphiBoard::readConfigHex(const char *name)
{
	int infoFn;
	int l;
	char filename[100];
	char dataString[100];
	int data;

	sprintf(filename, "/sys/class/uio/uio%d/%s", brdNumber, name);
	infoFn = open(filename, O_RDWR);
	if (infoFn < 0) {
		perror("Cannot get uio dev info:");
		return errno;
	}
	l = read(infoFn, dataString, sizeof(dataString) - 1);
	dataString[l] = 0;
	sscanf(dataString, "%x", &data);
	close(infoFn);
	return data;
}

int AlphiBoard::readUioResource()
{
	char filename[100];
	int nbrBarsRead = 0;
	uint64_t startAddr, endAddr, other;

	sprintf(filename, "/sys/class/uio/uio%d/device/resource", brdNumber);
	FILE *fp = fopen (filename, "r");
	if (fp == NULL) {
		perror("Cannot get uio dev info:");
		return nbrBarsRead;
	}

	for (int i=0; i<nbrOfBars; i++)
	{
		fscanf(fp,"%lx %lx %lx\n", &startAddr, &endAddr, &other);
		if (startAddr != 0) {
			uioBarSizes[i] = endAddr - startAddr + 1;
			nbrBarsRead++;
		}
		else
			uioBarSizes[i] = 0;
	}
	fclose(fp);
	return nbrBarsRead;
}

/** @brief Open a board using the UIO interface
 *
Establishes a connection to a board.
	@param brdNbr the board index to open.
	@return ERRCODE_INVALID_BOARD_NUM if there is no board corresponding to the number
*/
PCIeMini_status AlphiBoard::Open(int board_num)
{
	off_t addr;
//	int resfd3;
	void* bar0addr;
	void* bar2addr;
//	void* bar3;
	char filename[100];
	int i;

	brdNumber = board_num;

	// Try to open the basic files
	sprintf(filename, "/dev/uio%d", board_num);
 	printf("\n");
	uiofd = open(filename, O_RDWR);
	if (uiofd < 0) {
		perror("dev uio open:");
		return ERRCODE_INVALID_BOARD_NUM;
	}

	sprintf(filename, "/sys/class/uio/uio%d/device/config", board_num);
	configfd = open(filename, O_RDWR);
	if (configfd < 0) {
		perror("config open:");
		return ERRCODE_INVALID_BOARD_NUM;
	}

	sprintf(filename, "/sys/class/uio/uio%d/device/resource0", board_num);
	resfd0 = open(filename, O_RDWR);
	if (resfd0 < 0) {
		perror("sys resourse 0 open:");
		return ERRCODE_INVALID_BOARD_NUM;
	}

	sprintf(filename, "/sys/class/uio/uio%d/device/resource2", board_num);
	resfd2 = open(filename, O_RDWR);
	if (resfd2 < 0) {
		perror("sys resourse 2 open:");
		return ERRCODE_INVALID_BOARD_NUM;
	}

	// Get board information from UIO
	int l = sizeof(uioDev);
	readConfigString(uioDev, &l, "dev");
	l = sizeof(uioName);
	readConfigString(uioName, &l, "name");
	l = sizeof(uioVersion);
	readConfigString(uioVersion, &l, "version");
	l = sizeof(filename);

	l = readUioResource();
	printf("%d BARs found: Bar #0 size = 0x%lx, Bar #2 size = 0x%lx\n", l, uioBarSizes[0], uioBarSizes[2]);

	i = readConfigHex("device/vendor");
	if ((uint16_t)i != dwVendorId) {
		fprintf(stderr, "Invalid PCI manufacturer ID: 0x%04x (should be 0x%04x)\n", i, dwVendorId);
		return ERRCODE_INVALID_BOARD_NUM;
	}

	i = readConfigHex("device/device");
	if ((uint16_t)i != dwDeviceId) {
		fprintf(stderr, "Invalid PCI device ID: 0x%04x (should be 0x%04x)\n", i, dwDeviceId);
		return ERRCODE_INVALID_BOARD_NUM;
	}
	if (verbose) {
		printf("Manufacturer: 0x%04x, Device: 0x%04x\n", dwVendorId, dwDeviceId);
	}

	// Map the board registers into user space
	/* The parameter offset of the mmap() call has a special meaning for UIO devices: It is used to select
	 * which mapping of your device you want to map. To map the memory of mapping N, you have to use N times
	 * the page size as your offset:
	 * offset = N * getpagesize();
	 * N starts from zero, so if you’ve got only one memory range to map, set offset = 0.
	 * */
	addr = (0 * getpagesize()); // leave page offset right here

	bar0addr = mmap(NULL, uioBarSizes[0], PROT_READ|PROT_WRITE, MAP_SHARED, resfd0, addr);
	printf("bar0 mapped %p length 0x%04lx\n", bar0addr, uioBarSizes[0]);
	bar0.Address = bar0addr;
	bar0.Length = uioBarSizes[0];

	bar2addr = mmap(NULL, uioBarSizes[2], PROT_READ|PROT_WRITE, MAP_SHARED, resfd2, addr);
	printf("bar2 mapped %p length 0x%04lx\n", bar2addr, uioBarSizes[0]);
	bar2.Address = bar2addr;
	bar2.Length = uioBarSizes[2];

	brd_valid = true;

	// create the device objects
	sysid = new BoardVersion((volatile uint32_t*)getBar2Address(sysid_offset));
	cra = new PcieCra(getBar0Address(cra_address));

	// initialize interrupts
	unhookInterruptServiceRoutine();
	startIntThread();

	return ERRCODE_SUCCESS;
}

//! Get the FPGA ID of
/*!
		@return  The FPGA ID.
*/
uint32_t AlphiBoard::getFpgaID()
{
	return sysid->getVersion();
}

//! Return the timestamp corresponding to when the FPGA was compiled
/*!
		@return  a timestamp.
*/
time_t AlphiBoard::getFpgaTimeStamp()
{
	return sysid->getTimeStamp();
}

/** @brief set the verbose flag
 *
 * The verbose value is used to send more information to the log file or console. It is only partially
 * implemented.
 @param vb Verbosity level.
 */
void AlphiBoard::setVerbose(int vb)
{
	verbose = vb;
}


/** @brief Close a device handle 
	@return ERRCODE_INVALID_HANDLE if board is not opened, ERRCODE_NO_ERROR if successful.
*/
PCIeMini_status AlphiBoard::Close()
{
	if (brd_valid == false)
		return ERRCODE_INVALID_HANDLE;

	brd_valid = false;

	// unmap the bars
	munmap(bar0.Address, 0x4000);
	bar0.Address = NULL;
	close(resfd0);
	resfd0 = -1;

	munmap(bar2.Address, 0x1000);
	bar2.Address = NULL;
	close(resfd2);
	resfd2 = -1;

	close(configfd);
	close(uiofd);

	return ERRCODE_NO_ERROR;
}

/** @brief Return a pointer to an object in BAR 0
 *
 * @param offset Offset in BAR0
 * @retval Pointer to the object
 */
volatile void* AlphiBoard::getBar0Address(size_t offset)
{
	if (offset >= bar0.Length) return NULL;

	return (void*)((char*)bar0.Address + offset);
}

/** @brief Return a pointer to an object in BAR 2
 *
 * @param offset Offset in BAR2
 * @retval Pointer to the object
 */
volatile void* AlphiBoard::getBar2Address(size_t offset)
{
	if (offset >= bar2.Length) return NULL;

	return (void*)((char*)bar2.Address + offset);
}

/** @brief Return a pointer to an object in BAR 3
 *
 * BAR3 is used on a few board to locate dual-ported RAM.
 * @param offset Offset in BAR3
 * @retval Pointer to the object
 */
volatile void* AlphiBoard::getBar3Address(size_t offset)
{
	if (offset >= bar3.Length) return NULL;

	return (void*)((char*)bar3.Address + offset);
}

/** @brief constructor
 *
 * This constructor reads the chip register to initialize the data. It is called by the open and should not be called by the user.
 * @param addr Offset to the sysid controller in the BAR2 address space
 */
BoardVersion::BoardVersion(volatile uint32_t* addr)
{
	address = addr;
}

/** @brief Return the board type
* 
*/
uint32_t BoardVersion::getVersion()		
{
	return address[0];
}

/** @brief Return FPGA time stamp
*
* Date and time when the board firmware was compiled, it can be used to identify the version of the hardware.
*/
time_t BoardVersion::getTimeStamp()		
{
	return address[1];
}

#include <termios.h>
#include <sys/select.h>

static void enable_raw_mode(int *oldf, bool blocking = false)
{
    termios term;

    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    *oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (blocking)
    	fcntl(STDIN_FILENO, F_SETFL, *oldf);
    else
    	fcntl(STDIN_FILENO, F_SETFL, *oldf | O_NONBLOCK);
}

void disable_raw_mode(int *oldf)
{
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
}

char _getch()
{
	int oldf;

	enable_raw_mode(&oldf);
	char c = getchar();
	disable_raw_mode(&oldf);

	return c;
}

char getche()
{
	int oldf;

	enable_raw_mode(&oldf, true);
	char c = getchar();
	disable_raw_mode(&oldf);

	return c;
}

bool _kbhit()
{
    int byteswaiting;
	int oldf;

	enable_raw_mode(&oldf);
    ioctl(STDIN_FILENO, FIONREAD, &byteswaiting);
	disable_raw_mode(&oldf);

    return byteswaiting > 0;
}

