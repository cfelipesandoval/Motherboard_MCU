#ifndef DEFINITIONS
#define DEFINITIONS


#define DEBUG

// // CDCE6214 Definitions

// Data Pins
#define SDA 1 // Change to actual pin
#define SCL 2 // Change to actual pin

// Addresses
#define CLOCK_ADDRESS 0x68
#define DIVIDER_ADDRESS 0x19
#define N_ADDRESS 0x1E
#define PSA_ADDRESS 0x2F
#define CH1_ADDRESS 0x38
#define CH2_ADDRESS 0x3E

// MCU External Clock Frequency
#define CLOCK_FREQ 40 // In MHz


// // ADC3644 Definitions

// Data Pins (These are GPIO number so fix)
#define ADC_SCL GPIO_NUM_48
#define ADC_SDIO GPIO_NUM_38
#define ADC_CS GPIO_NUM_47
#define ADC_RESET GPIO_NUM_1

// Data Pins (With arduino pinout)
// #define ADC_SCL 13
// #define ADC_SDIO 11
// #define ADC_CS 12
// #define ADC_RESET 17

#define ADC_SPI_FREQ 1000000

// ADC Registers
#define ADC_RESET_ADDRESS 0x00

// Error Codes
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

// // Communication Protocol 
#define MCU_RX_PIN 50
#define MCU_TX_PIN 49

#endif