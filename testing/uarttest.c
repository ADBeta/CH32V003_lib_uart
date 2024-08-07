/******************************************************************************
* UART Library Tester program
*
*
* ADBeta (c)	03 Aug 2024
******************************************************************************/
#include "ch32v003fun.h"
#include "lib_uart.h"
#include <stdio.h>



int main()
{
	SystemInit();

	uint8_t uart_buffer[32] = {0};
	uart_init(
		uart_buffer,
		32,
		UART_BAUD_115200,
		UART_WORDLENGTH_8, 
		UART_PARITY_NONE,
		UART_STOPBITS_ONE
	);



	while(1)
	{
		uint8_t sec[32] = {0};
		size_t read = uart_read(sec, 32);
		printf("read %d bytes: ", read);

		for(int x = 0; x < 32; x++)
		{
			printf("%c", sec[x]);
		}

		printf("\n");
		Delay_Ms(2000);


	}  
	
}
