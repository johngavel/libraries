#include "watchdog.h"

#include "serialport.h"

Watchdog* Watchdog::watchdog = nullptr;

Watchdog* Watchdog::get() {
  if (watchdog == nullptr) watchdog = new Watchdog();
  return watchdog;
}

void Watchdog::setupTask() {
  rp2040.wdt_begin(watchdogTimeout);
  setRefreshMilli(watchdogPetCycle);
  petWatchdog();
  watchdogRunning = true;
  CONSOLE->println(PASSED, "Watchdog Complete");
}

void Watchdog::tickle() {
  static int lastCore = 0;
  int nextCore;
  core = rp2040.cpuid();
  lock.take();
  execution.start();
  if ((lastCore == 0) && (monitorCore[1] == true))
    nextCore = 1;
  else if ((lastCore == 1) && (monitorCore[0] == true))
    nextCore = 0;
  else
    nextCore = core;
  if ((core == nextCore) && (expired())) {
    lastCore = core;
    petWatchdog();
  }
  execution.stop();
  lock.give();
}

void Watchdog::petWatchdog() {
  if (resetFlag)
    rp2040.wdt_reset();
  else if (rebootTimer.expired())
    rp2040.reboot();
}

void Watchdog::reboot() {
  void (*cmd)() = (void (*)()) rebootCallback;
  if (rebootCallback) (*cmd)();
  if (!watchdogRunning) { rp2040.reboot(); }
  lock.take();
  petWatchdog();
  resetFlag = false;
  rebootTimer.setRefreshMilli(1000);
  rebootTimer.runTimer(true);
  lock.give();
}

void Watchdog::monitor(unsigned long core, bool monitor) {
  monitorCore[core] = monitor;
}
