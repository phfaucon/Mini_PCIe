/*       3717Driver.h
 *       Holt HI-3717 Demonstration Program 
 *
 *
 *      	This file demonstrates a simple ANSI C program structure for 
 *			the initializing and using a Holt HI-3717 device.
 *
 *
 *
 *			HOLT DISCLAIMER
 *
 *			THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *			EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO WARRANTIES
 *			OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *			NONINFRINGEMENT. 
 *			IN NO EVENT SHALL HOLT, INC BE LIABLE FOR ANY CLAIM, DAMAGES
 *			OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
 *			OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 *			SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *			Copyright (C) 2009 - 2010 by HOLT, Inc.
 *			All Rights Reserved.
 */
#include "stdint.h"
#include "Arinc3717interface.h"
#include "AlteraPio.h"
#include "ParallelInput.h"
 /*
General Information

Available external pin interrupts from the HI-3717

PIN   SIGNAL   USE BY THIS PROGRAM
-------------------
PP0   TRFO     
PP1   MATCH
PP2   ROVF  
PP3   TFIFO
PP7   TEMPTY


 ------------------------Definitions of the LEDs and the Dip Switches---------------------------


 LED1 - flashes to indicate a sync word was loaded into the TX FIFO.
 LED2 - flashes to indicate a sync word or data word that happens to match a sync word is received.
 LED3 - flashes to indicate any word is received from the RX FIFO.
 
 
 LED5 - On indicates Receiver FIFO Full
 LED6 - On indicates Transmitter FIFO Full 
 LED7 - Processor running indicator flashes at 1 second.
 LED8 - Red to indicate HI-3717 failed to initialize or to indicate when a data compare does not match.
 
DIP SWITCH ASSIGNMENTS ON THE SPI MAIN BOARD

MODE0 - MODE7 MODES:
   MODE0 = BOARD TEST
   MODE1 = TRANSMIT/RECEIVE
   MODE2 = TRANSMIT/RECEIVE IN TEST MODE
   MODE3 = TRANSMIT SPECIAL PATTERNS
   MODE4 = RECEIVE WITH NO COMPARE
   MODE5 = RECEIVE WITH COMPARE
   MODE6 = NOT USED
   MODE7 = SERIAL COMMANDS

   
BITRATE0-BITRATE2 7 RATES:
   0 = 64
   1 = 128
   2 = 256
   3 = 512
   4 = 1024
   5 = 2048
   6 = 4096
   7 = 8192
   
 
OPT1 = RXSEL   
       OPEN = BPRZ
       CLOSED = HBP

OPT2 = 32 WPS (THIS OVERRIDES OTHER RATE SWITCHES) 
       OPEN = 32WPS
       CLOSED = WORD RATE IS SELECTED BY BITRATE0-BITRATE7 SWITCHES */


// 3717 Device Write Registers
#define W_CTRL0               0x64
#define W_CTRL1               0x62
#define W_REC_FIFO_STATUS_PIN 0x6A
#define W_WORD_COUNT          0x72
#define W_XMTFIFO             0x74
#define W_XMTFIFO_FAST        0x70  // Lower nibble is a place holder temporary value


// 3717 Device Reads Registers
#define R_CTRL0               0xE4
#define R_CTRL1               0xE2
#define R_FIFO_STATUS_REG     0xE6
#define R_FIFO_XMT_REG        0xE8
#define R_REC_FIFO_STATUS_PIN 0xEA
#define R_WORD_COUNT_REG      0xF2
#define R_REC_FIFO_WORD       0xF6
#define R_REC_FIFO_WORD_COUNT 0xFE
#define R_READ_FIFO_FAST      0xC0     // Lower nibble is a place holder value

// Constants used
#define WPS32                 0x08     
#define INIT_FAIL             0x0F     // 3717 fail code
#define RFEMPTY               0x04
#define SRST                  0x08     // soft reset bit
#define RFFULL                0x10     // RX FIFO Full
#define TFFULL                0x80     // TX FIFO Full
#define W_WRDCNT              0x72

#define SFTSYNC               0x04     // 


// define chip select output pin for SPI /SS. 
#define SPI0_nSS    PTM_PTM3

class DLL Arinc3717 {
public:
    const unsigned char slewRate[8] = { // Transmitter slew rate table
    0,	   	// 64WPS  7.5 uS
    0,	   	// 128WPS 7.5 uS
    0,	   	// 256WPS 7.5 uS
    0,	   	// 512WPS 7.5 uS
    1,	   	// 1024WPS 3.75 uS
    3,	   	// 2048WPS 1.5 uS
    3,	   	// 4096WPS 1.5 uS
    3	   	// 8192WPS 1.5 uS
    };

    // control register bit definition
    static const uint32_t ctrl_reset_mask = 0x001;			///< When "1", the ARINC3717 is reset
	static const uint32_t ctrl_RSEL_mask = 0x002;			///< connected to the ARINC3717 RSEL line

    inline Arinc3717(volatile void* addr, AlteraPio* ctrl, ParallelInput* stat, uint8_t slave)
    {
        spi = new Arinc3717interface(addr, slave);
        controlReg = ctrl;
        statusReg = stat;
        slaveNbr = slave;
    }

    /** @brief reset the TCAN4550 chip
     */
    inline void reset()
    {
        uint32_t rst = ctrl_reset_mask;
        controlReg->setData(rst);
        Sleep(2);
        controlReg->setData(0);
        Sleep(2);
        spi->reset();
    }

    // Module function prototypes
    void W_Command(uint8_t cmd); 
    uint8_t Init3717(uint8_t test, uint8_t wordRate, uint8_t wps32, uint8_t rxsel);
    unsigned char txrx8bits(uint8_t, uint8_t);
    void CS_HL(void);
    unsigned char R_Register(uint8_t Reg);
    void W_CommandValue(uint8_t cmd, uint8_t value);
    void MultiByteRead(uint8_t, uint8_t count, uint8_t* passedArray);
    void TransmitCommandAndDataWords(uint8_t cmd, uint16_t* TXBuffer);
    void TxWord(unsigned int token);
    void FetchFIFOWord(uint8_t* passedArray);
    void FetchFIFOWordInt(uint16_t* passedArray);
    void FetchFIFOWordsWithCmd(uint8_t cmd, uint8_t count, uint16_t* passedArray);
    void softReset3717(void);
    void TransmitCommandAndData16(uint8_t cmd, uint16_t TXBuffer);

    Arinc3717interface* spi;

private:
    uint8_t txrx8bits_8(unsigned char txbyte, unsigned char return_when_done);
    AlteraPio* controlReg;
    uint8_t slaveNbr;
    ParallelInput* statusReg;
};

