#ifndef __GAVEL_ETHERNET
#define __GAVEL_ETHERNET

#include "architecture.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>

#define ETHERNET EthernetModule::get()

class EthernetModule : public Task {
public:
  static EthernetModule* get();

  void configure(byte* __macAddress, bool __isDHCP, byte* __ipAddress, byte* __dnsAddress, byte* __subnetMask, byte* __gatewayAddress);
  void setupTask();
  void executeTask();
  bool linkStatus();
  IPAddress getIPAddress();
  IPAddress getSubnetMask();
  IPAddress getGateway();
  IPAddress getDNS();
  bool ipChanged;
  // Only used for initial configuration
  bool isConfigured;
  byte* macAddress;
  bool isDHCP;

private:
  EthernetModule() : Task("EthernetModule"), ipChanged(false){};
  static EthernetModule* ethernetModule;
  bool setupSPI();
  bool resetW5500();
  bool setupW5500();

  // Only used for initial configuration
  byte* ipAddress;
  byte* dnsAddress;
  byte* subnetMask;
  byte* gatewayAddress;

  static void ipStat();
};

#endif
