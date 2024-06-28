#ifndef __GAVEL_FILES
#define __GAVEL_FILES

#include <Arduino.h>
#include <LittleFS.h>

#define FILES Files::get()
#define UPGRADE_FILE_NAME "pico.bin"
#define UPGRADE_COMMAND_FILE_NAME "otacommand.bin"

class Files {
public:
  static Files* get();
  void setup();
  File getFile(String path);
  void deleteFile(String path);
  File writeFile(String path);
  void printInfo();
  unsigned long availableSpace();
  void UPGRADE_SYSTEM();

private:
  Files(){};
  static Files* files;

  static void deleteCommand();
  static void printDir();
};

#endif
