#ifndef RPI_SERIAL
#define RPI_SERIAL

#include <Arduino.h>
#include "definitions.h"
#include "ADC_3644.h"

const char START_MARKER = '<';
const char END_MARKER = '>';
const char SEPARATOR = ',';


class RpiCommunicator 
{
  private:
    uint8_t processMessage(String message);
    // Command handlers
    void handleLED(String data);
    void handleSensorRequest(String data);

    void handleSetNCOfreq(String f_nco);
    void handleSetDecimationBy(String dec);
    void handleSetGain(String dec);
    void handleSetClockFreq(String dec);

    ADC3644 * _adc;

    String receivedMessage = "";
    boolean messageInProgress = false;

    // Callback function type for command handlers
    typedef void (RpiCommunicator::*CommandHandler)(String);

    // Map to store command handlers
    struct CommandMapping 
    {
      String command;
      CommandHandler handler;
    };

    // Command handlers array
    static const int MAX_COMMANDS = 10;
    CommandMapping commandHandlers[MAX_COMMANDS];
    int numCommands = 0;

  public:
    RpiCommunicator(ADC3644 * adc);
    void begin();
    void update();
    void registerCommand(String cmd, CommandHandler handler);
    void sendMessage(String command, String data = "");
    
    // uint8_t reset();
    
    double getClockFreq();

    // Set sensor data
    // void setTemperature(float temp);
    // void setHumidity(float humid);

};


#endif