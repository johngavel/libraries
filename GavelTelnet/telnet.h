#ifndef __GAVEL_TELNET
#define __GAVEL_TELNET

#include "architecture.h"
#include "networkinterface.h"

#include <Terminal.h>

#define TELNET TelnetModule::get()
#define TELNET_AVAILABLE TelnetModule::initialized()

class TelnetModule : public Task {
public:
  static TelnetModule* get();
  static bool initialized() { return telnetModule != nullptr; };
  void configure(VirtualServer* __server) { server = __server; };
  void setupTask();
  void executeTask();
  VirtualServer* getServer() { return server; };
  Terminal* terminal;
  Client* client;

private:
  TelnetModule();
  static TelnetModule* telnetModule;
  bool clientConnected;
  VirtualServer* server;
  static void closeTelnet(Terminal* terminal);
};

#endif
