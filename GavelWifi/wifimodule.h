#ifndef __GAVEL_WIFI
#define __GAVEL_WIFI

#include "architecture.h"
#include "networkinterface.h"

#include <Arduino.h>
#include <WiFi.h>

#define WIFI WifiModule::get()

class WifiModule : public Task, public VirtualNetwork, public VirtualServerFactory {
public:
  static WifiModule* get();

  void configure(String __ssid, String __password);
  void setupTask();
  void executeTask();
  bool linkStatus();
  String getSSID();
  IPAddress getIPAddress();
  IPAddress getSubnetMask();
  IPAddress getGateway();
  IPAddress getDNS();
  bool getDHCP() { return false; };
  VirtualServer* getServer(int port);
  bool ipChanged;
  // Only used for initial configuration
  bool isConfigured;
  byte* macAddress;

private:
  WifiModule() : Task("WifiModule"), ipChanged(false), isConfigured(false), macAddress(nullptr){};
  static WifiModule* wifiModule;
  bool setupWifi();

  // Only used for initial configuration
  String ssid;
  String password;
  static void wifiStat(Terminal* terminal);
  static void wifiScan(Terminal* terminal);
};

class WirelessClientManager {
public:
  WirelessClientManager(){};
  Client* setClient(WiFiClient __client);

private:
  WiFiClient client[MAX_CLIENTS];
  WiFiClient errorClient;
};

class WifiServer : public VirtualServer {
public:
  WifiServer(int __port) : server(nullptr), port(__port){};
  void begin();
  Client* accept();
  VirtualNetwork* getNetworkInterface() { return WIFI; };

private:
  WiFiServer* server = nullptr;
  WirelessClientManager clientManager;
  int port;
};

#endif
