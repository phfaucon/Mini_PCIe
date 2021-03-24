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

// Maintenance Log
//---------------------------------------------------------------------
//---------------------------------------------------------------------
#ifndef ESCC_DEFINES_H
#define ESCC_DEFINES_H

/* Write Register 0 */
#define RES_EXT_INT     0x10    /* Reset Ext. Status Interrupts */
#define SEND_ABORT      0x18    /* HDLC Abort */
#define RES_RxINT_FC    0x20    /* Reset RxINT on First Character */
#define RES_Tx_P        0x28    /* Reset TxINT Pending */
#define ERR_RES         0x30    /* Error Reset */
#define RES_H_IUS       0x38    /* Reset highest IUS */

#define RES_Rx_CRC      0x40    /* Reset Rx CRC Checker */
#define RES_Tx_CRC      0x80    /* Reset Tx CRC Checker */
#define RES_EOM_L       0xC0    /* Reset EOM latch */

/* Write Register 1 */
#define EXT_INT_ENAB    0x1     /* Ext Int Enable */
#define TxINT_ENAB      0x2     /* Tx Int Enable */
#define PAR_SPEC        0x4     /* Parity is special condition */

#define RxINT_DISAB     0       /* Rx Int Disable */
#define RxINT_FCERR     0x8     /* Rx Int on First Character Only or Error */
#define INT_ALL_Rx      0x10    /* Int on all Rx Characters or error */
#define INT_ERR_Rx      0x18    /* Int on error only */

#define WT_RDY_RT       0x20    /* Wait/Ready on R/T */
#define WT_FN_RDYFN     0x40    /* Wait/FN/Ready FN */
#define WT_RDY_ENAB     0x80    /* Wait/Ready Enable */

/* Write Register 3 */
#define RxENABLE        0x1     /* Rx Enable */
#define SYNC_L_INH      0x2     /* Sync Character Load Inhibit */
#define ADD_SM          0x4     /* Address Search Mode (SDLC) */
#define RxCRC_ENAB      0x8     /* Rx CRC Enable */
#define ENT_HM          0x10    /* Enter Hunt Mode */
#define AUTO_ENAB       0x20    /* Auto Enables */
#define Rx5             0x0     /* Rx 5 Bits/Character */
#define Rx7             0x40    /* Rx 7 Bits/Character */
#define Rx6             0x80    /* Rx 6 Bits/Character */
#define Rx8             0xc0    /* Rx 8 Bits/Character */

/* Write Register 4 */
#define PAR_ENA			0x01	/* Parity enable */
#define PAR_EVEN		0x02	/* Parity even */

#define SYNC_ENAB		0x00	/* Sync enabled */
#define SB1				0x04	/* 1 stop bit */
#define SB15			0x08	/* 1.5 stop bits */
#define SB2				0x0c	/* 2 stop bits */

#define MON_SYNC		0x00	/* 8-bit sync */
#define BISYNC			0x10	/* 16-bit synce */
#define SDLC			0x20	/*  */
#define EXTSYNC			0x30	/*  */

#define X1CLK			(uint8_t)0x00	/* x1 clock mode */
#define X16CLK			(uint8_t)0x40	/* x16 clock mode */
#define X32CLK			(uint8_t)0x80	/* x32 clock mode */
#define X64CLK			(uint8_t)0xc0	/* x64 clock mode */

/* Write Register 5 */
#define TxCRC_ENAB      (uint8_t)0x1     /* Tx CRC Enable */
#define RTS             (uint8_t)0x2     /* RTS */
#define SDLC_CRC        (uint8_t)0x4     /* SDLC/CRC-16 */
#define TxENABLE        (uint8_t)0x8     /* Tx Enable */
#define SND_BRK         (uint8_t)0x10    /* Send Break */
#define Tx5             (uint8_t)0x0     /* Tx 5 bits (or less)/character */
#define Tx7             (uint8_t)0x20    /* Tx 7 bits/character */
#define Tx6             (uint8_t)0x40    /* Tx 6 bits/character */
#define Tx8             (uint8_t)0x60    /* Tx 8 bits/character */
#define DTR             (uint8_t)0x80    /* DTR */

/* Write Register 9 (Master interrupt control */
#define VIS				(uint8_t)1       /* Vector Includes Status */
#define NV				(uint8_t)2       /* No Vector */
#define DLC				(uint8_t)4       /* Disable Lower Chain */
#define MIE				(uint8_t)8       /* Master Interrupt Enable */
#define STATHI			(uint8_t)0x10    /* Status high */
#define SOFTIACK		(uint8_t)0x20    /* Allow acknowledging the interrupt, when reading the vector register */
#define NORESET			(uint8_t)0       /* No reset on write to R9 */
#define CHRB			(uint8_t)0x40    /* Reset channel B */
#define CHRA			(uint8_t)0x80    /* Reset channel A */
#define FHWRES			(uint8_t)0xc0    /* Force hardware reset */

