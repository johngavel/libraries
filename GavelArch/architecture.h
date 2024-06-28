#ifndef __GAVEL_ARCHITECTURE
#define __GAVEL_ARCHITECTURE

#include "taskmanager.h"

#include <Wire.h>

#define CPU_CORES 2

typedef enum { HW_UNKNOWN, HW_RP2040_ZERO, HW_RASPBERRYPI_PICO, HW_RASPBERRYPI_PICOW } HW_TYPES;

#define HARDWARE_PORT ((SerialUART*) ProgramInfo::hardwareserial.serial)
class HardwareSerialPort {
public:
  HardwareSerialPort(SerialUART* __serial, unsigned long __pintx, unsigned long __pinrx) : serial(__serial), pintx(__pintx), pinrx(__pinrx){};
  SerialUART* serial;
  unsigned long pintx;
  unsigned long pinrx;
};

#define WIRE ((TwoWire*) ProgramInfo::hardwarewire.wire)
class HardwareWire {
public:
  HardwareWire(TwoWire* __wire, unsigned long __pinSDA, unsigned long __pinSCL) : wire(__wire), pinSDA(__pinSDA), pinSCL(__pinSCL){};
  TwoWire* wire;
  unsigned long pinSDA;
  unsigned long pinSCL;
};

class ProgramInfo {
public:
  static const char* AppName;
  static const char* ShortName;
  static const char* compileDate;
  static const char* compileTime;
  static const HW_TYPES hw_type;
  static const unsigned char ProgramNumber;
  static const unsigned char MajorVersion;
  static const unsigned char MinorVersion;
  static const char* AuthorName;
  static const HardwareWire hardwarewire;
  static const HardwareSerialPort hardwareserial;
};

#define COMM Communication::get()
#define COMM_TAKE COMM->take()
#define COMM_GIVE COMM->give()

class Communication : public Mutex {
public:
  static Mutex* get();

private:
  Communication(){};
  static Mutex* mutex;
};

#endif