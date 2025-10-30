#ifndef __GAVEL_IDLE_H
#define __GAVEL_IDLE_H

#include "task.h"

class IdleTask : public Task {
public:
  IdleTask();
  void addCmd(TerminalCommand* __termCmd) {};
  bool setupTask(OutputInterface* __terminal) { return true; };
  bool loop();
  bool executeTask() {
    delay(delay_ms);
    delay_ms = 0;
    return true;
  };
  void setDelay(unsigned long __delay_ms) {
    unsigned long __time = (__delay_ms > 100) ? 100 : __delay_ms;
    lock.take();
    delay_ms = __time;
    lock.give();
  };

private:
  unsigned int delay_ms = 0;
};

#endif