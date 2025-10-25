#include "gpiodesc.h"

#include "gpio.h"
#include "serialport.h"

#define DEBOUNCE_TIMER 100
#define PULSE_TIMER 100

const char* stringLocation(GPIO_LOCATION location);

GPIO_DESCRIPTION::GPIO_DESCRIPTION() {
  index = NOT_ACCESSIBLE;
  pinNumber = NOT_ACCESSIBLE;
  type = GPIO_UNACCESSIBLE;
  location = GPIO_UNKNOWN_LOCATION;
  led_type = GPIO_NOT_APPLICABLE;
  currentStatus = false;
  buttonStatus = false;
  currentValue = 0;
  currentFreq = 0;
  validExecute = true;
  timer.runTimer(false);
  valueChanged = true;
  const char* undefinedGPIOString = "GPIO has not been defined.";
  strncpy(description, undefinedGPIOString, DESCRIPTION_LENGTH - 1);
}

bool GPIO_DESCRIPTION::getCurrentStatus() {
  bool returnBool = currentStatus;
  if (type == GPIO_BUTTON) {
    returnBool = buttonStatus;
    buttonStatus = false;
  }
  return returnBool;
}

unsigned long GPIO_DESCRIPTION::getCurrentValue() {
  return currentValue;
}

unsigned long GPIO_DESCRIPTION::getCurrentFreq() {
  return currentFreq;
}

void GPIO_DESCRIPTION::setCurrentStatus(bool status) {
  if (currentStatus != status) {
    currentStatus = status;
    valueChanged = true;
  }
}

void GPIO_DESCRIPTION::setCurrentValue(unsigned long value) {
  currentValue = value;
  valueChanged = true;
}

void GPIO_DESCRIPTION::setCurrentFreq(unsigned long value) {
  currentFreq = value;
  valueChanged = true;
}

bool GPIO_DESCRIPTION::setup() {
  bool validConfiguration = true;
  if (location == GPIO_INTERNAL) {
    switch (type) {
    case GPIO_INPUT:
    case GPIO_BUTTON:
      pinMode(pinNumber, INPUT);
      setCurrentStatus(false);
      break;
    case GPIO_ADC:
      analogReadResolution(12);
      pinMode(pinNumber, INPUT);
      break;
    case GPIO_OUTPUT:
    case GPIO_LED:
    case GPIO_PULSE:
    case GPIO_TONE:
      pinMode(pinNumber, OUTPUT);
      setCurrentStatus(false);
      execute();
      break;
    case GPIO_PWM:
      pinMode(pinNumber, OUTPUT);
      analogWriteFreq(100);
      analogWriteRange(100);
      execute();
      break;
    case GPIO_RESERVED:
    case GPIO_UNACCESSIBLE:
    case GPIO_UNDEFINED: break;
    default: validConfiguration = false; break;
    }
#ifdef TCA9555_USED
  } else if ((GPIO->expander[0].valid) && (location == GPIO_EXTERNAL_EXPANDER_1)) {
    switch (type) {
    case GPIO_INPUT:
    case GPIO_BUTTON:
      COMM_TAKE;
      GPIO->expander[0].TCA1->pinMode1(pinNumber, INPUT);
      COMM_GIVE;
      break;
    case GPIO_OUTPUT:
    case GPIO_LED:
    case GPIO_PULSE:
      COMM_TAKE;
      GPIO->expander[0].TCA1->pinMode1(pinNumber, OUTPUT);
      COMM_GIVE;
      setCurrentStatus(false);
      break;
    case GPIO_RESERVED: break;
    default: validConfiguration = false; break;
    }
  } else if ((GPIO->expander[1].valid) && (location == GPIO_EXTERNAL_EXPANDER_2)) {
    switch (type) {
    case GPIO_INPUT:
    case GPIO_BUTTON:
      COMM_TAKE;
      GPIO->expander[1].TCA1->pinMode1(pinNumber, INPUT);
      COMM_GIVE;
      break;
    case GPIO_OUTPUT:
    case GPIO_LED:
    case GPIO_PULSE:
      COMM_TAKE;
      GPIO->expander[1].TCA1->pinMode1(pinNumber, OUTPUT);
      COMM_GIVE;
      setCurrentStatus(false);
      break;
    case GPIO_RESERVED: break;
    default: validConfiguration = false; break;
    }
#endif
  }
  if (!validConfiguration) {
    CONSOLE->println(ERROR, "Invalid Setup and Configuration of I/O " + String(stringLocation(location)) + " Pin " + String(pinNumber));
  }
  return validConfiguration;
}

