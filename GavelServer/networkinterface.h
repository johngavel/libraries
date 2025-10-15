#ifndef __GAVEL_HTTP_SERVER
#define __GAVEL_HTTP_SERVER

#include "Client.h"
#include "architecture.h"
#include "html.h"

#define TELNET_PORT 23
#define HTTP_PORT 80
#define TEST_PORT 4242
#define MAX_CLIENTS 16
#define BUFFER_SIZE 1024

class VirtualNetwork {
public:
  virtual IPAddress getIPAddress() = 0;
  virtual IPAddress getSubnetMask() = 0;
  virtual IPAddress getGateway() = 0;
  virtual IPAddress getDNS() = 0;
  virtual bool getDHCP() = 0;
  virtual byte* getMACAddress() = 0;
};

class VirtualServer {
public:
  virtual void begin() = 0;
  virtual Client* accept() = 0;
  virtual VirtualNetwork* getNetworkInterface() = 0;
};

class VirtualServerFactory {
public:
  virtual VirtualServer* getServer(int port) = 0;
};

char clientRead(Client* client);
unsigned int clientRead(Client* client, char* buffer, unsigned int length);
bool clientAvailable(Client* client);
unsigned int clientWrite(Client* client, char* buffer, unsigned int length);
unsigned int clientWrite(Client* client, HTMLBuilder* html);
bool clientConnected(Client* client);
void clientClose(Client* client);

#endif