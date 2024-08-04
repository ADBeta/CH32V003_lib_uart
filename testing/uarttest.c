/******************************************************************************
* UART Library Tester program
*
*
* ADBeta (c)	03 Aug 2024
******************************************************************************/
#include "ch32v003fun.h"
#include "lib_uart.h"
#include <stdio.h>


// Create a pointer to the UART Ring Buffer
//
// If not NULL, reading will fill into this buffer with interrupts, if it is 
// NULL, reading will just grab whatever data is available to it 
static uint8_t *uart_ring_ptr = NULL;
static size_t  uart_ring_pos = 0;


void USART1_IRQHandler(void) __attribute__((interrupt));
void USART1_IRQHandler(void) {
	// NOTE: This was needed at one point, but seems to have fixed itself.
	// Re-enables the RXNE Interrput
	// USART1->CTLR1 |= USART_CTLR1_RXNEIE;
	
	// If there is data available in the Data Register, append it to the 
	// ring buffer
	if(USART1->STATR & USART_FLAG_RXNE)
	{
		char recv = (char)USART1->DATAR;
		uart_write(&recv, 1);
	}
}


int main()
{
	SystemInit();
	
	uart_init(
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
