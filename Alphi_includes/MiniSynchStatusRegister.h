#pragma once

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
/** @file MiniSynchStatusRegister.h
* @brief Description of the PCIeMini Status and Control Register classes
*/

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		9/8/2020	phf	Written
//---------------------------------------------------------------------#pragma once

#include "AlteraPio.h"

/** @brief Status register class for the PCIeMini board
*/
class StatusRegister : public AlteraPio
{
public:
    static const uint32_t STATUS_BIT_VALID_MASK = 0x01;

    inline StatusRegister(volatile void* addr) :
        AlteraPio(addr, CAP_INPUT)
    {    }

    /** @brief Check the synchro error flag (BIT)
    * @retval true if the sysnchro output is not in error
    */
    inline bool isBITnValid()
    {
        return getData() && (STATUS_BIT_VALID_MASK != 0);
    }

    /** @brief Enable the synchro error interrupt
    *
    * The interrupt is generated on the error signal becoming active (BITn going low)
    *
    */
    inline void enableBitValidIrq()
    {
        uint32_t irqMask = getIrqMask();
        irqMask |= STATUS_BIT_VALID_MASK;
        setIrqMask(irqMask);
    }

    /** @brief Disable the synchro error interrupt
    *
    */
    inline void disableBitValidIrq()
    {
        uint32_t irqMask = getIrqMask();
        irqMask &= ~STATUS_BIT_VALID_MASK;
        setIrqMask(irqMask);
    }

};

/** @brief Status register class for the PCIeMini board
*/
class ControlRegister : public AlteraPio
{
public:
    /*
        Res:        Resolver resolution. 11 = 16 bits (default), 10 = 14 bits, 01 = 12 bits, 00 = 10 bits
        Shift:      1 Selects VEL1 components (default), 0 selects VEL2 components.
        DN_UPn:     Selects the gain of the amplifier driving the de-selected set of bandwidth
                        components (default 0 – pre-set resolution to increase).
        A_Q_B:      Enables encoder emulation (default 0). Do not change
        DSR:        Disables synthesized reference (default 0). Do not change
        DAC_RST:    Sets DAC channels to midrange (default 0).
        DAC_Span:   000 = +/- 10V (11.6 V inputs), 010 = +/- 2.5V (direct inputs)
        */

    enum Resolution {
        RES_10_BIT = 0,
        RES_12_BIT = 1,
        RES_14_BIT = 2,
        RES_16_BIT = 3
    };

    enum ShiftControl {
        VEL1 = 4,
        VEL2 = 0
    };

    static const uint32_t CTRL_BITn_MASK = 0x01;
    static const uint32_t CTRL_AQB_mask = 0x010;
    static const uint32_t CTRL_DSR_mask = 0x020;
    static const uint32_t CTRL_DA_CLEAR_mask = 0x040;   //< When set, reset the D/A programmation
    static const uint32_t CTRL_DaMode_mask = 0x400;     //< When set, use the DA as a regular D/A
    static const uint32_t CTRL_LDAC_mask = 0x800;       //< LDAC output to the D/A - active only in regular D/A mode

    /** @brief Constructor
     * 
     * Only called when the board is opened.
     * @param addr Pointer to the device in user's space.
     */
    inline ControlRegister(volatile void* addr) :
        AlteraPio(addr, CAP_OUTPUT)
    {    }

    /** @brief Select the type of output of the D/A
    *
    * @param enabled True when the D/As are general purpose, False when they are a synchro simulator
    * @retval ERRCODE_NO_ERROR
    */
    inline PCIeMini_status enableSpiDa(bool enabled)
    {
        if (enabled) {
            setData(getData() | CTRL_DaMode_mask | CTRL_LDAC_mask);
        }
        else {
            setData(getData() & ~(CTRL_DaMode_mask | CTRL_LDAC_mask));
        }
        return ERRCODE_NO_ERROR;
    }



};

