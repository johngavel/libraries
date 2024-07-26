#include "telnet.h"

#include "license.h"
#include "serialport.h"

#define BUFFER_SIZE 1024
#define HEADER_LENGTH 4096

TelnetModule* TelnetModule::telnetModule = nullptr;

TelnetModule::TelnetModule() : Task("Telnet") {
  setRefreshMilli(10);
  client = nullptr;
  terminal = nullptr;
  clientConnected = false;
}

TelnetModule* TelnetModule::get() {
  if (telnetModule == nullptr) telnetModule = new TelnetModule();
  return telnetModule;
}

void TelnetModule::setupTask() {
  TERMINAL_LICENSE;
  COMM_TAKE;
  server->begin();
  COMM_GIVE;
  TERM_CMD->addCmd("exit", "", "Closes the Telnet Session.", closeTelnet);
  CONSOLE->println(PASSED, "TELNET Server Complete");
}

void TelnetModule::executeTask() {
  if (!clientConnected) {
    COMM_TAKE;
    client = server->accept();
    COMM_GIVE;
    if (client->connected()) {
      clientConnected = true;
      if (terminal == nullptr) {
        terminal = new Terminal(client);
        terminal->configure(CONSOLE);
        terminal->setup();
        terminal->setEcho(false);
        terminal->useBS(true);
      } else {
        terminal->setStream(client);
      }
      COMM_TAKE;
      client->println("Starting Telnet Session.");
      COMM_GIVE;
      terminal->banner();
      terminal->prompt();
      COMM_TAKE;
      client->flush();
      COMM_GIVE;
    }
  } else {
    if (client->connected()) {
      terminal->loop();
      COMM_TAKE;
      client->flush();
      COMM_GIVE;
    } else {
      clientConnected = false;
    }
  }
}

void TelnetModule::closeTelnet(Terminal* terminal) {
  if (terminal == TELNET->terminal) {
    terminal->println(INFO, "Closing Telnet Session.");
    TELNET->client->flush();
    TELNET->client->stop();
  } else {
    terminal->println(ERROR, "Not supported on this terminal.");
    terminal->prompt();
  }
}
