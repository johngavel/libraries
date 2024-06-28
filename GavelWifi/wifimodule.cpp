#include "wifimodule.h"

#include "serialport.h"

WifiModule* WifiModule::wifiModule = nullptr;

WifiModule* WifiModule::get() {
  if (wifiModule == nullptr) wifiModule = new WifiModule();
  return wifiModule;
}

void WifiModule::configure(String __ssid, String __password) {
  isConfigured = true;
  ssid = __ssid;
  password = __password;
}

void WifiModule::setupTask() {
  bool status = false;
  bool initializing = true;
  unsigned int wifiStatus;
  PORT->addCmd("ip", "", "IP Stats", WifiModule::wifiStat);
  PORT->addCmd("wifiscan", "", "Scans the Wifi for Networks", WifiModule::wifiScan);
  setRefreshMilli(60000);
  if (isConfigured) {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("GavelWifiModule");
    PORT->print(INFO, "Connecting Wifi to " + String(ssid));
    WiFi.begin(ssid.c_str(), password.c_str());
    while (initializing) {
      wifiStatus = WiFi.status();
      switch (wifiStatus) {
      case WL_NO_MODULE:
      case WL_NO_SSID_AVAIL:
      case WL_CONNECT_FAILED:
        initializing = false;
        status = false;
        break;
      case WL_CONNECTED:
        initializing = false;
        status = true;
        break;
      default:
        delay(500);
        PORT->print(INFO, ".");
        break;
      }
    }
  }
  PORT->println();
  if (status)
    PORT->println(PASSED, "Wifi Complete");
  else
    PORT->println(FAILED, "Wifi Complete");
  runTimer(status);
}

void WifiModule::executeTask() {}

String WifiModule::getSSID() {
  String network;
  COMM_TAKE;
  network = WiFi.SSID();
  COMM_GIVE;
  return network;
}

IPAddress WifiModule::getIPAddress() {
  IPAddress address;
  COMM_TAKE;
  address = WiFi.localIP();
  COMM_GIVE;
  return address;
}

IPAddress WifiModule::getDNS() {
  IPAddress address;
  COMM_TAKE;
  address = WiFi.dnsIP();
  COMM_GIVE;
  return address;
}

IPAddress WifiModule::getSubnetMask() {
  IPAddress address;
  COMM_TAKE;
  address = WiFi.subnetMask();
  COMM_GIVE;
  return address;
}

IPAddress WifiModule::getGateway() {
  IPAddress address;
  COMM_TAKE;
  address = WiFi.gatewayIP();
  COMM_GIVE;
  return address;
}

bool WifiModule::linkStatus() {
  bool status;
  COMM_TAKE;
  status = WiFi.isConnected();
  COMM_GIVE;
  return status;
}

void WifiModule::wifiStat() {
  IPAddress ipAddress = WIFI->getIPAddress();
  bool linked = WIFI->linkStatus();
  PORT->println();
  PORT->println(INFO, "Network: " + WIFI->getSSID() + ((linked) ? " Connected" : " Unconnected"));
  PORT->println(INFO, "  IP Address:  " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") +
                          String(ipAddress[3]));
  ipAddress = WIFI->getSubnetMask();
  PORT->println(INFO, "  Subnet Mask: " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") +
                          String(ipAddress[3]));
  ipAddress = WIFI->getGateway();
  PORT->println(INFO, "  Gateway:     " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") +
                          String(ipAddress[3]));
  ipAddress = WIFI->getDNS();
  PORT->println(INFO, "  DNS Server:  " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") +
                          String(ipAddress[3]));
  PORT->prompt();
}

static const char* macToString(uint8_t mac[6]) {
  static char s[20];
  sprintf(s, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return s;
}

static const char* encToString(uint8_t enc) {
  switch (enc) {
  case ENC_TYPE_NONE: return "NONE";
  case ENC_TYPE_TKIP: return "WPA";
  case ENC_TYPE_CCMP: return "WPA2";
  case ENC_TYPE_AUTO: return "AUTO";
  }
  return "UNKN";
}

void WifiModule::wifiScan() {
  PORT->println();
  PORT->println(INFO, "Begining Scan.........");
  auto cnt = WiFi.scanNetworks();
  if (!cnt) {
    PORT->println(INFO, "No Networks Found.");
  } else {
    char line[80];
    sprintf(line, "Found %d networks\n", cnt);
    PORT->println(INFO, line);
    sprintf(line, "%32s %5s %17s %2s %4s", "SSID", "ENC", "BSSID        ", "CH", "RSSI");
    PORT->println(INFO, line);
    for (auto i = 0; i < cnt; i++) {
      uint8_t bssid[6];
      WiFi.BSSID(i, bssid);
      sprintf(line, "%32s %5s %17s %2d %4ld", WiFi.SSID(i), encToString(WiFi.encryptionType(i)), macToString(bssid), WiFi.channel(i), WiFi.RSSI(i));
      PORT->println(INFO, line);
    }
  }
  PORT->println(PASSED, "Scan Complete");
  PORT->prompt();
}
