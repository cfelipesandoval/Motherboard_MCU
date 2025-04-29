#ifndef ADC_3644
#define ADC_3644

#include <Arduino.h>
#include "external_clock.h"
#include "definitions.h"

class ADC3644
{
  public:
    


  private:
    uint8_t writeToReg(uint8_t reg, uint8_t data);


    CDCE6214 * ext_clock;
};




#endif 