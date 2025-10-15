#ifndef __GAVEL_BLINK
#define __GAVEL_BLINK

#include "architecture.h"

#define BLINK Blink::get()
#define BLINK_AVAILABLE Blink::initialized()

class Blink : public Task {
public:
  static Blink* get();
  static bool initialized() { return blink != nullptr; };
  virtual void setupTask();
  virtual void executeTask();

private:
  Blink() : Task("Blink"), state(true){};
  static Blink* blink;
  bool state;
};

#endif
