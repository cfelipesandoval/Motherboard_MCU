#ifndef ADC_3644
#define ADC_3644

#include <Arduino.h>
#include <SPI.h>
#include "CDCE_6214.h"
#include "definitions.h"

class ADC3644
{
  public:
    ADC3644(CDCE6214 * ext_clock, int scl = ADC_SCL, int sdio = ADC_SDIO, int cs = ADC_CS, int reset = ADC_RESET);
    uint8_t init();

    // Register Writes
    uint8_t reset();
    uint8_t setDecimationBy(int dec);
    uint8_t setNCOfreq(double freq);
    uint8_t setGain(int gain);
    uint8_t setClockFreq(double freq);
    double getClockFreq();


  private:
    uint8_t writeToReg(int reg, uint8_t data);

    // Variables
    int _decimationBy; // DDC Divider
    double _NCOfreq;
    double _clockfreq;
    

    // Pins
    int _scl; // Serial Clock pin
    int _sdio; // Serial Data In/Out Pin
    int _cs; // Chip Select pin
    int _reset; // Reset Pin

    CDCE6214 * _ext_clock;
};


#endif 