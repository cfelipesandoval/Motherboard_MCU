#ifndef ADC_3644
#define ADC_3644

#include <Arduino.h>
#include <SPI.h>
#include "CDCE_6214.h"
#include "definitions.h"

enum ADCResult 
{
  ADC_OK = 0,
  ADC_ERR_DECIMATION = 1,
  ADC_ERR_NCO_RANGE = 2,
  ADC_ERR_CLOCK_SET = 3,
  ADC_ERR_GAIN_INVALID = 4,
  ADC_ERR_WRITE_FAIL = 5,
  ADC_ERR_UNKNOWN = 255
};

class ADC3644
{
  public:
    ADC3644(CDCE6214 * ext_clock, int scl = ADC_SCL, int sdio = ADC_SDIO, int cs = ADC_CS, int reset = ADC_RESET);

    ADCResult init();
    ADCResult reset();
    ADCResult setDecimationBy(int dec);
    ADCResult setNCOfreq(double f_nco);
    ADCResult setGain(int gain);
    ADCResult setClockFreq(double freq);

    double getClockFreq();
    double getNCOFreq();
    int getDecimationBy();
    int getGain();

  private:
    ADCResult writeToReg(int reg, uint8_t data);

    int _decimationBy;
    int _gain;
    double _NCOfreq;
    double _clockfreq;

    int _scl;
    int _sdio;
    int _cs;
    int _reset;

    CDCE6214 * _ext_clock;
};

#endif
