#include "moduleCmd.h"

#include "debug.h"
#include "eeprom.h"
#include "onboardled.h"
#include "serialport.h"
#ifndef __GAVEL_WIFI
#include "ethernetmodule.h"
#endif
#include "files.h"
#include "gpio.h"
#include "license.h"
#include "scan.h"
#include "screen.h"
#include "servermodule.h"
#include "telnet.h"
#include "temperature.h"
#include "watchdog.h"
#ifndef __GAVEL_ETHERNET
#include "wifimodule.h"
#endif

static void moduleCommand(OutputInterface* terminal);

void addModuleCommand() {
  TERM_CMD->addCmd("modules", "", "Lists the availabilty of Gavel Modules", moduleCommand);
}

static String boolString(bool value) {
  if (value) return "Initialized";
  return "NOT AVAILABLE";
}
void moduleCommand(OutputInterface* terminal) {
  terminal->banner();
  terminal->println(HELP, "Blink        - ", boolString(BLINK_AVAILABLE));
  terminal->println(HELP, "Debug Screen - ", boolString(DEBUG_SCREEN_AVAILABLE));
  terminal->println(HELP, "EEPROM       - ", boolString(EEPROM_AVAILABLE));
#ifdef __GAVEL_ETHERNET
  terminal->println(HELP, "Ethernet     - ", boolString(ETHERNET_AVAILABLE));
#else
  terminal->println(HELP, "Ethernet     - ", boolString(false));
#endif
  terminal->println(HELP, "Files        - ", boolString(FILES_AVAILABLE));
  terminal->println(HELP, "GPIO Manager - ", boolString(GPIO_AVAILABLE));
  terminal->println(HELP, "License      - ", boolString(LICENSE_AVAILABLE));
  terminal->println(HELP, "Scan         - ", boolString(SCAN_AVAILABLE));
  terminal->println(HELP, "Screen       - ", boolString(SCREEN_AVAILABLE));
  terminal->println(HELP, "Serial Port  - ", boolString(PORT_AVAILABLE));
  terminal->println(HELP, "Server       - ", boolString(SERVER_AVAILABLE));
  terminal->println(HELP, "Task Manager - ", boolString(MANAGER_AVAILABLE));
  terminal->println(HELP, "Telent       - ", boolString(TELNET_AVAILABLE));
  terminal->println(HELP, "Temperature  - ", boolString(TEMPERATURE_AVAILABLE));
  terminal->println(HELP, "Watchdog     - ", boolString(WATCHDOG_AVAILABLE));
#ifdef __GAVEL_WIFI
  terminal->println(HELP, "Wifi         - ", boolString(WIFI_AVAILABLE));
#else
  terminal->println(HELP, "Wifi         - ", boolString(false));
#endif
  terminal->prompt();
}