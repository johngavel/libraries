#include "scan.h"

#include "gpio.h"
#include "serialport.h"

Scan* Scan::scanner = nullptr;

#define MAX_SCAN_DEVICES 6

Scan* Scan::get() {
  if (scanner == nullptr) scanner = new Scan();
  return scanner;
}

void Scan::setup() {
  GPIO->configurePinReserve(GPIO_INTERNAL, ProgramInfo::hardwarewire.pinSDA, "I2C SDA", true);
  GPIO->configurePinReserve(GPIO_INTERNAL, ProgramInfo::hardwarewire.pinSCL, "I2C SCL", true);

  PORT->addCmd("scan", "", "I2c Scanner", Scan::scani2c);
  PORT->println(PASSED, "I2C Scanner Complete");
}

void Scan::scan() {
  byte error, address;
  int nDevices;
  String devicesFound[MAX_SCAN_DEVICES];

  PORT->println(INFO, "\nI2C Scanner");
  PORT->println(INFO, "Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    COMM_TAKE;
    WIRE->beginTransmission(address);
    error = WIRE->endTransmission();
    COMM_GIVE;

    if (error == 0) {
      PORT->print(INFO, "I2C device found at address 0x");
      if (address < 16) PORT->print(INFO, "0");
      PORT->print(INFO, String(address, HEX));
      PORT->println(INFO, "  !");

      if (nDevices < MAX_SCAN_DEVICES) { devicesFound[nDevices] = String(nDevices) + ". 0x" + String(address, HEX); }

      nDevices++;

    } else if (error == 4) {
      PORT->print(WARNING, "Unknown error at address 0x");
      if (address < 16) PORT->print(WARNING, "0");
      PORT->println(WARNING, String(address, HEX));
    } else {
      // PORT->print(INFO, "No device found at address 0x");
      // if (address < 16) PORT->print(INFO, "0");
      // PORT->println(INFO, String(address, HEX));
    }
  }

  if (nDevices == 0)
    PORT->println(INFO, "No I2C devices found\n");
  else
    PORT->println(INFO, "done\n");
}

void Scan::scani2c() {
  PORT->println();
  SCAN->scan();
  PORT->prompt();
}
