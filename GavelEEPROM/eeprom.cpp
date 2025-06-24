#include "eeprom.h"

#include "gpio.h"
#include "license.h"
#include "serialport.h"

// #define FULL_DATA_SIZE I2C_DEVICESIZE_24LC16 / 8
// #define FULL_DATA_SIZE I2C_DEVICESIZE_24LC256 / 8

EEpromMemory* EEpromMemory::eeprom = nullptr;

EEpromMemory* EEpromMemory::get() {
  if (eeprom == nullptr) eeprom = new EEpromMemory();
  return eeprom;
}

void EEpromMemory::breakSeal() {
  seal = true;
}

void EEpromMemory::configure(unsigned long size) {
  memorySize = size;
}

void EEpromMemory::setupTask() {
  unsigned int fullDataSize = memorySize / 8;
  I2C_EEPROM_LICENSE;
  GPIO->configurePinReserve(GPIO_INTERNAL, ProgramInfo::hardwarewire.pinSDA, "I2C SDA", true);
  GPIO->configurePinReserve(GPIO_INTERNAL, ProgramInfo::hardwarewire.pinSCL, "I2C SCL", true);

  TERM_CMD->addCmd("wipe", "", "Wipe and Initialize EEPROM Memory", EEpromMemory::wipe);
  TERM_CMD->addCmd("mem", "", "Contents of Flash Memory", EEpromMemory::mem);
  setRefreshMilli(1000);
  dataSize = sizeof(PrivateAppInfo) + ((getData()) ? getData()->getLength() : 0);
  if (memorySize == 0) { CONSOLE->println(ERROR, "EEPROM Memory Chip Unconfigured. "); }
  if (dataSize > fullDataSize) {
    CONSOLE->println(ERROR, "EEPROM Data Structure is too large: " + String(dataSize) + "/" + String(fullDataSize));
    dataSize = fullDataSize;
  }
  COMM_TAKE;
  //  i2c_eeprom = new I2C_eeprom(0x50, I2C_DEVICESIZE_24LC16, WIRE);
  i2c_eeprom = new I2C_eeprom(0x50, memorySize, WIRE);
  i2c_eeprom->begin();
  runTimer(i2c_eeprom->isConnected());
  COMM_GIVE;
  readEEPROM();
  if ((appInfo.ProgramNumber != ProgramInfo::ProgramNumber) || (appInfo.MajorVersion != ProgramInfo::MajorVersion)) {
    initMemory();
    CONSOLE->println(ERROR, "EEPROM memory incorrect values, intializing default values");
  } else {
    CONSOLE->println(PASSED, "EEPROM memory success");
  }
  if ((appInfo.MinorVersion != ProgramInfo::MinorVersion)) {
    appInfo.MajorVersion = ProgramInfo::MajorVersion;
    appInfo.MinorVersion = ProgramInfo::MinorVersion;
    breakSeal();
  }
  if (!getTimerRun()) CONSOLE->println(ERROR, "EEPROM Not Connected");
  String memoryString =
      "Memory Complete: PRG Num: " + String(appInfo.ProgramNumber) + " PRG Ver: " + String(appInfo.MajorVersion) + "." + String(appInfo.MinorVersion);
  if (getData())
    getData()->setup();
  else
    CONSOLE->println(WARNING, "No User Data Available!");
  CONSOLE->println((getTimerRun()) ? PASSED : FAILED, memoryString);
}

void EEpromMemory::executeTask() {
  if (seal) {
    forceWrite();
  } else {
    setRefreshMilli(1000);
  }
}

void EEpromMemory::forceWrite() {
  setRefreshMilli(5000);
  seal = false;
  reset();
  writeEEPROM();
}

void EEpromMemory::initMemory() {
  appInfo.ProgramNumber = ProgramInfo::ProgramNumber;
  appInfo.MajorVersion = ProgramInfo::MajorVersion;
  appInfo.MinorVersion = ProgramInfo::MinorVersion;
  if (getData()) data->initMemory();
  breakSeal();
}

unsigned long EEpromMemory::getLength() {
  return dataSize;
}

byte EEpromMemory::readEEPROMbyte(unsigned long address) {
  byte result = 0xFF;
  result = i2c_eeprom->readByte(address);
  return result;
}

void EEpromMemory::writeEEPROMbyte(unsigned long address, byte value) {
  if (i2c_eeprom->isConnected() && !i2c_eeprom->updateByteVerify(address, value))
    CONSOLE->println(ERROR, "Error in Writing EEPROM " + String(address) + " = " + String(value, HEX));
}

void EEpromMemory::readEEPROM() {
  unsigned long index = 0;
  unsigned char* appInfoData = (unsigned char*) &appInfo;
  if (getTimerRun()) {
    COMM_TAKE;
    EEPROM_TAKE;
    for (index = 0; index < sizeof(PrivateAppInfo); index++) { appInfoData[index] = readEEPROMbyte(index); }
    if (getData()) {
      for (index = 0; index < data->getLength(); index++) { data->getData()[index] = readEEPROMbyte(index + sizeof(PrivateAppInfo)); }
    }
    EEPROM_GIVE;
    COMM_GIVE;
  }
}

void EEpromMemory::writeEEPROM() {
  unsigned long index = 0;
  unsigned char* appInfoData = (unsigned char*) &appInfo;
  if (getTimerRun()) {
    COMM_TAKE;
    EEPROM_TAKE;
    for (index = 0; index < sizeof(PrivateAppInfo); index++) { writeEEPROMbyte(index, appInfoData[index]); }
    if (getData()) {
      for (index = 0; index < data->getLength(); index++) { writeEEPROMbyte(index + sizeof(PrivateAppInfo), data->getData()[index]); }
    }
    EEPROM_GIVE;
    COMM_GIVE;
  }
}

void EEpromMemory::wipe(Terminal* terminal) {
  EEPROM->initMemory();
  if (EEPROM->getData()) EEPROM->initMemory();
  terminal->println((EEPROM->getTimerRun()) ? PASSED : FAILED, "EEPROM Initialize Memory Complete.");
  terminal->prompt();
}

void EEpromMemory::mem(Terminal* terminal) {
  terminal->println(PROMPT, String(ProgramInfo::AppName) + " Version: " + String(ProgramInfo::MajorVersion) + String(".") + String(ProgramInfo::MinorVersion) +
                                String(".") + String(ProgramInfo::BuildVersion));
  terminal->println(INFO, "Program: " + String(ProgramInfo::ProgramNumber));
  terminal->println(INFO, "EEPROM Size: " + String(EEPROM->getLength()) + "/" + String(EEPROM->getMemorySize() / 8) + " bytes.");
  if (EEPROM->getData())
    EEPROM->getData()->printData(terminal);
  else
    terminal->println(WARNING, "No User Data Available!");
  terminal->println(INFO, "EEPROM Read Memory Complete");
  terminal->prompt();
}
