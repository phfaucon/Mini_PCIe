#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "CanFdNiosComm.h"


void CanFdNiosComm::fifo_status()
{
	printf("FIFO status:\n");
	printf("NIOS to PC: Writing to %x, Reading from %x\n", *rxWriteBuffAddr, *rxReadBuffAddr);
	printf("PC to NIOS: Writing to %x, Reading from %x\n", *txWriteBuffAddr, *txReadBuffAddr);

	return;
}

void print_can_message(uint8_t chan, struct canfd_frame cf)
{

	int i;
	printf("CAN frame channel %d\n", chan);
	// just dump the error frame -- shouldn't go this direction
	if (cf.can_id & CAN_ERR_FLAG) { // Error Frame flag
		printf("Error Frame ID %lx # ", (cf.can_id & 0x1ff));
		for (i = 0; i < CAN_ERR_DLC; i++)
			printf(" %x ", cf.data[i]);
		printf("\n");
	}
	else {
		if (cf.can_id & CAN_EFF_FLAG)
			printf("EFF ID %lx # ", (cf.can_id & CAN_EFF_MASK));
		else
			printf("SFF ID %lx # ", (cf.can_id & CAN_SFF_MASK));

		printf("RTRR %d # ", !!(cf.can_id & CAN_RTR_FLAG));

		for (i = 0; i < cf.len; i++)
			printf(" %x ", cf.data[i]);
		printf("\n");
	}
}

void print_canfd_message(uint8_t chan, struct canfd_frame cfd)
{
	int i;
	printf("CAN FD frame channel %d\n", chan);
	// just dump the error frame -- shouldn't go this direction
	if (cfd.can_id & CAN_ERR_FLAG) { // Error Frame flag
		printf("Error Frame ID %lx # ", (cfd.can_id & 0x1ff));
		for (i = 0; i < CAN_ERR_DLC; i++)
			printf(" %x ", cfd.data[i]);
		printf("##\n");
	}
	else {
		if (cfd.can_id & CAN_EFF_FLAG)
			printf("EFF ID %lx # ", (cfd.can_id & CAN_EFF_MASK));
		else
			printf("SFF ID %lx # ", (cfd.can_id & CAN_SFF_MASK));

		printf("RTRR %d # ", !!(cfd.can_id & CAN_RTR_FLAG));
		printf("BRS %d # ", !!(cfd.flags & CANFD_BRS));
		printf("ESI %d # ", !!(cfd.flags & CANFD_ESI));

		for (i = 0; i < cfd.len; i++)
			printf(" %x ", cfd.data[i]);
		printf("##\n");
	}
}


