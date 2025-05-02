#include "Rpi_Communicator.h"

RpiCommunicator::RpiCommunicator(ADC3644 * adc) 
{
  _adc = adc;

  registerCommand("N", &RpiCommunicator::handleSetNCOfreq);
  registerCommand("D", &RpiCommunicator::handleSetDecimationBy);
  registerCommand("G", &RpiCommunicator::handleSetGain);
  registerCommand("C", &RpiCommunicator::handleSetClockFreq);
}

void RpiCommunicator::begin()
{
  // Start serial communication with Raspberry Pi
  Serial2.begin(115200, SERIAL_8N1, MCU_RX_PIN, MCU_TX_PIN);
  
  // Serial for debugging
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("ESP32 RpiCommunicator starting...");
  #endif
  
  // Example: Setup an LED
  pinMode(LED_BUILTIN, OUTPUT);
}
  
void RpiCommunicator::update() 
{
  // Check for incoming messages
  while (Serial2.available() > 0) 
  {
    char c = Serial2.read();
    
    if (c == START_MARKER) 
    {
      receivedMessage = "";
      messageInProgress = true;
    }
    else if (messageInProgress && c == END_MARKER) 
    {
      messageInProgress = false;
      processMessage(receivedMessage);
    }
    else if (messageInProgress) 
    {
      receivedMessage += c;
    }
  }
}
  
void RpiCommunicator::registerCommand(String cmd, CommandHandler handler) 
{
  if (numCommands < MAX_COMMANDS) 
  {
    commandHandlers[numCommands].command = cmd;
    commandHandlers[numCommands].handler = handler;
    numCommands++;
  }
}
  
void RpiCommunicator::sendMessage(String command, String data) 
{
  String message = String(START_MARKER) + command + String(SEPARATOR) + data + String(END_MARKER);
  Serial2.print(message);
  
  // Debug
  #ifdef DEBUG
  Serial.println("Sent: " + message);
  #endif
}

uint8_t RpiCommunicator::processMessage(String message) 
{
  #ifdef DEBUG
  Serial.println("Received: " + message);  // Debug
  #endif

  int separatorIndex = message.indexOf(SEPARATOR);
  String command = "";
  String data = "";
  
  if (separatorIndex != -1) 
  {
    command = message.substring(0, separatorIndex);
    data = message.substring(separatorIndex + 1);
  } 
  else 
  {
    command = message;
  }
  
  // Find and execute the appropriate command handler
  for (int i = 0; i < numCommands; i++) 
  {
    if (commandHandlers[i].command == command) 
    {
      (this->*commandHandlers[i].handler)(data);
      return EXIT_SUCCESS;
    }
  }
  
  // Command not found
  sendMessage("A", "Unknown command: " + command);

  return EXIT_FAILURE;
}
  
// Command handlers
void RpiCommunicator::handleSetNCOfreq(String data)
{
  if(_adc->setNCOfreq(data.toDouble()))
  {
    sendMessage("E", "Error setting NCO Frequency");
    return;
  }

  sendMessage("N", "NCO Frequency set to " + String(_adc->getNCOFreq()) + " MHz");
}

void RpiCommunicator::handleSetDecimationBy(String data)
{
  if(_adc->setDecimationBy(data.toInt()))
  {
    sendMessage("E", "Error setting Decimation By");
    return;
  }

  sendMessage("D", "Decimation By set to " + String(_adc->getDecimationBy()));
}

void RpiCommunicator::handleSetGain(String data)
{
  if(_adc->setGain(data.toInt()))
  {
    sendMessage("E", "Error setting Gain");
    return;
  }

  sendMessage("G", "Gain set to " + String(_adc->getGain()) + " dB");
}

void RpiCommunicator::handleSetClockFreq(String data)
{
  if(_adc->setClockFreq(data.toDouble()))
  {
    sendMessage("E", "Error setting Clock Frequency");
    return;
  }

  sendMessage("C", "Clock Frequency set to " + String(_adc->getClockFreq()) + " MHz");
}