#ifndef __GAVEL_SSE_CLIENT
#define __GAVEL_SSE_CLIENT

#include "networkinterface.h"

#define CLIENT_NAME_LENGTH 16

class SSEClient {
public:
  SSEClient();
  char* getSSEEventName() { return eventName; };
  char* getSSECommandName() { return commandName; };
  void setSSEEventName(const char* __name) { strncpy(eventName, __name, CLIENT_NAME_LENGTH - 1); };
  void setSSECommandName(const char* __name) { strncpy(commandName, __name, CLIENT_NAME_LENGTH - 1); };
  void connect(Client* __client);
  void executeTask();
  void processPost(Client* __client);

protected:
  void sseBroadcastEvent(const char* eventName, const String& payload);
  void sseBroadcastLine(const char* line);

private:
  Client* sseClient;
  char eventName[CLIENT_NAME_LENGTH];
  char commandName[CLIENT_NAME_LENGTH];
  void closeClient();
  virtual void connectClient() = 0;
  virtual void executeClient() = 0;
  virtual void executeCommand(String command) = 0;
};

#endif