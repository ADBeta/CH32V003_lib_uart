/******************************************************************************
* lib_uart - A simple but full-featured library for UART on the CH32V003
*
* See GitHub for more information: 
* https://github.com/ADBeta/CH32V003_lib_uart
* 
* 21 Sep 2024    Version 4.9
*
* Released under the MIT Licence
* Copyright ADBeta (c) 2024
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the 
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
* sell copies of the Software, and to permit persons to whom the Software is 
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in 
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
* USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/
#ifndef LIB_UART_H
#define LIB_UART_H

#include "ch32v003fun.h"
#include <stddef.h>
#include <stdbool.h>

/*** Configuration Flags *****************************************************/
/*
NOTE: Set these flags in funconfig.h. 
  Enable/Disable overwriting of the RX Ring Buffer.
  Enabled:  Incomming data will overwrite older data in the buffer
  Disabled: Incomming data will not be added to the buffer until space is free
  
  #define RX_RING_BUFFER_OVERWRITE
  
  
  Pinout        TX        RX        CTS        RTS
  Default       PD5       PD6
  
  #define UART_PINOUT_DEFAULT
*/

/*** Typedefs and defines  ***************************************************/
// Default Pinout Variables
#ifdef UART_PINOUT_DEFAULT
	#define UART_PORT_RCC   RCC_APB2Periph_GPIOD
	#define UART_PORT       GPIOD
	#define UART_PIN_TX     5
	#define UART_PIN_RX     6
#endif


/// @brief UART Error Values
typedef enum {
	UART_OK              = 0,
	UART_INVALID_ARGS,
	UART_NOT_INITIALIZED,
	UART_TIMEOUT,
	UART_BUFFER_EMPTY,
} uart_err_t;


/// @brief Defines some commonly used baud rates
/// DIV = (HCLK / (16 * BAUD)) * 16 (HCLK is 48MHz)
typedef enum {                                // Actual    Delta %
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


/// @brief UART Word Length Enum
typedef enum {
	UART_WORDLENGTH_8 = ((uint16_t)0x0000),
	UART_WORDLENGTH_9 = ((uint16_t)0x1000),
} uart_wordlength_t;


/// @brief UART Parity Enum
typedef enum {
	UART_PARITY_NONE = ((uint16_t)0x0000),
	UART_PARITY_EVEN = ((uint16_t)0x0400),
	UART_PARITY_ODD  = ((uint16_t)0x0600),
} uart_parity_t;


/// @brief UART Stop Bits Enum
typedef enum {
	UART_STOPBITS_ONE       = ((uint16_t)0x0000),
	UART_STOPBITS_HALF      = ((uint16_t)0x1000),
	UART_STOPBITS_TWO       = ((uint16_t)0x2000),
	UART_STOPBITS_ONE_HALF  = ((uint16_t)0x3000),
} uart_stopbits_t;

/// @brief UART Flow Control Masks
typedef enum {
	UART_FLOWCTRL_NONE      = ((uint16_t)0x0000),
	UART_FLOWCTRL_CTS       = ((uint16_t)0x0200),
	UART_FLOWCTRL_RTS       = ((uint16_t)0x0100),
	UART_FLOWCTRL_CTS_RTS   = ((uint16_t)0x0300)

} uart_flowctrl_t;


/// @brief UART Configuration Struct
// TODO: Break bit?
typedef struct {
	uart_baudrate_t    baudrate;
	uart_wordlength_t  wordlength;
	uart_parity_t      parity;
	uart_stopbits_t    stopbits;
	uart_flowctrl_t    flowctrl;
} uart_config_t;


/// @brief UART Ring Buffer Struct. Not user-modifyable. Only used internally
typedef struct {
	uint8_t   *buffer;
	uint32_t  size;
	volatile uint32_t  head;
	volatile uint32_t  tail;
} _uart_buffer_t;


/*** Initialisers ************************************************************/
/// @brief Initiliase the UART peripheral with the passed configuratiion.
/// @param uint8_t *, pointer to the rx ring buffer
/// @param uint32_t, size of the RX ring buffer
/// @param uart_config_t, UART configuration
/// @return uart_err_t, UART_NOT_INITIALIZED on error
uart_err_t uart_init( const uint8_t *rx_buffer_ptr,
					  const uint32_t rx_buffer_size,
					  const uart_config_t *conf    
);


/*** Write *******************************************************************/
/// @brief writes raw bytes to the UART
/// @param buffer, raw buffer, can be any type
/// @param size, number of bytes to transmit.
/// @return uart_err_t status
uart_err_t uart_write(const void *buffer, size_t size);

/// @brief Prints a string to the UART, without any added ternination
/// @param string, input c string to print
/// @return uart_err_t status
uart_err_t uart_print(const char *string);

/// @brief Prints a string to the UART, and adds termination \r\n characters
/// @param string, input c string to print
/// @return uart_err_t status
uart_err_t uart_println(const char *string);


/** Read *********************************************************************/
/// @brief reads len number of bytes from the RX Ring Buffer.
/// @param *buffer, the buffer to read to
/// @param len, the maximum number of bytes to read to the buffer
/// @return size_t number of bytes read
size_t uart_read(uint8_t *buffer, size_t len);

/*
/// @brief reads from the RX Ring Buffer until it finds a newline delimiter
/// (\n or \r) then a non-delim char, or until it has read -len- bytes.
/// Ring Buffer method is only enabled when RING_BUFFER_ENABLE is defined.
/// @param *buffer, the buffer to read to
/// @param len, the maximum number of bytes to read to the buffer
/// @return size_t number of bytes read
size_t uart_readln(uint8_t *buffer, size_t len)
{
}
*/

#endif
