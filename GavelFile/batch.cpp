#include "batch.h"

#include "files.h"
#include "timer.h"
#include "watchdog.h"

#include <Terminal.h>

static void runCommand(Terminal* terminal);
static void delayCommand(Terminal* terminal);
static void echoCommand(Terminal* terminal);

void addBatchTerminalCommands() {
  TERM_CMD->addCmd("run", "[filename]", "Runs the commands located in the file", runCommand);
  TERM_CMD->addCmd("delay", "[n]", "Delays for [n] number of seconds (1 - 10)", delayCommand);
  TERM_CMD->addCmd("echo", "...", "Displays a line of text", echoCommand);
}

void runCommand(Terminal* terminal) {
  char* value;
  value = terminal->readParameter();
  if (value != NULL) {
    if (FILES_AVAILABLE && (FILES->verifyFile(value))) {
      File file = FILES->getFile(value);
      Terminal runTerminal(&file, terminal->getOutput());
      runTerminal.configure(terminal);
      runTerminal.setup();
      runTerminal.setEcho(false);
      runTerminal.usePrompt(false);
      while (file.available()) runTerminal.loop();
    } else {
      terminal->println(ERROR, "\"" + String(value) + "\" File does not exist!!!");
    }
  } else {
    terminal->invalidParameter();
  }
  terminal->prompt();
}

void delayCommand(Terminal* terminal) {
  char* value;
  int intValue;
  Timer timer;
  value = terminal->readParameter();
  if (value != NULL) {
    intValue = (unsigned long) atoi(value);
    if ((intValue >= 1) && (intValue <= 60)) {
      timer.setRefreshSeconds(intValue);
      while (!timer.expired()) {
        delay(100);
        WATCHDOG->petWatchdog();
      }
    } else {
      terminal->invalidParameter();
    }
  } else {
    terminal->invalidParameter();
  }
  terminal->prompt();
}

void echoCommand(Terminal* terminal) {
  char* value;
  Timer timer;
  value = terminal->readParameter();
  while (value != NULL) {
    terminal->print(INFO, value);
    terminal->print(INFO, " ");
    value = terminal->readParameter();
  }
  terminal->println();
  terminal->prompt();
}