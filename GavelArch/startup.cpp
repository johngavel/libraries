#include "eeprom.h"
#include "gpio.h"
#include "license.h"
#include "lock.h"
#include "serialport.h"
#include "stdtermcmd.h"
#include "watchdog.h"

static Mutex startupMutex;
static Mutex startupMutex1;

void WatchDogReboot() {
  EEPROM_FORCE;
}

void setup0Start() {
  startupMutex.take();

  ARDUINO_IDE_LICENSE;
  RASPBERRY_PICO_LICENSE;

  CONSOLE->println();
  CONSOLE->println(PROMPT, "************************************************************");
  CONSOLE->println(INFO, String(ProgramInfo::AppName) + " Version: " + String(ProgramInfo::MajorVersion) + String(".") + String(ProgramInfo::MinorVersion) +
                             String(".") + String(ProgramInfo::BuildVersion));
}

void setup1Start() {
  startupMutex1.take();
}

void setup0Complete() {
  startupMutex.give();
  startupMutex1.take();
  MANAGER->setup();
  addStandardTerminalCommands();
  if (WATCHDOG_AVAILABLE) WATCHDOG->setRebootCallback(WatchDogReboot);
  startupMutex1.give();

  CONSOLE->println(PASSED, ProgramInfo::AppName + String(" - Startup Complete"));
  CONSOLE->println(PROMPT, "************************************************************");
  CONSOLE->banner();
  CONSOLE->prompt();
}

void setup1Complete() {
  startupMutex1.give();

  startupMutex.take();
  startupMutex.give();
}
