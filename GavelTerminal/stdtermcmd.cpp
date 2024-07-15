#include "stdtermcmd.h"

#include "termcmd.h"
#include "terminal.h"
#include "watchdog.h"

void help(Terminal* terminal);
void helpHist(Terminal* terminal);
void rebootPico(Terminal* terminal);
void uploadPico(Terminal* terminal);

void addStandardTerminalCommands() {
  TERM_CMD->addCmd("?", "", "Print Help", help);
  TERM_CMD->addCmd("help", "", "Print Help", help);
  TERM_CMD->addCmd("history", "", "Command History", helpHist);
  TERM_CMD->addCmd("reboot", "", "Software Reboot the Pico", rebootPico);
  TERM_CMD->addCmd("upload", "", "Software Reboot the Pico into USB mode", uploadPico);
}

void help(Terminal* terminal) {
  terminal->banner();
  terminal->println();

  int maxStringLength = 0;
  for (int i = 0; i < TERM_CMD->getCmdCount(); i++) {
    int length = TERM_CMD->getCmd(i).length() + TERM_CMD->getParameter(i).length() + 2;
    maxStringLength = (maxStringLength > length) ? maxStringLength : length;
  }
  for (int i = 0; i < TERM_CMD->getCmdCount(); i++) {
    String line1 = TERM_CMD->getCmd(i) + " " + TERM_CMD->getParameter(i) + "                                         ";
    String line2 = "- " + TERM_CMD->getDescription(i);
    line1 = line1.substring(0, maxStringLength);
    terminal->println(HELP, line1, line2);
  }

  terminal->println();
  terminal->prompt();
}

void helpHist(Terminal* terminal) {
  terminal->println();
  terminal->println(INFO, "Command History");
  for (unsigned long i = 0; i < terminal->lastBuffer.count(); i++) terminal->println(HELP, String(i) + ". ", (char*) terminal->lastBuffer.get(i));
  terminal->println(PASSED, "Command History");
  terminal->prompt();
}

void rebootPico(Terminal* terminal) {
  terminal->println();
  terminal->println(WARNING, "Rebooting....");
  delay(100);
  WATCHDOG->reboot();
}

void uploadPico(Terminal* terminal) {
  terminal->println();
  terminal->println(WARNING, "Rebooting in USB Mode....");
  delay(100);
  rp2040.rebootToBootloader();
}
