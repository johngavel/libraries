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
  EthernetServer* server;
  EthernetClient client;
  int port;
};

#endif
