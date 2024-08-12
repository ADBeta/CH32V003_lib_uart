# CH32V003_lib_uart

## **NOTE:** This library requires use of the ch32v003fun library  
lib_uart is a light and simple UART Library for the CH32V003 (and related) 
Microcontrollers.  
The Library supports
* Multiple Baud Rates from `1200` to `921600`
* RX into a configurable Ring Buffer (Default configuration)
* RX in realtime, using timeouts (Optional configuration)
* Print strings directly to the UART, with or without newlines
* Configurable Word Length, Stopbits, and parity on init

See `lib_uart.h` for configuration flags

## How to use
Simply add `lib_uart.h` and `lib_uart.c` to your project files, `#include "lib_uart.h"`.  
See `/testing/uarttest.c` for a basic usage example of the library

----
Copyright (c) 2024 ADBeta
