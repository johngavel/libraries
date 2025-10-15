#include "debug.h"

#include "debugscreen.h"
#include "serialport.h"

#include <Arduino.h>

void DEBUG(String s) {
  CONSOLE->println(TRACE, s);
  if (DEBUG_SCREEN_AVAILABLE) DEBUG_SCREEN->add(s);
}
