/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2003 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
******************************************************************************/
/** @file AlteraSpi.cpp 
 * @brief Implementation of the low-level access routines to the SPI. 
 */

#include "stdint.h"
#include "AlteraSpi.h"

 /** @brief Constructor
 *
 * Called only when the board is opened.
 * @param addr Pointer to the device in user space.
 * @param width Word size in byte, default is 1.
 */
AlteraSpi::AlteraSpi(volatile void* addr, uint8_t width)
{
    base = (volatile uint32_t *)addr;
    wordSize = width;
}

/** @brief Send an SPI command
 *
 * This is a very simple routine which performs one SPI master transaction.
 * It would be possible to implement a more efficient version using interrupts
 * and sleeping threads but this is probably not worthwhile initially.
 *
 *  @param slave Slave number select 0-31
 *  @param write_length Number of bytes to send
 *  @param write_data A pointer to the buffer containing the data to write
 *  @param read_length Number of bytes to receive
 *  @param read_data A pointer to the buffer where the received data is going
 *  @param flags A bit mask, only ALT_AVALON_SPI_COMMAND_TOGGLE_SS_N is used.
 *  @retval Number of bytes read - in SPI read and write are simultaneous so it cannot be 0.
 */

int AlteraSpi::sendSpiCommand(uint32_t slave,
                           uint32_t write_length, const uint32_t * write_data,
                           uint32_t read_length, uint32_t * read_data,
                           uint32_t flags)
{
  const uint32_t * write_end = write_data + write_length;
  uint32_t * read_end = read_data + read_length;

  uint32_t write_zeros = read_length;
  uint32_t read_ignore = write_length;
  uint32_t status;

  /* We must not send more than two bytes to the target before it has
   * returned any as otherwise it will overflow. */
  /* Unfortunately the hardware does not seem to work with credits > 1,
   * leave it at 1 for now. */
  uint32_t credits = 1;

  /* Warning: this function is not currently safe if called in a multi-threaded
   * environment, something above must perform locking to make it safe if more
   * than one thread intends to use it.
   */
  selectSlave(1 << slave);
  
  /* Set the SSO bit (force chipselect) only if the toggle flag is not set */
  if ((flags & ALT_AVALON_SPI_COMMAND_TOGGLE_SS_N) == 0) {
      setControl(ALTERA_AVALON_SPI_CONTROL_SSO_MSK);
  }

  /*
   * Discard any stale data present in the RXDATA register, in case
   * previous communication was interrupted and stale data was left
   * behind.
   */
  getRxData();
    
  /* Keep clocking until all the data has been processed. */
  for ( ; ; )
  {
    
    do
    {
      status = getStatus();
    }
    while (((status & status_TRDY_mask) == 0 || credits == 0) &&
            (status & status_RRDY_mask) == 0);

    if ((status & status_TRDY_mask) != 0 && credits > 0)
    {
      credits--;

      if (write_data < write_end)
          setTxData(*write_data++);
      else if (write_zeros > 0)
      {
        write_zeros--;
        setTxData(0);
      }
      else
        credits = -1024;
    };

    if ((status & status_RRDY_mask) != 0)
    {
      uint32_t rxdata = getRxData();

      if (read_ignore > 0)
        read_ignore--;
      else
        *read_data++ = rxdata;
      credits++;

      if (read_ignore == 0 && read_data == read_end)
        break;
    }
    
  }

  /* Wait until the interface has finished transmitting */
  do
  {
    status = getStatus();
  }
  while ((status & status_TMT_mask) == 0);

  /* Clear SSO (release chipselect) unless the caller is going to
   * keep using this chip
   */
  if ((flags & ALT_AVALON_SPI_COMMAND_MERGE) == 0)
      setControl(0);

  return read_length;
}

