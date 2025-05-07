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
    String receivedMessage = "";
    bool messageInProgress = false;

    typedef void (RpiCommunicator::*CommandHandler)(String);

    struct CommandMapping 
    {
      String command;
      CommandHandler handler;
    };

    static const int MAX_COMMANDS = 10;
    CommandMapping commandHandlers[MAX_COMMANDS];
    int numCommands = 0;

    ADC3644* adc;

    String resultToMessage(ADCResult res);

    void handleSetNCOfreq(String f_nco);
    void handleSetDecimationBy(String dec);
    void handleSetGain(String gain);
    void handleSetClockFreq(String freq);
    void handleReset(String unused);

  public:
    RpiCommunicator(ADC3644* adc_ptr);
    void begin();
    void update();
    void registerCommand(String cmd, CommandHandler handler);
    void sendMessage(String command, String data = "");
    uint8_t processMessage(String message);
};

#endif
