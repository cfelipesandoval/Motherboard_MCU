#include <Arduino.h>
#include "CDCE_6214.h"
#include "ADC_3644.h"
#include "Rpi_Communicator.h"
#include "definitions.h"

CDCE6214 ext_clock(GPIO_NUM_8, GPIO_NUM_9); // Need to make sure this is GPIO 8 and 9 (SDA and SCL)
ADC3644 adc(&ext_clock);
RpiCommunicator raspi(&adc);

void setup()
{
  while(!(Serial.read() == 'y'));

  setCpuFrequencyMhz(240);

  Serial.println("Init start"); // Default 40 MHz MCU and ~50 MHz ADC
  
  // // Initialize Clocks
  Serial.println("\nInitiazlizing Clocks");

  if(ext_clock.init(40, 50))
  {
    Serial.println("Error Initializing Clock");
    return;
  }

  Serial.println("Successful Clocks Initialization");

  // ADC Initialization
  Serial.println("\nInitializing ADC");

  adc.init();

  Serial.println("Successful ADC Initialization");

  Serial.println("Initializing Communication with Raspberry PI");

  raspi.begin();

  Serial.println("Successful Communication with Raspberry PI Initialization");

  Serial.println("\nInit success");
}

void loop()
{
  raspi.update();
}

