#ifndef __GAVEL_TERMINAL
#define __GAVEL_TERMINAL

#include "datastructure.h"
#include "lock.h"

typedef enum { TRACE, INFO, WARNING, ERROR, HELP, PASSED, FAILED, PROMPT } PRINT_TYPES;

typedef enum { Normal = 0, Black = 30, Red, Green, Yellow, Blue, Magenta, Cyan, White } COLOR;

typedef enum { NO_PROCESSING, HELP_FUNCTION_CALLED, EMPTY_STRING, ERROR_NO_CMD_FOUND } ReadLineReturn;

#define MAX_LINE 128
#define HISTORY 10

class Terminal {
public:
  Terminal(Stream* __stream) : stream(__stream){};
  void setStream(Stream* __stream) { stream = __stream; };
  void setupTask();
  void executeTask();
  void println();
  void print(COLOR color, String line);
  void print(PRINT_TYPES type, String line);
  void print(PRINT_TYPES type, String line, String line2);
  void println(PRINT_TYPES type, String line);
  void println(PRINT_TYPES type, String line, String line2);
  void hexdump(byte* buffer, unsigned long length);
  void banner();
  void prompt();
  char* readParameter();
  void invalidParameter();
  void setEcho(bool __echo) { echo = __echo; };
  ClassicQueue lastBuffer = ClassicQueue(HISTORY, MAX_LINE);

private:
  Stream* stream = nullptr;
  bool echo = true;

  Mutex terminalMutex;

  void printColor(COLOR color);
  void __print(String line);
  void __print(char character);
  void __println(String line);
  void __println(char character);

  void setupReading();
  ReadLineReturn readline();
  char* lastCmd();
  int lastCmdIndex;

  char cmdBuffer[MAX_LINE];
  unsigned long cmdBufferIndex = 0;
  char parameterParsing[MAX_LINE];
  char* parameterParseSave;
  unsigned long historyIndex = 0;

  static void helpHist(Terminal* terminal);
};

#endif
