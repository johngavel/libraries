#ifndef __GAVEL_FILES
#define __GAVEL_FILES

#include <Arduino.h>
#include <LittleFS.h>
#include <Terminal.h>

#define FILES Files::get()
#define FILES_AVAILABLE Files::initialized()
#define UPGRADE_FILE_NAME "pico.bin"
#define UPGRADE_COMMAND_FILE_NAME "otacommand.bin"

class Files {
public:
  static Files* get();
  static bool initialized() { return (files != nullptr); };
  void setup();
  bool format();
  bool verifyFile(String path);
  bool catFile(String path, OutputInterface* terminal);
  File getFile(String path);
  void deleteFile(String path, OutputInterface* terminal);
  File writeFile(String path);
  unsigned long sizeFile(String path);
  void printDirectory(OutputInterface* terminal, String path);
  void printInfo(OutputInterface* terminal);
  unsigned long availableSpace();
  void UPGRADE_SYSTEM();

private:
  Files(){};
  static Files* files;

  static void deleteCommand(OutputInterface* terminal);
  static void printDir(OutputInterface* terminal);
  static void catCommand(OutputInterface* terminal);
  static void formatCommand(OutputInterface* terminal);
  static void copyconCommand(OutputInterface* terminal);
};

#endif
