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


#include <stdio.h>


/*** Static Variables ********************************************************/

static uint8_t buf[32] = {0};
// Ring Buffer Buffer TODO: maybe configurable? 
// If not NULL, reading will fill into this buffer with interrupts, if it is 
// NULL, read() will read at real-time, losing un-caught data 
static uint8_t *uart_ring_ptr  = buf;
static size_t   uart_ring_size = 32;
static size_t   uart_ring_head = 0; // write position
static size_t   uart_ring_tail = 0; // read position

/*** Static Functions ********************************************************/
// UART RX interrupt handler. TODO: decide on how the buffer gets filled

/// @breif UART Receiver Interrupt handler - Puts the data received into the
/// UART Ring Buffer
/// @param None
/// @return None
void USART1_IRQHandler(void) __attribute__((interrupt));
void USART1_IRQHandler(void) {
	// Calculate the next write position
	size_t next_head = (uart_ring_head + 1) & (uart_ring_size - 1);

	// If the next position is the same as the tail, TODO:
	if(next_head == uart_ring_tail) 
	{
		// Reject any data that overfills the buffer
		return;
	}

	uint8_t recv = (uint8_t)USART1->DATAR;
	uart_ring_ptr[uart_ring_head] = recv;

	// Update the head position
	uart_ring_head = next_head;
}



static size_t uart_read_buffer(uint8_t *buffer, size_t len)
{
	// Make sure the buffer passed and length are valid
	if(buffer == NULL || len == 0) return 0;

	size_t bytes_read = 0;

	while(len--)
	{
		// If the buffer has no more data, return buffer empty
		if(uart_ring_head == uart_ring_tail) break;

		*buffer++ = uart_ring_ptr[uart_ring_tail];
		uart_ring_tail = (uart_ring_tail + 1) & (uart_ring_size - 1);

		bytes_read++;
	}

	return bytes_read;
}





/*** Initialisers ************************************************************/
uart_err_t uart_init(
	const uint8_t *buffer,
	const size_t buffsize,
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
	USART1->CTLR1 = USART_Mode_Tx | USART_Mode_Rx | wordlength | parity;
	// Set CTLR2 Register (Stopbits)
	USART1->CTLR2 = stopbits;
	// Set CTLR3 Register TODO: Interrupts and flow control
	USART1->CTLR3 = (uint16_t)0x0000;

	// Set the Baudrate, assuming 48KHz
	USART1->BRR = baud;

	// If the RX Buffer is NOT NULL, and the buffer size is NOT 0, set IRQ
	if(buffer != NULL && buffsize != 0)
	{
		// Set static buffer and size variables
		// uart_ring_ptr  = buffer;
		// uart_ring_size = buffsize;
		// uart_ring_head = 0;
		// uart_ring_tail = 0;

		// Enable the IRQ 
		USART1->CTLR1 |= USART_CTLR1_RXNEIE;
		NVIC_EnableIRQ(USART1_IRQn);
	}

	// Enable the UART
	USART1->CTLR1 |= CTLR1_UE_Set;

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
