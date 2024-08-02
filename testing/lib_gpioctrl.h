/******************************************************************************
* lib_gpioctrl
* A runtime-capable GPIO Library, with Digital Read/Write and Analog Read
*
* See GitHub for details: https://github.com/ADBeta/CH32V003_lib_gpioctrl
*
* ADBeta (c)    Aug 2024    Ver 1.2.0
******************************************************************************/
#ifndef LIB_GPIOCTRL_H
#define LIB_GPIOCTRL_H

#include <stdint.h>
#include <stddef.h>

/*** GPIO Pin Enumeration ****************************************************/
/// @breif This enum is used as binary data for pin and port addressing. 
/// 0x[PIN][PORT] - 0x 0603 6th pin of port 4 (PORTD)
/// NOTE: Little-Endian Architecture means value in memory is [PORT][PIN] order
typedef enum {
	GPIO_PA0	  = 0x0000,
	GPIO_PA1	  = 0x0100, 
	GPIO_PA2	  = 0x0200, 
	GPIO_PA3	  = 0x0300,
	GPIO_PA4	  = 0x0400,
	GPIO_PA5	  = 0x0500,
	GPIO_PA6	  = 0x0600,
	GPIO_PA7	  = 0x0700,

	GPIO_PB0	  = 0x0001,
	GPIO_PB1	  = 0x0101, 
	GPIO_PB2	  = 0x0201, 
	GPIO_PB3	  = 0x0301,
	GPIO_PB4	  = 0x0401,
	GPIO_PB5	  = 0x0501,
	GPIO_PB6	  = 0x0601,
	GPIO_PB7	  = 0x0701,

	GPIO_PC0	  = 0x0002,
	GPIO_PC1	  = 0x0102, 
	GPIO_PC2	  = 0x0202, 
	GPIO_PC3	  = 0x0302,
	GPIO_PC4	  = 0x0402,
	GPIO_PC5	  = 0x0502,
	GPIO_PC6	  = 0x0602,
	GPIO_PC7	  = 0x0702,

	GPIO_PD0	  = 0x0003,
	GPIO_PD1	  = 0x0103, 
	GPIO_PD2	  = 0x0203, 
	GPIO_PD3	  = 0x0303,
	GPIO_PD4	  = 0x0403,
	GPIO_PD5	  = 0x0503,
	GPIO_PD6	  = 0x0603,
	GPIO_PD7	  = 0x0703,

	// Map Analog Pins
	// If the CPU Is a CH32V003
	#ifdef CH32V003
	GPIO_A0	   = 0x0200,	 // PA2
	GPIO_A1	   = 0x0100,	 // PA1
	GPIO_A2	   = 0x0402,	 // PC4
	GPIO_A3	   = 0x0203,	 // PD2
	GPIO_A4	   = 0x0303,	 // PD3
	GPIO_A5	   = 0x0503,	 // PD5
	GPIO_A6	   = 0x0603,	 // PD6
	GPIO_A7	   = 0x0403,	 // PD4
	#endif
} GPIO_PIN;

/// @breif analog input pins, sorted by channel
typedef enum {
	GPIO_ADC_A0	     = 0x00,
	GPIO_ADC_A1	     = 0x01,
	GPIO_ADC_A2	     = 0x02,
	GPIO_ADC_A3	     = 0x03,
	GPIO_ADC_A4	     = 0x04,
	GPIO_ADC_A5	     = 0x05,
	GPIO_ADC_A6	     = 0x06,
	GPIO_ADC_A7	     = 0x07,
	GPIO_ADC_VREF	 = 0x08,
	GPIO_ADC_VCAL	 = 0x09,
} GPIO_ANALOG_CHANNEL;


/*** GPIO Pin Mode Enumeration ***********************************************/
/// @breif GPIO Pin Mode data. The lower nibble is the raw binary data for the
/// [R32_GPIOx_CFGLR] Register. The upper nibble is used for additional flags 
typedef enum {
	INPUT_ANALOG	   = 0x00,
	INPUT_FLOATING	   = 0x04,
	// Mapped to INPUT_PP, Sets OUTDR based on the upper nibble
	INPUT_PULLUP	   = 0x18,
	INPUT_PULLDOWN	   = 0x08,
	//
	OUTPUT_10MHZ_PP	   = 0x01,
	OUTPUT_10MHZ_OD	   = 0x05,
	//
	OUTPUT_2MHZ_PP	   = 0x02,
	OUTPUT_2MHZ_OD	   = 0x06,
	//
	OUTPUT_PP_AF	   = 0x08,
	OUTPUT_OD_AF	   = 0x0C,
} GPIO_MODE;

