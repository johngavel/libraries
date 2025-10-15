#include "stringutils.h"

String tab(int lengthPrinted, int totalLength) {
  String spaces = "";
  for (int i = lengthPrinted; i < totalLength; i++) spaces += " ";
  return spaces;
}

String hexByteString(byte value) {
  String string = "";
  if (value < 16) string += "0";
  string += String(value, HEX);
  return string;
}

String decByteString(byte value) {
  String string = "";
  if (value < 10) string += "0";
  string += String(value);
  return string;
}

String getMacString(byte* mac) {
  String string = "INVALID";
  if (mac) {
    string = hexByteString(mac[0]) + ":";
    string += hexByteString(mac[1]) + ":";
    string += hexByteString(mac[2]) + ":";
    string += hexByteString(mac[3]) + ":";
    string += hexByteString(mac[4]) + ":";
    string += hexByteString(mac[5]);
  }
  return string;
}

String getIPString(byte* ip) {
  String string = String(ip[0]) + ".";
  string += String(ip[1]) + ".";
  string += String(ip[2]) + ".";
  string += String(ip[3]);
  return string;
}

String timeString(unsigned long seconds) {
  return (String(seconds / 3600) + ":" + decByteString((seconds / 60) % 60) + ":" + decByteString(seconds % 60));
}

const char* printBuffer(byte* buffer, unsigned long length) {
  static byte* pBuffer;
  static unsigned long bufferIndex;
  static unsigned long bufferLength;
  static String string;
  string = "";
  unsigned long loop;
  if (buffer != nullptr) {
    pBuffer = buffer;
    bufferIndex = 0;
    bufferLength = length;
  }
  if (bufferIndex >= bufferLength) return nullptr;
  string += hexByteString(bufferIndex >> 24 & 0xFF);
  string += hexByteString(bufferIndex >> 16 & 0xFF);
  string += hexByteString(bufferIndex >> 8 & 0xFF);
  string += hexByteString(bufferIndex & 0xFF);
  string += ": ";
  unsigned long remaining = bufferLength - bufferIndex;
  loop = (remaining < 16) ? remaining : 16;
  loop += bufferIndex;
  for (; bufferIndex < loop; bufferIndex++) { string += hexByteString(pBuffer[bufferIndex]) + " "; }
  return string.c_str();
}
