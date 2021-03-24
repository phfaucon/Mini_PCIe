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
/** @file TCAN4550.h
* @brief Description of the object connecting the Alphi hardware and the TI library
*/
#ifndef TCAN4550_H_
#define TCAN4550_H_
#include "eusci_b_spi.h"
#include "TCAN4x5x_SPI.h"
#include "TCAN4x5x_Reg.h"
#include "TCAN4x5x_Data_Structs.h"
#include "AlteraPio.h"
#include "ParallelInput.h"
#include "TcanInterface.h"

//! If TCAN4x5x_MCAN_VERIFY_CONFIGURATION_WRITES is defined, then each MCAN configuration write will be read and verified for correctness
#define TCAN4x5x_MCAN_VERIFY_CONFIGURATION_WRITES

//! If TCAN4x5x_DEVICE_VERIFY_CONFIGURATION_WRITES is defined, then each device configuration write will be read and verified for correctness
#define TCAN4x5x_DEVICE_VERIFY_CONFIGURATION_WRITES

typedef enum { RXFIFO0, RXFIFO1 } TCAN4x5x_MCAN_FIFO_Enum;
typedef enum { TCAN4x5x_WDT_60MS, TCAN4x5x_WDT_600MS, TCAN4x5x_WDT_3S, TCAN4x5x_WDT_6S } TCAN4x5x_WDT_Timer_Enum;
typedef enum { TCAN4x5x_DEVICE_TEST_MODE_NORMAL, TCAN4x5x_DEVICE_TEST_MODE_PHY, TCAN4x5x_DEVICE_TEST_MODE_CONTROLLER } TCAN4x5x_Device_Test_Mode_Enum;
typedef enum { TCAN4x5x_DEVICE_MODE_NORMAL, TCAN4x5x_DEVICE_MODE_STANDBY, TCAN4x5x_DEVICE_MODE_SLEEP } TCAN4x5x_Device_Mode_Enum;

/** @brief TI CAN library
* 
*/
class DLL TCAN4550
{
public:
	// control register bit definition
	static const uint32_t ctrl_reset_mask = 0x001;			///< When "1", the TCAN is reset
//	static const uint32_t ctrl_nWAKE_mask = 0x002;			///< Not implemented
	static const uint32_t ctrl_TERM_EN_mask = 0x004;		///< When "1", the CAN terminations are disabled
	static const uint32_t ctrl_VBAT_EN_mask = 0x008;		///< When "1", the CAN uses the outside VBAT power supply.

	// status register bit definition
	static const uint32_t stat_int_n_mask = 0x001;			///< nINT output from the TCAN4550
	static const uint32_t stat_wake_n_mask = 0x002;			///< nWAKE output from the TCAN4550
	static const uint32_t stat_gpio1_mask = 0x004;			///< GPIO1 output from the TCAN4550
	static const uint32_t stat_gpo2_mask = 0x0008;			///< GPO2 output from the TCAN4550

/*	static const int BUFF_LEN = 256;
	uint8_t msgBufferOut[BUFF_LEN];
	uint8_t msgBufferIn[BUFF_LEN];
	int msgLength;*/
	TcanInterface* can;
	ParallelInput* status;
	uint8_t slaveNbr;

	inline TCAN4550(volatile void* addr, AlteraPio* ctrl, ParallelInput* stat, uint8_t slave)
	{
		can = new TcanInterface(addr, slave);
		controlReg = ctrl;
		slaveNbr = slave;

		status = stat;
//		reset();
//		status->base[status->polarity_index] = 0xffff;		// active low
//		status->base[status->edgeReg_Index] = 0;			// on level
	}

	/** @brief reset the TCAN4550 chip
	 */
	inline void reset()
	{
			uint32_t rst = ctrl_reset_mask;
			controlReg->setData(rst);
			usleep(2000);
			controlReg->setData(0);
			usleep(2000);
			can->reset();
			Device_ClearInterruptsAll();
	}

	/** @brief Get CAN termination state
	*
	* The method checks the termination bit in the TCAN control register. The termination is
	* enabled when the bit is set to 0.
	*
	* @retval When the CAN termination is enabled returns true, else false.
	*
	 */
	inline bool isCanTerminationEnabled()
	{
		uint32_t val = controlReg->getData() & ctrl_TERM_EN_mask;
		return val == 0;
	}

	/** @brief Set CAN termination on or off
	*
	* The method set or reset a bit in the TCAN control register. The termination is
	* enabled when the bit is set to 0.
	*
	* @param isOn Enable the CAN termination when true, disable it when false.
	*
	 */
	inline void setCanTermination(bool isOn)
	{
		uint32_t val = controlReg->getData();
		if (isOn) {
			controlReg->setData(val & (~ctrl_TERM_EN_mask));
		}
		else {
			controlReg->setData(val | ctrl_TERM_EN_mask);
		}
	}


	/** @brief enable the TCAN interrupt line to be routed to the PCIe interface.
	*/
	inline void enableIrq()
	{
		status->setIrqEnable(stat_int_n_mask);
	}

	/** @brief disable the TCAN interrupt line.
	*/
	inline void disableIrq()
	{
		status->setIrqDisable(stat_int_n_mask);
	}

