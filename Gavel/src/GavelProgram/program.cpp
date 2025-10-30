#include "program.h"

#include "../GavelUtil/stringutils.h"

#include <Terminal.h>

#if defined ARDUINO_RASPBERRY_PI_PICO
const HW_TYPES ProgramInfo::hw_type = HW_RASPBERRYPI_PICO;
#elif defined ARDUINO_RASPBERRY_PI_PICO_W
const HW_TYPES ProgramInfo::hw_type = HW_RASPBERRYPI_PICOW;
#elif defined ARDUINO_WAVESHARE_RP2040_ZERO
const HW_TYPES ProgramInfo::hw_type = HW_RP2040_ZERO;
#elif defined ARDUINO_GENERIC_RP2040
const HW_TYPES ProgramInfo::hw_type = HW_GAVEL_MINI_PICO_ETH;
#else
#error "This architecture does not support this Hardware!"
#endif

#ifdef GAVEL_VER
#define GAVEL_VERSION GAVEL_VER
#else
#define GAVEL_VERSION 65535
#endif

const char* ProgramInfo::compileDate = __DATE__;
const char* ProgramInfo::compileTime = __TIME__;
const unsigned long ProgramInfo::BuildVersion = GAVEL_VERSION;

const char* stringHardware(HW_TYPES hw_type) {
  const char* hwString;
  switch (hw_type) {
  case HW_RP2040_ZERO: hwString = "Waveshare RP2040 Zero"; break;
  case HW_RASPBERRYPI_PICO: hwString = "Raspberry Pi Pico"; break;
  case HW_RASPBERRYPI_PICOW: hwString = "Raspberry Pi Pico W"; break;
  case HW_GAVEL_MINI_PICO_ETH: hwString = "Gavel Mini Pico - Ethernet"; break;
  default: hwString = "Unknown"; break;
  }
  return hwString;
}

void banner(OutputInterface* terminal) {
  if (!terminal) return;
  terminal->println();
  StringBuilder sb;
  sb + ProgramInfo::AppName + " Version: " + ProgramInfo::MajorVersion + "." + ProgramInfo::MinorVersion + "." + ProgramInfo::BuildVersion;

  terminal->println(INFO, sb.c_str());

  terminal->println(INFO, "Program: " + String(ProgramInfo::ProgramNumber));
  terminal->println(INFO, "Build Date: " + String(ProgramInfo::compileDate) + " Time: " + String(ProgramInfo::compileTime));
  terminal->println();
  terminal->print(INFO, "Microcontroller: ");
  terminal->println(INFO, stringHardware(ProgramInfo::hw_type));
  terminal->print(INFO, "Core is running at ");
  terminal->print(INFO, String(rp2040.f_cpu() / 1000000));
  terminal->println(INFO, " Mhz");
  int used = rp2040.getUsedHeap();
  int total = rp2040.getTotalHeap();
  int percentage = (used * 100) / total;
  terminal->print(INFO, "RAM Memory Usage: ");
  terminal->print(INFO, String(used));
  terminal->print(INFO, "/");
  terminal->print(INFO, String(total));
  terminal->print(INFO, " --> ");
  terminal->print(INFO, String(percentage));
  terminal->println(INFO, "%");
  terminal->print(INFO, "CPU Temperature: ");
  terminal->print(INFO, String((9.0 / 5.0 * analogReadTemp()) + 32.0, 0));
  terminal->println(INFO, "°F.");
}