/*** ADC Setting Enumeration *************************************************/
/// @breif a sample of ADC clock division settings (Bits 15:11 in RCC->CFGR0)
typedef enum {
	ADC_CLOCK_DIV_2   = (uint32_t)0x00000000,
	ADC_CLOCK_DIV_32  = (uint32_t)0x00003800,
	ADC_CLOCK_DIV_64  = (uint32_t)0x00007800,
	ADC_CLOCK_DIV_128 = (uint32_t)0x0000F800,
} ADC_CLOCK_DIV;


/// @brief ADC Sample Settings. Pre-calculated Sample Bits for all channels
typedef enum {
	ADC_SAMPLE_CYCLES_3    = (uint32_t)0x00000000,
	ADC_SAMPLE_CYCLES_9    = (uint32_t)0x09249249,
	ADC_SAMPLE_CYCLES_15   = (uint32_t)0x12492492,
	ADC_SAMPLE_CYCLES_30   = (uint32_t)0x1B6DB6DB,
	ADC_SAMPLE_CYCLES_43   = (uint32_t)0x24924924,
	ADC_SAMPLE_CYCLES_57   = (uint32_t)0x2DB6DB6D,
	ADC_SAMPLE_CYCLES_73   = (uint32_t)0x36DB6DB6,
	ADC_SAMPLE_CYCLES_241  = (uint32_t)0x3FFFFFFF,
} ADC_SAMPLE_CYCLES;

/*** GPIO Output State Enumerations ******************************************/
/// @breif GPIO Pin State Enum, simple implimentation of a HIGH/LOW System
typedef enum {
	GPIO_LOW	       = 0x00,
	GPIO_HIGH	       = 0x01,
} GPIO_STATE;


/*** Registers for GPIO Port *************************************************/
/// @breif GPIO Port Register, Directly Maps to Memory starting at 
/// [R32_GPIOx_CFGLR] for each PORT Respectively
typedef struct {
	volatile uint32_t CFGLR;  // Configuration Register (lower)
	volatile uint32_t CFGHR;  // Configuration Register (upper)
	volatile uint32_t INDR;   // Input Data Register
	volatile uint32_t OUTDR;  // Output Data Register
	volatile uint32_t BSHR;   // Set/Reset Register
	volatile uint32_t BCR;	  // Port Reset Register
	volatile uint32_t LCKR;   // Lock Register
} GPIO_PORT_REG_TypeDef;

/// @breif ADC Register. Directly Maps to Memory starting at R32_ADC_STATR
typedef struct {
	volatile uint32_t STATR;    // ADC status register
	volatile uint32_t CTLR1;    // ADC control register 1
	volatile uint32_t CTLR2;    // ADC control register 2
	volatile uint32_t SAMPTR1;  // ADC sample time register 1
	volatile uint32_t SAMPTR2;  // ADC sample time register 2
	volatile uint32_t IOFR1;    // ADC injected channel data offset register 1
	volatile uint32_t IOFR2;    // ADC injected channel data offset register 2
	volatile uint32_t IOFR3;    // ADC injected channel data offset register 3
	volatile uint32_t IOFR4;    // ADC injected channel data offset register 4
	volatile uint32_t WDHTR;    // ADC watchdog high threshold register
	volatile uint32_t WDLTR;    // ADC watchdog low threshold register
	volatile uint32_t RSQR1;    // ADC regular sequence register 1
	volatile uint32_t RSQR2;    // ADC regular sequence register 2
	volatile uint32_t RSQR3;    // ADC regular sequence register 3
	volatile uint32_t ISQR;     // ADC injected sequence register
	volatile uint32_t IDATAR1;  // ADC injected data register 1
	volatile uint32_t IDATAR2;  // ADC injected data register 2
	volatile uint32_t IDATAR3;  // ADC injected data register 3
	volatile uint32_t IDATAR4;  // ADC injected data register 4
	volatile uint32_t RDATAR;   // ADC regular data register
	volatile uint32_t DLYR;     // ADC delayed data register 
} ADC_REG_TypeDef;

/// @breif RCC Port Register. Directly Maps to Memory starting at R32_RCC_CTLR
typedef struct {
	volatile uint32_t CTLR;	      // Clock control register
	volatile uint32_t CFGR0;	  // Clock configuration register 0
	volatile uint32_t INTR;	      // Clock interrupt register
	volatile uint32_t APB2PRSTR;  // PB2 peripheral reset register
	volatile uint32_t APB1PRSTR;  // PB1 peripheral reset register
	volatile uint32_t AHBPCENR;   // HB peripheral clock enable register
	volatile uint32_t APB2PCENR;  // PB2 peripheral clock enable register
	volatile uint32_t APB1PCENR;  // PB1 peripheral clock enable register
	volatile uint32_t BDCTLR;	  // RESERVED on CH32V003 
	volatile uint32_t RSTSCKR;	  // Control/status register
} RCC_REG_TypeDef;

