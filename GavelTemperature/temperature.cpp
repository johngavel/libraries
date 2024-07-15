#include "temperature.h"

#include "gpio.h"
#include "license.h"
#include "serialport.h"

Temperature* Temperature::temperature = nullptr;

Temperature* Temperature::get() {
  if (temperature == nullptr) temperature = new Temperature();
  return temperature;
}

bool Temperature::validTemperature() {
  return validTemp;
}
void Temperature::configure(int __pin, int __drift) {
  pin = __pin;
  drift = __drift;
  configured = true;
}

void Temperature::setupTask() {
  DHT_LICENSE;
  ADAFRUIT_UNIFIED_LICENSE;
  if (configured) {
    GPIO->configurePinReserve(GPIO_INTERNAL, pin, "Temperature Sensor");
    TERM_CMD->addCmd("temp", "", "Temperature Status", Temperature::temperatureStatus);
    dht = new DHT(pin, DHT11);
    setRefreshMilli(refreshRateInValid);
    dht->begin();
    readTemperature();
    runTimer(true);
  } else {
    CONSOLE->println(ERROR, "Temperature Sensor Unconfigured");
    runTimer(false);
  }
  CONSOLE->println((validTemperature()) ? PASSED : FAILED, "Temperature Sensor Complete");
}

void Temperature::executeTask() {
  readTemperature();
  if (validTemp) {
    setRefreshMilli(refreshRateValid);
  } else {
    setRefreshMilli(refreshRateInValid);
  }
}

int Temperature::readTemperature() {
  if (dht != nullptr) {
    int value;
    value = ((int) dht->readTemperature(true) + drift);
    if ((value > 0) && (value < 150)) {
      validTemp = true;
      temp = value;
    }
  }
  return temp;
}

const int Temperature::getTemperature() {
  return temp;
}

void Temperature::temperatureStatus(Terminal* terminal) {
  terminal->println();
  terminal->print(INFO, "Temperature Sensor is ");
  (TEMPERATURE->isConfigured()) ? terminal->println(INFO, "Configured ") : terminal->println(WARNING, "Unconfigured ");
  terminal->print(INFO, "Temperature Data is ");
  (TEMPERATURE->validTemperature()) ? terminal->println(INFO, "Valid ") : terminal->println(WARNING, "Invalid ");
  terminal->print(INFO, "Temperature: ");
  terminal->print(INFO, String(TEMPERATURE->getTemperature()));
  terminal->println(INFO, "°F.");
  terminal->prompt();
}
