#include "gpio.h"

#include "asciitable.h"
#include "license.h"
#include "serialport.h"

#include <Terminal.h>

const char* stringHardware(HW_TYPES hw_type);
const char* stringLocation(GPIO_LOCATION location);

GPIOManager* GPIOManager::gpioManager = nullptr;

GPIOManager::GPIOManager() : Task("GPIO") {
#ifdef TCA9555_USED
  for (int i = 0; i < 2; i++) {
    expander[i].address = 0;
    expander[i].configured = false;
  }
#endif
  for (int i = 0; i < GPIO_MAX_TYPES; i++) gpioTypeConfigured[i] = false;
  currentConfiguredPins = 0;
  invalidOverallConfiguration = true;
  configureHW();
  setRefreshMilli(5);
}

GPIOManager* GPIOManager::get() {
  if (gpioManager == nullptr) gpioManager = new GPIOManager();
  return gpioManager;
}

void GPIOManager::configureHW() {
  switch (ProgramInfo::hw_type) {
  case (HW_RASPBERRYPI_PICO):
  case (HW_RASPBERRYPI_PICOW):
    invalidOverallConfiguration = false;
    configurePinUndefined(23);
    configurePinUndefined(24);
    configurePinUndefined(29);
    break;
  case (HW_RP2040_ZERO):
    invalidOverallConfiguration = false;
    configurePinUndefined(17);
    configurePinUndefined(18);
    configurePinUndefined(19);
    configurePinUndefined(20);
    configurePinUndefined(21);
    configurePinUndefined(22);
    configurePinUndefined(23);
    configurePinUndefined(24);
    configurePinUndefined(25);
    break;
  case (HW_GAVEL_MINI_PICO_ETH):
    invalidOverallConfiguration = false;
    configurePinUndefined(2);
    configurePinUndefined(3);
    configurePinUndefined(6);
    configurePinUndefined(7);
    configurePinUndefined(8);
    configurePinUndefined(9);
    configurePinUndefined(10);
    configurePinUndefined(11);
    configurePinUndefined(12);
    configurePinUndefined(13);
    configurePinUndefined(14);
    configurePinUndefined(20);
    configurePinUndefined(21);
    configurePinUndefined(22);
    configurePinUndefined(23);
    configurePinUndefined(24);
    configurePinUndefined(26);
    configurePinUndefined(27);
    configurePinUndefined(28);
    configurePinUndefined(29);
    break;
  default: CONSOLE->println(ERROR, "GPIO Invalid configure of Hardware Type"); break;
  }
}

#ifdef TCA9555_USED
void GPIOManager::configureExpander(unsigned long index, int address) {
  int error;
  TCA9555_LICENSE;
  if ((index < 2) && (expander[index].configured == false)) {
    GPIO->configurePinReserve(GPIO_INTERNAL, ProgramInfo::hardwarewire.pinSDA, "I2C SDA", true);
    GPIO->configurePinReserve(GPIO_INTERNAL, ProgramInfo::hardwarewire.pinSCL, "I2C SCL", true);
    COMM_TAKE;
    expander[index].configured = true;
    expander[index].address = address;
    WIRE->beginTransmission(address);
    error = WIRE->endTransmission();
    COMM_GIVE;
    if (error == 0) {
      COMM_TAKE;
      expander[index].valid = true;
      expander[index].TCA1 = new TCA9555(address, WIRE);
      expander[index].valid |= expander[index].TCA1->begin();
      expander[index].valid |= expander[index].TCA1->write16(0x00);
      COMM_GIVE;
    }
    if (!expander[index].valid) {
      CONSOLE->println(ERROR, "Unable to access TCA1 GPIO Expander at address: " + String(address));
      invalidOverallConfiguration = true;
    }
    COMM_GIVE;
  }
}
#endif

void GPIOManager::addPinConfiguration(int index, int pinNumber, GPIO_TYPE type, GPIO_LOCATION location, GPIO_LED_TYPE led_type, const char* description) {
  GPIO_DESCRIPTION* entry = &table[currentConfiguredPins];
  entry->index = index;
  entry->pinNumber = pinNumber;
  entry->type = type;
  entry->location = location;
  entry->led_type = led_type;
  entry->setCurrentStatus(0);
  entry->setCurrentValue(0);
  strncpy(entry->description, description, DESCRIPTION_LENGTH - 1);
  currentConfiguredPins++;
  gpioTypeConfigured[type] = true;
}

