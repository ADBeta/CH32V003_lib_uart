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

#define UART_BUFFER_SIZE 16

// The UART RX Ring Buffer (Must be > 0. Must be a Power of 2)
uint8_t buff[UART_BUFFER_SIZE] = {0};
uint8_t uart_rx_buffer[UART_BUFFER_SIZE] = {0x00};

int main()
{
	SystemInit();


	uart_config_t uart_conf = {
		.baudrate    = UART_BAUD_115200,
		.wordlength  = UART_WORDLENGTH_8,
		.parity      = UART_PARITY_NONE,
		.stopbits    = UART_STOPBITS_ONE,
		.flowctrl    = UART_FLOWCTRL_NONE,
	};

	// Init the UART system. See `lib_uart.h` for baud, and other config vars
	uart_init(
		uart_rx_buffer,
		UART_BUFFER_SIZE,
		&uart_conf
	);

	// Simple string printing example
	uart_print("This string will be one line 1");
	uart_println(" -- This string will be on the same line");
	uart_println("This string will be on its own line");

	while(1)
	{
		// Clear the read buffer
		memset(buff, 0x00, UART_BUFFER_SIZE);

		// Read up to 128 bytes into the UART Buffer.
		// Returns the number of bytes actualy read
		size_t bytes_read = uart_read(buff, UART_BUFFER_SIZE);

		
		// Only print/modify data if there was some read
		if(bytes_read != 0)
		{
			// Replace any \r with \r\n.
			// NOTE: This WILL corrupt data. It is only to allow the user to 
			// type normally and have newlines
			for(uint8_t chr = 0; chr < UART_BUFFER_SIZE - 1; chr++)
			{
				if(buff[chr] == '\r')
				{
					buff[chr + 1] = '\n';
					bytes_read++;
				}
			}

			// Write the number of bytes read to the UART
			uart_write((uint8_t *)buff, bytes_read);
		}
	}
	
}
