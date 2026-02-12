/* Auto-generated library definitions */
#ifndef LIBRARIES_H
#define LIBRARIES_H

#if __has_include("library_used.h")
#include "library_used.h"
#else
// Removing all Licenses from the system if not defined
// #define ADAFRUIT_BUSIO_USED
// #define ADAFRUIT_GFX_LIBRARY_USED
// #define ADAFRUIT_MCP4725_USED
// #define ADAFRUIT_NEOPIXEL_USED
// #define ADAFRUIT_SSD1306_USED
// #define ADAFRUIT_UNIFIED_SENSOR_USED
// #define ARDUINOJSON_USED
// #define DHT_SENSOR_LIBRARY_USED
// #define ETHERNET_USED
// #define I2C_EEPROM_USED
// #define TCA9555_USED
// #define TERMINAL_USED
#endif

#ifdef ADAFRUIT_BUSIO_USED
#include "Adafruit_BusIO_LICENSE.h"
#endif
#ifdef ADAFRUIT_GFX_LIBRARY_USED
#include "Adafruit_GFX_Library_license.h"
#endif
#ifdef ADAFRUIT_MCP4725_USED
#include "Adafruit_MCP4725_license.h"
#endif
#ifdef ADAFRUIT_NEOPIXEL_USED
#include "Adafruit_NeoPixel_COPYING.h"
#endif
#ifdef ADAFRUIT_SSD1306_USED
#include "Adafruit_SSD1306_license.h"
#endif
#ifdef ADAFRUIT_UNIFIED_SENSOR_USED
#include "Adafruit_Unified_Sensor_LICENSE.h"
#endif
#ifdef ARDUINOJSON_USED
#include "ArduinoJson_LICENSE.h"
#endif
#ifdef DHT_SENSOR_LIBRARY_USED
#include "DHT_sensor_library_license.h"
#endif
#ifdef ETHERNET_USED
#include "Ethernet_README.h"
#endif
#ifdef I2C_EEPROM_USED
#include "I2C_EEPROM_LICENSE.h"
#endif
#ifdef TCA9555_USED
#include "TCA9555_LICENSE.h"
#endif
#ifdef TERMINAL_USED
#include "Terminal_license.h"
#endif

typedef struct {
  const char* name;
  const char* version;
  const char* license_file;
  const unsigned char* license_buffer;
  const unsigned int license_size;
  const char* license_name;
} LibraryInfo;

#define NULL_LIBRARY_ENTRY(s)                                                                                          \
  { s, "0.0", "null", nullptr, 0, }