/* Write Register 10 */
#define SYNC6BIT			0x01	/* 6/8 bit sync */
#define LOOPMODE		0x02	/* SDLC loop mode */
#define ABUNDER			0x04	/* Abort flag on SDLC transmit underrun */
#define MARKIDLE		0x08	/* Mark flag on idle */
#define GAOP			0x10	/* Go active on poll */
#define NRZ				0x00	/*  */
#define NRZI			0x20	/*  */
#define FM1				0x40	/*  */
#define FM0				0x60	/*  */
#define CRCPS			0x80	/* CRC Preset I/O */

/* Write Register 11 */
#define TRxCXT  0       /* TRxC = Xtal output */
#define TRxCTC  1       /* TRxC = Transmit clock */
#define TRxCBR  2       /* TRxC = BR Generator Output */
#define TRxCDP  3       /* TRxC = DPLL output */
#define TRxCOI  4       /* TRxC O/I */
#define TCRTxCP 0       /* Transmit clock = RTxC pin */
#define TCTRxCP 8       /* Transmit clock = TRxC pin */
#define TCBR    0x10    /* Transmit clock = BR Generator output */
#define TCDPLL  0x18    /* Transmit clock = DPLL output */
#define RCRTxCP 0       /* Receive clock = RTxC pin */
#define RCTRxCP 0x20    /* Receive clock = TRxC pin */
#define RCBR    0x40    /* Receive clock = BR Generator output */
#define RCDPLL  0x60    /* Receive clock = DPLL output */
#define RTxCX   0x80    /* RTxC Xtal/No Xtal */

/* Write Register 14 */
#define BRENABL 1       /* Baud rate generator enable */
#define BRSRC   2       /* Baud rate generator source */
#define DTRREQ  4       /* DTR/Request function */
#define AUTOECHO 8      /* Auto Echo */
#define LOOPBAK 0x10    /* Local loopback */
#define SEARCH  0x20    /* Enter search mode */
#define RMC     0x40    /* Reset missing clock */
#define DISDPLL 0x60    /* Disable DPLL */
#define SSBR    0x80    /* Set DPLL source = BR generator */
#define SSRTxC  0xa0    /* Set DPLL source = RTxC */
#define SFMM    0xc0    /* Set FM mode */
#define SNRZI   0xe0    /* Set NRZI mode */

/* Write Register 15 (external/status interrupt control) */
#define PRIME   1       /* R5' etc register access (Z85C30/230 only) */
#define ZCIE    2       /* Zero count IE */
#define FIFOE   4       /* Z85230 only */
#define DCDIE   8       /* DCD IE */
#define SYNCIE  0x10    /* Sync/hunt IE */
#define CTSIE   0x20    /* CTS IE */
#define TxUIE   0x40    /* Tx Underrun/EOM IE */
#define BRKIE   0x80    /* Break/Abort IE */

/* Read Register 0 */
#define Rx_CH_AV		0x01	/* Rx Character Available */
#define ZCOUNT			0x02	/* Zero count */
#define Tx_BUF_EMP		0x04	/* Tx Buffer empty */
#define DCD				0x08	/* DCD */
#define SYNC_HUNT		0x10	/* Sync/hunt */
#define CTS				0x20	/* CTS */
#define TxEOM			0x40	/* Tx underrun */
#define BRK_ABRT		0x80	/* Break/Abort */

/* Read Register 1 */
#define ALL_SNT         0x1     /* All sent */
/* Residue Data for 8 Rx bits/char programmed */
#define RES3            0x8     /* 0/3 */
#define RES4            0x4     /* 0/4 */
#define RES5            0xc     /* 0/5 */
#define RES6            0x2     /* 0/6 */
#define RES7            0xa     /* 0/7 */
#define RES8            0x6     /* 0/8 */
#define RES18           0xe     /* 1/8 */
#define RES28           0x0     /* 2/8 */
/* Special Rx Condition Interrupts */
#define PAR_ERR         0x10    /* Parity error */
#define Rx_OVR          0x20    /* Rx Overrun Error */
#define CRC_ERR         0x40    /* CRC/Framing Error */
#define END_FR          0x80    /* End of Frame (SDLC) */

/* Read Register 2 (channel b only) - Interrupt vector */

/* Read Register 3 (interrupt pending register) ch a only */
#define CHBEXT  0x1             /* Channel B Ext/Stat IP */
#define CHBTxIP 0x2             /* Channel B Tx IP */
#define CHBRxIP 0x4             /* Channel B Rx IP */
#define CHAEXT  0x8             /* Channel A Ext/Stat IP */
#define CHATxIP 0x10            /* Channel A Tx IP */
#define CHARxIP 0x20            /* Channel A Rx IP */

/* Read Register 8 (receive data register) */

/* Read Register 10  (misc status bits) */
#define ONLOOP  2               /* On loop */
#define LOOPSEND 0x10           /* Loop sending */
#define CLK2MIS 0x40            /* Two clocks missing */
#define CLK1MIS 0x80            /* One clock missing */



#endif // ESCC_DEFINES_H
