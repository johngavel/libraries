#ifndef __GAVEL_GPIO_DESC
#define __GAVEL_GPIO_DESC

#include "lock.h"
#include "timer.h"

#define MAX_ANALOG_VALUE 4096
#define NOT_ACCESSIBLE -1

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
  bool buttonStatus;
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

#endif
