#include "external_clock.h"

CDCE6214::CDCE6214(uint8_t sda, uint8_t scl, uint8_t clock_address)
{
  Wire.setPins(sda, scl);
  Wire.begin();

  _address = clock_address;
  crystal_freq = CLOCK_FREQ;
}

uint8_t CDCE6214::init(double ch1freq, double ch2freq)
{
  Serial.println("Setting D");
  if(setD(0.5)) return EXIT_ERROR;
  
  Serial.println("Setting N");
  if(setN(0x0100)) return EXIT_ERROR;
  
  Serial.println("Setting PSA");
  if(setPSA(4)) return EXIT_ERROR;
  
  Serial.println("Setting CH1");
  if(setChannelFreq(ch1freq, 0)) return EXIT_ERROR;
  
  Serial.println("Setting CH2"); // Sets MCU Clock to 40 MHz
  if(setChannelFreq(ch2freq, 1)) return EXIT_ERROR; // Sets ADC Clock to ~50 MHz (50.196078 MHz)
  
  Serial.print("\nChannel 1 Frequency: ");
  Serial.println(CHfreq[0]);

  Serial.print("\nChannel 2 Frequency: ");
  Serial.println(CHfreq[1]);

  return EXIT_SUCCESS;
}

uint8_t CDCE6214::setD(double divider)
{
  int div;

  div = 0x0400 + floor(divider);

  if(writeToReg(DIVIDER_ADDRESS, div)) return EXIT_ERROR;

  D = divider;

  return EXIT_SUCCESS;
}

uint8_t CDCE6214::setN(int n)
{
  if(writeToReg(N_ADDRESS, n)) return EXIT_ERROR;

  N = n;

  return EXIT_SUCCESS;
}

uint8_t CDCE6214::setPSA(int psa)
{
  int ps;

  if(psa == 4)
  {
    ps = 0x80;
  }
  else if(psa == 5)
  {
    ps = 0x88;
  }
  else if(psa == 6)
  {
    ps = 0x90;
  }
  else
  {
    Serial.println("Error, PSA Must be 4, 5 or 6");
    return EXIT_ERROR;
  }

  if(writeToReg(PSA_ADDRESS, ps)) return EXIT_ERROR;

  PSA = psa;

  return EXIT_SUCCESS;
}

uint8_t CDCE6214::setCHD(int chd, int channel)
{
 if(channel == 0)
 {
  if(writeToReg(CH1_ADDRESS, chd)) return EXIT_ERROR;
 } 
 else if(channel == 1)
 {
  if(writeToReg(CH2_ADDRESS, chd)) return EXIT_ERROR;
 }

  return EXIT_SUCCESS;
}


uint8_t CDCE6214::setFreq(double freq, int channel)
{
  if(freq > 125)
  {
    Serial.println("Frequency Must be Below 125 MHz");

    return EXIT_ERROR;
  } 
  else if(freq <= 0.01)
  {
    Serial.println("Frequency Must be Above 10 kHz");

    return EXIT_ERROR;
  } 

  CHD[channel] = floor(crystal_freq * N / (D * PSA * freq));
  CHfreq[channel] = crystal_freq * N / (D * PSA * CHD[channel]);

  return EXIT_SUCCESS;
}

uint8_t CDCE6214::setChannelFreq(double freq, int channel)
{
  if(setFreq(freq, channel)) return EXIT_ERROR;

  if(channel == 0)
 {
  if(writeToReg(CH1_ADDRESS, CHD[channel])) return EXIT_ERROR;
 } 
 else if(channel == 1)
 {
  if(writeToReg(CH2_ADDRESS, CHD[channel])) return EXIT_ERROR;
 }

  return EXIT_SUCCESS;
}

uint8_t CDCE6214::writeToReg(int reg, int data)
{
  Serial.print("Writing to Register 0x");
  Serial.print(reg, HEX);
  Serial.print(" 0x");
  Serial.print(data, HEX);

  Wire.beginTransmission(_address); // Probably gonna be an error here
  
  // if(!Wire.read())
  // {
  //   Serial.println("Error Initializing I2C");

  //   return EXIT_ERROR;
  // }

  Wire.write((reg & 0xFF00) >> 8); // Upper half of register address
  // if(!Wire.read())
  // {

  //   Serial.println("Error Upper Half of Register");

  //   return EXIT_ERROR;
  // }

  Wire.write(reg & 0xFF); // Lower half of register address
  // if(!Wire.read())
  // {
  //   Serial.println("Error Lower Half of Register");


  //   return EXIT_ERROR;
  // }

  Wire.write((data & 0xFF00) >> 8); // Upper half of data
  // if(!Wire.read())
  // {
  //   if(Serial)
  //   {
  //     Serial.println("Error Upper Half of Data");
  //   }

  //   return EXIT_ERROR;
  // }

  Wire.write(data & 0xFF); // Lower half of data
  // if(!Wire.read())
  // {
  //   if(Serial)
  //   {
  //     Serial.println("Error Lower Half of Data");
  //   }

  //   return EXIT_ERROR;
  // }

  Wire.endTransmission();

  Serial.println("\nRegister Write Success");

  return EXIT_SUCCESS;
}

double CDCE6214::getChannelFreq(int channel)
{
  return CHfreq[channel];
}