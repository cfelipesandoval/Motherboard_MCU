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

uint8_t ADC3644::init()
{
  Serial.print("\nStart SPI?: ");
  while(!(Serial.read() == 'y'));

  // // Writing to registers (Check datasheet from page 46 to see what each register does)

  // Reset (may be unnecessary)
  reset();

  // Serial.print("\nCMOS?: ");
  // while(!(Serial.read() == 'y'));

  // Set output to 1-Wire CMOS
  writeToReg(0x07, 0x6C); // 011 0 1 100
  writeToReg(0x13, 0x01); // 0000000 1

  // Serial.print("\nPowerDown?: ");
  // while(!(Serial.read() == 'y'));

  // Power down unsued outputs
  writeToReg(0x0A, 0xFF);
  writeToReg(0x0B, 0xEE);
  writeToReg(0x0C, 0xFD);

  // Serial.print("\ndclkin en?: ");
  // while(!(Serial.read() == 'y'));

  // Enable DCLKIN? Ideally MCU would be slave but will test
  writeToReg(0x18, 0x10);

  // Serial.print("\nfclk en?: ");
  // while(!(Serial.read() == 'y'));

  // Enable FCLK
  writeToReg(0x19, 0x82); // 1 00 0 00 1 0

  // Serial.print("\n16bit?: ");
  // while(!(Serial.read() == 'y'));

  // Select 16-bit output
  writeToReg(0x1B, 0x08); // 0 0 001 000

  // Serial.print("\ndclkin on rise?: ");
  // while(!(Serial.read() == 'y'));

  // Enable DCLKIN on rising AND falling edge 
  writeToReg(0x1F, 0x50); // 0 1 0 1 0 000, otherwise 0 1 0 1 1 000 (0x58) for rising edge only
  
  // Serial.print("\nfclk for 16bit?: ");
  // while(!(Serial.read() == 'y'));

  // Configure FCLK pattern for 16-bit Complex Decimation
  writeToReg(0x20, 0xFF); 
  writeToReg(0x21, 0xFF); 
  writeToReg(0x22, 0x0F);

  // Serial.print("\ndecimation en?: ");
  // while(!(Serial.read() == 'y'));

  // Enable decimation filter
  writeToReg(0x24, 0x06); // 00 0 00 1 1 0

  // Serial.print("\ndecimation by 8?: ");
  // while(!(Serial.read() == 'y'));

  // Set decimation by 8 (or whatever is needed)
  setDecimationBy(8);

  // Serial.print("\nset nco?: ");
  // while(!(Serial.read() == 'y'));

  // Set NCO frequency to 30 MHz
  setNCOfreq(30);

  // Serial.print("\nset gain?: ");
  // while(!(Serial.read() == 'y'));

  // Set Gain to 3 dB
  setGain(3);

  return EXIT_SUCCESS;
}

uint8_t ADC3644::setDecimationBy(int dec)
{
  uint8_t val;

  if(dec == 0)
  {
    val = 0b000;
  }
  else if(dec == 2)
  {
    val = 0b001;
  }
  else if(dec == 4)
  {
    val = 0b010;
  }
  else if(dec == 8)
  {
    val = 0b011;
  }
  else if(dec == 16)
  {
    val = 0b100;
  }else if(dec == 32)
  {
    val = 0b101;
  }
  else
  {
    Serial.println("Error: Decimation Divider Must be 2, 4, 8, 16 or 32");

    return EXIT_FAILURE; 
  }

  // Setup decimation divider (bits 6-4 control decimation by)
  writeToReg(0x25, val << 4); // 0 xxx 0 00 0 

  _decimationBy = dec;

  return EXIT_SUCCESS;
}

// Freq in MHz
// Really need to make sure this does the right math
// Also right now if clock frequency is too low it just doubles whatever f_nco is, this might not be smart
uint8_t ADC3644::setNCOfreq(double f_nco)
{
  if(abs(f_nco) > (_clockfreq / 2))
  {
    Serial.println("Frequency has to be within -Fs/2 < f < Fs/2");
    
    Serial.println("Setting Fs to 2 * freq_nco");
    if(setClockFreq(2.08 * f_nco)) // Set clock to twice as much plus 8%
    {
      return EXIT_FAILURE;
    }
  }

  int32_t f_out = f_nco * (4294967296) / _clockfreq; // Check that this does 2's complement
  
  // ADC Channel A
  writeToReg(0x2A, (f_out & 0xFF000000) >> 24);
  writeToReg(0x2B, (f_out & 0xFF0000) >> 16);
  writeToReg(0x2C, (f_out & 0xFF00) >> 8);
  writeToReg(0x2D, (f_out & 0xFF));

  // ADC Channel B
  writeToReg(0x31, (f_out & 0xFF000000) >> 24);
  writeToReg(0x32, (f_out & 0xFF0000) >> 16);
  writeToReg(0x33, (f_out & 0xFF00) >> 8);
  writeToReg(0x34, (f_out & 0xFF));
  
  return EXIT_SUCCESS;
}

uint8_t ADC3644::setGain(int gain)
{
  uint8_t val;
  uint8_t out;

  if(gain == 0)
  {
    val = 0b0;
  }
  else if(gain == 3)
  {
    val = 0b1;
  }
  else if(gain == 6)
  {
    val = 0b10;
  }
  else
  {
    Serial.println("Error: gain can only be set to 0, 3 or 6 dB");

    return EXIT_FAILURE;
  }

  // Set gain and toggle the toggle bits (0x22)
  out = (val << 6) | (val << 2) | 0x22; // xx 1 0 xx 1 0
  writeToReg(0x26, out);

  out = (val << 6) | (val << 2); // xx 0 0 xx 0 0
  writeToReg(0x26, out);

  return EXIT_SUCCESS;
}

uint8_t ADC3644::reset()
{
  return writeToReg(ADC_RESET_ADDRESS, 0x0);
}

uint8_t ADC3644::setClockFreq(double freq)
{
  uint8_t out = _ext_clock->setChannelFreq(freq, 1);
  _clockfreq = getClockFreq();

  return out;
}

double ADC3644::getClockFreq()
{
  return _ext_clock->getChannelFreq(1);
}

uint8_t ADC3644::writeToReg(int reg, uint8_t data)
{
  Serial.print("Writing to Register 0x");
  Serial.print(reg, HEX);
  Serial.print(" 0x");
  Serial.print(data, HEX);

  uint8_t upper = (reg & 0xFF00) >> 8; // Most significant 8-bits of address + leading zeros
  uint8_t lower = (reg & 0xFF); // Least significant 8-bits of address

  // Setting up one buffer with register address and data
  uint8_t buf[] = {upper, lower, data};
  uint8_t numBytes = sizeof(buf)/sizeof(buf[0]);

  // Send through SPI
  SPI.beginTransaction(SPISettings(ADC_SPI_FREQ, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);

  SPI.transfer(buf, numBytes);

  digitalWrite(_cs, HIGH);
  SPI.endTransaction();

  return EXIT_SUCCESS;
}