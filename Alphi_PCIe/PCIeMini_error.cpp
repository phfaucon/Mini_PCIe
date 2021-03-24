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
/** @file PCIeMini_error.cpp
* @brief Error code to string conversion
*/
// Maintenance Log
//---------------------------------------------------------------------
// v1.0		7/23/2020	phf	Written
//---------------------------------------------------------------------
#include "stdio.h" 
#include <string.h>
#include "AlphiErrorCodes.h" 
//char* wdErrorToString(PCIeMini_status errCode);

/** @brief Gives the string description of an error code
 *
 * @param errCode Error code returned by a function
 * @retval C-string, description of the error
 */
DLL char * getAlphiErrorMsg(PCIeMini_status errCode)
{
	static char ErrMsg[256];
	switch (errCode) {
	case  ERRCODE_NO_ERROR:
		strcpy(ErrMsg, "No error.");
		break;
	case  ERRCODE_INTERNAL_ERROR:
		strcpy(ErrMsg, "Internal error.");
		break;
	case  ERRCODE_INVALID_BOARD_NUM:
		strcpy(ErrMsg, "Invalid board number.");
		break;
	case  ERRCODE_INVALID_HANDLE:
		strcpy(ErrMsg, "Invalid handle.");
		break;
	case  ERRCODE_INVALID_CHANNEL_NUM:
		strcpy(ErrMsg, "Invalid channel number.");
		break;
	case  ERRCODE_INVALID_SELF_TEST_ENABLE_VAL:
		strcpy(ErrMsg, "Invalid self-test-enable value.");
		break;
	case  ERRCODE_INVALID_VALUE:
		strcpy(ErrMsg, "Invalid value.");
		break;
	case  ERRCODE_INPUT_MODE:
		strcpy(ErrMsg, "Input mode error.");
		break;
	case  ERRCODE_OUTPUT_MODE:
		strcpy(ErrMsg, "Output mode error.");
		break;
	case  ERRCODE_INVALID_LOGIC_SEL:
		strcpy(ErrMsg, "Invalid logic selection.");
		break;
	case  ERRCODE_INVALID_STROBE_MODE:
		strcpy(ErrMsg, "Invalid strobe mode.");
		break;
	case  ERRCODE_INVALID_GROUP:
		strcpy(ErrMsg, "Invalid group.");
		break;
	case  ERRCODE_INVALID_FREQUENCY:
		strcpy(ErrMsg, "Invalid frequency.");
		break;
	case  ERRCODE_INVALID_INPUT_MODE:
		strcpy(ErrMsg, "Invalid input mode.");
		break;
	case  ERRCODE_INVALID_MASK_VALUE:
		strcpy(ErrMsg, "Invalid mask value.");
		break;
	case  ERRCODE_INVALID_MODE:
		strcpy(ErrMsg, "Invalid mode.");
		break;
	case  ERRCODE_INVALID_SELF_TEST_DATA:	
		strcpy(ErrMsg, "Invalid self-test data.");
		break;
	case  ERRCODE_SELF_TEST_DISABLE:
		strcpy(ErrMsg, "Self test disabled.");
		break;
	case  ERRCODE_FAILED_SELF_TEST:
		strcpy(ErrMsg, "Failed self test.");
		break;
	case  ERRCODE_INVALID_TIME_BOUNCE_VAL:	
		strcpy(ErrMsg, "Invalid time bounce value.");
		break;
	case  ERRCODE_INT_ALREADY_ENABLED:
		strcpy(ErrMsg, "Interrupt already enabled.");
		break;
	case  ERRCODE_INT_NOT_ENABLED:
		strcpy(ErrMsg, "Interrupt not enabled.");
		break;
	case  ERRCODE_INVALID_TRANRECVSTS:
		strcpy(ErrMsg, "Invalid sts value for the clear FIFO.");
		break;
	case  ERRCODE_INVALID_TRANSNUMBER:
		strcpy(ErrMsg, "Invalid transmitter number.");
		break;
	case  ERRCODE_INVALID_RECVNUMBER:
		strcpy(ErrMsg, "Invalid receiver number.");
		break;
	case  ERRCODE_INVALID_TRPAIR:
		strcpy(ErrMsg, "Invalid transmitter receiver pair.");
		break;
	case  ERRCODE_INVALID_TRANSSTATUS:
		strcpy(ErrMsg, "Invalid transmitter status.");
		break;
	case  ERRCODE_INVALID_LABELNUMBER:
		strcpy(ErrMsg, "Invalid label number.");
		break;
	case  ERRCODE_INVALID_SDI:
		strcpy(ErrMsg, "Invalid sdi value.");
		break;
	case  ERRCODE_INVALID_PARITY:
		strcpy(ErrMsg, "Invalid parity.");
		break;
	case  ERRCODE_INVALID_DATARATE:
		strcpy(ErrMsg, "Invalid data rate.");
		break;
	case  ERRCODE_INVALID_DATALENGTH:
		strcpy(ErrMsg, "Invalid data length.");
		break;
	case  ERRCODE_INVALID_SSM:
		strcpy(ErrMsg, "Invalid SSM.");
		break;
	case  ERRCODE_INVALID_COMMAND:
		strcpy(ErrMsg, "The command is not valid.");
		break;
	case  ERRCODE_RX_UNDERFLOW:
		strcpy(ErrMsg, "There was no data in the Rx FIFO.");
		break;
	case ERRCODE_TX_OVERFLOW:
		strcpy(ErrMsg, "Tx FIFO overflow.");
		break;
	case ERRCODE_INVALID_ALIGNMENT:
		strcpy(ErrMsg, "Invalid alignment.");
		break;
	default:
//		char* wdErr = wdErrorToString(errCode);
//		if (wdErr != NULL) return wdErr;
		sprintf(ErrMsg, "Unknown error 0x%08x.", errCode);
	}
	return ErrMsg;
}