bool GPIOManager::validConfiguration(GPIO_LOCATION location, int pinNumber) {
  bool valid = true;
  if (currentConfiguredPins >= MAX_PINS) valid = false;
  switch (location) {
  case GPIO_INTERNAL:
    if (ProgramInfo::hw_type == HW_UNKNOWN) valid = false;
    if ((pinNumber > 29) || (pinNumber < 0)) valid = false;
    if ((ProgramInfo::hw_type == HW_RASPBERRYPI_PICOW) && (pinNumber == LED_BUILTIN)) valid = true;
    break;
#ifdef TCA9555_USED
  case GPIO_EXTERNAL_EXPANDER_1:
    if (expander[0].valid == false) valid = false;
    if ((pinNumber > 15) || (pinNumber < 0)) valid = false;
    break;
  case GPIO_EXTERNAL_EXPANDER_2:
    if (expander[1].valid == false) valid = false;
    if ((pinNumber > 15) || (pinNumber < 0)) valid = false;
    break;
#endif
  default: valid = false; break;
  }
  for (int i = 0; i < MAX_PINS; i++) {
    if ((table[i].location == location) && (table[i].pinNumber == pinNumber)) valid = false;
  }
  return valid;
}

bool GPIOManager::validConfiguration(GPIO_LOCATION location, int pinNumber, int index, GPIO_TYPE type) {
  bool valid = validConfiguration(location, pinNumber);
  for (int i = 0; i < MAX_PINS; i++) {
    if ((table[i].type == type) && (table[i].index == index)) valid = false;
  }
  return valid;
}

void GPIOManager::configurePinReserve(GPIO_LOCATION location, int pinNumber, const char* description, bool isShared) {
  if (validConfiguration(location, pinNumber)) {
    addPinConfiguration(NOT_ACCESSIBLE, pinNumber, GPIO_RESERVED, location, GPIO_NOT_APPLICABLE, description);
  } else {
    if (!isShared) {
      CONSOLE->println(ERROR, "GPIO Invalid Reserve Configuration " + String(stringLocation(location)) + ":" + String(pinNumber));
      invalidOverallConfiguration = true;
    }
  }
}

void GPIOManager::configurePinUndefined(int pinNumber) {
  if (validConfiguration(GPIO_INTERNAL, pinNumber)) {
    addPinConfiguration(NOT_ACCESSIBLE, pinNumber, GPIO_UNDEFINED, GPIO_INTERNAL, GPIO_NOT_APPLICABLE, "Not Accessible");
  } else {
    CONSOLE->println(ERROR, "GPIO Invalid Undefined Configuration " + String(stringLocation(GPIO_INTERNAL)) + ":" + String(pinNumber));
    invalidOverallConfiguration = true;
  }
}

void GPIOManager::configurePinIO(GPIO_TYPE type, GPIO_LOCATION location, int pinNumber, int index, const char* description) {
  bool validType = false;
  if (location == GPIO_INTERNAL) {
    validType = type == GPIO_INPUT;
    validType |= type == GPIO_OUTPUT;
    validType |= type == GPIO_BUTTON;
    validType |= type == GPIO_PULSE;
    validType |= type == GPIO_PWM;
    validType |= type == GPIO_TONE;
    validType |= type == GPIO_ADC;
  } else {
    validType = type == GPIO_INPUT;
    validType |= type == GPIO_OUTPUT;
    validType |= type == GPIO_BUTTON;
    validType |= type == GPIO_PULSE;
  }
  if (validType && (validConfiguration(location, pinNumber, index, type))) {
    addPinConfiguration(index, pinNumber, type, location, GPIO_NOT_APPLICABLE, description);
  } else {
    CONSOLE->println(ERROR, "GPIO Invalid I/O Configuration " + String(stringLocation(location)) + ":" + String(pinNumber));
    invalidOverallConfiguration = true;
  }
}

void GPIOManager::configurePinLED(GPIO_LOCATION location, int pinNumber, GPIO_LED_TYPE type, int index, const char* description) {
  if ((type != GPIO_NOT_APPLICABLE) && (validConfiguration(location, pinNumber, index, GPIO_LED))) {
    addPinConfiguration(index, pinNumber, GPIO_LED, location, type, description);
  } else {
    CONSOLE->println(ERROR, "GPIO Invalid LED Configuration " + String(stringLocation(location)) + ":" + String(pinNumber));
    invalidOverallConfiguration = true;
  }
}

GPIO_DESCRIPTION* GPIOManager::getPin(GPIO_TYPE __type, int __index) {
  GPIO_DESCRIPTION* gpio = nullptr;
  for (int i = 0; i < MAX_PINS; i++) {
    if ((table[i].type == __type) && (table[i].index == __index)) gpio = &table[i];
  }
  return gpio;
}

