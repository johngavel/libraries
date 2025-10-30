#ifndef __GAVEL_SERIAL
#define __GAVEL_SERIAL

#include "../GavelTask/task.h"
#include "../GavelUtil/lock.h"

#include <Terminal.h>

class SerialPort : public Task {
public:
  SerialPort();
  void addCmd(TerminalCommand* __termCmd);
  bool setupTask(OutputInterface* __terminal);
  bool executeTask();

  void configureSerial1(int __txPin, int __rxPin);
  void configureUSBSerial();
  OutputInterface* getMainSerialPort();
  OutputInterface* getSerial1Terminal() { return terminalSerial1; };
  OutputInterface* getUSBSerialTerminal() { return terminalUSB; };

private:
  Terminal* terminalSerial1 = nullptr;
  Terminal* terminalUSB = nullptr;
};

#endif
