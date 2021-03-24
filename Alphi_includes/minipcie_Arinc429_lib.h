#ifndef _MINIPCIE_LIB_H_
#define _MINIPCIE_LIB_H_

/*****************************************************************************
*  File: minipcie_arinc429_lib.h
*
*  Header file of a sample library for accessing Altera PCI Express cards
*  with Qsys design, using the WinDriver WDC_API.
*  The sample was tested with Altera's Stratix IV GX development kit.
*  For more information on the Qsys design, refer to Altera's
*  "PCI Express in Qsys Example Designs" wiki page:
*  http://alterawiki.com/wiki/PCI_Express_in_Qsys_Example_Designs
*  You can download the WinDriver Development kit from here:
*  http://www.jungo.com/st/do/download_new.php?product=WinDriver&tag=GrandMenu
*
*  Jungo Connectivity Confidential. Copyright (c) 2015 Jungo Connectivity Ltd.
*  http://www.jungo.com
******************************************************************************/

#include <stdint.h>
#include <wdc_lib.h>
#include <pci_regs.h>
#include <bits.h>

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************
  General definitions
 *************************************************************/

/* Default vendor and device IDs (0 == all) */
#define MINIPCIE_ARINC429_DEFAULT_VENDOR_ID 0x13c5 /* Vendor ID */
#define MINIPCIE_ARINC429_DEFAULT_DEVICE_ID 0x508    /* All Altera devices */
  /* TODO: Change the device ID value to match your specific device. */

/* Qsys revision */
#define QSYS_REVISION               9

/** @brief Interrupt result information structure */
typedef struct
{
    DWORD dwCounter; ///< Number of interrupts received 
    DWORD dwLost;    ///< Number of interrupts not yet handled 
    WD_INTERRUPT_WAIT_RESULT waitResult; ///< See WD_INTERRUPT_WAIT_RESULT values in windrvr.h
    DWORD dwEnabledIntType; ///< Interrupt type that was actually enabled (MSI/MSI-X/Level Sensitive/Edge-Triggered) 
    DWORD dwLastMessage; /**< Message data of the last received MSI/MSI-X
                            (Windows Vista and higher); N/A to line-based
                            interrupts) */
} MINIPCIE_INT_RESULT;
/* TODO: You can add fields to MINIPCIE_INT_RESULT to store any additional
         information that you wish to pass to your diagnostics interrupt
         handler routine (DiagIntHandler() in minipcie_arinc429_diag.c). */

/* minipcie_arinc429 diagnostics interrupt handler function type */
typedef void (*MINIPCIE_INT_HANDLER)(WDC_DEVICE_HANDLE hDev,
    void *pIntResult);

/* minipcie_arinc429 diagnostics plug-and-play and power management events handler function
 * type */
typedef void (*MINIPCIE_EVENT_HANDLER)(WDC_DEVICE_HANDLE hDev,
    DWORD dwAction);

/*************************************************************
  Function prototypes
 *************************************************************/
/* -----------------------------------------------
    minipcie_arinc429 and WDC libraries initialize/uninitialize
   ----------------------------------------------- */
/* Initialize the Altera minipcie_arinc429 and WDC libraries */
DWORD MINIPCIE_ARINC429_LibInit(void);
/* Uninitialize the Altera minipcie_arinc429 and WDC libraries */
DWORD MINIPCIE_ARINC429_LibUninit(void);

/* -----------------------------------------------
    Device open/close
   ----------------------------------------------- */
/* Open a device handle */
WDC_DEVICE_HANDLE MINIPCIE_ARINC429_DeviceOpen(WD_PCI_CARD_INFO *pDeviceInfo,
    char *kp_name);
/* Close a device handle */
BOOL MINIPCIE_ARINC429_DeviceClose(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Interrupts
   ----------------------------------------------- */
/* Enable interrupts */
DWORD MINIPCIE_ARINC429_IntEnable(WDC_DEVICE_HANDLE hDev,
    MINIPCIE_INT_HANDLER funcIntHandler);
/* Disable interrupts */
DWORD MINIPCIE_ARINC429_IntDisable(WDC_DEVICE_HANDLE hDev);
/* Check whether interrupts are enabled for the given device */
BOOL MINIPCIE_ARINC429_IntIsEnabled(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Plug-and-play and power management events
   ----------------------------------------------- */
/* Register a plug-and-play or power management event */
DWORD MINIPCIE_ARINC429_EventRegister(WDC_DEVICE_HANDLE hDev,
    MINIPCIE_EVENT_HANDLER funcEventHandler);
/* Unregister a plug-and-play or power management event */
DWORD MINIPCIE_ARINC429_EventUnregister(WDC_DEVICE_HANDLE hDev);
/* Check whether a given plug-and-play or power management event is registered
 */
BOOL MINIPCIE_ARINC429_EventIsRegistered(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Qsys Revision Verification
   ----------------------------------------------- */
/* Verify Qsys revision */
BOOL MINIPCIE_ARINC429_IsQsysRevision(WDC_DEVICE_HANDLE hDev);

/* -----------------------------------------------
    Read register
   ----------------------------------------------- */
/**
 * Function name:  MINIPCIE_ARINC429_ReadReg8 / MINIPCIE_ARINC429_ReadReg16 / MINIPCIE_ARINC429_ReadReg32
 * Description:    Read from a 8/16/32-bit register
 * Parameters:
 *     @hDev:        (IN) Handle to the card, received from MINIPCIE_ARINC429_DeviceOpen()
 *     @dwAddrSpace: (IN) Address space containing the register
 *     @dwOffset:    (IN) Offset of the register within the address space
 * Return values:  Read data
 * Scope: Global
 **/
BYTE MINIPCIE_ARINC429_ReadReg8(WDC_DEVICE_HANDLE hDev, DWORD dwAddrSpace, DWORD dwOffset);
WORD MINIPCIE_ARINC429_ReadReg16(WDC_DEVICE_HANDLE hDev, DWORD dwAddrSpace, DWORD dwOffset);
uint32_t MINIPCIE_ARINC429_ReadReg32(WDC_DEVICE_HANDLE hDev, DWORD dwAddrSpace,
    DWORD dwOffset);

/* -----------------------------------------------
    Debugging and error handling
   ----------------------------------------------- */
/* Get last error */
const char *MINIPCIE_ARINC429_GetLastErr(void);

#ifdef __cplusplus
}
#endif

#endif /* _MINIPCIE_LIB_H_ */

