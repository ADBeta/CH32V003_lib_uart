/******************************************************************************
* lib_uart - A simple but full-featured library for UART on the CH32V003
*
* See GitHub for more information: 
* https://github.com/ADBeta/CH32V003_lib_uart
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
#include "lib_uart.h"

#include "ch32v003fun.h"
#include <stddef.h>

/*** Static Variables ********************************************************/
_uart_buffer_t _rx_buffer = {NULL, 0,0,0};
//_uart_buffer_t

/*** IRQ Handler for UART ****************************************************/
/// TODO: 
/// @brief UART Receiver Interrupt handler - Puts the data received into the
/// UART Ring Buffer
/// @param None
/// @return None
void USART1_IRQHandler(void) __attribute__((interrupt));
void USART1_IRQHandler(void)
{
	if(USART1->STATR & USART_RXNE) 
	{
		// TODO: Only if receiver interrput
		// Read from the DATAR Register to reset the flag
		uint8_t recv = (uint8_t)USART1->DATAR;

		// Calculate the next write position
		size_t next_head = (uart_ring_head + 1) & uart_ring_mask;

		// If the next position is the same as the tail, either reject the new data
		// or overwrite old data
		if(next_head == uart_ring_tail) 
		{
			#ifdef RING_BUFFER_OVERWRITE
				// Increment the tail position
				uart_ring_tail = (uart_ring_tail + 1) & uart_ring_mask;
			#else
				// Reject any data that overfills the buffer
				return;
			#endif
		}

		// Add the received data to the current head position
		uart_ring_buff[uart_ring_head] = recv;
		// Update the head position
		uart_ring_head = next_head;
	}
}

/*** Initialiser *************************************************************/
void uart_init(
	const uart_buffer_t *rx_buffer,
	const uart_baudrate_t baud,
	const uart_wordlength_t wordlength,
	const uart_parity_t parity,
	const uart_stopbits_t stopbits)
{
	// TODO: Set up rx buffer


	// Enable UART1 Clock
	RCC->APB2PCENR |= RCC_APB2Periph_USART1;

	// TODO: GPIOD port clock enable

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

	// If the Ring Buffer is enabled, enable the UART RXNE Interrupt
	#ifdef RING_BUFFER_ENABLE
	USART1->CTLR1 |= USART_CTLR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);
	#endif

	// Enable the UART
	USART1->CTLR1 |= CTLR1_UE_Set;
}

/*** Write *******************************************************************/
uart_err_t uart_write(const void *buffer, size_t size)
{
	uart_err_t ret_err = UART_INVALID_ARGS;
	
	if(buffer != NULL && size != 0) 
	{
		// Cast the input to a uint8_t
		const uint8_t *bytes = (const uint8_t *)buffer;
		// Send each byte
		while(size--)
		{
			// Wait for the current transmission to finish
			while(!(USART1->STATR & USART_FLAG_TC));
			USART1->DATAR = *bytes++;
		}	
		ret_err = UART_OK;
	}

	return ret_err;
}


uart_err_t uart_print(const char *string)
{
	uart_err_t ret_err = UART_INVALID_ARGS;

	if(string != NULL)
	{
		while(*string != '\0')
		{
			// Wait for the current transmission to finish
			while(!(USART1->STATR & USART_FLAG_TC));
			USART1->DATAR = *string++;
		}
		ret_err = UART_OK;
	}

	return ret_err;
}


uart_err_t uart_println(const char *string)
{
	// Catches NULL input also
	uart_err_t ret_err = uart_print(string);
	
	if(ret_err == UART_OK);
	{
		// Print the terminating characters
		while(!(USART1->STATR & USART_FLAG_TC));
		USART1->DATAR = '\r';
		while(!(USART1->STATR & USART_FLAG_TC));
		USART1->DATAR = '\n';
	}

	return ret_err;
}


/** Read *********************************************************************/
size_t uart_read(uint8_t *buffer, size_t len)
{
	size_t bytes_read = 0;

	// Make sure the buffer passed and length are valid
	if(buffer != NULL && len != 0)
	{
		while(len--)
		{
			// TODO: convert to new buffer method
			// If the buffer has no more data, return how many bytes were read
			if(uart_ring_head == uart_ring_tail) break; 
		
			// Add the current tail byte to the buffer
			*buffer++ = uart_ring_buff[uart_ring_tail];
			// Increment the ring buffer tail position
			uart_ring_tail = (uart_ring_tail + 1) & uart_ring_mask;
			// Increment the count of bytes
			bytes_read++;
		}
	}

	return bytes_read;
}

