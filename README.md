# CH32V003_lib_uart

## **NOTE:** This library requires use of the ch32v003fun library  
`lib_uart` is a light, but fully-featured UART Library for the CH32V003 
Microcontroller Series.  
The Library supports
* Support for all Alternative Pinouts (V003)
* Many Baud Rates from `1200` to `921600`
* RX into a configurable Ring Buffer, with interrupts
* Print strings directly to the UART, with or without newlines
* Configurable Word Length, Stopbits, and parity on init

**NOTE:** All configuration flags are in `funconfig.h`  
If no configuration flags are used, the following configuration will be used:
```

```

See `/testing/uart_test.c` for a basic usage example of the library.

## TODO
* TX Ring buffer & interrupt method

----
Copyright (c) 2024 ADBeta
