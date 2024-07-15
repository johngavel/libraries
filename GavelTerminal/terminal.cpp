#include "terminal.h"

#include "architecture.h"
#include "debug.h"
#include "stringutils.h"
#include "termcmd.h"

#define BS_CHAR (char) 0x08
#define NL_CHAR (char) 0x0A
#define CR_CHAR (char) 0x0D
#define DEL_CHAR (char) 0x7F
#define ESC_CHAR (char) 0x1B

void Terminal::__print(String line) {
  if (stream == nullptr) return;
  COMM_TAKE;
  terminalMutex.take();
  stream->print(line);
  terminalMutex.give();
  COMM_GIVE;
}

void Terminal::__print(char character) {
  __print(String(character));
}

void Terminal::__println(String line) {
  if (stream == nullptr) return;
  COMM_TAKE;
  terminalMutex.take();
  stream->println(line);
  terminalMutex.give();
  COMM_GIVE;
}

void Terminal::__println(char character) {
  __println(String(character));
}

void Terminal::printColor(COLOR color) {
  char colorString[32];
  sprintf(colorString, "\033[%dm", color);
  __print(colorString);
}

void Terminal::print(COLOR color, String line) {
  printColor(color);
  __print(line);
  printColor(Normal);
}

void Terminal::print(PRINT_TYPES type, String line) {
  printColor(Normal);
  switch (type) {
  case TRACE:
    __print("[");
    print(Cyan, "  DEBUG ");
    __print("] ");
    printColor(Cyan);
    break;
  case PROMPT: printColor(Green); break;
  case ERROR:
    __print("[");
    print(Red, "  ERROR ");
    __print("] ");
    printColor(Red);
    break;
  case PASSED:
    __print("[");
    print(Green, "   OK   ");
    __print("] ");
    break;
  case FAILED:
    __print("[");
    print(Red, " FAILED ");
    __print("] ");
    break;
  case WARNING: printColor(Magenta); break;
  case HELP:
  case INFO:
  default: break;
  }
  __print(line);
  printColor(Normal);
}

void Terminal::print(PRINT_TYPES type, String line, String line2) {
  printColor(Normal);
  switch (type) {
  case HELP:
    print(type, line);
    printColor(Yellow);
    __print(line2);
    printColor(Normal);
    break;
  case ERROR:
    print(type, line);
    print(WARNING, line2);
    break;
  case PASSED:
  case FAILED:
    print(type, line);
    print(INFO, line2);
    break;
  default:
    print(type, line);
    print(type, line2);
    break;
  }
}
void Terminal::println() {
  __println(String(""));
}

void Terminal::println(PRINT_TYPES type, String line) {
  print(type, line);
  println();
}

void Terminal::println(PRINT_TYPES type, String line, String line2) {
  print(type, line, line2);
  println();
}

void Terminal::hexdump(byte* buffer, unsigned long length) {
  char* string = (char*) printBuffer(buffer, length);
  while (string != nullptr) {
    println(TRACE, String(string));
    string = (char*) printBuffer(nullptr, 0);
  }
}

void Terminal::setupTask() {
  setupReading();
  echo = true;
}

void Terminal::executeTask() {
  ReadLineReturn ret;
  ret = readline();

  if (ret == ERROR_NO_CMD_FOUND) {
    println();
    print(ERROR, "Unrecognized command: ");
    println(WARNING, lastCmd());
    println(INFO, "Enter \'?\' or \'help\' for a list of commands.");
    prompt();
  }
}

char* Terminal::readParameter() {
  return strtok_r(NULL, " ", &parameterParseSave);
}

void Terminal::invalidParameter() {
  println();
  println(ERROR, "Unrecognized parameter: " + TERM_CMD->getParameter(lastCmdIndex) + ": ");
  println(WARNING, lastCmd());
  println(INFO, "Enter \'?\' or \'help\' for a list of commands.");
}

extern const char* stringHardware(HW_TYPES hw_type);

void Terminal::banner() {
  println();
  println(PROMPT, String(ProgramInfo::AppName) + " Version: " + String(ProgramInfo::MajorVersion) + String(".") + String(ProgramInfo::MinorVersion));
  println(INFO, "Program: " + String(ProgramInfo::ProgramNumber));
  println(INFO, "Build Date: " + String(ProgramInfo::compileDate) + " Time: " + String(ProgramInfo::compileTime));
  println();
  print(INFO, "Microcontroller: ");
  println(INFO, stringHardware(ProgramInfo::hw_type));
  print(INFO, "Core is running at ");
  print(INFO, String(rp2040.f_cpu() / 1000000));
  println(INFO, " Mhz");
  int used = rp2040.getUsedHeap();
  int total = rp2040.getTotalHeap();
  int percentage = (used * 100) / total;
  print(INFO, "RAM Memory Usage: ");
  print(INFO, String(used));
  print(INFO, "/");
  print(INFO, String(total));
  print(INFO, " --> ");
  print(INFO, String(percentage));
  println(INFO, "%");
  print(INFO, "CPU Temperature: ");
  print(INFO, String((9.0 / 5.0 * analogReadTemp()) + 32.0, 0));
  println(INFO, "°F.");
}

