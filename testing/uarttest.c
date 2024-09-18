/******************************************************************************
* UART Library Tester program
* lib_uart is under the MIT License. See LICENSE for more information
*
* ADBeta (c)	18 Sep 2024
******************************************************************************/
#include "ch32v003fun.h"
#include "lib_uart.h"
#include <string.h>
#include <stdio.h>

// The UART RX Ring Buffer (Must be > 0. Must be a Power of 2)
char uart_rx_buffer[128] = {0};

int main()
{
	SystemInit();


	uart_config_t uart_config = {
		.baudrate    = UART_BAUD_115200,
		.wordlength  = UART_WORDLENGTH_8,
		.parity      = UART_PARITY_NONE,
		.stopbits    = UART_STOPBITS_ONE,
		.cts         = false,
		.rts         = false,
	};

	// Init the UART system. See `lib_uart.h` for baud, and other config vars
	uart_init(
		(uint8_t *)uart_rx_buffer,
		128,
		&uart_config
	);

	// Simple string printing example
	uart_print("This string will be one line 1");
	uart_println(" -- This string will be on the same line");
	uart_println("This string will be on its own line");

	while(1)
	{
		// Clear the read buffer
		memset(uart_rx_buffer, 0x00, 128);

		// Read up to 128 bytes into the UART Buffer.
		// Returns the number of bytes actualy read
		size_t bytes_read = uart_read((uint8_t *)uart_rx_buffer, 128);

		// Only print/modify data if there was some read
		if(bytes_read != 0)
		{
			// Replace any \r with \r\n.
			// NOTE: This WILL corrupt data. It is only to allow the user to 
			// type normally and have newlines
			for(uint8_t chr = 0; chr < 127; chr++)
			{
				if(uart_rx_buffer[chr] == '\r') uart_rx_buffer[chr + 1] = '\n';
			}

			// Write the number of bytes read to the UART
			uart_write((uint8_t *)uart_rx_buffer, 128);
		}
	}  
	
}