static const LibraryInfo libraries[] = {
#ifdef ADAFRUITBUSIOLICENSE_H
    {"Adafruit BusIO", "1.17.4", "Adafruit_BusIO_LICENSE", AdafruitBusIOLICENSE, AdafruitBusIOLICENSE_len,
     AdafruitBusIOLICENSE_string},
#else
    NULL_LIBRARY_ENTRY("Adafruit BusIO"),
#endif
#ifdef ADAFRUITGFXLIBRARYLICENSE_H
    {"Adafruit GFX Library", "1.12.4", "Adafruit_GFX_Library_license.txt", AdafruitGFXLibrarylicense,
     AdafruitGFXLibrarylicense_len, AdafruitGFXLibrarylicense_string},
#else
    NULL_LIBRARY_ENTRY("Adafruit GFX Library"),
#endif
#ifdef ADAFRUITMCPLICENSE_H
    {"Adafruit MCP4725", "2.0.2", "Adafruit_MCP4725_license.txt", AdafruitMCPlicense, AdafruitMCPlicense_len,
     AdafruitMCPlicense_string},
#else
    NULL_LIBRARY_ENTRY("Adafruit MCP4725"),
#endif
#ifdef ADAFRUITNEOPIXELCOPYING_H
    {"Adafruit NeoPixel", "1.15.4", "Adafruit_NeoPixel_COPYING", AdafruitNeoPixelCOPYING, AdafruitNeoPixelCOPYING_len,
     AdafruitNeoPixelCOPYING_string},
#else
    NULL_LIBRARY_ENTRY("Adafruit NeoPixel"),
#endif
#ifdef ADAFRUITSSDLICENSE_H
    {"Adafruit SSD1306", "2.5.16", "Adafruit_SSD1306_license.txt", AdafruitSSDlicense, AdafruitSSDlicense_len,
     AdafruitSSDlicense_string},
#else
    NULL_LIBRARY_ENTRY("Adafruit SSD1306"),
#endif
#ifdef ADAFRUITUNIFIEDSENSORLICENSE_H
    {"Adafruit Unified Sensor", "1.1.15", "Adafruit_Unified_Sensor_LICENSE.txt", AdafruitUnifiedSensorLICENSE,
     AdafruitUnifiedSensorLICENSE_len, AdafruitUnifiedSensorLICENSE_string},
#else
    NULL_LIBRARY_ENTRY("Adafruit Unified Sensor"),
#endif
#ifdef ARDUINOJSONLICENSE_H
    {"ArduinoJson", "7.4.2", "ArduinoJson_LICENSE.txt", ArduinoJsonLICENSE, ArduinoJsonLICENSE_len,
     ArduinoJsonLICENSE_string},
#else
    NULL_LIBRARY_ENTRY("ArduinoJson"),
#endif
#ifdef DHTSENSORLIBRARYLICENSE_H
    {"DHT sensor library", "1.4.6", "DHT_sensor_library_license.txt", DHTsensorlibrarylicense,
     DHTsensorlibrarylicense_len, DHTsensorlibrarylicense_string},
#else
    NULL_LIBRARY_ENTRY("DHT sensor library"),
#endif
#ifdef ETHERNETREADME_H
    {"Ethernet", "2.0.2", "Ethernet_README.adoc", EthernetREADME, EthernetREADME_len, EthernetREADME_string},
#else
    NULL_LIBRARY_ENTRY("Ethernet"),
#endif
#ifdef ICEEPROMLICENSE_H
    {"I2C_EEPROM", "1.9.4", "I2C_EEPROM_LICENSE", ICEEPROMLICENSE, ICEEPROMLICENSE_len, ICEEPROMLICENSE_string},
#else
    NULL_LIBRARY_ENTRY("I2C_EEPROM"),
#endif
#ifdef TCALICENSE_H
    {"TCA9555", "0.4.4", "TCA9555_LICENSE", TCALICENSE, TCALICENSE_len, TCALICENSE_string},
#else
    NULL_LIBRARY_ENTRY("TCA9555"),
#endif
#ifdef TERMINALLICENSE_H
    {"Terminal", "1.0.12", "Terminal_license.txt", Terminallicense, Terminallicense_len, Terminallicense_string},
#else
    NULL_LIBRARY_ENTRY("Terminal"),
#endif
};

#define LIBRARY_COUNT (sizeof(libraries) / sizeof(libraries[0]))

/* Individual library macros */
#define ADAFRUIT_BUSIO_LICENSE LICENSE_ADD(libraries[0], 0)
#define ADAFRUIT_GFX_LIBRARY_LICENSE LICENSE_ADD(libraries[1], 1)
#define ADAFRUIT_MCP4725_LICENSE LICENSE_ADD(libraries[2], 2)
#define ADAFRUIT_NEOPIXEL_LICENSE LICENSE_ADD(libraries[3], 3)
#define ADAFRUIT_SSD1306_LICENSE LICENSE_ADD(libraries[4], 4)
#define ADAFRUIT_UNIFIED_SENSOR_LICENSE LICENSE_ADD(libraries[5], 5)
#define ARDUINOJSON_LICENSE LICENSE_ADD(libraries[6], 6)
#define DHT_SENSOR_LIBRARY_LICENSE LICENSE_ADD(libraries[7], 7)
#define ETHERNET_LICENSE LICENSE_ADD(libraries[8], 8)
#define I2C_EEPROM_LICENSE LICENSE_ADD(libraries[9], 9)
#define TCA9555_LICENSE LICENSE_ADD(libraries[10], 10)
#define TERMINAL_LICENSE LICENSE_ADD(libraries[11], 11)
#define RASPBERRY_PICO_LICENSE LICENSE_ADD("Arduino-Pico", "5.5.0", "Pico_LICENSE", -1)

#endif /* LIBRARIES_H */
