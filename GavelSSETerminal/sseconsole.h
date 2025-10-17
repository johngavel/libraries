#ifndef __SSE_CONSOLE_H
#define __SSE_CONSOLE_H

#include "datastructure.h"
#include "sseclient.h"
#include "terminal.h"
#include "timer.h"

#include <commonhtml.h>

#define STREAM_BUFFER_SIZE 8192

class SSEConsole : public SSEClient, Stream {
public:
  SSEConsole();

private:
  virtual void connectClient();
  virtual void executeClient();
  virtual void executeCommand(String command);
  Timer heartbeatTimer;
  Terminal terminal = Terminal(this);
  virtual int available() { return readQueue.count(); };
  virtual int read() {
    char read;
    readQueue.pop(&read);
    return read;
  };
  virtual int peek() {
    char read;
    read = *((char*) readQueue.get(0));
    return read;
  };
  virtual size_t write(uint8_t buffer) {
    char write = (char) buffer;
    return (size_t) writeQueue.push(&write);
  };
  ClassicQueue writeQueue = ClassicQueue(STREAM_BUFFER_SIZE, 1);
  ClassicQueue readQueue = ClassicQueue(STREAM_BUFFER_SIZE, 1);
};

class TerminalPage : public BasicPage {
public:
  TerminalPage() {
    setPageName("terminal");
    refresh = 10;
  };
  void conductAction() {};
  HTMLBuilder* getHtml(HTMLBuilder* html);
  int refresh;
};

#endif