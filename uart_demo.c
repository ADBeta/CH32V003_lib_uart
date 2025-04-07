/******************************************************************************
* UART Library Tester program
* lib_uart is under the MIT License. See LICENSE for more information
*
* ADBeta (c)	07 Apr 2025    v2.1
******************************************************************************/
#include "ch32fun.h"
#include "lib_uart.h"
#include <stdio.h>


// UART Ring Buffer, change size if needed
#define UART_BUFFER_SIZE 32
uint8_t ring_buffer[UART_BUFFER_SIZE];

int main(void)
{
	SystemInit();

	// Create a UART Configuration 
	uart_config_t uart_conf = {
		.baudrate    = UART_BAUD_9600,
		.wordlength  = UART_WORDLENGTH_8,
		.parity      = UART_PARITY_NONE,
		.stopbits    = UART_STOPBITS_ONE,
		.flowctrl    = UART_FLOWCTRL_NONE,
	};

	uart_err_t err_state;

	// Initialise the UART
	err_state = uart_init(ring_buffer, UART_BUFFER_SIZE, &uart_conf );
	if(err_state != UART_OK)
	{
		printf("Failed to Initialise the UART\n");
		return 1;
	}
	
	// Simple string printing example - all return uart_err_t error codes
	uart_print("This string will be one line 1");
	uart_println(" -- This string will be on the same line");
	uart_println("This string will be on its own line");

	// Loop forever, echoing UART data back to the sender
	while(1)
	{
		// Declare an array as filled with 0x00.
		uint8_t user_buffer[UART_BUFFER_SIZE] = {0x00};

		// Read maximum [UART_BUFFER_SIZE] bytes from the ring buffer, into
		// the user buffer. Returns the number of bytes actualy read
		size_t bytes_read = uart_read(user_buffer, UART_BUFFER_SIZE);

		// Only print/modify data if there was some read
		if(bytes_read != 0)
		{
			// Replace any \r with \r\n.
			// NOTE: This WILL corrupt data. It is only to allow the user to 
			// type normally and have newlines
			for(uint8_t chr = 0; chr < UART_BUFFER_SIZE - 1; chr++)
			{
				if(user_buffer[chr] == '\r')
				{
					user_buffer[chr + 1] = '\n';
					bytes_read++;
				}
			}

			// Write the number of bytes read to the UART
			uart_write(user_buffer, bytes_read);
		}
	}
	
	return 0;
}