/*** Register Address Definitions ********************************************/
// Base Registers
#define PORTA_GPIO_REGISTER_BASE 0x40010800
// NOTE: PORTB is not available for the CH32V003.
#define PORTB_GPIO_REGISTER_BASE 0x40010C00
#define PORTC_GPIO_REGISTER_BASE 0x40011000
#define PORTD_GPIO_REGISTER_BASE 0x40011400
#define ADC1_REGISTER_BASE	     0x40012400
#define RCC_REGISTER_BASE	     0x40021000

// Register typedef Declarations

#define GPIO_PORTA ((GPIO_PORT_REG_TypeDef *)PORTA_GPIO_REGISTER_BASE)
// NOTE: PORTB is not available for the CH32V003.
#define GPIO_PORTB ((GPIO_PORT_REG_TypeDef *)PORTB_GPIO_REGISTER_BASE)
#define GPIO_PORTC ((GPIO_PORT_REG_TypeDef *)PORTC_GPIO_REGISTER_BASE)
#define GPIO_PORTD ((GPIO_PORT_REG_TypeDef *)PORTD_GPIO_REGISTER_BASE)
#define GPIO_ADC1  ((ADC_REG_TypeDef *)ADC1_REGISTER_BASE)
#define GPIO_RCC   ((RCC_REG_TypeDef *)RCC_REGISTER_BASE)

/// @breif The GPIO Ports are places into an array for easy indexing in the
/// GPIO Functions
/// NOTE: Only 3 PORTs are usable in the CH32V003, 4 for other MCUs
extern GPIO_PORT_REG_TypeDef *GPIO_PORT_MAP[4]; 

/*** Register Known Values ***************************************************/
// RCC
#define RCC_APB2PCENR_AFIO   ((uint32_t)0x00000001)
#define RCC_APB2PCENR_IOPxEN ((uint32_t)0x00000004)
#define RCC_APB2Periph_ADC1  ((uint32_t)0x00000200)
// ADC
#define ADC_EOC              ((uint8_t)0x02)
#define ADC_ADON             ((uint32_t)0x00000001)
#define ADC_RSTCAL           ((uint32_t)0x00000008)
#define ADC_EXTSEL           ((uint32_t)0x000E0000)
#define ADC_SWSTART          ((uint32_t)0x00400000)

/*** GPIO Mode Setting *******************************************************/
/// @breif Sets the Config and other needed Registers for a passed pin and mode
/// @param GPIO_PIN pin, the GPIO Pin & Port Variable (e.g GPIO_PD6)
/// @param GPIO_MODE mode, the GPIO Mode Variable (e.g OUTPUT_10MHZ_PP)
/// @return None
void gpio_set_mode(const GPIO_PIN pin, const GPIO_MODE mode);

/*** Digital Write/Read ******************************************************/
/// @breif Sets the OUTDR Register for the passed Pin
/// @param GPIO_PIN pin, the GPIO Pin & Port Variable (e.g GPIO_PD6)
/// @param GPIO_STATE state, GPIO State to be set (e.g GPIO_HIGH)
/// @return None
void gpio_digital_write(const GPIO_PIN pin, const GPIO_STATE state);

/// @breif Reads the INDR Register of the specified pin and returns state
/// @param GPIO_PIN pin, the GPIO Pin & Port Variable (e.g GPIO_PD6)
/// @return GPIO_STATE, the current state of the pin, (e.g GPIO_HIGH)
GPIO_STATE gpio_digital_read(const GPIO_PIN pin);

/// @breif initialises the ADC registers, sets polling mode, and calibrates
/// @param ADC_CLOCK_DIV, ADC Clock Divisor
/// @param ADC_SAMPLE_CYCLES, Number of cycles to sample the ADC
/// @retun none
void gpio_init_adc(const ADC_CLOCK_DIV div, const ADC_SAMPLE_CYCLES cycles);

/// @breif Reads the specified pin, only if it is one of the analog inputs
/// @param GPIO_ANALOG_CHANNEL chan, the Analog GPIO Channel to read
/// @return uint16_t 10-bit Analog Value
/// NOTE: This function uses the polled adc method, which is slow compared to
/// injection mode multi-channel reads
uint16_t gpio_analog_read(const GPIO_ANALOG_CHANNEL chan);

#endif
