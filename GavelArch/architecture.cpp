#include "architecture.h"

#if defined ARDUINO_RASPBERRY_PI_PICO
const HW_TYPES ProgramInfo::hw_type = HW_RASPBERRYPI_PICO;
#elif defined ARDUINO_RASPBERRY_PI_PICO_W
const HW_TYPES ProgramInfo::hw_type = HW_RASPBERRYPI_PICOW;
#elif defined ARDUINO_WAVESHARE_RP2040_ZERO
const HW_TYPES ProgramInfo::hw_type = HW_RP2040_ZERO;
#else
#error "This architecture does not support this Hardware!"
#endif

const char* ProgramInfo::compileDate = __DATE__;
const char* ProgramInfo::compileTime = __TIME__;

Mutex* Communication::mutex = nullptr;

Mutex* Communication::get() {
  if (mutex == nullptr) {
    mutex = new Mutex();
    WIRE->setSDA(ProgramInfo::hardwarewire.pinSDA);
    WIRE->setSCL(ProgramInfo::hardwarewire.pinSCL);
    WIRE->begin();
  }
  return mutex;
}
