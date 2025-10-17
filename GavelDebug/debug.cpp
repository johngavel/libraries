#include "debug.h"

#include "debugscreen.h"
#include "serialport.h"
#include "telnet.h"

#include <Arduino.h>

void DEBUG(String s) {
  CONSOLE->println(TRACE, s);
  if (DEBUG_SCREEN_AVAILABLE) DEBUG_SCREEN->add(s);
  if (TELNET_AVAILABLE) TELNET->terminal->println(TRACE, s);
}
