#ifndef __GAVEL_SERIAL
#define __GAVEL_SERIAL

#include "architecture.h"
#include "lock.h"

#include <Terminal.h>

#define PORT SerialPort::get()
#define PORT_AVAILABLE SerialPort::initialized()
#define CONSOLE ((PORT->terminal) ? (PORT->terminal) : (PORT->terminalUSB))

void banner(Terminal* terminal);

class SerialPort : public Task {
public:
  static SerialPort* get();
  static bool initialized() { return serialPort != nullptr; };
  void configurePins(int __txPin, int __rxPin);
  void setupTask();
  void executeTask();
  Terminal* terminal;
  Terminal* terminalUSB;

private:
  SerialPort() : Task("SerialPort") { serialSetup(); };
  static SerialPort* serialPort;

  void serialSetup();
};

#endif
