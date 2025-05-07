#ifndef CDCE_6214_H
#define CDCE_6214_H

#include <Arduino.h>
#include <Wire.h>
#include "definitions.h"

class CDCE6214
{
  public:
    CDCE6214(uint8_t sda = SDA, uint8_t scl = SCL, uint8_t clock_address = CLOCK_ADDRESS);

    uint8_t init(double ch1freq = 40, double ch2freq = 50);

    uint8_t setD(double divider);
    uint8_t setN(int n);
    uint8_t setPSA(int psa);
    uint8_t setCHD(int chd, int channel);

    uint8_t setChannelFreq(double freq, int channel);
    double getChannelFreq(int channel);

  private:
    uint8_t setFreq(double freq, int channel);
    uint8_t writeToReg(uint16_t reg, uint16_t data);

    uint8_t _address; // I2C Address
    double crystal_freq;

    double D;
    int N;
    int PSA;
    int CHD[2]; // Channel dividers
    double CHfreq[2]; // Output frequencies for channels 1 and 2 (0 or 1 respectively)
};

#endif
