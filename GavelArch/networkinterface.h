#ifndef __GAVEL_HTTP_SERVER
#define __GAVEL_HTTP_SERVER

#include "Client.h"

#define TELNET_PORT 23
#define HTTP_PORT 80
#define TEST_PORT 4242

class VirtualNetwork {
public:
  virtual IPAddress getIPAddress() = 0;
  virtual IPAddress getSubnetMask() = 0;
  virtual IPAddress getGateway() = 0;
  virtual IPAddress getDNS() = 0;
  virtual bool getDHCP() = 0;
};

class VirtualServer {
public:
  virtual void begin() = 0;
  virtual Client* accept() = 0;
  virtual void closeClient() = 0;
  virtual VirtualNetwork* getNetworkInterface() = 0;
};

class VirtualServerFactory {
public:
  virtual VirtualServer* getServer(int port) = 0;
};

#endif