#ifndef __GAVEL_TIMER
#define __GAVEL_TIMER

#include <Arduino.h>

class Timer {
public:
  Timer() : refresh(micros()), timeout(100), run(true){};
  void setRefreshMilli(unsigned long time) { timeout = (unsigned long) (time * 1000.0); };
  void setRefreshMicro(unsigned long time) { timeout = time; };
  unsigned long getRefreshMilli() { return (timeout / 1000); };
  unsigned long getRefreshMicro() { return (timeout); };
  void runTimer(bool __run, unsigned long __refresh = micros()) {
    run = __run;
    reset(__refresh);
  };
  bool getTimerRun() { return run; };
  bool expired();
  int expiredMilli(unsigned long timeStamp);
  int expiredMicro(unsigned long timeStamp);
  void reset(unsigned long __refresh = micros()) { refresh = __refresh; };

  unsigned long getLastExpired() { return refresh; };

private:
  unsigned long refresh;
  unsigned long timeout;
  bool run;
};

#endif
