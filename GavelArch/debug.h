#ifndef __GAVEL_DEBUG
#define __GAVEL_DEBUG

#include "serialport.h"

#define DEBUG(s) PORT->println(TRACE, s)

#endif