unsigned long GPIO_DESCRIPTION::execute() {
  unsigned long returnValue = 0;
  mutex.take();
  switch (type) {
  case GPIO_INPUT:
    readDigital();
    returnValue = currentStatus;
    break;
  case GPIO_BUTTON:
    readDigital();
    if ((currentStatus == false) && (buttonStatus == false) && (!timer.getTimerRun())) {
      timer.setRefreshMilli(DEBOUNCE_TIMER);
      timer.runTimer(true);
    } else if ((currentStatus == true) && (buttonStatus == false) && (timer.getTimerRun()) && (timer.expired())) {
      buttonStatus = true;
      timer.runTimer(false);
    } else if (currentStatus == true) {
      timer.runTimer(false);
    }
    returnValue = currentStatus;
    break;
  case GPIO_ADC:
    readAnalog();
    returnValue = currentValue;
    break;
  case GPIO_OUTPUT:
    if (valueChanged == true) { writeDigital(currentStatus); }
    returnValue = currentStatus;
    break;
  case GPIO_LED:
    if (led_type == GPIO_SOURCE)
      writeDigital(currentStatus);
    else
      writeDigital(!currentStatus);
    returnValue = currentStatus;
    break;
  case GPIO_PULSE:
    if ((currentStatus == true) && (valueChanged == true)) {
      timer.setRefreshMilli(PULSE_TIMER);
      timer.runTimer(true);
      writeDigital(true);
    } else if ((currentStatus == true) && (timer.expired())) {
      timer.runTimer(false);
      setCurrentStatus(false);
      writeDigital(false);
    } else if ((currentStatus == false) && (valueChanged == true)) {
      valueChanged = false;
      writeDigital(false);
    }
    returnValue = currentStatus;
    break;
  case GPIO_PWM:
    if (valueChanged == true) {
      analogWriteFreq(currentFreq);
      analogWriteRange(100);
      analogWrite(pinNumber, currentValue);
      valueChanged = false;
    }
    returnValue = currentValue;
    break;
  case GPIO_TONE:
    if (valueChanged == true) {
      tone(pinNumber, currentFreq);
      valueChanged = false;
    }
    returnValue = currentFreq;
    break;
  default: break;
  }
  mutex.give();
  return returnValue;
}

void GPIO_DESCRIPTION::readDigital() {
  if (location == GPIO_INTERNAL) {
    currentStatus = digitalRead(pinNumber);
#ifdef TCA9555_USED
  } else if (location == GPIO_EXTERNAL_EXPANDER_1) {
    COMM_TAKE;
    currentStatus = GPIO->expander[0].TCA1->read1(pinNumber);
    COMM_GIVE;
  } else if (location == GPIO_EXTERNAL_EXPANDER_2) {
    COMM_TAKE;
    currentStatus = GPIO->expander[1].TCA1->read1(pinNumber);
    COMM_GIVE;
#endif
  }
}

void GPIO_DESCRIPTION::readAnalog() {
  if (location == GPIO_INTERNAL) { currentValue = analogRead(pinNumber); }
}

void GPIO_DESCRIPTION::writeDigital(bool status) {
  if (valueChanged == true) {
    if (location == GPIO_INTERNAL) {
      digitalWrite(pinNumber, status);
#ifdef TCA9555_USED
    } else if (location == GPIO_EXTERNAL_EXPANDER_1) {
      COMM_TAKE;
      GPIO->expander[0].TCA1->write1(pinNumber, status);
      COMM_GIVE;
    } else if (location == GPIO_EXTERNAL_EXPANDER_2) {
      COMM_TAKE;
      GPIO->expander[1].TCA1->write1(pinNumber, status);
      COMM_GIVE;
#endif
    }
    valueChanged = false;
  }
}
