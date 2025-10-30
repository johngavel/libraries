#ifndef __GAVEL_EXPANDER
#define __GAVEL_EXPANDER

#if __has_include("library_used.h")
#include "library_used.h"
#else
#include "library_everything.h"
#endif

#ifdef TCA9555_GPIO
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
#endif
