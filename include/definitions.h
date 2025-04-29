#ifndef DEFINITIONS
#define DEFINITIONS


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
// #define ADC_SCL 48
// #define ADC_SDIO 38
// #define ADC_CS 47
// #define ADC_RESET 1

#define ADC_SCL 13
#define ADC_SDIO 11
#define ADC_CS 12
#define ADC_RESET 17


// Error Codes
#define EXIT_SUCCESS 0
#define EXIT_ERROR 1



#endif