/* Auto-generated library definitions */
#ifndef LIBRARIES_H
#define LIBRARIES_H

#include "Adafruit_BusIO_LICENSE.h"
#include "Adafruit_GFX_Library_license.h"
#include "Adafruit_MCP4725_license.h"
#include "Adafruit_NeoPixel_COPYING.h"
#include "Adafruit_SSD1306_license.h"
#include "Adafruit_Unified_Sensor_LICENSE.h"
#include "DHT_sensor_library_license.h"
#include "Ethernet_README.h"
#include "I2C_EEPROM_LICENSE.h"
#include "TCA9555_LICENSE.h"
#include "Terminal_license.h"

typedef struct {
  const char* name;
  const char* version;
  const char* license_file;
  const unsigned char* license_buffer;
  const unsigned int license_size;
  const char* license_name;
} LibraryInfo;

static const LibraryInfo libraries[] = {
    {"Adafruit BusIO", "1.17.4", "Adafruit_BusIO_LICENSE", AdafruitBusIOLICENSE, AdafruitBusIOLICENSE_len, AdafruitBusIOLICENSE_string},
    {"Adafruit GFX Library", "1.12.3", "Adafruit_GFX_Library_license.txt", AdafruitGFXLibrarylicense, AdafruitGFXLibrarylicense_len, AdafruitGFXLibrarylicense_string},
    {"Adafruit MCP4725", "2.0.2", "Adafruit_MCP4725_license.txt", AdafruitMCPlicense, AdafruitMCPlicense_len, AdafruitMCPlicense_string},
    {"Adafruit NeoPixel", "1.15.2", "Adafruit_NeoPixel_COPYING", AdafruitNeoPixelCOPYING, AdafruitNeoPixelCOPYING_len, AdafruitNeoPixelCOPYING_string},
    {"Adafruit SSD1306", "2.5.15", "Adafruit_SSD1306_license.txt", AdafruitSSDlicense, AdafruitSSDlicense_len, AdafruitSSDlicense_string},
    {"Adafruit Unified Sensor", "1.1.15", "Adafruit_Unified_Sensor_LICENSE.txt", AdafruitUnifiedSensorLICENSE, AdafruitUnifiedSensorLICENSE_len, AdafruitUnifiedSensorLICENSE_string},
    {"DHT sensor library", "1.4.6", "DHT_sensor_library_license.txt", DHTsensorlibrarylicense, DHTsensorlibrarylicense_len, DHTsensorlibrarylicense_string},
    {"Ethernet", "2.0.2", "Ethernet_README.adoc", EthernetREADME, EthernetREADME_len, EthernetREADME_string},
    {"I2C_EEPROM", "1.9.4", "I2C_EEPROM_LICENSE", ICEEPROMLICENSE, ICEEPROMLICENSE_len, ICEEPROMLICENSE_string},
    {"TCA9555", "0.4.3", "TCA9555_LICENSE", TCALICENSE, TCALICENSE_len, TCALICENSE_string},
    {"Terminal", "1.0.12", "Terminal_license.txt", Terminallicense, Terminallicense_len, Terminallicense_string},
};

#define LIBRARY_COUNT (sizeof(libraries) / sizeof(libraries[0]))

/* Individual library macros */
#define ADAFRUIT_BUSIO_LICENSE LICENSE_ADD(libraries[0], 0)
#define ADAFRUIT_GFX_LIBRARY_LICENSE LICENSE_ADD(libraries[1], 1)
#define ADAFRUIT_MCP4725_LICENSE LICENSE_ADD(libraries[2], 2)
#define ADAFRUIT_NEOPIXEL_LICENSE LICENSE_ADD(libraries[3], 3)
#define ADAFRUIT_SSD1306_LICENSE LICENSE_ADD(libraries[4], 4)
#define ADAFRUIT_UNIFIED_SENSOR_LICENSE LICENSE_ADD(libraries[5], 5)
#define DHT_SENSOR_LIBRARY_LICENSE LICENSE_ADD(libraries[6], 6)
#define ETHERNET_LICENSE LICENSE_ADD(libraries[7], 7)
#define I2C_EEPROM_LICENSE LICENSE_ADD(libraries[8], 8)
#define TCA9555_LICENSE LICENSE_ADD(libraries[9], 9)
#define TERMINAL_LICENSE LICENSE_ADD(libraries[10], 10)
#define RASPBERRY_PICO_LICENSE LICENSE_ADD("Arduino-Pico", "5.4.0", "Pico_LICENSE", -1)

#endif /* LIBRARIES_H */
