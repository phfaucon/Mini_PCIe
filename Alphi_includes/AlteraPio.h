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
/** @file AlteraPio.h
* @brief Description of the Altera PIO controller class
*/

// Maintenance Log
//---------------------------------------------------------------------
// v1.0		9/8/2020	phf	Written
//---------------------------------------------------------------------#pragma once

#include "AlphiDll.h"
#include <stdint.h>
#include "AlphiErrorCodes.h"

/** @brief Altera Avalon Pio controller class
*/
class AlteraPio
{
public:
    static const uint16_t CAP_INPUT = 0x01;
    static const uint16_t CAP_OUTPUT = 0x02;
    static const uint16_t CAP_INPUT_OUTPUT = 0x03;

    inline AlteraPio(volatile void* addr, uint16_t capabilities)
    {
        base = (volatile uint32_t*)addr;
        options = capabilities;
    }

    /** @brief Reset the PIO
     *
     * Whenever supported, set the direction register to all input, the data register to 0, 
     * and disable interrupts.
     * @retval Always success
     */
    inline PCIeMini_status reset()
    {
        base[data_index] = 0;
        setIrqMask(0);                  // ignore possible error if not supported by instance
        clearEdgeCapture(0xffffffff);   // ignore possible error if not supported by instance

        return ERRCODE_NO_ERROR;
    }

    inline uint32_t getData()
    {
        return base[data_index];
    }

    /** @brief set the output data
     *
     * This operation is valid only for output or bidirectional parallel ports. On input-only devices,
     * it will return ERRCODE_INVALID_INPUT_MODE 
     * @param data Data to be output. The values corresponding to input bits are ignored.
     * @retval ERRCODE_NO_ERROR, or if it is an illegal operation ERRCODE_INVALID_INPUT_MODE.
     */
    inline PCIeMini_status setData(uint32_t data)
    {
        if ((options & CAP_OUTPUT) == 0) {
            return ERRCODE_INVALID_INPUT_MODE;
        }
        base[data_index] = data;
        return ERRCODE_NO_ERROR;
    }

    /** @brief Retrieve the interrupt mask
     *
     * Returns 1 for the bits corresponding to input bits able to generate interrupts. On output-only devices,
     * it will return 0.
     * @retval A 32-bit bit map of which bit can generate interrupts.
     */
    inline uint32_t getIrqMask()
    {
        if ((options & CAP_INPUT) == 0) {
            return 0;
        }
        return base[irqMask_Index];
    }

    /** @brief Set the interrupt mask
     *
     * Set 1s for the bits corresponding to input bits able to generate interrupts. On output-only devices,
     * it will return ERRCODE_INVALID_INPUT_MODE
     * @param data A bit map of which inputs are able to request interrupts.
     * @retval A 32-bit bit map of which bit can generate interrupts.
     */
    inline PCIeMini_status setIrqMask(uint32_t mask)
    {
        if ((options & CAP_INPUT) == 0) {
            return ERRCODE_INVALID_INPUT_MODE;
        }
        base[irqMask_Index] = mask;
        return ERRCODE_NO_ERROR;
    }

    inline uint32_t getEdgeCapture()
    {
        return base[edgeCap_index];
    }

    inline PCIeMini_status clearEdgeCapture(uint32_t mask)
    {
        if ((options & CAP_INPUT) == 0) {
            return ERRCODE_INVALID_INPUT_MODE;
        }
        base[edgeCap_index] = mask;
        return ERRCODE_NO_ERROR;
    }

private:
    volatile uint32_t* base;
    uint16_t options;


    int data_index = 0;
    int direction_index = 1;
    int irqMask_Index = 2;
    int edgeCap_index = 3;
    int setOuput_index = 4;
    int clearOuput_index = 5;
};
