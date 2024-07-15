#ifndef __GAVEL_SERIAL
#define __GAVEL_SERIAL

#include "architecture.h"
#include "helplist.h"
#include "lock.h"
#include "terminal.h"

#define PORT SerialPort::get()
#define CONSOLE PORT->terminal

class SerialPort : public Task {
public:
  static SerialPort* get();
  void configurePins(int __txPin, int __rxPin);
  void setupTask();
  void executeTask();
  Terminal* terminal;

private:
  SerialPort() : Task("SerialPort") { serialSetup(); };
  static SerialPort* serialPort;

  void serialSetup();
};

#endif
