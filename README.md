# CH32V003 lib_uart

`lib_uart` is a light, but fully-featured UART Library for the CH32V003 
Microcontroller.  
The Library supports:
* Support for all Alternative Pinouts (V003)
* Many Baud Rates from `1200` to `921600`
* RX into a configurable Ring Buffer, with interrupts
* Print strings directly to the UART, with or without newlines
* Configurable Word Length, Stopbits, and parity on init

**NOTE:** All configuration flags are in `funconfig.h`  

## TODO
* TX Ring buffer & interrupt method

----
Copyright (c) 2024-2025 ADBeta
