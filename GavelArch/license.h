#ifndef __GAVEL_LICENSE
#define __GAVEL_LICENSE

#include "datastructure.h"
#include "termcmd.h"
#include "terminal.h"

#include <Arduino.h>

#define LICENSE LicenseManager::get()
#define LICENSE_ADD LicenseManager::addLicense
#define LICENSE_AVAILABLE LicenseManager::initialized()

#define ARDUINO_IDE_LICENSE LICENSE_ADD("Arduino IDE", "Ver. 2.3.2", "")
#define RASPBERRY_PICO_LICENSE LICENSE_ADD("Arduino-Pico", "Ver. 3.9.3", "Pico_LICENSE")
#define ADAFRUIT_BUS_LICENSE LICENSE_ADD("Adafruit BusIO", "Ver. 1.16.1", "BusIO_LICENSE")
#define ADAFRUIT_GFX_LICENSE LICENSE_ADD("Adafruit GFX Library", "Ver. 1.11.9", "GFX_license.txt")
#define ADAFRUIT_MCP4725_LICENSE LICENSE_ADD("Adafruit MCP4725", "Ver. 2.0.2", "")
#define ADAFRUIT_PIXEL_LICENSE LICENSE_ADD("Adafruit NeoPixel", "Ver. 1.12.3", "NeoPixel_COPYING")
#define ADAFRUIT_SSD1306_LICENSE LICENSE_ADD("Adafruit SSD1306", "Ver. 2.5.10", "SSD1306_license.txt")
#define ADAFRUIT_UNIFIED_LICENSE LICENSE_ADD("Adafruit_Unified_Sensor", "", "Sensor_LICENSE.txt")
#define DHT_LICENSE LICENSE_ADD("DHT sensor library", "Ver. 1.4.6", "DHT_license.txt")
#define ETHERNET_LICENSE LICENSE_ADD("Ethernet", "Ver. 2.0.2", "Ethernet_README.adoc")
#define I2C_EEPROM_LICENSE LICENSE_ADD("I2C_EEPROM", "Ver. 1.8.5", "I2C_EEPROM_LICENSE")
#define TCA9555_LICENSE LICENSE_ADD("TCA9555", "Ver. 0.3.2", "TCA9555_LICENSE")

struct LicenseFile {
  char libraryName[40];
  char version[20];
  char link[80];
};

class LicenseManager {
public:
  static LicenseManager* get();
  static bool initialized() { return licenseManager != nullptr; };
  static void addLicense(String libraryName, String version, String link);

  void setup();
  unsigned long count() { return licenseList->count(); };
  bool getFile(unsigned long index, LicenseFile* element) { return licenseList->get(index, element); };

private:
  static LicenseManager* licenseManager;
  static void printTable(Terminal* terminal);
  static void printLicense(Terminal* terminal);
  LicenseManager();
  void addLicenseToDatabase(String libraryName, String version, String link);
  ClassicQueue* licenseList;
};

#endif
