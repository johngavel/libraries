/* Auto-generated library definitions */
#ifndef LIBRARIES_H
#define LIBRARIES_H

typedef struct {
  const char* name;
  const char* version;
  const char* license_file;
} LibraryInfo;

static const LibraryInfo libraries[] = {
    {"Adafruit BusIO", "1.17.4", "Adafruit_BusIO_LICENSE"},
    {"Adafruit GFX Library", "1.12.2", "Adafruit_GFX_Library_license.txt"},
    {"Adafruit MCP4725", "2.0.2", "Adafruit_MCP4725_license.txt"},
    {"Adafruit NeoPixel", "1.15.1", "Adafruit_NeoPixel_COPYING"},
    {"Adafruit SSD1306", "2.5.15", "Adafruit_SSD1306_license.txt"},
    {"Adafruit Unified Sensor", "1.1.15", "Adafruit_Unified_Sensor_LICENSE.txt"},
    {"DHT sensor library", "1.4.6", "DHT_sensor_library_license.txt"},
    {"Ethernet", "2.0.2", "Ethernet_README.adoc"},
    {"I2C_EEPROM", "1.9.4", "I2C_EEPROM_LICENSE"},
    {"TCA9555", "0.4.3", "TCA9555_LICENSE"},
    {"Terminal", "1.0.8", "Terminal_license.txt"},
};

#define LIBRARY_COUNT (sizeof(libraries) / sizeof(libraries[0]))

/* Individual library macros */
#define ADAFRUIT_BUSIO_LICENSE LICENSE_ADD(libraries[0])
#define ADAFRUIT_GFX_LIBRARY_LICENSE LICENSE_ADD(libraries[1])
#define ADAFRUIT_MCP4725_LICENSE LICENSE_ADD(libraries[2])
#define ADAFRUIT_NEOPIXEL_LICENSE LICENSE_ADD(libraries[3])
#define ADAFRUIT_SSD1306_LICENSE LICENSE_ADD(libraries[4])
#define ADAFRUIT_UNIFIED_SENSOR_LICENSE LICENSE_ADD(libraries[5])
#define DHT_SENSOR_LIBRARY_LICENSE LICENSE_ADD(libraries[6])
#define ETHERNET_LICENSE LICENSE_ADD(libraries[7])
#define I2C_EEPROM_LICENSE LICENSE_ADD(libraries[8])
#define TCA9555_LICENSE LICENSE_ADD(libraries[9])
#define TERMINAL_LICENSE LICENSE_ADD(libraries[10])
#define RASPBERRY_PICO_LICENSE LICENSE_ADD("Arduino-Pico", "5.1.0", "Pico_LICENSE")

#endif /* LIBRARIES_H */
