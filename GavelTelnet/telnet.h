#ifndef __GAVEL_TELNET
#define __GAVEL_TELNET

#include "architecture.h"
#include "networkinterface.h"

#include <Terminal.h>

#define TELNET TelnetModule::get()

class TelnetModule : public Task {
public:
  static TelnetModule* get();
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
