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

  private:
    uint8_t writeToReg(int reg, int data);

    int _scl; // Serial Clock pin
    int _sdio; // Serial Data In/Out Pin
    int _cs; // Chip Select pin

    int _reset; // Reset Pin

    CDCE6214 * _ext_clock;
};




#endif 