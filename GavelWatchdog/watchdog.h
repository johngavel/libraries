#ifndef __GAVEL_WATCHDOG
#define __GAVEL_WATCHDOG

#include "architecture.h"

#define WATCHDOG Watchdog::get()
#define WATCHDOG_AVAILABLE Watchdog::initialized()

class Watchdog : public Task {
public:
  static Watchdog* get();
  static bool initialized() { return watchdog != nullptr; };
  void tickle(); // This function runs on both cores unlike tasks which run 1 per cycle.
  void setupTask();
  void executeTask() {};
  void petWatchdog();
  void reboot();
  void monitor(unsigned long core, bool monitor);
  void setRebootCallback(void function()) { rebootCallback = (void*) function; };

private:
  Watchdog(uint32_t timeout = 8300, uint32_t petCycle = 1000)
      : Task("Watchdog"), watchdogTimeout(timeout), watchdogPetCycle(petCycle), resetFlag(true), watchdogRunning(false) {
    monitorCore[0] = true;
    monitorCore[1] = true;
    rebootCallback = NULL;
  };
  static Watchdog* watchdog;

  uint32_t watchdogTimeout;
  uint32_t watchdogPetCycle;
  bool resetFlag;
  Timer rebootTimer;
  bool monitorCore[CPU_CORES];
  bool watchdogRunning;
  void* rebootCallback;
};

#endif