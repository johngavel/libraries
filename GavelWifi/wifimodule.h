#ifndef __GAVEL_WIFI
#define __GAVEL_WIFI

#include "architecture.h"

#include <Arduino.h>
#include <WiFi.h>

#define WIFI WifiModule::get()

class WifiModule : public Task {
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
  static void wifiStat();
  static void wifiScan();
};

#endif
