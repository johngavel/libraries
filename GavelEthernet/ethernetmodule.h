#ifndef __GAVEL_ETHERNET
#define __GAVEL_ETHERNET

#include "architecture.h"
#include "networkinterface.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>

#define ETHERNET EthernetModule::get()

class EthernetModule : public Task, public VirtualNetwork, public VirtualServerFactory {
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
  bool getDHCP() { return isDHCP; };
  VirtualServer* getServer(int port);
  bool ipChanged = false;
  // Only used for initial configuration
  bool isConfigured = false;
  byte* macAddress = nullptr;
  bool isDHCP = false;

private:
  EthernetModule() : Task("EthernetModule"), ipChanged(false){};
  static EthernetModule* ethernetModule;
  bool setupSPI();
  bool resetW5500();
  bool setupW5500();

  // Only used for initial configuration
  byte* ipAddress = nullptr;
  byte* dnsAddress = nullptr;
  byte* subnetMask = nullptr;
  byte* gatewayAddress = nullptr;

  static void ipStat(Terminal* terminal);
};

class WiredServer : public VirtualServer {
public:
  WiredServer(int __port) : port(__port){};
  void begin();
  Client* accept();
  void closeClient();
  VirtualNetwork* getNetworkInterface() { return ETHERNET; };

private:
  EthernetServer* server = nullptr;
  EthernetClient client;
  int port;
};

#endif
