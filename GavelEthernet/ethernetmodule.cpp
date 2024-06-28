#include "ethernetmodule.h"

#include "gpio.h"
#include "license.h"
#include "serialport.h"

EthernetModule* EthernetModule::ethernetModule = nullptr;

EthernetModule* EthernetModule::get() {
  if (ethernetModule == nullptr) ethernetModule = new EthernetModule();
  return ethernetModule;
}

bool EthernetModule::setupSPI() {
  bool status = true;
  GPIO->configurePinReserve(GPIO_INTERNAL, 18, "SPI0 SCK");
  GPIO->configurePinReserve(GPIO_INTERNAL, 19, "SPI0 TX");
  GPIO->configurePinReserve(GPIO_INTERNAL, 16, "SPI0 RX");
  GPIO->configurePinReserve(GPIO_INTERNAL, 17, "SPI0 CS");
  status &= SPI.setSCK(18);
  status &= SPI.setTX(19);
  status &= SPI.setRX(16);
  status &= SPI.setCS(17);

  COMM_TAKE;
  SPI.begin();
  COMM_GIVE;
  PORT->println(PASSED, "SPI Start Complete");
  return status;
}

bool EthernetModule::resetW5500() {
  GPIO->configurePinReserve(GPIO_INTERNAL, 15, "W5500 Reset Pin");
  pinMode(15, OUTPUT);
  digitalWrite(15, LOW);
  delay(200);
  digitalWrite(15, HIGH);
  delay(200);
  PORT->println(PASSED, "W5500 Restart Complete");
  return true;
}

bool EthernetModule::setupW5500() {
  bool status = true;
  status &= resetW5500();
  COMM_TAKE;
  Ethernet.init(17);
  COMM_GIVE;
  if (isDHCP) {
    COMM_TAKE;
    Ethernet.begin(macAddress, 3000, 1500);
    COMM_GIVE;
    ipChanged = true;
  } else {
    COMM_TAKE;
    Ethernet.begin(macAddress, ipAddress, dnsAddress, gatewayAddress, subnetMask);
    COMM_GIVE;
  }
  COMM_TAKE;
  int hardwareStatus = Ethernet.hardwareStatus();
  COMM_GIVE;
  if (hardwareStatus == EthernetNoHardware) {
    status = false;
    PORT->println(ERROR, "Ethernet Module was not found.");
  } else if (hardwareStatus == EthernetW5100) {
    PORT->println(PASSED, "W5100 Ethernet controller detected.");
  } else if (hardwareStatus == EthernetW5200) {
    PORT->println(PASSED, "W5200 Ethernet controller detected.");
  } else if (hardwareStatus == EthernetW5500) {
    status &= true;
    PORT->println(PASSED, "W5500 Ethernet controller detected.");
  }
  return status;
}

void EthernetModule::configure(byte* __macAddress, bool __isDHCP, byte* __ipAddress, byte* __dnsAddress, byte* __subnetMask, byte* __gatewayAddress) {
  isConfigured = true;
  macAddress = __macAddress;
  isDHCP = __isDHCP;
  ipAddress = __ipAddress;
  dnsAddress = __dnsAddress;
  subnetMask = __subnetMask;
  gatewayAddress = __gatewayAddress;
}

void EthernetModule::setupTask() {
  bool status = false;
  ETHERNET_LICENSE;
  PORT->addCmd("ip", "", "IP Stats", EthernetModule::ipStat);
  setRefreshMilli(60000);
  if (isConfigured) {
    status = setupSPI();
    status &= setupW5500();
  }
  if (status)
    PORT->println(PASSED, "Ethernet Complete");
  else
    PORT->println(FAILED, "Ethernet Complete");
  runTimer(status);
}

void EthernetModule::executeTask() {
  COMM_TAKE;
  int maintain = Ethernet.maintain();
  COMM_GIVE;
  if (maintain == 4) { ipChanged = true; }
}

IPAddress EthernetModule::getIPAddress() {
  IPAddress address;
  COMM_TAKE;
  address = Ethernet.localIP();
  COMM_GIVE;
  return address;
}

IPAddress EthernetModule::getDNS() {
  IPAddress address;
  COMM_TAKE;
  address = Ethernet.dnsServerIP();
  COMM_GIVE;
  return address;
}

IPAddress EthernetModule::getSubnetMask() {
  IPAddress address;
  COMM_TAKE;
  address = Ethernet.subnetMask();
  COMM_GIVE;
  return address;
}

IPAddress EthernetModule::getGateway() {
  IPAddress address;
  COMM_TAKE;
  address = Ethernet.gatewayIP();
  COMM_GIVE;
  return address;
}

bool EthernetModule::linkStatus() {
  bool status;
  COMM_TAKE;
  status = Ethernet.linkStatus();
  COMM_GIVE;
  return status;
}

void EthernetModule::ipStat() {
  IPAddress ipAddress = ETHERNET->getIPAddress();
  bool linked = ETHERNET->linkStatus();
  PORT->println();
  PORT->print(INFO, "MAC Address:  ");
  PORT->print(INFO, String(ETHERNET->macAddress[0], HEX) + ":");
  PORT->print(INFO, String(ETHERNET->macAddress[1], HEX) + ":");
  PORT->print(INFO, String(ETHERNET->macAddress[2], HEX) + ":");
  PORT->print(INFO, String(ETHERNET->macAddress[3], HEX) + ":");
  PORT->print(INFO, String(ETHERNET->macAddress[4], HEX) + ":");
  PORT->println(INFO, String(ETHERNET->macAddress[5], HEX));
  PORT->println(INFO, "IP Address is " + String((ETHERNET->isDHCP) ? "DHCP" : "Static"));
  PORT->println(INFO, String((linked) ? "Connected" : "Unconnected"));
  PORT->println(INFO, "  IP Address:  " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") +
                          String(ipAddress[3]));
  ipAddress = ETHERNET->getSubnetMask();
  PORT->println(INFO, "  Subnet Mask: " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") +
                          String(ipAddress[3]));
  ipAddress = ETHERNET->getGateway();
  PORT->println(INFO, "  Gateway:     " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") +
                          String(ipAddress[3]));
  ipAddress = ETHERNET->getDNS();
  PORT->println(INFO, "  DNS Server:  " + String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") +
                          String(ipAddress[3]));
  PORT->prompt();
}
