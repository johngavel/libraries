#include "datastructure.h"
#include "gpio.h"
#include "serialport.h"

#define MAX_LINE 128
static char cmdBuffer[MAX_LINE];
static unsigned long cmdBufferIndex = 0;
static char parameterParsing[MAX_LINE];
static char* parameterParseSave;
#define HISTORY 10
static ClassicQueue lastBuffer = ClassicQueue(HISTORY, MAX_LINE);
static unsigned long historyIndex = 0;

#define CR_CHAR (char) 0x0D
#define DEL_CHAR (char) 0x7F
#define ESC_CHAR (char) 0x1B

void SerialPort::setupReading() {
  memset(cmdBuffer, 0, MAX_LINE);
  memset(parameterParsing, 0, MAX_LINE);
}

ReadLineReturn SerialPort::readline() {
  char readChar[3];
  ReadLineReturn functionCalled = NO_PROCESSING;

  if (HARDWARE_PORT->available() > 0) {
    HARDWARE_PORT->readBytes(&readChar[0], 1);
    if (isPrintable(readChar[0])) {
      if (cmdBufferIndex < MAX_LINE) {
        HARDWARE_PORT->print(readChar[0]);
        cmdBuffer[cmdBufferIndex] = readChar[0];
        cmdBufferIndex++;
      }
    } else if (readChar[0] == CR_CHAR) {
      if (cmdBufferIndex > 0) {
        char* cmdName;
        functionCalled = ERROR_NO_CMD_FOUND;
        HARDWARE_PORT->print(readChar[0]);
        if (lastBuffer.count() >= HISTORY) { lastBuffer.pop(); }
        lastBuffer.push(cmdBuffer);
        historyIndex = lastBuffer.count() - 1;
        memset(parameterParsing, 0, MAX_LINE);
        memcpy(parameterParsing, cmdBuffer, MAX_LINE);
        memset(cmdBuffer, 0, MAX_LINE);
        cmdBufferIndex = 0;
        cmdName = strtok_r(parameterParsing, " ", &parameterParseSave);
        int cmdIndex = list.findHelp(String(cmdName));
        if (cmdIndex != -1) {
          lastCmdIndex = cmdIndex;
          functionCalled = HELP_FUNCTION_CALLED;
          list.callFunction(cmdIndex);
        }
      } else {
        functionCalled = EMPTY_STRING;
        HARDWARE_PORT->println();
        prompt();
      }
    } else if (readChar[0] == DEL_CHAR) {
      if (cmdBufferIndex > 0) {
        cmdBufferIndex--;
        cmdBuffer[cmdBufferIndex] = 0;
        HARDWARE_PORT->print(DEL_CHAR);
      }
    } else if (readChar[0] == ESC_CHAR) {
      while (HARDWARE_PORT->available() < 2)
        ;
      HARDWARE_PORT->readBytes(&readChar[0], 2);
      if (readChar[0] == '[') {
        if (readChar[1] == 'A') { // UP Arrow
          for (unsigned long i = 0; i < cmdBufferIndex; i++) HARDWARE_PORT->print(DEL_CHAR);
          memset(cmdBuffer, 0, MAX_LINE);
          cmdBufferIndex = 0;
          lastBuffer.get(historyIndex, cmdBuffer);
          if (historyIndex > 0) historyIndex--;
          cmdBufferIndex = strlen(cmdBuffer);
          HARDWARE_PORT->print(cmdBuffer);
        } else if (readChar[1] == 'B') { // Down Arrow
          for (unsigned long i = 0; i < cmdBufferIndex; i++) HARDWARE_PORT->print(DEL_CHAR);
          memset(cmdBuffer, 0, MAX_LINE);
          cmdBufferIndex = 0;
          if (historyIndex < (lastBuffer.count() - 1)) historyIndex++;
          lastBuffer.get(historyIndex, cmdBuffer);
          cmdBufferIndex = strlen(cmdBuffer);
          HARDWARE_PORT->print(cmdBuffer);
        } else {
        }; // DEBUG("Unknown escape sequence = " + String(readChar[0]) +
           // String(readChar[1]));
      }
    } else {
    }; // DEBUG("Unknown character = " + String((int)readChar[0], HEX));
  }
  return functionCalled;
}

char* SerialPort::lastCmd() {
  return (char*) lastBuffer.get(lastBuffer.count() - 1);
}

char* SerialPort::nextParameter() {
  return strtok_r(NULL, " ", &parameterParseSave);
}

void SerialPort::helpHist() {
  PORT->println();
  PORT->println(INFO, "Command History");
  for (unsigned long i = 0; i < lastBuffer.count(); i++) PORT->println(HELP, String(i) + ". ", (char*) lastBuffer.get(i));
  PORT->println(PASSED, "Command History");
  PORT->prompt();
}