void GPIOManager::setupTask() {
  TERM_CMD->addCmd("gpio", "[v|all]", "Prints the configured GPIO Table", GPIOManager::printTable);
  if (gpioTypeConfigured[GPIO_TONE] == true) { TERM_CMD->addCmd("tone", "[n] [Hz]", "Sets a Square Wave in Hz on Tone Pin n ", GPIOManager::toneCmd); }
  if (gpioTypeConfigured[GPIO_PWM] == true) { TERM_CMD->addCmd("pwm", "[n] [f] [%]", "Sets the frequency and % Duty Cycyle PWM Pin n ", GPIOManager::pwmCmd); }
  if (gpioTypeConfigured[GPIO_PULSE] == true) { TERM_CMD->addCmd("pulse", "[n]", "Command a Output n to pulse", GPIOManager::pulseCmd); }
  if (gpioTypeConfigured[GPIO_INPUT] == true) { TERM_CMD->addCmd("stat", "[n]", "Status of Input n", GPIOManager::statusCmd); }

  // Configure all the pins
  GPIO_DESCRIPTION* gpio = nullptr;
  for (int i = 0; i < MAX_PINS; i++) {
    gpio = &table[i];
    invalidOverallConfiguration |= !gpio->setup();
    gpio->execute();
  }
  CONSOLE->println((invalidOverallConfiguration) ? FAILED : PASSED, "GPIO Manager Complete");
}

void GPIOManager::executeTask() {
  GPIO_DESCRIPTION* gpio = nullptr;
  for (int i = 0; i < MAX_PINS; i++) {
    gpio = &table[i];
    if (gpio->getValidExecute() == true) gpio->execute();
  }
}

const char* stringHardware(HW_TYPES hw_type) {
  const char* hwString;
  switch (hw_type) {
  case HW_RP2040_ZERO: hwString = "Waveshare RP2040 Zero"; break;
  case HW_RASPBERRYPI_PICO: hwString = "Raspberry Pi Pico"; break;
  case HW_RASPBERRYPI_PICOW: hwString = "Raspberry Pi Pico W"; break;
  case HW_GAVEL_MINI_PICO_ETH: hwString = "Gavel Mini Pico - Ethernet"; break;
  default: hwString = "Unknown"; break;
  }
  return hwString;
}

const char* stringLocation(GPIO_LOCATION location) {
  const char* locationString;
  switch (location) {
  case GPIO_INTERNAL: locationString = "Internal"; break;
  case GPIO_EXTERNAL_EXPANDER_1: locationString = "Expander 1"; break;
  case GPIO_EXTERNAL_EXPANDER_2: locationString = "Expander 2"; break;
  default: locationString = "Unknown"; break;
  }
  return locationString;
}

const char* stringType(GPIO_TYPE type) {
  const char* typeString;
  switch (type) {
  case GPIO_INPUT: typeString = "Input"; break;
  case GPIO_OUTPUT: typeString = "Output"; break;
  case GPIO_LED: typeString = "LED"; break;
  case GPIO_BUTTON: typeString = "Button"; break;
  case GPIO_PULSE: typeString = "Pulse"; break;
  case GPIO_PWM: typeString = "PWM"; break;
  case GPIO_TONE: typeString = "Tone"; break;
  case GPIO_ADC: typeString = "ADC"; break;
  case GPIO_RESERVED: typeString = "Reserved"; break;
  case GPIO_UNACCESSIBLE: typeString = "Unaccessible"; break;
  case GPIO_UNDEFINED: typeString = "Undefined"; break;
  default: typeString = "Unknown"; break;
  }
  return typeString;
}

int comp(const void* __lhs, const void* __rhs) {
  int lhs = *((int*) __lhs);
  int rhs = *((int*) __rhs);
  int c = 1;
  if (GPIO->table[lhs].location < GPIO->table[rhs].location) c = -1;
  if ((GPIO->table[lhs].location == GPIO->table[rhs].location) && (GPIO->table[lhs].pinNumber < GPIO->table[rhs].pinNumber)) c = -1;
  return c;
}

static int sorted[MAX_PINS];

