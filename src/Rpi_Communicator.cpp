#include "Rpi_Communicator.h"

RpiCommunicator::RpiCommunicator(ADC3644* adc_ptr)
  : adc(adc_ptr) {}

void RpiCommunicator::begin()
{
  #ifdef DEBUG
  Serial.println("ESP32 RpiCommunicator starting with USB CDC...");
  #endif

  // Register command handlers
  registerCommand("N", &RpiCommunicator::handleSetNCOfreq);
  registerCommand("D", &RpiCommunicator::handleSetDecimationBy);
  registerCommand("G", &RpiCommunicator::handleSetGain);
  registerCommand("C", &RpiCommunicator::handleSetClockFreq);
  registerCommand("R", &RpiCommunicator::handleReset);
}

void RpiCommunicator::update() 
{
  if (!Serial.available()) return;

  while (Serial.available() > 0) 
  {
    char c = Serial.read();

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
  Serial.print(message);

  #ifdef DEBUG
  Serial.println("Sent: " + message);
  #endif
}

uint8_t RpiCommunicator::processMessage(String message) 
{
  #ifdef DEBUG
  Serial.println("Received: " + message);
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

  for (int i = 0; i < numCommands; i++) 
  {
    if (commandHandlers[i].command == command) 
    {
      (this->*commandHandlers[i].handler)(data);
      return EXIT_SUCCESS;
    }
  }

  sendMessage("A", "Unknown command: " + command);
  return EXIT_FAILURE;
}

String RpiCommunicator::resultToMessage(ADCResult res)
{
  switch (res) 
  {
    case ADC_OK: return "OK";
    case ADC_ERR_DECIMATION: return "Invalid decimation value";
    case ADC_ERR_NCO_RANGE: return "NCO frequency out of range";
    case ADC_ERR_CLOCK_SET: return "Failed to set clock frequency";
    case ADC_ERR_GAIN_INVALID: return "Invalid gain value";
    case ADC_ERR_WRITE_FAIL: return "Failed to write to register";
    default: return "Unknown ADC error";
  }
}

// Command handlers

void RpiCommunicator::handleSetNCOfreq(String data)
{
  ADCResult res = adc->setNCOfreq(data.toDouble());
  if (res != ADC_OK) 
  {
    sendMessage("E", resultToMessage(res));
    return;
  }
  sendMessage("N", "NCO Frequency set to " + String(adc->getNCOFreq()) + " MHz");
}

void RpiCommunicator::handleSetDecimationBy(String data)
{
  ADCResult res = adc->setDecimationBy(data.toInt());
  if (res != ADC_OK) 
  {
    sendMessage("E", resultToMessage(res));
    return;
  }
  sendMessage("D", "Decimation By set to " + String(adc->getDecimationBy()));
}

void RpiCommunicator::handleSetGain(String data)
{
  ADCResult res = adc->setGain(data.toInt());
  if (res != ADC_OK) 
  {
    sendMessage("E", resultToMessage(res));
    return;
  }
  sendMessage("G", "Gain set to " + String(adc->getGain()) + " dB");
}

void RpiCommunicator::handleSetClockFreq(String data)
{
  ADCResult res = adc->setClockFreq(data.toDouble());
  if (res != ADC_OK) 
  {
    sendMessage("E", resultToMessage(res));
    return;
  }
  sendMessage("C", "Clock Frequency set to " + String(adc->getClockFreq()) + " MHz");
}

void RpiCommunicator::handleReset(String unused)
{
  ADCResult res = adc->reset();
  if (res != ADC_OK) 
  {
    sendMessage("E", resultToMessage(res));
    return;
  }
  sendMessage("R", "ADC Successfully Reset");
}