void print_ints(uint8_t chan, uint32_t chan_status, uint32_t chan_globint, uint32_t chan_mcanint)
{

	printf("Channel %d\n", chan);
	if (chan_status & (1 << 0))     printf("Status: Interrupt\n");
	if (chan_status & (1 << 1))     printf("Status: SPI Error\n");
	if (chan_status & (1 << 2))     printf("Status: Internal Error\n");
	if (chan_status & (1 << 3))     printf("Status: Internal Multi-accessn\n");
	if (chan_status & (1 << 4))     printf("Status: Read FIFO Available\n");
	if (chan_status & (1 << 5))     printf("Status: Write FIFO Available\n");
	if (chan_status & (1 << 16))    printf("Status: SPI Read Underflo\n");
	if (chan_status & (1 << 17))    printf("Status: SPI Read Overflo\n");
	if (chan_status & (1 << 18))    printf("Status: SPI Write Underflo\n");
	if (chan_status & (1 << 19))    printf("Status: SPI Write Overflo\n");
	if (chan_status & (1 << 20))    printf("Status: SPI Invalid Command\n");
	if (chan_status & (1 << 21))    printf("Status: SPI End Invalid\n");
	if (chan_status & (1 << 24))    printf("Status: Write FIFO Overflo\n");
	if (chan_status & (1 << 25))    printf("Status: Read FIFO Empty\n");
	if (chan_status & (1 << 26))    printf("Status: Read FIFO Underflo\n");
	if (chan_status & (1 << 27))    printf("Status: Internal Error Log Wrt\n");
	if (chan_status & (1 << 28))    printf("Status: Internal Write Error\n");
	if (chan_status & (1 << 29))    printf("Status: Internal Read Error\n");

	if (chan_globint & (1 << 0))	printf("Global Int: Volt Temp or WDTO\n");
	if (chan_globint & (1 << 1))	printf("Global Int: M CAN INT\n");
	if (chan_globint & (1 << 3))	printf("Global Int: SPI Error\n");
	if (chan_globint & (1 << 5))	printf("Global Int: CAN Error\n");
	if (chan_globint & (1 << 6))	printf("Global Int: Wake Request\n");
	if (chan_globint & (1 << 7))	printf("Global Int: Global Error\n");
	if (chan_globint & (1 << 8))	printf("Global Int: CAN Dominant\n");
	if (chan_globint & (1 << 10))	printf("Global Int: CAN Silent\n");
	if (chan_globint & (1 << 13))	printf("Global Int: Wake Error\n");
	if (chan_globint & (1 << 14))	printf("Global Int: Local Wake\n");
	if (chan_globint & (1 << 15))	printf("Global Int: Wake on CAN\n");
	if (chan_globint & (1 << 16))	printf("Global Int: ECC Error\n");
	if (chan_globint & (1 << 18))	printf("Global Int: WDTO\n");
	if (chan_globint & (1 << 19))	printf("Global Int: Thermal Shutdown\n");
	if (chan_globint & (1 << 20))	printf("Global Int: Power On\n");
	if (chan_globint & (1 << 21))	printf("Global Int: VIO Undervolt\n");
	if (chan_globint & (1 << 22))	printf("Global Int: VSUP Undervolt\n");
	if (chan_globint & (1 << 23))	printf("Global Int: Sleeping now\n");
	if (chan_globint & (1 << 31))	printf("Global Int: CAN Normal\n");

	if (chan_mcanint & (1 << 0))	printf("M CAN Int: RF0N Fifo 0 New Message\n");
	if (chan_mcanint & (1 << 1))	printf("M CAN Int: RF0W Fifo 0 at Watermark\n");
	if (chan_mcanint & (1 << 2))	printf("M CAN Int: RF0F Fifo 0 Full\n");
	if (chan_mcanint & (1 << 3))	printf("M CAN Int: RF0L Fifo 0 Message Lost\n");
	if (chan_mcanint & (1 << 4))	printf("M CAN Int: RF1N Fifo 1 New Message\n");
	if (chan_mcanint & (1 << 5))	printf("M CAN Int: RF1W Fifo 1 at Watermark\n");
	if (chan_mcanint & (1 << 6))	printf("M CAN Int: RF1F Fifo 1 Full\n");
	if (chan_mcanint & (1 << 7))	printf("M CAN Int: RF1L Fifo 1 Message Lost\n");
	if (chan_mcanint & (1 << 8))	printf("M CAN Int: HPM  Hi Priority Message\n");
	if (chan_mcanint & (1 << 9))	printf("M CAN Int: TC   Transmission Complete\n");
	if (chan_mcanint & (1 << 10))	printf("M CAN Int: TCF  Cancel Complete\n");
	if (chan_mcanint & (1 << 11))	printf("M CAN Int: TFE  TX FIFO Empty\n");
	if (chan_mcanint & (1 << 12))	printf("M CAN Int: TEFN TX Event FIFO New\n");
	if (chan_mcanint & (1 << 13))	printf("M CAN Int: TEFW TX Event FIFO Watermark\n");
	if (chan_mcanint & (1 << 14))	printf("M CAN Int: TEFF TX Event FIFO Full\n");
	if (chan_mcanint & (1 << 15))	printf("M CAN Int: TEFF TX Event Lost\n");
	if (chan_mcanint & (1 << 16))	printf("M CAN Int: TSW  Timestamp Wrap\n");
	if (chan_mcanint & (1 << 17))	printf("M CAN Int: MRF  Message RAM Failure\n");
	if (chan_mcanint & (1 << 18))	printf("M CAN Int: TOO  Timeout Occured\n");
	if (chan_mcanint & (1 << 19))	printf("M CAN Int: DRX  Message to Dedicated RX\n");
	if (chan_mcanint & (1 << 20))	printf("M CAN Int: BEC  Bit Error Corrected\n");
	if (chan_mcanint & (1 << 21))	printf("M CAN Int: BEU  Bit Error Uncorrected\n");
	if (chan_mcanint & (1 << 22))	printf("M CAN Int: ELO  Error Log Overflow\n");
	if (chan_mcanint & (1 << 23))	printf("M CAN Int: EP   Error Passive\n");
	if (chan_mcanint & (1 << 24))	printf("M CAN Int: EW   Warning Status\n");
	if (chan_mcanint & (1 << 25))	printf("M CAN Int: BO   Bus Off\n");
	if (chan_mcanint & (1 << 26))	printf("M CAN Int: WDI  Watchdog Int\n");
	if (chan_mcanint & (1 << 27))	printf("M CAN Int: PEA  Arbit Protocol Error\n");
	if (chan_mcanint & (1 << 28))	printf("M CAN Int: PED  Data Protocol Error\n");
	if (chan_mcanint & (1 << 29))	printf("M CAN Int: ARA  Access to Reserved Address\n");

	return;

}

