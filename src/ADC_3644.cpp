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

  // Throw away first transaction (check)
  SPI.beginTransaction(SPISettings(ADC_SPI_FREQ, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  digitalWrite(cs, HIGH);
  SPI.endTransaction();

  _ext_clock = ext_clock;
  _clockfreq = ext_clock->getChannelFreq(1);
}

ADCResult ADC3644::init()
{
  // Reset (may be unnecessary)
  reset();

  // Set output to 1-Wire CMOS
  writeToReg(0x07, 0x6C); // 011 0 1 100
  writeToReg(0x13, 0x01); // 0000000 1

  // Power down unused outputs
  writeToReg(0x0A, 0xFF);
  writeToReg(0x0B, 0xEE);
  writeToReg(0x0C, 0xFD);

  // Enable DCLKIN? Ideally MCU would be slave but will test
  writeToReg(0x18, 0x10);

  // Enable FCLK
  writeToReg(0x19, 0x82); // 1 00 0 00 1 0

  // Select 16-bit output
  writeToReg(0x1B, 0x08); // 0 0 001 000

  // Enable DCLKIN on rising AND falling edge
  writeToReg(0x1F, 0x50); // 0 1 0 1 0 000 (rising+falling), 0x58 for rising only

  // Configure FCLK pattern for 16-bit Complex Decimation
  writeToReg(0x20, 0xFF);
  writeToReg(0x21, 0xFF);
  writeToReg(0x22, 0x0F);

  // Enable decimation filter
  writeToReg(0x24, 0x06); // 00 0 00 1 1 0

  // Set decimation by 8 (or whatever is needed)
  if (setDecimationBy(8) != ADC_OK) return ADC_ERR_DECIMATION;

  // Set NCO frequency to 30 MHz
  if (setNCOfreq(30) != ADC_OK) return ADC_ERR_NCO_RANGE;

  // Set Gain to 3 dB
  if (setGain(3) != ADC_OK) return ADC_ERR_GAIN_INVALID;

  return ADC_OK;
}

ADCResult ADC3644::setDecimationBy(int dec)
{
  uint8_t val;

  if(dec == 0) val = 0b000;
  else if(dec == 2) val = 0b001;
  else if(dec == 4) val = 0b010;
  else if(dec == 8) val = 0b011;
  else if(dec == 16) val = 0b100;
  else if(dec == 32) val = 0b101;
  else return ADC_ERR_DECIMATION;

  // Setup decimation divider (bits 6-4 control decimation by)
  if (writeToReg(0x25, val << 4) != ADC_OK) return ADC_ERR_WRITE_FAIL; // 0 xxx 0 00 0 

  _decimationBy = dec;
  return ADC_OK;
}

ADCResult ADC3644::setNCOfreq(double f_nco)
{
  if(abs(f_nco) > (_clockfreq / 2))
  {
    // Frequency too high, increase sampling rate
    if(setClockFreq(2.08 * f_nco) != ADC_OK) return ADC_ERR_CLOCK_SET;
  }

  int32_t f_out = f_nco * (4294967296.0) / _clockfreq; // Check that this does 2's complement
  _NCOfreq = f_nco;

  // ADC Channel A
  writeToReg(0x2A, (f_out >> 24) & 0xFF);
  writeToReg(0x2B, (f_out >> 16) & 0xFF);
  writeToReg(0x2C, (f_out >> 8) & 0xFF);
  writeToReg(0x2D, f_out & 0xFF);

  // ADC Channel B
  writeToReg(0x31, (f_out >> 24) & 0xFF);
  writeToReg(0x32, (f_out >> 16) & 0xFF);
  writeToReg(0x33, (f_out >> 8) & 0xFF);
  writeToReg(0x34, f_out & 0xFF);

  return ADC_OK;
}

ADCResult ADC3644::setGain(int gain)
{
  uint8_t val;

  if(gain == 0) val = 0b00;
  else if(gain == 3) val = 0b01;
  else if(gain == 6) val = 0b10;
  else return ADC_ERR_GAIN_INVALID;

  // Set gain and toggle the toggle bits (0x22)
  uint8_t out1 = (val << 6) | (val << 2) | 0x22; // xx 1 0 xx 1 0
  uint8_t out2 = (val << 6) | (val << 2);       // xx 0 0 xx 0 0

  if (writeToReg(0x26, out1) != ADC_OK || writeToReg(0x26, out2) != ADC_OK) return ADC_ERR_WRITE_FAIL;

  _gain = gain;
  return ADC_OK;
}

ADCResult ADC3644::reset()
{
  return writeToReg(ADC_RESET_ADDRESS, 0x0);
}

ADCResult ADC3644::setClockFreq(double freq)
{
  if(_ext_clock->setChannelFreq(freq, 1)) return ADC_ERR_CLOCK_SET;
  _clockfreq = getClockFreq();
  return ADC_OK;
}

double ADC3644::getClockFreq()
{
  return _ext_clock->getChannelFreq(1);
}

double ADC3644::getNCOFreq()
{
  return _NCOfreq;
}

int ADC3644::getDecimationBy()
{
  return _decimationBy;
}

int ADC3644::getGain()
{
  return _gain;
}

ADCResult ADC3644::writeToReg(int reg, uint8_t data)
{
  uint8_t upper = (reg >> 8) & 0xFF; // Most significant 8-bits of address + leading zeros
  uint8_t lower = (reg & 0xFF); // Least significant 8-bits of address

  // Setting up one buffer with register address and data
  uint8_t buf[] = {upper, lower, data};

  // Send through SPI
  SPI.beginTransaction(SPISettings(ADC_SPI_FREQ, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  SPI.transfer(buf, sizeof(buf));
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();

  return ADC_OK;
}