	inline void printStatusRegister()
	{
		printf("TCAN status register: ");

		uint8_t val = status->getData();
		printf("%s, ", (val & stat_int_n_mask) ? "IRQ" : "NO IRQ");
		printf("%s, ", (val & stat_wake_n_mask) ? "WRQ" : "NO WRQ");
		printf("%s, ", (val & stat_gpio1_mask) ? "GPIO1" : "NO GPIO1");
		printf("%s\n", (val & stat_gpo2_mask) ? "GPO2" : "NO GPO2");
	}

	inline void printControlRegister()
	{
		printf("TCAN control register: ");

		uint8_t val = controlReg->getData();
		printf("%s, ", (val & ctrl_reset_mask) ? "Reset" : "Run");
		printf("%s, ", (val & ctrl_TERM_EN_mask) ? "Termination disabled" : "Termination enabled");
		printf("%s\n", (val & ctrl_VBAT_EN_mask) ? "Use VBAT" : "Normal power supply");
	}

	// ~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
	//                            MCAN Device Functions
	// ~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*

	bool MCAN_EnableProtectedRegisters(void);
	bool MCAN_DisableProtectedRegisters(void);
	bool MCAN_ConfigureCCCRRegister(TCAN4x5x_MCAN_CCCR_Config* cccr);
	void MCAN_ReadCCCRRegister(TCAN4x5x_MCAN_CCCR_Config* cccrConfig);
	void MCAN_ReadDataTimingFD_Simple(TCAN4x5x_MCAN_Data_Timing_Simple* dataTiming);
	void MCAN_ReadDataTimingFD_Raw(TCAN4x5x_MCAN_Data_Timing_Raw* dataTiming);
	bool MCAN_ConfigureDataTiming_Simple(TCAN4x5x_MCAN_Data_Timing_Simple* dataTiming);
	bool MCAN_ConfigureDataTiming_Raw(TCAN4x5x_MCAN_Data_Timing_Raw* dataTiming);
	void MCAN_ReadNominalTiming_Simple(TCAN4x5x_MCAN_Nominal_Timing_Simple* nomTiming);
	void MCAN_ReadNominalTiming_Raw(TCAN4x5x_MCAN_Nominal_Timing_Raw* nomTiming);
	bool MCAN_ConfigureNominalTiming_Simple(TCAN4x5x_MCAN_Nominal_Timing_Simple* nomTiming);
	bool MCAN_ConfigureNominalTiming_Raw(TCAN4x5x_MCAN_Nominal_Timing_Raw* nomTiming);


	bool MRAM_Configure(TCAN4x5x_MRAM_Config* MRAMConfig);
	void MRAM_Clear(void);
	void MCAN_ReadInterrupts(TCAN4x5x_MCAN_Interrupts* ir);
	void MCAN_ClearInterrupts(TCAN4x5x_MCAN_Interrupts* ir);
	void MCAN_ClearInterruptsAll(void);
	void MCAN_ReadInterruptEnable(TCAN4x5x_MCAN_Interrupt_Enable* ie);
	void MCAN_ConfigureInterruptEnable(TCAN4x5x_MCAN_Interrupt_Enable* ie);
	uint8_t MCAN_ReadNextFIFO(TCAN4x5x_MCAN_FIFO_Enum FIFODefine, TCAN4x5x_MCAN_RX_Header* header, uint8_t dataPayload[]);
	uint8_t MCAN_ReadRXBuffer(uint8_t bufIndex, TCAN4x5x_MCAN_RX_Header* header, uint8_t dataPayload[]);
	uint32_t MCAN_WriteTXBuffer(uint8_t bufIndex, TCAN4x5x_MCAN_TX_Header* header, uint8_t dataPayload[]);
	bool MCAN_TransmitBufferContents(uint8_t bufIndex);
	bool MCAN_WriteSIDFilter(uint8_t filterIndex, TCAN4x5x_MCAN_SID_Filter* filter);
	bool MCAN_WriteXIDFilter(uint8_t fifoIndex, TCAN4x5x_MCAN_XID_Filter* filter);
	uint8_t MCAN_DLCtoBytes(uint8_t inputDLC);
	uint8_t MCAN_TXRXESC_DataByteValue(uint8_t inputESCValue);




	// ~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
	//                            Non-MCAN Device Functions
	// ~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
	uint16_t Device_ReadDeviceVersion(void); 
	void Device_ReadDeviceIdent(uint32_t *id);
	void Device_ReadInterrupts(TCAN4x5x_Device_Interrupts* ir);
	void Device_ClearInterrupts(TCAN4x5x_Device_Interrupts* ir);
	void Device_ClearInterruptsAll(void);
	void Device_ReadInterruptEnable(TCAN4x5x_Device_Interrupt_Enable* ie);
	bool Device_ConfigureInterruptEnable(TCAN4x5x_Device_Interrupt_Enable* ie);
	bool Device_SetMode(TCAN4x5x_Device_Mode_Enum modeDefine);
	TCAN4x5x_Device_Mode_Enum Device_ReadMode(void);
	bool Device_EnableTestMode(TCAN4x5x_Device_Test_Mode_Enum modeDefine);
	bool Device_DisableTestMode(void);
	TCAN4x5x_Device_Test_Mode_Enum Device_ReadTestMode(void);


	bool WDT_Configure(TCAN4x5x_WDT_Timer_Enum WDTtimeout);
	TCAN4x5x_WDT_Timer_Enum WDT_Read(void);
	bool WDT_Enable(void);
	bool WDT_Disable(void);
	void WDT_Reset(void);

private:
	AlteraPio* controlReg;

};



#endif