void GPIOManager::printTable(Terminal* terminal) {
  bool all = false;
  bool verbose = false;
  char* value = terminal->readParameter();

  if ((value != NULL) && (strncmp("all", value, 3) == 0)) {
    all = true;
    verbose = true;
  } else if ((value != NULL) && (strncmp("v", value, 1) == 0)) {
    verbose = true;
  } else if (value != NULL) {
    terminal->invalidParameter();
    terminal->prompt();
    return;
  }

  for (int i = 0; i < MAX_PINS; i++) sorted[i] = i;
  qsort(sorted, MAX_PINS, sizeof(int), comp);

  terminal->println(INFO, "GPIO Table");
  terminal->println(INFO, "Hardware is " + String(stringHardware(ProgramInfo::hw_type)));
#ifdef TCA9555_USED
  for (int i = 0; i < 2; i++) {
    if (GPIO->expander[i].configured)
      terminal->println(INFO,
                        "Expander " + String(i + 1) + " Address: " + String(GPIO->expander[i].address) + ((GPIO->expander[i].valid) ? " Valid" : " Invalid"));
  }
#endif

  AsciiTable table(terminal);
  table.addColumn(Normal, "#", 4);
  table.addColumn(Normal, "Type", 10);
  table.addColumn(Normal, "Pin", 5);
  table.addColumn(Normal, "Location", 10);
  table.addColumn(Normal, "Index", 7);
  table.addColumn(Normal, "Value", 7);
  table.addColumn(Normal, "Note", 30);
  table.printHeader();
  for (int i = 0; i < MAX_PINS; i++) {
    GPIO_DESCRIPTION* entry = &GPIO->table[sorted[i]];
    bool printPin = true;
    if (entry->type == GPIO_UNACCESSIBLE) printPin = false;
    if ((entry->type == GPIO_UNDEFINED) && (all == false)) printPin = false;
    if ((entry->type == GPIO_RESERVED) && (verbose == false)) printPin = false;
    if (printPin)
      table.printData(String(i + 1), String(stringType(entry->type)), String(entry->pinNumber), String(stringLocation(entry->location)),
                      (entry->index == -1) ? " " : String(entry->index), String(entry->getCurrentStatus()), String(entry->description));
  }
  table.printDone("GPIO Table");

  terminal->println();
  terminal->prompt();
}

void GPIOManager::toneCmd(Terminal* terminal) {
  unsigned long freq;
  unsigned long index;
  GPIO_DESCRIPTION* gpio;
  char* value;
  char* value2;
  value = terminal->readParameter();
  value2 = terminal->readParameter();
  if ((value != NULL) && (value2 != NULL)) {
    index = (unsigned long) atoi(value);
    freq = (unsigned long) atoi(value2);
    gpio = GPIO->getPin(GPIO_TONE, index);
    if (gpio != nullptr) {
      gpio->setCurrentFreq(freq);
      gpio->execute();
    } else {
      terminal->println(ERROR, "Cannot find Tone Pin Index: " + String(index));
    }
  } else {
    terminal->invalidParameter();
  }
  terminal->prompt();
}

void GPIOManager::pwmCmd(Terminal* terminal) {
  unsigned long frequency;
  unsigned long dutyCycle;
  unsigned long index;
  GPIO_DESCRIPTION* gpio;
  char* value;
  char* value2;
  char* value3;
  value = terminal->readParameter();
  value2 = terminal->readParameter();
  value3 = terminal->readParameter();
  if ((value != NULL) && (value2 != NULL) && (value3 != NULL)) {
    index = (unsigned long) atoi(value);
    frequency = (unsigned long) atoi(value2);
    dutyCycle = (unsigned long) atoi(value3);
    gpio = GPIO->getPin(GPIO_PWM, index);
    if (gpio != nullptr) {
      gpio->setCurrentFreq(frequency);
      gpio->setCurrentValue(dutyCycle);
      gpio->execute();
    } else {
      terminal->println(ERROR, "Cannot find PWM Pin Index: " + String(index));
    }
  } else {
    terminal->invalidParameter();
  }
  terminal->prompt();
}

void GPIOManager::pulseCmd(Terminal* terminal) {
  unsigned long index;
  GPIO_DESCRIPTION* gpio;
  char* value;
  value = terminal->readParameter();
  if (value != NULL) {
    index = (unsigned long) atoi(value);
    gpio = GPIO->getPin(GPIO_PULSE, index);
    if (gpio != nullptr) {
      gpio->setCurrentStatus(true);
    } else {
      terminal->println(ERROR, "Cannot find Pulse Pin Index: " + String(index));
    }
  } else {
    terminal->invalidParameter();
  }
  terminal->prompt();
}

void GPIOManager::statusCmd(Terminal* terminal) {
  unsigned long index;
  GPIO_DESCRIPTION* gpio;
  char* value;
  value = terminal->readParameter();
  if (value != NULL) {
    index = (unsigned long) atoi(value);
    gpio = GPIO->getPin(GPIO_INPUT, index);
    if (gpio != nullptr) {
      terminal->println(INFO, (gpio->getCurrentStatus()) ? "ON" : "OFF");
    } else {
      terminal->println(ERROR, "Cannot find Input Pin Index: " + String(index));
    }
  } else {
    terminal->invalidParameter();
  }
  terminal->prompt();
}