void CanFdNiosComm::int_function()
{

	int i, data_words;
	uint8_t* buff_ptr;
	uint16_t buff_idx;
	uint16_t buff_end;
	uint8_t cmd;
	unsigned char r_string[12];
	uint32_t r_buffer[5];
	uint8_t chan;
	uint16_t spi_addr;
	uint16_t msg_len;
	uint16_t data_len;
	uint32_t av_address, av_data;
	uint32_t chan_status;
	uint32_t chan_globint;
	uint32_t chan_mcanint;
	struct canfd_frame cf;

	buff_end = *rxWriteBuffAddr; // where the NIOS is writing
	buff_idx = *rxReadBuffAddr; // where we are reading
	buff_ptr = (uint8_t *)rxBuffBase;
	while (buff_idx < buff_end)
	{

		printf("NIOS pointer: 0x%08x, our pointer: 0x%08x\n", buff_end, buff_idx);
		cmd = *((volatile uint8_t*)(buff_ptr + buff_idx));
		printf("Reading %x from %x\n", cmd, buff_idx);
		buff_idx = buff_idx + 1;


		switch (cmd) {

		case CAN_RECV_FRAME:
			chan = *((volatile uint8_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 1;
			msg_len = *((volatile uint16_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 2;
			cf.can_id = *((volatile uint32_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 4;
			cf.len = *((volatile uint8_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 1;
			cf.flags = *((volatile uint8_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 1;
			cf.__res0 = *((volatile uint8_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 1;
			cf.__res1 = *((volatile uint8_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 1;

			if (cf.len & 3)
				data_words = (cf.len >> 2) + 1; // round up to the next 32 bit word
			else
				data_words = (cf.len >> 2);


			for (i = 0; i < data_words; i++) {
				cf.data[i] = *((volatile uint8_t*)(buff_ptr + buff_idx));
				buff_idx = buff_idx + 1;
				cf.data[i + 1] = *((volatile uint8_t*)(buff_ptr + buff_idx));
				buff_idx = buff_idx + 1;
				cf.data[i + 2] = *((volatile uint8_t*)(buff_ptr + buff_idx));
				buff_idx = buff_idx + 1;
				cf.data[i + 3] = *((volatile uint8_t*)(buff_ptr + buff_idx));
				buff_idx = buff_idx + 1;
			}

			*rxReadBuffAddr = buff_idx; // ack receive
			print_can_message(chan, cf);  // Send_canfd_frame to TCAN

			break;

		case CANFD_RECV_FRAME:
			chan = *((volatile uint8_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 1;
			msg_len = *((volatile uint16_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 2;
			cf.can_id = *((volatile uint32_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 4;
			cf.len = *((volatile uint8_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 1;
			cf.flags = *((volatile uint8_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 1;
			cf.__res0 = *((volatile uint8_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 1;
			cf.__res1 = *((volatile uint8_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 1;

			if (cf.len & 3)
				data_words = (cf.len >> 2) + 1; // round up to the next 32 bit word
			else
				data_words = (cf.len >> 2);


			for (i = 0; i < data_words; i++) {
				cf.data[i] = *((volatile uint8_t*)(buff_ptr + buff_idx));
				buff_idx = buff_idx + 1;
				cf.data[i + 1] = *((volatile uint8_t*)(buff_ptr + buff_idx));
				buff_idx = buff_idx + 1;
				cf.data[i + 1] = *((volatile uint8_t*)(buff_ptr + buff_idx));
				buff_idx = buff_idx + 1;
				cf.data[i + 1] = *((volatile uint8_t*)(buff_ptr + buff_idx));
				buff_idx = buff_idx + 1;
			}

			*rxReadBuffAddr = buff_idx; // ack receive
			print_canfd_message(chan, cf);  // Send_canfd_frame to TCAN



			break;

		case CAN_INTS_MSG:

			chan = *((volatile uint8_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 1;
			msg_len = *((volatile uint16_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 2;
			chan_status = *((volatile uint32_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 4;
			chan_globint = *((volatile uint32_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 4;
			chan_mcanint = *((volatile uint32_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 4;

			*rxReadBuffAddr = buff_idx; // ack receive
			print_ints(chan, chan_status, chan_globint, chan_mcanint);

			break;

		case CAN_RX_SPI_MSG:

			chan = *((volatile uint8_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 1;
			msg_len = *((volatile uint16_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 2;
			spi_addr = *((volatile uint16_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 2;
			data_len = *((volatile uint16_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 2;

			for (i = 0; i < data_len; i++) {
				r_buffer[i] = *((volatile uint32_t*)(buff_ptr + buff_idx));
				buff_idx = buff_idx + 4;
			}

			memcpy(r_string, &r_buffer, 10);
			for (i = 0; i < 8; i++)
				printf("%c", r_string[i]);
			printf("\n");

			*rxReadBuffAddr = buff_idx; // ack receive

			break;

		case CAN_READ_REG:
			buff_idx = buff_idx + 1; // toss 1 bytes
			msg_len = *((volatile uint16_t*)(buff_ptr + buff_idx)); //
			buff_idx = buff_idx + 2;
			av_address = *((volatile uint32_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 4;
			av_data = *((volatile uint32_t*)(buff_ptr + buff_idx));
			buff_idx = buff_idx + 4;

			*rxReadBuffAddr = buff_idx; // ack receive
			printf("Read %x from avalon address %x\n", av_data, av_address);

			break;

		default:
			*rxReadBuffAddr = buff_idx;
			fifo_status();
			printf("Command %x not found!!\n", cmd);

		}
	};
	return;
}

void CanFdNiosComm::ask_for_board_id()
{

	uint8_t msgBuf[128] = { 0 };
	uint8_t rcvBuf[128] = { 0 };

	printf("Getting Board ID\n");

	msgBuf[0] = CAN_READ_REG;
	msgBuf[1] = CAN_READ_REG;
	msgBuf[2] = 8;		//length of this message
	msgBuf[3] = 8;		//length of this message
	// we leave the address at 0
	for (int i = 0; i < 0x10000 ; i++) {
		rxBuffBase[i] = 0;
		txBuffBase[i] = 0;
	}
	printf("Before sending the command\n");
	fifo_status();
	sendNiosMessage(msgBuf, 8);
	printf("After sending the command\n");
	fifo_status();
	Sleep(100);
	printf("After sleeping\n");
	fifo_status();
	for (int i = 0; i < 0x10000; i++) {
		if (rxBuffBase[i] != 0)
			printf("rxBuffBase[0x%04x] = 0x%02x\n", i, rxBuffBase[i]);
	}
	for (int i = 0; i < 0x10000; i++) {
		if (txBuffBase[i] != 0)
			printf("txBuffBase[0x%04x] = 0x%02x\n", i, txBuffBase[i]);
	}
	rcvNiosMessage(rcvBuf);
	printf("Received message: ");
	for (int i = 0; i < 16; i++) {
		printf("<%x>", rcvBuf[i]);
	}
	printf("\n");
	printf("After receiving\n");
	fifo_status();
	return;
}

void CanFdNiosComm::ask_for_timestamp()
{

	uint8_t* buff_ptr;
	uint16_t buff_idx;

	//	fifo_status(bar2, bar3);

	buff_idx = *txWriteBuffAddr;
	buff_ptr = (uint8_t*)txBuffBase;

	printf("Getting Timestamp\n");
	printf("Writing %x, to %x\n", CAN_READ_REG, buff_idx);

	*((volatile uint8_t*)(buff_ptr + buff_idx)) = CAN_READ_REG; // read from avalon mm address
	buff_idx = buff_idx + 1;
	*((volatile uint8_t*)(buff_ptr + buff_idx)) = 0; // channel -- dummy value
	buff_idx = buff_idx + 1;
	*((volatile uint16_t*)(buff_ptr + buff_idx)) = 8; //length of this message
	buff_idx = buff_idx + 2;
	*((volatile uint32_t*)(buff_ptr + buff_idx)) = 0x00000004; //32 bit address
	buff_idx = buff_idx + 4;

	*txWriteBuffAddr = buff_idx;

	return;
}

void CanFdNiosComm::ask_for_CAN_ID(uint8_t chan)
{
	uint8_t* buff_ptr;
	uint16_t buff_idx;

	//	fifo_status(bar2, bar3);

	buff_idx = *txWriteBuffAddr;
	buff_ptr = (uint8_t*)txBuffBase;

	printf("Getting CAN ID %d\n", chan);
	printf("Writing %x, to %x\n", CAN_RX_SPI_MSG, buff_idx);

	*((volatile uint8_t*)(buff_ptr + buff_idx)) = CAN_RX_SPI_MSG; // read from avalon mm address
	buff_idx = buff_idx + 1;
	*((volatile uint8_t*)(buff_ptr + buff_idx)) = chan; // channel -- dummy value
	buff_idx = buff_idx + 1;
	*((volatile uint16_t*)(buff_ptr + buff_idx)) = 8; //length of this message
	buff_idx = buff_idx + 2;
	*((volatile uint16_t*)(buff_ptr + buff_idx)) = 0x0000; //16 bit spi address
	buff_idx = buff_idx + 2;
	*((volatile uint16_t*)(buff_ptr + buff_idx)) = 0x0004; //16 bit transfer length # 32 bit reads
	buff_idx = buff_idx + 2;

	*txWriteBuffAddr = buff_idx;

	return;

}

void CanFdNiosComm::send_CAN_message(uint8_t chan, struct canfd_frame* cf)
{
	int i, data_words;
	uint8_t* buff_ptr;
	uint16_t buff_idx;

	//	fifo_status(bar2, bar3);

	buff_idx = *txWriteBuffAddr;
	buff_ptr = (uint8_t *)txBuffBase;

	printf("CAN RX message %d\n", chan);
	printf("Writing %x, to %x\n", CAN_SEND_FRAME, buff_idx);

	if (cf->len & 3)
		data_words = (cf->len >> 2) + 1; // round up to the next 32 bit word
	else
		data_words = (cf->len >> 2);
	*((volatile uint8_t*)(buff_ptr + buff_idx)) = CAN_SEND_FRAME; // read from avalon mm address
	buff_idx = buff_idx + 1;
	*((volatile uint8_t*)(buff_ptr + buff_idx)) = chan; // channel
	buff_idx = buff_idx + 1;
	*((volatile uint16_t*)(buff_ptr + buff_idx)) = data_words * 4 + 12; //length of this message
	buff_idx = buff_idx + 2;
	*((volatile uint32_t*)(buff_ptr + buff_idx)) = cf->can_id; //
	buff_idx = buff_idx + 4; // 8
	*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->len; //
	buff_idx = buff_idx + 1;
	*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->flags; //
	buff_idx = buff_idx + 1;
	*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->__res0; //
	buff_idx = buff_idx + 1;
	*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->__res1; //
	buff_idx = buff_idx + 1; // 12


	for (i = 0; i < data_words; i++) { // could be optimized to one transfer
		*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->data[i];
		buff_idx = buff_idx + 1;
		*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->data[i + 1];
		buff_idx = buff_idx + 1;
		*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->data[i + 2];
		buff_idx = buff_idx + 1;
		*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->data[i + 3];
		buff_idx = buff_idx + 1;
	}
	*txWriteBuffAddr = buff_idx; // notify
}

void CanFdNiosComm::send_CANFD_message(uint8_t chan, struct canfd_frame* cf)
{
	int i, data_words;
	uint8_t* buff_ptr;
	uint16_t buff_idx;

	//	fifo_status(bar2, bar3);

	buff_idx = *txWriteBuffAddr;
	buff_ptr = (uint8_t*)txBuffBase;

	printf("CAN RX message %d\n", chan);
	printf("Writing %x, to %x\n", CANFD_SEND_FRAME, buff_idx);

	if (cf->len & 3)
		data_words = (cf->len >> 2) + 1; // round up to the next 32 bit word
	else
		data_words = (cf->len >> 2);
	*((volatile uint8_t*)(buff_ptr + buff_idx)) = CANFD_SEND_FRAME; // read from avalon mm address
	buff_idx = buff_idx + 1;
	*((volatile uint8_t*)(buff_ptr + buff_idx)) = chan; // channel
	buff_idx = buff_idx + 1;
	*((volatile uint16_t*)(buff_ptr + buff_idx)) = data_words * 4 + 12; //length of this message
	buff_idx = buff_idx + 2;
	*((volatile uint32_t*)(buff_ptr + buff_idx)) = cf->can_id; //
	buff_idx = buff_idx + 4; // 8
	*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->len; //
	buff_idx = buff_idx + 1;
	*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->flags; //
	buff_idx = buff_idx + 1;
	*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->__res0; //
	buff_idx = buff_idx + 1;
	*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->__res1; //
	buff_idx = buff_idx + 1; // 12


	for (i = 0; i < data_words; i++) { // could be optimized to one transfer
		*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->data[i];
		buff_idx = buff_idx + 1;
		*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->data[i + 1];
		buff_idx = buff_idx + 1;
		*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->data[i + 2];
		buff_idx = buff_idx + 1;
		*((volatile uint8_t*)(buff_ptr + buff_idx)) = cf->data[i + 3];
		buff_idx = buff_idx + 1;
	}
	*txWriteBuffAddr = buff_idx; // notify
}

uint16_t CanFdNiosComm::rcvNiosMessage(uint8_t* msg)
{
	int msgLen = *rxWriteBuffAddr - *rxReadBuffAddr;
	printf("rcvNiosMessage: length= %d from %x to %x\n", msgLen, *rxReadBuffAddr, *rxWriteBuffAddr);
	if (msgLen < 4) {
		printf("nothing to receive!\n");
		return 0;
	}
	uint16_t buff_end = *rxWriteBuffAddr; // where the NIOS is writing
	uint16_t buff_idx = *rxReadBuffAddr; // where we are reading
	uint16_t rem = buff_idx & 0x3;
	if (rem > 0) {
		printf("rcvNiosMessage: Error: Message address not multiple of 4\n");
		*rxReadBuffAddr += 4 - rem;
	}

	printf("rxBuffBase: ");
	for (int i = 0; i < 16; i++) {
		msg[i] = rxBuffBase[*rxReadBuffAddr + i];
		printf("<%x>", msg[i]);
	}
	printf("\n");
	printf("txBuffBase: ");
	for (int i = 0; i < 16; i++) {
		msg[i] = txBuffBase[*txReadBuffAddr + i];
		printf("<%x>", msg[i]);
	}
	printf("\n");

	buff_idx = *rxReadBuffAddr;
	uint8_t cc = rxBuffBase[buff_idx];
	uint16_t len = rxBuffBase[buff_idx + 2];

	*rxReadBuffAddr += msgLen;
	return len;
}