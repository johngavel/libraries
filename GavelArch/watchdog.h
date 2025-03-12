#ifndef __GAVEL_WATCHDOG
#define __GAVEL_WATCHDOG

#include "architecture.h"

#define WATCHDOG Watchdog::get()

class Watchdog : public Task {
public:
  static Watchdog* get();
  void setupTask();
  void executeTask();
  void petWatchdog();
  void reboot();
  void monitor(unsigned long core, bool monitor);

private:
  Watchdog(uint32_t timeout = 8300, uint32_t petCycle = 1000)
      : Task("Watchdog"), watchdogTimeout(timeout), watchdogPetCycle(petCycle), resetFlag(true), watchdogRunning(false) {
    monitorCore[0] = true;
    monitorCore[1] = true;
  };
  static Watchdog* watchdog;

  uint32_t watchdogTimeout;
  uint32_t watchdogPetCycle;
  bool resetFlag;
  Timer rebootTimer;
  bool monitorCore[CPU_CORES];
  bool watchdogRunning;
};

#endif