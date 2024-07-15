#ifndef __GAVEL_EEPROM
#define __GAVEL_EEPROM

#include "architecture.h"
#include "terminal.h"

#include <I2C_eeprom.h>

#define MEMORY_CONFIG_FILE (String(ProgramInfo::ShortName) + "_config.txt")

class Data {
public:
  virtual void setup() = 0;
  virtual void initMemory() = 0;
  virtual void printData(Terminal* terminal) = 0;
  virtual unsigned char* getData() = 0;
  virtual unsigned long getLength() = 0;
  virtual void exportMem() = 0;
  virtual void importMem() = 0;
};

#define EEPROM EEpromMemory::get()
#define EEPROM_AVAILABLE EEpromMemory::initialized()
#define EEPROM_FORCE                                                                                                                                           \
  {                                                                                                                                                            \
    if (EEPROM_AVAILABLE) EEPROM->forceWrite();                                                                                                                \
  }
#define EEPROM_TAKE EEPROM->mutex.take()
#define EEPROM_GIVE EEPROM->mutex.give()

class EEpromMemory : public Task {
public:
  static EEpromMemory* get();
  static bool initialized() { return eeprom != nullptr; };
  void configure(unsigned long size);
  void setupTask();
  void executeTask();
  void forceWrite();
  void breakSeal();
  bool getSeal() { return seal; };
  void initMemory();
  void setData(Data* __data) { data = __data; };
  Data* getData() { return data; };
  unsigned long getLength();
  unsigned long getMemorySize() { return memorySize; };
  Mutex mutex;

private:
  EEpromMemory() : Task("EEPROM"){};
  static EEpromMemory* eeprom;
  void readEEPROM();
  void writeEEPROM();
  bool seal;
  byte readEEPROMbyte(unsigned long address);
  void writeEEPROMbyte(unsigned long address, byte value);
  I2C_eeprom* i2c_eeprom;
  Data* data = nullptr;
  unsigned int memorySize = 0;
  unsigned int dataSize = 0;

  struct PrivateAppInfo {
    unsigned char ProgramNumber;
    unsigned char MajorVersion;
    unsigned char MinorVersion;
  };
  PrivateAppInfo appInfo;

  static void wipe(Terminal* terminal);
  static void mem(Terminal* terminal);
};

#endif
