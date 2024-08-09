/******************************************************************************
* lib_uart
* A simple but full-featured library for UART on the CH32V003
*
* See GitHub for details: https://github.com/ADBeta/CH32V003_lib_uart
*
* ADBeta (c) 2024
******************************************************************************/
#ifndef LIB_UART_H
#define LIB_UART_H

#include "ch32v003fun.h"
#include <stddef.h>

/*** Configuration Flags *****************************************************/
// Enable or disable the RX Ring Buffer. When Disabled, read() will read
// directly from the DATAR Register, using a timeout.
#define RING_BUFFER_ENABLE
//#define RING_BUFFER_DISABLE

// Enable or disable overwriting of the UART Ring Buffer. Rejects new bytes
// when disabled
#define RING_BUFFER_OVERWRITE
#define RING_BUFFER_SIZE 32

// If the ring buffer is disabled, use a timeout of variable (milliseconds)
#define READ_TIMEOUT_MS 100

/*** Typedefs and structures *************************************************/
/// @breif UART Error Values
typedef enum {
	UART_OK              = 0,
	UART_TIMEOUT,
	UART_INVALID_ARGS,
	UART_BUFFER_EMPTY,
} uart_err_t;

/// @breif Defines some commonly used baud rates
/// DIV = (HCLK / (16 * BAUD)) * 16 (HCLK is 48MHz)
typedef enum {
	UART_BAUD_921600 = ((uint16_t)0x0034),    // 923076    0.16% Fast
	UART_BAUD_460800 = ((uint16_t)0x0068),    // 461538    0.16% Fast
	UART_BAUD_230400 = ((uint16_t)0x00D3),    // 227488    1.27% Fast
	UART_BAUD_115200 = ((uint16_t)0x01A1),    // 115107    0.08% Slow
	UART_BAUD_57600  = ((uint16_t)0x0341),    // 57623     0.04% Fast
	UART_BAUD_38400  = ((uint16_t)0x04E2),    // 38400     0.00% 
	UART_BAUD_19200  = ((uint16_t)0x09C4),    // 19200     0.00%
	UART_BAUD_9600   = ((uint16_t)0x1388),    // 9600      0.00%
	UART_BAUD_4800   = ((uint16_t)0x2710),    // 4800      0.00%
	UART_BAUD_2400   = ((uint16_t)0x4E20),    // 2400      0.00%
	UART_BAUD_1200   = ((uint16_t)0x9C40),    // 1200      0.00%
} uart_baudrate_t;

/// @breif UART Word Length Enum
typedef enum {
	UART_WORDLENGTH_8 = ((uint16_t)0x0000),
	UART_WORDLENGTH_9 = ((uint16_t)0x1000),
} uart_wordlength_t;

/// @breif UART Parity Enum
typedef enum {
	UART_PARITY_NONE = ((uint16_t)0x0000),
	UART_PARITY_EVEN = ((uint16_t)0x0400),
	UART_PARITY_ODD  = ((uint16_t)0x0600),
} uart_parity_t;

/// @breif UART Stop Bits Enum
typedef enum {
	UART_STOPBITS_ONE       = ((uint16_t)0x0000),
	UART_STOPBITS_HALF      = ((uint16_t)0x1000),
	UART_STOPBITS_TWO       = ((uint16_t)0x2000),
	UART_STOPBITS_ONE_HALF  = ((uint16_t)0x3000),
} uart_stopbits_t;


// TODO: Control Bits selection


/*** Initialisers ************************************************************/
/// @breif Initiliase the UART peripheral with the passed configuratiion.
/// Uses the default pins (PD5-TX  PD6-RX)
///
/// NOTE: if -buffer- is NULL, or -buffsize- is 0, the driver will fall-back
/// into realtime mode, where data is only read when requested - any data
/// received before the function is called will be lost.
///
/// @param buffer, the uint8_t buffer to use as an RX buffer (Optional)
/// @param buffsize, the size of the RX  Buffer in bytes. 
/// @param baud, buadrate of the interface (921600 - 1200)
/// @param wordlength, interface word length (8 or 9 bits)
/// @param parity, Parity variable (None, Even or Odd)
/// @param stopbits, how many stop bits to transmit (0.5, 1, 2, 1.5)
/// @param config, the uart_config_t configuration struct
/// @return uart_err_t status
uart_err_t uart_init(
	const uart_baudrate_t baud,
	const uart_wordlength_t wordlength,
	const uart_parity_t parity,
	const uart_stopbits_t stopbits
);


/*** Write *******************************************************************/
/// @breif writes raw bytes to the UART
/// @param buffer, raw buffer, can be any type
/// @param size, number of bytes to transmit.
/// @return uart_err_t status
uart_err_t uart_write(const void *buffer, const size_t size);

/// @breif Prints a string to the UART, without any added ternination
/// @param string, input c string to print
/// @return uart_err_t status
uart_err_t uart_print(const char *string);

/// @breif Prints a string to the UART, and adds termination \r\n characters
/// @param string, input c string to print
/// @return uart_err_t status
uart_err_t uart_println(const char *string);


/** Read *********************************************************************/
/// @breif reads the specified number of bytes
/// @param buffer, buffer to read bytes into
/// @param size, numbe rof bytes to read
/// @return size_t number of bytes read
size_t uart_read(uint8_t *buffer, size_t len);

#endif
