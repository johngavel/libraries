#include "serialport.h"

#include "gpio.h"
#include "license.h"
#include "stringutils.h"
#include "watchdog.h"

void rebootPico(Terminal* terminal);
void uploadPico(Terminal* terminal);

SerialPort* SerialPort::serialPort = nullptr;

SerialPort* SerialPort::get() {
  if (serialPort == nullptr) serialPort = new SerialPort();
  return serialPort;
}

void SerialPort::serialSetup() {
  if (ProgramInfo::hardwareserial.serialUART != nullptr) {
    ProgramInfo::hardwareserial.serialUART->setRX(ProgramInfo::hardwareserial.pinrx);
    ProgramInfo::hardwareserial.serialUART->setTX(ProgramInfo::hardwareserial.pintx);
    ProgramInfo::hardwareserial.serialUART->begin(115200);
    terminal = new Terminal(ProgramInfo::hardwareserial.serialUART);
    terminal->setup();
    terminal->useColor(true);
    terminal->setPrompt(String(ProgramInfo::ShortName) + ":\\>");
    terminal->setBannerFunction(banner);
    Serial.begin();
    terminalUSB = new Terminal(&Serial);
    terminalUSB->setup();
    terminalUSB->configure(terminal);
  } else {
    Serial.begin();
    terminal = new Terminal(&Serial);
    terminal->setup();
    terminal->useColor(true);
    terminal->setPrompt(String(ProgramInfo::ShortName) + ":\\>");
    terminal->setBannerFunction(banner);
    terminalUSB = nullptr;
  }
}

void SerialPort::setupTask() {
  setRefreshMilli(10);
  TERMINAL_LICENSE;
  GPIO->configurePinReserve(GPIO_INTERNAL, ProgramInfo::hardwareserial.pintx, "Serial UART Tx");
  GPIO->configurePinReserve(GPIO_INTERNAL, ProgramInfo::hardwareserial.pinrx, "Serial UART Rx");
  TERM_CMD->addCmd("reboot", "", "Software Reboot the Pico", rebootPico);
  TERM_CMD->addCmd("upload", "", "Software Reboot the Pico into USB mode", uploadPico);

  CONSOLE->println(PASSED, "Serial Port Complete");
}

void SerialPort::executeTask() {
  if (terminal != nullptr) terminal->loop();
  if (terminalUSB != nullptr) terminalUSB->loop();
}

void rebootPico(Terminal* terminal) {
  terminal->println(WARNING, "Rebooting....");
  delay(100);
  WATCHDOG->reboot();
}

void uploadPico(Terminal* terminal) {
  terminal->println(WARNING, "Rebooting in USB Mode....");
  delay(100);
  rp2040.rebootToBootloader();
}

extern const char* stringHardware(HW_TYPES hw_type);

void banner(Terminal* terminal) {
  terminal->println();
  terminal->println(PROMPT, String(ProgramInfo::AppName) + " Version: " + String(ProgramInfo::MajorVersion) + String(".") + String(ProgramInfo::MinorVersion) +
                                String(".") + String(ProgramInfo::BuildVersion));
  terminal->println(INFO, "Program: " + String(ProgramInfo::ProgramNumber));
  terminal->println(INFO, "Build Date: " + String(ProgramInfo::compileDate) + " Time: " + String(ProgramInfo::compileTime));
  terminal->println();
  terminal->print(INFO, "Microcontroller: ");
  terminal->println(INFO, stringHardware(ProgramInfo::hw_type));
  terminal->print(INFO, "Core is running at ");
  terminal->print(INFO, String(rp2040.f_cpu() / 1000000));
  terminal->println(INFO, " Mhz");
  int used = rp2040.getUsedHeap();
  int total = rp2040.getTotalHeap();
  int percentage = (used * 100) / total;
  terminal->print(INFO, "RAM Memory Usage: ");
  terminal->print(INFO, String(used));
  terminal->print(INFO, "/");
  terminal->print(INFO, String(total));
  terminal->print(INFO, " --> ");
  terminal->print(INFO, String(percentage));
  terminal->println(INFO, "%");
  terminal->print(INFO, "CPU Temperature: ");
  terminal->print(INFO, String((9.0 / 5.0 * analogReadTemp()) + 32.0, 0));
  terminal->println(INFO, "°F.");
}
