#ifndef __GAVEL_EXPANDER
#define __GAVEL_EXPANDER

#include <TCA9555.h>

class EXPANDER {
public:
  EXPANDER() : configured(false), valid(false), address(0x00), TCA1(nullptr){};
  bool configured;
  bool valid;
  int address;
  TCA9555* TCA1;
};

#endif