#if 0
char *wdErrorToString(PCIeMini_status errCode)
{
	static char ErrMsg[256];
	switch (errCode) {
	case  WD_WINDRIVER_STATUS_ERROR:
		strcpy(ErrMsg, "WinDriver Status Error.");
		break;
	case  WD_INVALID_HANDLE:
		strcpy(ErrMsg, "WinDriver Invalid Handle.");
		break;
	case  WD_INVALID_PIPE_NUMBER:
		strcpy(ErrMsg, "WinDriver Status Error.");
		break;
	case  WD_READ_WRITE_CONFLICT:
		strcpy(ErrMsg, "WinDriver Write Conflict.");
		break;
	case  WD_ZERO_PACKET_SIZE:
		strcpy(ErrMsg, "WinDriver Zero Packet Size.");
		break;
	case  WD_INSUFFICIENT_RESOURCES:
		strcpy(ErrMsg, "WinDriver Insufficient Resources.");
		break;
	case  WD_UNKNOWN_PIPE_TYPE:
		strcpy(ErrMsg, "WinDriver Unknown Pipe Type.");
		break;
	case  WD_SYSTEM_INTERNAL_ERROR:
		strcpy(ErrMsg, "WinDriver System Internal Error.");
		break;
	case  WD_DATA_MISMATCH:
		strcpy(ErrMsg, "WinDriver Data Mismatch.");
		break;
	case  WD_NO_LICENSE:
		strcpy(ErrMsg, "WinDriver No License.");
		break;
	case  WD_NOT_IMPLEMENTED:
		strcpy(ErrMsg, "WinDriver Not Implemented.");
		break;
	case  WD_KERPLUG_FAILURE:
		strcpy(ErrMsg, "WinDriver Kerplug failure.");
		break;
	case  WD_FAILED_ENABLING_INTERRUPT:
		strcpy(ErrMsg, "WinDriver Failed Enabling Interrupt.");
		break;
	case  WD_INTERRUPT_NOT_ENABLED:
		strcpy(ErrMsg, "WinDriver Interrupt not Enabled.");
		break;
	case  WD_RESOURCE_OVERLAP:
		strcpy(ErrMsg, "WinDriver Resource Overlap.");
		break;
	case  WD_DEVICE_NOT_FOUND:
		strcpy(ErrMsg, "WinDriver Device Not Found.");
		break;
	case  WD_WRONG_UNIQUE_ID:
		strcpy(ErrMsg, "WinDriver Wrong Unique ID.");
		break;
	case  WD_OPERATION_ALREADY_DONE:
		strcpy(ErrMsg, "WinDriver Operation Already Done.");
		break;
	case  WD_USB_DESCRIPTOR_ERROR:
		strcpy(ErrMsg, "WinDriver USB Descriptor Error.");
		break;
	case  WD_SET_CONFIGURATION_FAILED:
		strcpy(ErrMsg, "WinDriver Set Configuration Failed.");
		break;
	case  WD_CANT_OBTAIN_PDO:
		strcpy(ErrMsg, "WinDriver WD_CANT_OBTAIN_PDO Error.");
		break;
	case  WD_TIME_OUT_EXPIRED:
		strcpy(ErrMsg, "WinDriver WD_TIME_OUT_EXPIRED Error.");
		break;
	case  WD_IRP_CANCELED:
		strcpy(ErrMsg, "WinDriver WD_IRP_CANCELED Error.");
		break;
	case  WD_FAILED_USER_MAPPING:
		strcpy(ErrMsg, "WinDriver WD_FAILED_USER_MAPPING Error.");
		break;
	case  WD_FAILED_KERNEL_MAPPING:
		strcpy(ErrMsg, "WinDriver WD_FAILED_KERNEL_MAPPING Error.");
		break;
	case  WD_NO_RESOURCES_ON_DEVICE:
		strcpy(ErrMsg, "WinDriver WD_NO_RESOURCES_ON_DEVICE Error.");
		break;
	case  WD_NO_EVENTS:
		strcpy(ErrMsg, "WinDriver WD_NO_EVENTS Error.");
		break;
	case  WD_INVALID_PARAMETER:
		strcpy(ErrMsg, "WinDriver WD_INVALID_PARAMETER Error.");
		break;
	case  WD_INCORRECT_VERSION:
		strcpy(ErrMsg, "WinDriver WD_INCORRECT_VERSION Error.");
		break;
	case  WD_TRY_AGAIN:
		strcpy(ErrMsg, "WinDriver WD_TRY_AGAIN Error.");
		break;
	case  WD_WINDRIVER_NOT_FOUND:
		strcpy(ErrMsg, "WinDriver WD_WINDRIVER_NOT_FOUND Error.");
		break;
	case  WD_INVALID_IOCTL:
		strcpy(ErrMsg, "WinDriver WD_INVALID_IOCTL Error.");
		break;
	case  WD_OPERATION_FAILED:
		strcpy(ErrMsg, "WinDriver WD_OPERATION_FAILED Error.");
		break;
	case  WD_INVALID_32BIT_APP:
		strcpy(ErrMsg, "WinDriver WD_INVALID_32BIT_APP Error.");
		break;
	case  WD_TOO_MANY_HANDLES:
		strcpy(ErrMsg, "WinDriver WD_TOO_MANY_HANDLES Error.");
		break;
	case  WD_NO_DEVICE_OBJECT:
		strcpy(ErrMsg, "WinDriver WD_NO_DEVICE_OBJECT Error.");
		break;
	default:
		return NULL;
	}
	return ErrMsg;
}
#endif

