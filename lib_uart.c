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

/*** Macro Functions *********************************************************/
#define IS_POWER_OF_2(x) (((x) != 0) && (((x) & ((x) - 1)) == 0))

/*** Configuration ***********************************************************/
// Make sure only one ring buffer setting is selected
#if defined(RING_BUFFER_ENABLE) && defined(RING_BUFFER_DISABLE)
	#error "CONFIG ERROR: Ring Buffer is enabled and disabled simultaniously"
#endif
// Make sure at least one ring buffer is selected
#if !defined(RING_BUFFER_ENABLE) && !defined(RING_BUFFER_DISABLE)
	#error "CONFIG ERROR: Must define one of RING_BUFFER_ENABLE or RING_BUFFER_DISABLE"
#endif

/*** Ring Buffer Enabled ***/
// If the Ring Buffer is enabled, configure it
#ifdef RING_BUFFER_ENABLE

// Make sure the size of the buffer is not 0, and is a power of 2
#if !IS_POWER_OF_2(RING_BUFFER_SIZE)
	#error "CONFIG ERROR: Ring Buffer Size must be a Power of 2"
#endif

// Create the buffer, head/tail, reset mask and other values
static const size_t uart_ring_mask = RING_BUFFER_SIZE - 1;
static uint8_t      uart_ring_buff[RING_BUFFER_SIZE];
static size_t       uart_ring_head = 0;
static size_t       uart_ring_tail = 0;

// define and declare the UART IRQ Function
/// @breif UART Receiver Interrupt handler - Puts the data received into the
/// UART Ring Buffer
/// @param None
/// @return None
void USART1_IRQHandler(void) __attribute__((interrupt));
void USART1_IRQHandler(void)
{
	// Read from the DATAR Register to reset the flag
	uint8_t recv = (uint8_t)USART1->DATAR;

	// Calculate the next write position
	size_t next_head = (uart_ring_head + 1) & uart_ring_mask;

	// If the next position is the same as the tail, either reject the new data
	// or overwrite old data
	if(next_head == uart_ring_tail) 
	{
		#ifdef RING_BUFFER_OVERWRITE
			// Incriment the tail position
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

#endif


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
	
	for(const char *c = string; *c; ++c)
	{
		// Wait for the current transmission to finish
		while(!(USART1->STATR & USART_FLAG_TC));
		USART1->DATAR = *c;
	}

	return UART_OK;
}


uart_err_t uart_println(const char *string)
{
	uart_err_t fres = uart_print(string);
	if(fres != UART_OK) return fres;

	// Print the terminating characters
	while(!(USART1->STATR & USART_FLAG_TC));
	USART1->DATAR = '\r';
	while(!(USART1->STATR & USART_FLAG_TC));
	USART1->DATAR = '\n';

	return UART_OK;
}


/*** Read ********************************************************************/
#ifdef RING_BUFFER_ENABLE
/// @breif reads len number of bytes from the RX Ring Buffer. 
/// Ring Buffer method is only enabled when RING_BUFFER_ENABLE is deinfed.
/// @param *buffer, the buffer to read to
/// @param len, the maximum number of bytes to read to the buffer
/// @return size_t number of bytes read
size_t uart_read(uint8_t *buffer, size_t len)
{
	// Make sure the buffer passed and length are valid
	if(buffer == NULL || len == 0) return 0;

	size_t bytes_read = 0;
	while(len--)
	{
		// If the buffer has no more data, return how many bytes were read
		if(uart_ring_head == uart_ring_tail) break; 
		
		// Add the current tail byte to the buffer
		*buffer++ = uart_ring_buff[uart_ring_tail];
		// Incriment the ring buffer tail position
		uart_ring_tail = (uart_ring_tail + 1) & uart_ring_mask;
		// Incriment the count of bytes
		bytes_read++;
	}

	return bytes_read;
}

/// @breif reads from the RX Ring Buffer until it finds a newline delimiter
/// (\n or \r) then a non-delim char, or until it has read -len- bytes.
/// Ring Buffer method is only enabled when RING_BUFFER_ENABLE is defined.
/// @param *buffer, the buffer to read to
/// @param len, the maximum number of bytes to read to the buffer
/// @return size_t number of bytes read
/*
size_t uart_readln(uint8_t *buffer, size_t len)
{
}
*/
#endif


#ifdef RING_BUFFER_DISABLE
/// @breif reads len number of bytes in realtime, until buffer is full, or
/// the timeout is reached.
/// @param *buffer, the buffer to read to
/// @param len, the maximum number of bytes to read to the buffer
/// @return size_t number of bytes read
size_t uart_read(uint8_t *buffer, size_t len)
{
	// Make sure the buffer passed and length are valid
	if(buffer == NULL || len == 0) return 0;

	size_t bytes_read = 0;
	while(len--)
	{
		// TODO: Use systick or current_millis to timeout
		// Wait for a byte to be in the buffer. If it exceeds timeout, 
		// exit the function. Uses a poor timout method for now. Will fix when
		// there is a current_millis() fumction.
		uint32_t timeout_ticks = 0;
		while(!(USART1->STATR & USART_FLAG_RXNE))
		{
			if(timeout_ticks++ == READ_TIMEOUT_MS) return bytes_read;
			Delay_Ms(1);
		}

		*buffer++ = (uint8_t)USART1->DATAR;
		// Incriment the count of bytes
		bytes_read++;
	}

	return bytes_read;
}

/// @breif reads from the RX Ring Buffer until it finds a newline delimiter
/// (\n or \r) then a non-delim char, or until it has read -len- bytes.
/// Ring Buffer method is only enabled when RING_BUFFER_ENABLE is defined.
/// @param *buffer, the buffer to read to
/// @param len, the maximum number of bytes to read to the buffer
/// @return size_t number of bytes read
/*
size_t uart_readln(uint8_t *buffer, size_t len)
{
}
*/
#endif
