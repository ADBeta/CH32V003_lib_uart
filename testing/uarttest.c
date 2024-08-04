/******************************************************************************
* UART Library Tester program
*
*
* ADBeta (c)	03 Aug 2024
******************************************************************************/
#include "ch32v003fun.h"
#include "lib_uart.h"
#include <stdio.h>


uint8_t urxb[1024];

int main()
{
	SystemInit();
	
	uart_init(
		urxb,
		1024,
		UART_BAUD_115200,
		UART_WORDLENGTH_8, 
		UART_PARITY_NONE,
		UART_STOPBITS_ONE
	);



	while(1)
	{

		//uart_read(c, 16);



		//uart_write(c, 16);

	}  
	
}
