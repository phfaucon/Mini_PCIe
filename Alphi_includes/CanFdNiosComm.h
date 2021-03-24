#pragma once
#include <stdint.h>
#include "AlphiDll.h"

/* special address description flags for the CAN_ID */
#define CAN_EFF_FLAG 0x80000000U /* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG 0x40000000U /* remote transmission request */
#define CAN_ERR_FLAG 0x20000000U /* error message frame */

/* valid bits in CAN ID for frame formats */
#define CAN_SFF_MASK 0x000007FFU /* standard frame format (SFF) */
#define CAN_EFF_MASK 0x1FFFFFFFU /* extended frame format (EFF) */
#define CAN_ERR_MASK 0x1FFFFFFFU /* omit EFF, RTR, ERR flags */

#define CAN_MAX_DLC 8
#define CAN_MAX_DLEN 8
#define CANFD_MAX_DLC 15
#define CANFD_MAX_DLEN 64
#define CAN_ERR_DLC 8

#define CANFD_BRS 0x01 /* bit rate switch (second bitrate for payload data) */
#define CANFD_ESI 0x02 /* error state indicator of the transmitting node */

typedef uint32_t canid_t;

struct can_frame {
	canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */

	uint8_t    can_dlc; /* frame payload length in byte (0 .. CAN_MAX_DLEN) */
	uint8_t    __pad;   /* padding */
	uint8_t    __res0;  /* reserved / padding */
	uint8_t    __res1;  /* reserved / padding */

	uint8_t    data[CAN_MAX_DLEN];
};

struct canfd_frame {
	canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */

	uint8_t    len;     /* frame payload length in byte */
	uint8_t    flags;   /* additional flags for CAN FD */
	uint8_t    __res0;  /* reserved / padding */
	uint8_t    __res1;  /* reserved / padding */

	uint8_t    data[CANFD_MAX_DLEN];
};

class DLL CanFdNiosComm
{

public:
	enum CommandCode {
		CAN_RECV_FRAME = 1,
		CANFD_RECV_FRAME = 2,
		CAN_SEND_FRAME = 3,
		CANFD_SEND_FRAME = 4,
		CAN_WRITE_REG = 5,
		CAN_READ_REG = 6,
		CAN_RX_SPI_MSG = 7,
		CAN_TX_SPI_MSG = 8,
		CAN_TIMING_MSG = 9,
		CAN_INTS_MSG = 10
	};

	volatile uint32_t* mddr;
	volatile uint16_t* dpr;

	inline CanFdNiosComm(volatile void* dpr_addr, volatile uint16_t* mddr_addr)
	{
		mddr = (volatile uint32_t*)mddr_addr;
		dpr = (volatile uint16_t*)dpr_addr;

		txBuffBase = (volatile uint8_t*)((char*)mddr + txBuffBase_offset);
		txReadBuffAddr = (volatile uint16_t*)((char*)dpr + txReadBuffAddr_offset);
		txWriteBuffAddr = (volatile uint16_t*)((char*)dpr + txWriteBuffAddr_offset);

		rxBuffBase = (volatile uint8_t*)((char*)mddr + rxBuffBase_offset);
		rxReadBuffAddr = (volatile uint16_t*)((char*)dpr + rxReadBuffAddr_offset);
		rxWriteBuffAddr = (volatile uint16_t*)((char*)dpr + rxWriteBuffAddr_offset);

		printf("To NIOS: PC pointer: 0x%8p, NIOS pointer: 0x%8p\n", txWriteBuffAddr, txReadBuffAddr);
		printf("from NIOS: NIOS pointer: 0x%8p, PC pointer: 0x%8p\n", rxWriteBuffAddr, rxReadBuffAddr);
		printf("buffer from NIOS: 0x%8p, to NIOS: 0x%8p\n", rxBuffBase, txBuffBase);
		uint64_t b3 = (uint64_t)mddr;
		printf("buffer offset from NIOS: 0x%08lx, to NIOS: 0x%08lx\n",
			(uint64_t)rxBuffBase - b3,
			(uint64_t)txBuffBase - b3);

		//		*txReadBuffAddr = 0;
		//*txWriteBuffAddr = *txReadBuffAddr;
		//		*rxWriteBuffAddr = 0;
		//*rxReadBuffAddr = *rxWriteBuffAddr;
	}

	void int_function();
	void ask_for_board_id();
	void ask_for_CAN_ID(uint8_t chan);
	void ask_for_timestamp();
	void fifo_status();

	void sendNiosMessage(uint8_t *msg, uint16_t lengthInBytes) {
		printf("Sending to NIOS (%x): ", *txWriteBuffAddr);
		uint16_t i;
		for (i = 0; i < lengthInBytes; i++) {
			txBuffBase[*txWriteBuffAddr + i] = msg[i];
			printf("<%x>", msg[i]);
		}
		printf("\n");
		*txWriteBuffAddr += i;
	}

	bool isCommandCodeValid(uint8_t cc)
	{
		return cc > 0 && cc <= 10;
	}

	uint16_t rcvNiosMessage(uint8_t* msg);
	void send_CAN_message(uint8_t chan, struct canfd_frame* cf);
	void send_CANFD_message(uint8_t chan, struct canfd_frame* cf);

private:
	volatile uint8_t* txBuffBase;
	volatile uint16_t* txReadBuffAddr;
	volatile uint16_t* txWriteBuffAddr;

	volatile uint8_t* rxBuffBase;
	volatile uint16_t* rxReadBuffAddr;
	volatile uint16_t* rxWriteBuffAddr;

	static const uint32_t txBuffBase_offset = 0x3fd0000ul;
	static const uint32_t txReadBuffAddr_offset = 0x108;		///< offset in relation with the beginning of the DPR
	static const uint32_t txWriteBuffAddr_offset = 0x10a;

	static const uint32_t rxBuffBase_offset = 0x3fe0000ul;
	static const uint32_t rxReadBuffAddr_offset = 0x100;
	static const uint32_t rxWriteBuffAddr_offset = 0x10a;


};
