/******************************************************************************
* lib_uart
* A simple but full-featured library for UART on the CH32V003
*
* See GitHub for details: https://github.com/ADBeta/CH32V003_lib_uart
*
* ADBeta (c) 2024
******************************************************************************/
#include "lib_uart.h"
#include "ch32v003fun.h"

#include <stddef.h>
#include <string.h>

// TODO:
#include <stdio.h>

/*** Static Variables ********************************************************/
// Ring Buffer Buffer 

/*** Static Functions ********************************************************/

/*** Initialisers ************************************************************/
uart_err_t uart_init(
	const uart_baudrate_t baud, 
	const uart_wordlength_t wordlength,
	const uart_parity_t parity, 
	const uart_stopbits_t stopbits)
{
	// Enable GPIOD and UART1 Clock
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1;

	// Set the RX and TX Pins on PORTD. RX INPUT_FLOATING, TX 10MHz PP AF
	GPIOD->CFGLR &= ~((0x0F << (4*6)) | (0x0F << (4*5)));  // Clear PD6 & PD5
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF) << (4*5); // PD5 TX
	GPIOD->CFGLR |= (GPIO_CNF_IN_FLOATING << (4*6));                  // PD6 RX
	
	// Set CTLR1 Register (Enable RX & TX, set Word Length and Parity)
	USART1->CTLR1 = (uint16_t)0x0000 | USART_Mode_Tx | USART_Mode_Rx |
		                               wordlength | parity;
	// Set CTLR2 Register (Stopbits)
	USART1->CTLR2 = (uint16_t)0x0000 | stopbits;
	// Set CTLR3 Register TODO: Interrupts and flow control
	USART1->CTLR3 = (uint16_t)0x0000;

	// Set the Baudrate, assuming 48KHz
	USART1->BRR = baud;

	// Enable the UART
	USART1->CTLR1 |= CTLR1_UE_Set;


	// TODO:
	USART1->CTLR1 |= USART_CTLR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);


	return UART_OK;
}



/*** Write *******************************************************************/
uart_err_t uart_write(const void *buffer, size_t size)
{
	if(buffer == NULL || size == 0) return UART_INVALID_ARGS;

	// Cast the input to a uint8_t
	const uint8_t *bytes = (const uint8_t *)buffer;
	// Send each byte
	while(size--)
	{
		// Wait for the current transmission to finish
		while(!(USART1->STATR & USART_FLAG_TC));
		USART1->DATAR = *bytes++;
	}

	return UART_OK;
}


uart_err_t uart_print(const char *string)
{
	if(string == NULL) return UART_INVALID_ARGS;
	
	size_t len = strlen(string);
	while(len--)
	{
		// Wait for the current transmission to finish
		while(!(USART1->STATR & USART_FLAG_TC));
		USART1->DATAR = *string++;
	}

	return UART_OK;
}


uart_err_t uart_println(const char *string)
{
	if(string == NULL) return UART_INVALID_ARGS;

	size_t len = strlen(string);
	while(len--)
	{
		// Wait for the current transmission to finish
		while(!(USART1->STATR & USART_FLAG_TC));
		USART1->DATAR = *string++;
	}

	// Print the terminating characters
	while(!(USART1->STATR & USART_FLAG_TC));
	USART1->DATAR = '\r';
	while(!(USART1->STATR & USART_FLAG_TC));
	USART1->DATAR = '\n';

	return UART_OK;
}


/*** Read ********************************************************************/
uart_err_t uart_read(void *buffer, const size_t size)
{
	if(buffer == NULL || size == 0) return UART_INVALID_ARGS;

	uint8_t *bytes = (uint8_t *)buffer;

	// Keep track of many bytes have been read
	// TODO: Timeout
	size_t read = 0;
	while(read < size)
	{
		// Wait for a byte to be in the buffer
		while(!(USART1->STATR & USART_FLAG_RXNE));
		*bytes++ = (uint8_t)USART1->DATAR;
		++read;
	}

	return UART_OK;
}
