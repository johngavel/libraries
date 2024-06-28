#ifndef __GAVEL_STRING
#define __GAVEL_STRING
#include "Arduino.h"

String tab(int lengthPrinted, int totalLength);

String hexByteString(byte value);

String decByteString(byte value);

String getMacString(byte* mac);

String getIPString(byte* ip);

String timeString(unsigned long seconds);

const char* printBuffer(byte* buffer, unsigned long length);

#endif