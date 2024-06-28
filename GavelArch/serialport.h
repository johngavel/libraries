#ifndef __GAVEL_SERIAL
#define __GAVEL_SERIAL

#include "architecture.h"
#include "helplist.h"
#include "lock.h"

#define PORT SerialPort::get()
#define DEBUG(s) PORT->println(TRACE, s)

typedef enum { TRACE, INFO, WARNING, ERROR, HELP, PASSED, FAILED, PROMPT } PRINT_TYPES;

typedef enum { Normal = 0, Black = 30, Red, Green, Yellow, Blue, Magenta, Cyan, White } COLOR;

typedef enum { NO_PROCESSING, HELP_FUNCTION_CALLED, EMPTY_STRING, ERROR_NO_CMD_FOUND } ReadLineReturn;

class SerialPort : public Task {
public:
  static SerialPort* get();
  void configurePins(int __txPin, int __rxPin);
  void setupTask();
  void executeTask();
  int addCmd(String command, String parameterDesc, String description, void (*)());
  void println();
  void print(PRINT_TYPES type, String line);
  void print(PRINT_TYPES type, String line, String line2);
  void println(PRINT_TYPES type, String line);
  void println(PRINT_TYPES type, String line, String line2);
  void hexdump(byte* buffer, unsigned long length);
  static void banner();
  void prompt();
  void complete();
  char* readParameter();
  void invalidParameter();
  HelpList list;

private:
  SerialPort() : Task("SerialPort") { serialSetup(); };
  static SerialPort* serialPort;

  void serialSetup();
  Mutex serialMutex;

  void printColor(COLOR color);
  void __print(String line);
  void __println(String line);

  void setupReading();
  ReadLineReturn readline();
  char* lastCmd();
  int lastCmdIndex;
  char* nextParameter();

  static void helpHist();
  static void help();
  static void rebootPico();
  static void uploadPico();
};

#endif
