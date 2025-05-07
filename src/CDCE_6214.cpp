#include "CDCE_6214.h"

CDCE6214::CDCE6214(uint8_t sda, uint8_t scl, uint8_t clock_address)
{
  Wire.setPins(sda, scl);
  Wire.begin();

  _address = clock_address;
  crystal_freq = CLOCK_FREQ;
}

uint8_t CDCE6214::init(double ch1freq, double ch2freq)
{
#ifdef DEBUG
  Serial.println("Setting D");
#endif
  if(setD(0.5)) return EXIT_FAILURE;

#ifdef DEBUG
  Serial.println("Setting N");
#endif
  if(setN(0x0100)) return EXIT_FAILURE;

#ifdef DEBUG
  Serial.println("Setting PSA");
#endif
  if(setPSA(4)) return EXIT_FAILURE;

#ifdef DEBUG
  Serial.println("Setting CH1");
#endif
  if(setChannelFreq(ch1freq, 0)) return EXIT_FAILURE;

#ifdef DEBUG
  Serial.println("Setting CH2");
#endif
  if(setChannelFreq(ch2freq, 1)) return EXIT_FAILURE;

#ifdef DEBUG
  Serial.print("\nChannel 1 Frequency: ");
  Serial.println(CHfreq[0]);
  Serial.print("\nChannel 2 Frequency: ");
  Serial.println(CHfreq[1]);
#endif

  return EXIT_SUCCESS;
}

uint8_t CDCE6214::setD(double divider)
{
  uint16_t div = 0x0400 + floor(divider);
  if(writeToReg(DIVIDER_ADDRESS, div)) return EXIT_FAILURE;
  D = divider;
  return EXIT_SUCCESS;
}

uint8_t CDCE6214::setN(int n)
{
  if(writeToReg(N_ADDRESS, n)) return EXIT_FAILURE;
  N = n;
  return EXIT_SUCCESS;
}

uint8_t CDCE6214::setPSA(int psa)
{
  uint16_t ps;

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
#ifdef DEBUG
    Serial.println("Error, PSA Must be 4, 5 or 6");
#endif
    return EXIT_FAILURE;
  }
  if(writeToReg(PSA_ADDRESS, ps)) return EXIT_FAILURE;
  PSA = psa;
  return EXIT_SUCCESS;
}

uint8_t CDCE6214::setCHD(int chd, int channel)
{
  if(channel == 0) return writeToReg(CH1_ADDRESS, chd);
  else if(channel == 1) return writeToReg(CH2_ADDRESS, chd);
  return EXIT_FAILURE;
}

uint8_t CDCE6214::setFreq(double freq, int channel)
{
  if(freq > 125 || freq <= 0.01)
  {
#ifdef DEBUG
    Serial.println("Frequency must be between 0.01 and 125 MHz");
#endif
    return EXIT_FAILURE;
  }

  CHD[channel] = floor(crystal_freq * N / (D * PSA * freq));
  CHfreq[channel] = crystal_freq * N / (D * PSA * CHD[channel]);

  return EXIT_SUCCESS;
}

uint8_t CDCE6214::setChannelFreq(double freq, int channel)
{
  if(setFreq(freq, channel)) return EXIT_FAILURE;
  return setCHD(CHD[channel], channel);
}

uint8_t CDCE6214::writeToReg(uint16_t reg, uint16_t data)
{
#ifdef DEBUG
  Serial.print("Writing to Register 0x");
  Serial.print(reg, HEX);
  Serial.print(" 0x");
  Serial.println(data, HEX);
#endif

  Wire.beginTransmission(_address);
  Wire.write((reg >> 8) & 0xFF); // Upper byte of register
  Wire.write(reg & 0xFF);        // Lower byte of register
  Wire.write((data >> 8) & 0xFF); // Upper byte of data
  Wire.write(data & 0xFF);        // Lower byte of data
  uint8_t err = Wire.endTransmission();

  if (err != 0) {
#ifdef DEBUG
    Serial.print("I2C write failed with error code: ");
    Serial.println(err);
#endif
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

double CDCE6214::getChannelFreq(int channel)
{
  return CHfreq[channel];
}
