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
    PORT->addCmd("temp", "", "Temperature Status", Temperature::temperatureStatus);
    dht = new DHT(pin, DHT11);
    setRefreshMilli(refreshRateInValid);
    dht->begin();
    readTemperature();
    runTimer(true);
  } else {
    PORT->println(ERROR, "Temperature Sensor Unconfigured");
    runTimer(false);
  }
  PORT->println((validTemperature()) ? PASSED : FAILED, "Temperature Sensor Complete");
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

void Temperature::temperatureStatus() {
  PORT->println();
  PORT->print(INFO, "Temperature Sensor is ");
  (TEMPERATURE->isConfigured()) ? PORT->println(INFO, "Configured ") : PORT->println(WARNING, "Unconfigured ");
  PORT->print(INFO, "Temperature Data is ");
  (TEMPERATURE->validTemperature()) ? PORT->println(INFO, "Valid ") : PORT->println(WARNING, "Invalid ");
  PORT->print(INFO, "Temperature: ");
  PORT->print(INFO, String(TEMPERATURE->getTemperature()));
  PORT->println(INFO, "°F.");
  PORT->prompt();
}
