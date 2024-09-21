#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

#define FUNCONF_USE_DEBUGPRINTF  1
#define CH32V003                 1

// Enable/Disable overwriting of the RX Ring Buffer.
// Enabled:  Incomming data will overwrite older data in the buffer
// Disabled: Incomming data will not be added to the buffer until space is free
#define RX_RING_BUFFER_OVERWRITE

// Pinout        TX        RX        CTS        RTS
// Default       PD5       PD6
#define UART_PINOUT_DEFAULT


#endif

