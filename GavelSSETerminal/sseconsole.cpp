#include "sseconsole.h"

#include "consoleScript.h"

#include <serialport.h>

HTMLBuilder* TerminalPage::getHtml(HTMLBuilder* html) {
  conductAction();
  sendPageBegin(html);
  for (unsigned int i = 0; i < SCRIPTHTML_COUNT; i++) html->println(scriptHTML[i]);
  html->brTag()->println();
  sendPageEnd(html);
  return html;
}

SSEConsole::SSEConsole() {
  setSSEEventName("sse_events");
  setSSECommandName("sse_command");
  heartbeatTimer.setRefreshSeconds(5);
  terminal.setup();
  writeQueue.clear();
  readQueue.clear();
}

void SSEConsole::connectClient() {
  heartbeatTimer.reset();
  writeQueue.clear();
  readQueue.clear();
  terminal.configure(CONSOLE);
  terminal.setEcho(false);
  terminal.useColor(true);
  terminal.clearHistory();
  sseBroadcastLine("Web Browser Terminal: ");
  terminal.banner();
  terminal.prompt();
}

static char executeBuffer[STREAM_BUFFER_SIZE];
void SSEConsole::executeClient() {
  unsigned int length = 0;
  memset(executeBuffer, 0, STREAM_BUFFER_SIZE);
  char c;
  terminal.loop();
  if (heartbeatTimer.expired()) { sseBroadcastEvent("heartbeat", "ping"); }
  if (writeQueue.count() > 0) {
    while (writeQueue.count() > 0) {
      writeQueue.pop(&c);
      if (c == '\n') {
        sseBroadcastLine(executeBuffer);
        length = 0;
        memset(executeBuffer, 0, STREAM_BUFFER_SIZE);
      } else {
        executeBuffer[length] = c;
        length++;
      }
    }
    if (length > 0) { sseBroadcastLine(executeBuffer); }
  }
}

void SSEConsole::executeCommand(String command) {
  command += '\n';
  const char* string = command.c_str();
  int length = strlen(string);
  for (int i = 0; i < length; i++) { readQueue.push((void*) &string[i]); }
}