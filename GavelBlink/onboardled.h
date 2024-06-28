#ifndef __GAVEL_BLINK
#define __GAVEL_BLINK

#include "architecture.h"

#define BLINK Blink::get()

class Blink : public Task {
public:
  static Blink* get();
  virtual void setupTask();
  virtual void executeTask();

private:
  Blink() : Task("Blink"), state(true){};
  static Blink* blink;
  bool state;
};

#endif
