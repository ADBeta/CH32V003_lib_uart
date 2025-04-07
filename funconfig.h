#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

#define FUNCONF_USE_DEBUGPRINTF  1
#define CH32V003                 1

// Enable/Disable overwriting of the RX Ring Buffer.
// Enabled:  Incomming data will overwrite older data in the buffer
// Disabled: Incomming data will not be added to the buffer until space is free
// #define RX_RING_BUFFER_OVERWRITE

// NOTE: UART_ALTERNATE_1 will disable SWIO (PD1)
//  Pinout        TX        RX        CTS        RTS
//  Default       PD5       PD6       PD3        PC2
//  Alternate 1   PD0       PD1       PC3        PC2
//  Alternate 2   PD6       PD5       PC6        PC7
//  Alternate 3   PC0       PC1       PC6        PC7
#define UART_PINOUT_DEFAULT
//#define UART_PINOUT_ALTERNATE_1
//#define UART_PINOUT_ALTERNATE_2
//#define UART_PINOUT_ALTERNATE_3

#endif

