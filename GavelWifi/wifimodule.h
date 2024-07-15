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
  bool getDHCP() { return isDHCP; };
  VirtualServer* getServer(int port);
  bool ipChanged;
  // Only used for initial configuration
  bool isConfigured;
  byte* macAddress;
  bool isDHCP;

private:
  WifiModule() : Task("WifiModule"), ipChanged(false){};
  static WifiModule* wifiModule;
  bool setupWifi();

  // Only used for initial configuration
  String ssid;
  String password;
  static void wifiStat(Terminal* terminal);
  static void wifiScan(Terminal* terminal);
};

class WifiServer : public VirtualServer {
public:
  WifiServer(int __port) : port(__port){};
  void begin();
  Client* accept();
  void closeClient();
  VirtualNetwork* getNetworkInterface() { return WIFI; };

private:
  WiFiServer* server;
  WiFiClient client;
  int port;
};

#endif
