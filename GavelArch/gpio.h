#ifndef __GAVEL_GPIO
#define __GAVEL_GPIO

#include "architecture.h"
#include "expander.h"
#include "gpiodesc.h"
#include "lock.h"
#include "timer.h"

// LED_BUILTIN is defined for the PICO and PICOW, it is not defined for the ZERO
// This pin is directly connected to the LED on the board for BLINK purposes.
#ifndef LED_BUILTIN
#define LED_BUILTIN 16
#endif

#define GPIO GPIOManager::get()
#define MAX_PINS 64

class GPIOManager : public Task {
public:
  static GPIOManager* get();
  void configureExpander(unsigned long index, int address);
  void configurePinReserve(GPIO_LOCATION location, int pinNumber, const char* description, bool isShared = false);
  void configurePinUndefined(int pinNumber);
  void configurePinIO(GPIO_TYPE type, GPIO_LOCATION location, int pinNumber, int index, const char* description);
  void configurePinLED(GPIO_LOCATION location, int pinNumber, GPIO_LED_TYPE type, int index, const char* description);

  GPIO_DESCRIPTION* getPin(GPIO_TYPE __type, int __index);
  void setupTask();
  void executeTask();
  GPIO_DESCRIPTION table[MAX_PINS];
  EXPANDER expander[2];

private:
  GPIOManager();
  void configureHW();
  void addPinConfiguration(int index, int pinNumber, GPIO_TYPE type, GPIO_LOCATION location, GPIO_LED_TYPE led_type, const char* description);
  static GPIOManager* gpioManager;
  bool validConfiguration(GPIO_LOCATION location, int pinNumber);
  bool validConfiguration(GPIO_LOCATION location, int pinNumber, int index, GPIO_TYPE type);
  int currentConfiguredPins;
  bool invalidOverallConfiguration;
  bool gpioTypeConfigured[GPIO_MAX_TYPES];

  static void printTable();
  static void toneCmd();
  static void pwmCmd();
  static void pulseCmd();
  static void statusCmd();
};

#endif