void Terminal::prompt() {
  print(PROMPT, String(ProgramInfo::ShortName) + ":\\> ");
}

void Terminal::setupReading() {
  memset(cmdBuffer, 0, MAX_LINE);
  memset(parameterParsing, 0, MAX_LINE);
}

ReadLineReturn Terminal::readline() {
  char readChar[3];
  int available = 0;
  ReadLineReturn functionCalled = NO_PROCESSING;
  if (stream == nullptr) return functionCalled;
  COMM_TAKE;
  available = stream->available();
  COMM_GIVE;
  if (available > 0) {
    COMM_TAKE;
    stream->readBytes(&readChar[0], 1);
    COMM_GIVE;
    if (isPrintable(readChar[0])) {
      if (cmdBufferIndex < MAX_LINE) {
        if (echo) __print(readChar[0]);
        cmdBuffer[cmdBufferIndex] = readChar[0];
        cmdBufferIndex++;
      }
    } else if (readChar[0] == CR_CHAR) {
      if (cmdBufferIndex > 0) {
        char* cmdName;
        functionCalled = ERROR_NO_CMD_FOUND;
        if (echo) __print(readChar[0]);
        if (lastBuffer.count() >= HISTORY) { lastBuffer.pop(); }
        lastBuffer.push(cmdBuffer);
        historyIndex = lastBuffer.count() - 1;
        memset(parameterParsing, 0, MAX_LINE);
        memcpy(parameterParsing, cmdBuffer, MAX_LINE);
        memset(cmdBuffer, 0, MAX_LINE);
        cmdBufferIndex = 0;
        cmdName = strtok_r(parameterParsing, " ", &parameterParseSave);
        int cmdIndex = TERM_CMD->findCmd(String(cmdName));
        if (cmdIndex != -1) {
          lastCmdIndex = cmdIndex;
          functionCalled = HELP_FUNCTION_CALLED;
          TERM_CMD->callFunction(cmdIndex, this);
        }
      } else {
        functionCalled = EMPTY_STRING;
        if (echo) println();
        prompt();
      }
    } else if (readChar[0] == NL_CHAR) {
      if (echo) __print(NL_CHAR);
    } else if (readChar[0] == DEL_CHAR) {
      if (cmdBufferIndex > 0) {
        cmdBufferIndex--;
        cmdBuffer[cmdBufferIndex] = 0;
        if (echo) __print(DEL_CHAR);
      }
    } else if (readChar[0] == BS_CHAR) {
      if (cmdBufferIndex > 0) {
        cmdBufferIndex--;
        cmdBuffer[cmdBufferIndex] = 0;
        __print(" ");
        __print(BS_CHAR);
      }
    } else if (readChar[0] == ESC_CHAR) {
      COMM_TAKE;
      while (stream->available() < 2)
        ;
      stream->readBytes(&readChar[0], 2);
      COMM_GIVE;
      if (readChar[0] == '[') {
        if (readChar[1] == 'A') { // UP Arrow
          for (unsigned long i = 0; i < cmdBufferIndex; i++) __print(DEL_CHAR);
          memset(cmdBuffer, 0, MAX_LINE);
          cmdBufferIndex = 0;
          lastBuffer.get(historyIndex, cmdBuffer);
          if (historyIndex > 0) historyIndex--;
          cmdBufferIndex = strlen(cmdBuffer);
          __print(cmdBuffer);
        } else if (readChar[1] == 'B') { // Down Arrow
          for (unsigned long i = 0; i < cmdBufferIndex; i++) __print(DEL_CHAR);
          memset(cmdBuffer, 0, MAX_LINE);
          cmdBufferIndex = 0;
          if (historyIndex < (lastBuffer.count() - 1)) historyIndex++;
          lastBuffer.get(historyIndex, cmdBuffer);
          cmdBufferIndex = strlen(cmdBuffer);
          __print(cmdBuffer);
        } else
          DEBUG("Unknown escape sequence = " + String(readChar[0]) + String(readChar[1]));
      }
    } else
      DEBUG("Unknown character = " + String((int) readChar[0], HEX));
  }
  return functionCalled;
}

char* Terminal::lastCmd() {
  return (char*) lastBuffer.get(lastBuffer.count() - 1);
}

void Terminal::helpHist(Terminal* terminal) {
  terminal->println();
  terminal->println(INFO, "Command History");
  for (unsigned long i = 0; i < terminal->lastBuffer.count(); i++) terminal->println(HELP, String(i) + ". ", (char*) terminal->lastBuffer.get(i));
  terminal->println(PASSED, "Command History");
  terminal->prompt();
}
