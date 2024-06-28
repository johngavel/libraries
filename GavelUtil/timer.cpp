#include "timer.h"

bool Timer::expired() {
  return (expiredMicro(micros()) > 0);
}

int Timer::expiredMilli(unsigned long timeStamp) {
  return expiredMicro(timeStamp * 1000);
}

int Timer::expiredMicro(unsigned long timeStamp) {
  int returnCount = 0;

  if (timeout == 0) {
    returnCount = 1;
  } else {
    while (run && ((timeStamp - refresh) >= timeout)) {
      refresh += timeout;
      returnCount++;
      if (returnCount > 1000) { reset(timeStamp); }
    }
  }
  return returnCount;
}
