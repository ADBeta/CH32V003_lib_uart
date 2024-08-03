/******************************************************************************
* UART Library Tester program
*
*
* ADBeta (c)	03 Aug 2024
******************************************************************************/
#include "ch32v003fun.h"
#include <stdio.h>
#include <string.h> // TODO: strlen only is needed


// TODO: RX Interupt


/// @breif Defines some commonly used baud rates
/// DIV = (HCLK / (16 * BAUD)) * 16 (HCLK is 48MHz)
typedef enum {
	UART_BAUD_921600 = ((uint16_t)0x0034),    // 923076    0.16% Fast
	UART_BAUD_460800 = ((uint16_t)0x0068),    // 461538    0.16% Fast
	UART_BAUD_230400 = ((uint16_t)0x00D3),    // 227488    1.27% Fast
	UART_BAUD_115200 = ((uint16_t)0x01A1),    // 115107    0.08% Slow
	UART_BAUD_57600  = ((uint16_t)0x0341),    // 57623     0.04% Fast
	UART_BAUD_38400  = ((uint16_t)0x04E2),    // 38400     0.00% 
	UART_BAUD_19200  = ((uint16_t)0x09C4),    // 19200     0.00%
	UART_BAUD_9600   = ((uint16_t)0x1388),    // 9600      0.00%
	UART_BAUD_4800   = ((uint16_t)0x2710),    // 4800      0.00%
	UART_BAUD_2400   = ((uint16_t)0x4E20),    // 2400      0.00%
	UART_BAUD_1200   = ((uint16_t)0x9C40),    // 1200      0.00%
} uart_baudrate_e;

/// @breif UART Word Length Enum
typedef enum {
	UART_WORDLENGTH_8 = ((uint16_t)0x0000),
	UART_WORDLENGTH_9 = ((uint16_t)0x1000),
} uart_wordlength_e;

/// @breif UART Parity Enum
typedef enum {
	UART_PARITY_NONE = ((uint16_t)0x0000),
	UART_PARITY_EVEN = ((uint16_t)0x0400),
	UART_PARITY_ODD  = ((uint16_t)0x0600),
} uart_parity_e;

/// @breif UART Stop Bits Enum
typedef enum {
	UART_STOPBITS_ONE       = ((uint16_t)0x0000),
	UART_STOPBITS_HALF      = ((uint16_t)0x1000),
	UART_STOPBITS_TWO       = ((uint16_t)0x2000),
	UART_STOPBITS_ONE_HALF  = ((uint16_t)0x3000),
} uart_stopbits_e;


// TODO: Control Bits selection

/// @breif Initiliase the UART peripheral with the passed configuration
/// vairables. Uses the default pins (PD5-TX  PD6-RX)
/// @param baud, buadrate of the interface (921600 - 1200)
/// @param wordlength, interface word length (8 or 9 bits)
/// @param parity, Parity variable (None, Even or Odd)
/// @param stopbits, how many stop bits to transmit (0.5, 1, 2, 1.5)
/// @return None TODO: Flags
void uart_init(const uart_baudrate_e baud, const uart_wordlength_e wordlength,
			   const uart_parity_e parity, const uart_stopbits_e stopbits);

/// @breif Prints a string to the UART, without any added ternination
/// @param string, input c string to print
/// @return None TODO: flags
void uart_print(const char *string);


void uart_print(const char *string)
{
	uint32_t len = strlen(string);
	while(len--)
	{
		// Wait for the current transmission to finish
		while(!(USART1->STATR & USART_FLAG_TC));
		USART1->DATAR = *string++;
	}
}


void uart_init(const uart_baudrate_e baud, const uart_wordlength_e wordlength,
			   const uart_parity_e parity, const uart_stopbits_e stopbits)
{
	// Enable GPIOD and UART1 Clock
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1;

	// Set D5 to 10MHz Push-Pull, Alternate Function Mode
	GPIOD->CFGLR &= ~((0x0F << (4*6)) | (0x0F << (4*5)));  // Clear PD6 & PD5
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF) << (4*5); // PD5 TX
	GPIOD->CFGLR |= (GPIO_CNF_IN_FLOATING << (4*6));                  // PD6 RX
	
	// Set CTLR1 Register	
	USART1->CTLR1 = (uint16_t)0x0000 | USART_Mode_Tx | USART_Mode_Rx |
		                               wordlength | parity;
	// Set CTLR2 Register
	USART1->CTLR2 = (uint16_t)0x0000 | stopbits;
	// Set CTLR3 Register TODO: Interrupts and flow control
	USART1->CTLR3 = (uint16_t)0x0000;

	// Set the Baudrate NOTE: Assumes 48MHz clock
	USART1->BRR = baud;

	// Enable the UART
	USART1->CTLR1 |= CTLR1_UE_Set;
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
		uart_print("Hello world");


		/*
		if( (USART1->STATR & USART_FLAG_RXNE) != 0)
		{
			char c = (char)USART1->DATAR;
			printf("%c", c);

			while( !(USART1->STATR & USART_FLAG_TC));
	    	USART1->DATAR = c;
		}
		*/

	}  
	
}
