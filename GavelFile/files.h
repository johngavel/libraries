#ifndef __GAVEL_FILES
#define __GAVEL_FILES

#include "terminal.h"

#include <Arduino.h>
#include <LittleFS.h>

#define FILES Files::get()
#define UPGRADE_FILE_NAME "pico.bin"
#define UPGRADE_COMMAND_FILE_NAME "otacommand.bin"

class Files {
public:
  static Files* get();
  void setup();
  bool format();
  bool verifyFile(String path);
  bool catFile(String path, Terminal* terminal);
  File getFile(String path);
  void deleteFile(String path, Terminal* terminal);
  File writeFile(String path);
  void printInfo(Terminal* terminal);
  unsigned long availableSpace();
  void UPGRADE_SYSTEM();

private:
  Files(){};
  static Files* files;

  static void deleteCommand(Terminal* terminal);
  static void printDir(Terminal* terminal);
  static void catCommand(Terminal* terminal);
  static void formatCommand(Terminal* terminal);
};

#endif
