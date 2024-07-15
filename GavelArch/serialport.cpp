#include "serialport.h"

#include "gpio.h"
#include "stringutils.h"
#include "watchdog.h"

SerialPort* SerialPort::serialPort = nullptr;

SerialPort* SerialPort::get() {
  if (serialPort == nullptr) serialPort = new SerialPort();
  return serialPort;
}

void SerialPort::serialSetup() {
  HARDWARE_PORT->setRX(ProgramInfo::hardwareserial.pinrx);
  HARDWARE_PORT->setTX(ProgramInfo::hardwareserial.pintx);
  HARDWARE_PORT->begin(115200);
  terminal = new Terminal(HARDWARE_PORT);
  terminal->setupTask();
}

void SerialPort::setupTask() {
  setRefreshMilli(10);
  GPIO->configurePinReserve(GPIO_INTERNAL, ProgramInfo::hardwareserial.pintx, "Serial UART Tx");
  GPIO->configurePinReserve(GPIO_INTERNAL, ProgramInfo::hardwareserial.pinrx, "Serial UART Rx");

  CONSOLE->println(PASSED, "Serial Port Complete");
}

void SerialPort::executeTask() {
  terminal->executeTask();
}
