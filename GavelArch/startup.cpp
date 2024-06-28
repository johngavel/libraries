#include "gpio.h"
#include "lock.h"
#include "serialport.h"

static Mutex startupMutex;
static Mutex startupMutex1;

void setup0Start() {
  startupMutex.take();
  PORT->println();
  PORT->println(PROMPT, "************************************************************");
  PORT->println(INFO, String(ProgramInfo::AppName) + " Version: " + String(ProgramInfo::MajorVersion) + String(".") + String(ProgramInfo::MinorVersion));
}

void setup1Start() {
  startupMutex1.take();
}

void setup0Complete() {
  startupMutex.give();
  startupMutex1.take();
  MANAGER->setup();
  startupMutex1.give();

  PORT->println(PASSED, ProgramInfo::AppName + String(" - Startup Complete"));
  PORT->println(PROMPT, "************************************************************");
  PORT->complete();
}

void setup1Complete() {
  startupMutex1.give();

  startupMutex.take();
  startupMutex.give();
}
