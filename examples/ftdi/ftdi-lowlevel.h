#ifndef __FTDI_LOWLEVEL_h__
#define __FTDI_LOWLEVEL_h__

struct ftdi_mpsse_data_command {
	uint8_t negativeCLKonWrite 	: 1;
	uint8_t bitMode			: 1;
	uint8_t negativeCLKonRead	: 1;
	uint8_t lsbFirst		: 1;
	uint8_t writeTDIDO		: 1;
	uint8_t readTDODI		: 1;
	uint8_t writeTMSCS		: 1;
	uint8_t	cmdMode			: 1;
};

#define	NEGATIVE_CLK_ON_WRITE		(1 << 0)
#define BIT_MODE			(1 << 1)
#define NEGATIVE_CLK_ON_READ		(1 << 2)
#define LSB_FIRST			(1 << 3)
#define WRITE_TDI_DO			(1 << 4)
#define READ_TDO_DI			(1 << 5)
#define WRITE_TMS_CS			(1 << 6)

#define SET_DATA			(1 << 7)

struct ftdi_mpsse_request_shift_bits {
	struct ftdi_mpsse_data_shift_command cmd; // Make sure BIT_MODE is set
};

/*
If the device detects a bad command it will send back 2 bytes to the PC:
0xFA,
followed by the bad command byte.
*/

struct ftdi_mpsse_request {
	struct ftdi_mpsse_data_shift_command cmd;
	union ftdi_mpsse_data {
		struct ftdi_mpsse_request_shift_bits {
			BYTE bits; // Number of bits -- 00 == 1 bit, 0x07 == 8 bits!?
			BYTE data; // Bits to shift in/out
		} bit_request;
		struct ftdi_mpsse_request_shift_bytes {
			DWORD length;
			BYTE data[];
		} byte_request;
	} data;
};

/**
 * IN Endpoint
 *
 * The device reserves the first two bytes of data on this endpoint to contain
 * the current values of the modem and line status registers. In the absence of
 * data, the device generates a message consisting of these two status bytes
 * every 40 ms
 *
 * Byte 0: Modem Status
 *
 * Offset       Description
 * B0   Reserved - must be 1
 * B1   Reserved - must be 0
 * B2   Reserved - must be 0
 * B3   Reserved - must be 0
 * B4   Clear to Send (CTS)
 * B5   Data Set Ready (DSR)
 * B6   Ring Indicator (RI)
 * B7   Receive Line Signal Detect (RLSD)
 *
 * Byte 1: Line Status
 *
 * Offset       Description
 * B0   Data Ready (DR)
 * B1   Overrun Error (OE)
 * B2   Parity Error (PE)
 * B3   Framing Error (FE)
 * B4   Break Interrupt (BI)
 * B5   Transmitter Holding Register (THRE)
 * B6   Transmitter Empty (TEMT)
 * B7   Error in RCVR FIFO
 *
 */

// #define FTDI_STATUS_B0_MASK     (FTDI_RS0_CTS | FTDI_RS0_DSR | FTDI_RS0_RI | FTDI_RS0_RLSD)
// #define FTDI_STATUS_B1_MASK     (FTDI_RS_BI)
/*
struct ftdi_uart_status {
	BYTE // status byte 1
	BYTE // status byte 2?
};
*/

/*
 * OUT Endpoint
 *
 * This device reserves the first bytes of data on this endpoint contain the
 * length and port identifier of the message. For the FTDI USB Serial converter
 * the port identifier is always 1.
 *
 * Byte 0: Line Status
 *
 * Offset       Description
 * B0   Reserved - must be 1
 * B1   Reserved - must be 0
 * B2..7        Length of message - (not including Byte 0)
 *
 */

#endif // __FTDI_LOWLEVEL_h__
