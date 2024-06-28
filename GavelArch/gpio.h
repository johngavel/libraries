#ifndef __GAVEL_GPIO
#define __GAVEL_GPIO

#include "architecture.h"
#include "expander.h"
#include "lock.h"
#include "timer.h"

// LED_BUILTIN is defined for the PICO and PICOW, it is not defined for the ZERO
// This pin is directly connected to the LED on the board for BLINK purposes.
#ifndef LED_BUILTIN
#define LED_BUILTIN 16
#endif

#define GPIO GPIOManager::get()
#define MAX_ANALOG_VALUE 4096

typedef enum {
  GPIO_INPUT,
  GPIO_OUTPUT,
  GPIO_LED,
  GPIO_BUTTON,
  GPIO_PULSE,
  GPIO_PWM,
  GPIO_TONE,
  GPIO_ADC,
  GPIO_RESERVED,
  GPIO_UNACCESSIBLE,
  GPIO_UNDEFINED,
  GPIO_MAX_TYPES
} GPIO_TYPE;

typedef enum { GPIO_INTERNAL, GPIO_EXTERNAL_EXPANDER_1, GPIO_EXTERNAL_EXPANDER_2, GPIO_UNKNOWN_LOCATION } GPIO_LOCATION;

typedef enum { GPIO_SINK, GPIO_SOURCE, GPIO_NOT_APPLICABLE } GPIO_LED_TYPE;

#define DESCRIPTION_LENGTH 100

class GPIO_DESCRIPTION {
public:
  GPIO_DESCRIPTION();
  int index;
  int pinNumber;
  GPIO_TYPE type;
  GPIO_LOCATION location;
  GPIO_LED_TYPE led_type;
  char description[DESCRIPTION_LENGTH];
  bool getCurrentStatus();
  unsigned long getCurrentValue();
  unsigned long getCurrentFreq();
  void setCurrentStatus(bool status);
  void setCurrentValue(unsigned long value);
  void setCurrentFreq(unsigned long value);
  void setValidExecute(bool __validExecute) { validExecute = __validExecute; };
  bool getValidExecute() { return validExecute; };
  bool setup();
  unsigned long execute();

private:
  bool currentStatus;
  unsigned long currentValue;
  unsigned long currentFreq;
  bool valueChanged;
  void readDigital();
  void writeDigital(bool status);
  void readAnalog();
  Timer timer;
  Mutex mutex;
  bool validExecute;
};

#define MAX_PINS 64
#define NOT_ACCESSIBLE -1

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
