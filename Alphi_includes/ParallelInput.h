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
/** @file ParallelInput.h
* @brief Description of the Alphi Parallel Input class
*/

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		9/8/2020	phf	Written
//---------------------------------------------------------------------#pragma once

#include "AlphiDll.h"
#include <stdint.h>
#include "AlphiErrorCodes.h"

/** @brief Alphi Avalon digital input controller class
*/
class ParallelInput
{
public:
    static const uint16_t CAP_INPUT = 0x01;
    static const uint16_t CAP_OUTPUT = 0x02;
    static const uint16_t CAP_INPUT_OUTPUT = 0x03;

    inline ParallelInput(volatile void* addr)
    {
        base = (volatile uint32_t*)addr;
    }

    /** @brief Reset the PIO
     *
     * Whenever supported, set the direction register to all input, the data register to 0,
     * and disable interrupts.
     * @retval Always success
     */
    inline PCIeMini_status reset()
    {
        base[irqEnable_index] = 0;
        clearIrqStatus(0xffffffff);     

        return ERRCODE_NO_ERROR;
    }

    inline uint32_t getData()
    {
        return base[data_index];
    }

    /** @brief Retrieve the interrupt mask
     *
     * Returns 1 for the bits corresponding to input bits able to generate interrupts. On output-only devices,
     * it will return 0.
     * @retval A 32-bit bit map of which bit can generate interrupts.
     */
    inline uint32_t getIrqEnable()
    {
        return base[irqEnable_index];
    }

    /** @brief Enable bits in the interrupt mask
     *
     * Returns 1 for the bits corresponding to input bits able to generate interrupts. On output-only devices,
     * it will return 0.
     * @param mask a bit mask of which bits to enable
     * @retval A 32-bit bit map of which bit can generate interrupts.
     */
    inline uint32_t setIrqEnable(uint32_t mask)
    {
        base[irqEnable_index] |= mask;
        return base[irqEnable_index];
    }

    /** @brief Disable bits in the interrupt mask
     *
     * Returns 1 for the bits corresponding to input bits able to generate interrupts. On output-only devices,
     * it will return 0.
     * @param mask a bit mask of which bits to disable
     * @retval A 32-bit bit map of which bit can generate interrupts.
     */
    inline uint32_t setIrqDisable(uint32_t mask)
    {
        base[irqEnable_index] &= ~mask;
        return base[irqEnable_index];
    }

    /** @brief Return the interrupt status
    * 
    * @retval interrupt bit pattern. A "1" means that the particular bit is requesting an interrupt
    */
    inline uint32_t getIrqStatus()
    {
        return base[irqStatus_index];
    }

    /** @brief Clear the interrupt requests
    *
    * @retval Interrupt status register after the clear.
    */
    inline uint32_t resetIrq()
    {
        base[irqStatus_index] = 0;
        return base[irqStatus_index];
    }


    inline PCIeMini_status clearIrqStatus(uint32_t mask)
    {
        base[irqStatus_index] = mask;
        return ERRCODE_NO_ERROR;
    }

    volatile uint32_t* base;

    inline void setDataOut(uint32_t data)
    {
        base[dataOut_index] = data;
    }

    inline void setEdgeCapture(uint32_t data)
    {
        base[edgeReg_Index] = data;
    }

    int data_index = 0;         ///< Input bit status
    int polarity_index = 1;     ///< polarity: when set to 1, the interrupt is requested if the corresponding bit is low.
    int edgeReg_Index = 2;      ///< edge register: when a bit is set to 1, the interrupt is generated on an edge
    int irqStatus_index = 3;    ///< Which bit is requesting an interrupt
    int irqEnable_index = 4;    ///< Which bit can request an interrupt
    int direction_index = 5;    ///< Each bit set to 1 indicates that the corresponding bit in the data register is coming from the data_out register
    int dataOut_index = 6;      ///< used to simulate an input
    int irqDelay_index = 7;     ///< Debouncing constant
private:
};
