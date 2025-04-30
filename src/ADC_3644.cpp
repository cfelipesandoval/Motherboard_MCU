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
  digitalWrite(cs,LOW);
  digitalWrite(cs,HIGH);
  SPI.endTransaction();

  _ext_clock = ext_clock
}

uint8_t ADC3644::init()
{
  Serial.print("\nStart SPI?: ");
  while(!(Serial.read() == 'y'));

  // // Writing to registers (Check datasheet from page 46 to see what each register does)

  // Reset (may be unnecessary)
  reset();

  // Set output to 1-Wire CMOS
  writeToReg(0x07, 0x6C); // 011 0 1 100
  writeToReg(0x13, 0x01); // 0000000 1

  // Power down unsued outputs
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
  writeToReg(0x1F, 0x50); // 0 1 0 1 0 000, otherwise 0 1 0 1 1 000 (0x58) for rising edge only
  
  // Configure FCLK pattern for 16-bit Complex Decimation
  writeToReg(0x20, 0xFF); 
  writeToReg(0x21, 0xFF); 
  writeToReg(0x22, 0x0F);

  // Enable decimation filter
  writeToReg(0x24, 0x06); // 00 0 00 1 1 0



  

  return EXIT_SUCCESS;
}

// Complex decimation setting. This applies to both channels.
// 000: Bypass mode (no decimation)
// 001: Decimation by 2
// 010: Decimation by 4
// 011: Decimation by 8
// 100: Decimation by 16
// 101: Decimation by 32
// others: not used
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

    return EXIT_ERROR; 
  }

  
  // Setup decimation divider (bits 6-4 control decimation by)
  writeToReg(0x25, val << 4); // 0 xxx 0 00 0 
}

uint8_t ADC3644::reset()
{
  return writeToReg(ADC_RESET_ADDRESS, 0x0);
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