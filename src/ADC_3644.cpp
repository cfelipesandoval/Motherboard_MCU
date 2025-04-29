#include "ADC_3644.h"

ADC3644::ADC3644(CDCE6214 * ext_clock, int scl, int sdio, int cs, int reset)
{
  _scl = scl;
  _sdio = sdio;
  _cs = cs;
  _reset = reset;

  pinMode(cs, OUTPUT);
  pinMode(reset, OUTPUT);

  // digitalWrite(cs, HIGH);

  SPI.begin(scl, sdio, sdio, cs); // Probably gotta add specific pins here
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

  _ext_clock = ext_clock;
}

uint8_t ADC3644::init()
{
  writeToReg(0b0000111111111111, 0b10101010);

  return EXIT_SUCCESS;
}

uint8_t ADC3644::writeToReg(int reg, int data)
{
  Serial.print("Writing to Register 0x");
  Serial.print(reg, HEX);
  Serial.print(" 0x");
  Serial.print(data, HEX);
  int upper = (reg & 0xFF00) >> 8;
  int lower = (reg & 0xFF);

  // int message = reg << 

  while(1)
  {
    digitalWrite(_cs, LOW);
    
    // try using write bytes or something I dont like how much it pauses between writes
    SPI.write(upper);
    SPI.write(lower);

    SPI.write(data);

    digitalWrite(_cs, HIGH);
    delay(1);
  }
  return EXIT_SUCCESS;
}