#ifndef __GAVEL_TEMPERATURE
#define __GAVEL_TEMPERATURE

#include "architecture.h"

#include <DHT.h>

#define TEMPERATURE Temperature::get()

class Temperature : public Task {
public:
  static Temperature* get();
  void configure(int __pin, int __drift);
  void setupTask();
  void executeTask();
  bool validTemperature();
  bool isConfigured() { return configured; };
  const int getTemperature();

private:
  Temperature() : Task("Temperature"), temp(0), validTemp(false), dht(nullptr), configured(false), pin(0), drift(0){};
  static Temperature* temperature;
  int temp;
  bool validTemp;
  DHT* dht;
  int readTemperature();
  const unsigned long refreshRateValid = 60000;
  const unsigned long refreshRateInValid = 4000;
  bool configured;
  unsigned long pin;
  unsigned long drift;

  static void temperatureStatus(Terminal* terminal);
};

#endif