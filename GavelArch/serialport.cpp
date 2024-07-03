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
  setupReading();
  HARDWARE_PORT->setRX(ProgramInfo::hardwareserial.pinrx);
  HARDWARE_PORT->setTX(ProgramInfo::hardwareserial.pintx);
  HARDWARE_PORT->begin(115200);
}

void SerialPort::__print(String line) {
  serialMutex.take();
  HARDWARE_PORT->print(line);
  serialMutex.give();
}

void SerialPort::__println(String line) {
  serialMutex.take();
  HARDWARE_PORT->println(line);
  serialMutex.give();
}

void SerialPort::printColor(COLOR color) {
  char colorString[32];
  sprintf(colorString, "\033[%dm", color);
  __print(colorString);
}

void SerialPort::print(COLOR color, String line) {
  printColor(color);
  __print(line);
  printColor(Normal);
}

void SerialPort::print(PRINT_TYPES type, String line) {
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

void SerialPort::print(PRINT_TYPES type, String line, String line2) {
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
void SerialPort::println() {
  __println(String(""));
}

void SerialPort::println(PRINT_TYPES type, String line) {
  print(type, line);
  println();
}

void SerialPort::println(PRINT_TYPES type, String line, String line2) {
  print(type, line, line2);
  println();
}

void SerialPort::hexdump(byte* buffer, unsigned long length) {
  char* string = (char*) printBuffer(buffer, length);
  while (string != nullptr) {
    println(TRACE, String(string));
    string = (char*) printBuffer(nullptr, 0);
  }
}

int SerialPort::addCmd(String command, String parameterDesc, String description, void (*function)()) {
  return list.setHelp(command, parameterDesc, description, function);
}

void SerialPort::setupTask() {
  setRefreshMilli(10);
  GPIO->configurePinReserve(GPIO_INTERNAL, ProgramInfo::hardwareserial.pintx, "Serial UART Tx");
  GPIO->configurePinReserve(GPIO_INTERNAL, ProgramInfo::hardwareserial.pinrx, "Serial UART Rx");

  addCmd("?", "", "Print Help", SerialPort::help);
  addCmd("help", "", "Print Help", SerialPort::help);
  addCmd("history", "", "Command History", SerialPort::helpHist);
  addCmd("reboot", "", "Software Reboot the Pico", SerialPort::rebootPico);
  addCmd("upload", "", "Software Reboot the Pico into USB mode", SerialPort::uploadPico);
  PORT->println(PASSED, "Serial Port Complete");
}

void SerialPort::executeTask() {
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

void SerialPort::complete() {
  banner();
  prompt();
}

char* SerialPort::readParameter() {
  return nextParameter();
}

void SerialPort::invalidParameter() {
  println();
  println(ERROR, "Unrecognized parameter: " + list.getParameter(lastCmdIndex) + ": ");
  println(WARNING, lastCmd());
  println(INFO, "Enter \'?\' or \'help\' for a list of commands.");
}

void SerialPort::rebootPico() {
  PORT->println();
  PORT->println(WARNING, "Rebooting....");
  delay(100);
  WATCHDOG->reboot();
}

void SerialPort::uploadPico() {
  PORT->println();
  PORT->println(WARNING, "Rebooting in USB Mode....");
  delay(100);
  rp2040.rebootToBootloader();
}
extern const char* stringHardware(HW_TYPES hw_type);

void SerialPort::banner() {
  PORT->println();
  PORT->println(PROMPT, String(ProgramInfo::AppName) + " Version: " + String(ProgramInfo::MajorVersion) + String(".") + String(ProgramInfo::MinorVersion));
  PORT->println(INFO, "Program: " + String(ProgramInfo::ProgramNumber));
  PORT->println(INFO, "Build Date: " + String(ProgramInfo::compileDate) + " Time: " + String(ProgramInfo::compileTime));
  PORT->println();
  PORT->print(INFO, "Microcontroller: ");
  PORT->println(INFO, stringHardware(ProgramInfo::hw_type));
  PORT->print(INFO, "Core is running at ");
  PORT->print(INFO, String(rp2040.f_cpu() / 1000000));
  PORT->println(INFO, " Mhz");
  int used = rp2040.getUsedHeap();
  int total = rp2040.getTotalHeap();
  int percentage = (used * 100) / total;
  PORT->print(INFO, "RAM Memory Usage: ");
  PORT->print(INFO, String(used));
  PORT->print(INFO, "/");
  PORT->print(INFO, String(total));
  PORT->print(INFO, " --> ");
  PORT->print(INFO, String(percentage));
  PORT->println(INFO, "%");
  PORT->print(INFO, "CPU Temperature: ");
  PORT->print(INFO, String((9.0 / 5.0 * analogReadTemp()) + 32.0, 0));
  PORT->println(INFO, "°F.");
}

void SerialPort::help() {
  banner();
  PORT->println();

  int maxStringLength = 0;
  for (int i = 0; i < PORT->list.getHelpCount(); i++) {
    int length = PORT->list.getCmd(i).length() + PORT->list.getParameter(i).length() + 2;
    maxStringLength = (maxStringLength > length) ? maxStringLength : length;
  }
  for (int i = 0; i < PORT->list.getHelpCount(); i++) {
    String line1 = PORT->list.getCmd(i) + " " + PORT->list.getParameter(i) + "                                         ";
    String line2 = "- " + PORT->list.getDescription(i);
    line1 = line1.substring(0, maxStringLength);
    PORT->println(HELP, line1, line2);
  }

  PORT->println();
  PORT->prompt();
}

void SerialPort::prompt() {
  print(PROMPT, String(ProgramInfo::ShortName) + ":\\> ");
}
