#include <Arduino.h>
#include "external_clock.h"
#include "definitions.h"

CDCE6214 ext_clock(5,6);

void setup() 
{
  // Serial.begin(9600);
  // Initialize Clocks
  while(!(Serial.read() == 'y'));

  Serial.println("Init start"); // Default 40 MHz MCU and ~50 MHz ADC
  if(ext_clock.init())
  {
    Serial.println("Error Initializing Clock");
    return;
  } 

  Serial.println("Init success");
}

void loop() 
{

}
