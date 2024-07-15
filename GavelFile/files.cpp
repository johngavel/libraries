#include "files.h"

#include "serialport.h"
#include "termcmd.h"
#include "watchdog.h"

#include <PicoOTA.h>

Files* Files::files = nullptr;

Files* Files::get() {
  if (files == nullptr) files = new Files();
  return files;
}

void Files::setup() {
  TERM_CMD->addCmd("del", "[name|*]", "Deletes a file", Files::deleteCommand);
  TERM_CMD->addCmd("dir", "", "Contents of File System", Files::printDir);
  TERM_CMD->addCmd("cat", "[name]", "Displays the contents of a file", Files::catCommand);
  TERM_CMD->addCmd("format", "", "Formats the File System", Files::formatCommand);
  if (LittleFS.begin())
    CONSOLE->println(PASSED, "File System Complete");
  else
    CONSOLE->println(FAILED, "File System FAILED");
  if (LittleFS.exists(UPGRADE_COMMAND_FILE_NAME)) deleteFile(UPGRADE_COMMAND_FILE_NAME, CONSOLE);
  if (LittleFS.exists(UPGRADE_FILE_NAME)) deleteFile(UPGRADE_FILE_NAME, CONSOLE);
}

bool Files::format() {
  return LittleFS.format();
}

bool Files::verifyFile(String path) {
  bool success = false;
  if (LittleFS.exists(path)) { success = true; }
  return success;
}

File Files::getFile(String path) {
  File file;
  if (verifyFile(path)) {
    file = LittleFS.open(path, "r");
  } else
    CONSOLE->println(ERROR, "FS: " + path + " file does not exist!!!!");

  return file;
}

void Files::deleteFile(String path, Terminal* terminal) {
  File file;
  if (path == String("*")) {
    Dir dir = LittleFS.openDir("/");
    terminal->println(INFO, "Deleting ALL Files");
    while (dir.next()) {
      LittleFS.remove(dir.fileName());
      terminal->println(INFO, dir.fileName() + String(" file deleted."));
    }
    terminal->println(PASSED, "All Files Deleted");
  } else if (verifyFile(path)) {
    LittleFS.remove(path);
    terminal->println(INFO, path + String(" file deleted."));
  } else
    terminal->println(WARNING, "FS: " + path + " file does not exist!!!!");
}

File Files::writeFile(String path) {
  File file;
  file = LittleFS.open(path, "w");
  if (!file) CONSOLE->println(WARNING, "FS: " + path + " opening file for writing!!!!");

  return file;
}

void Files::printInfo(Terminal* terminal) {
  int size = 0;
  int count = 0;
  FSInfo info;
  LittleFS.info(info);

  terminal->println(INFO, "File system info:");

  terminal->print(INFO, "Total space:      ");
  terminal->print(INFO, String(info.totalBytes));
  terminal->println(INFO, "byte");

  terminal->print(INFO, "Total space used: ");
  terminal->print(INFO, String(info.usedBytes));
  terminal->println(INFO, "byte");

  terminal->print(INFO, "Total space free: ");
  terminal->print(INFO, String(info.totalBytes - info.usedBytes));
  terminal->println(INFO, "byte");
  terminal->println();

  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    if (dir.isFile()) {
      terminal->print(INFO, dir.fileName());
      if (dir.fileSize()) {
        File file = dir.openFile("r");
        count++;
        size += file.size();
        for (int i = dir.fileName().length(); i < 17; i++) terminal->print(INFO, " ");
        terminal->println(INFO, " " + String(file.size()));
        file.close();
      }
    }
  }
  String countString = "   " + String(count) + " File(s)";
  terminal->print(INFO, countString);
  for (int i = countString.length(); i < 17; i++) terminal->print(INFO, " ");
  terminal->println(INFO, " " + String(size));
}

unsigned long Files::availableSpace() {
  FSInfo info;
  LittleFS.info(info);
  return (info.totalBytes - info.usedBytes);
}

void Files::UPGRADE_SYSTEM() {
  if (LittleFS.exists(UPGRADE_FILE_NAME)) {
    CONSOLE->println();
    CONSOLE->println(WARNING, "Starting Upgrade Process.....");
    picoOTA.begin();
    picoOTA.addFile(UPGRADE_FILE_NAME);
    picoOTA.commit();
    LittleFS.end();
    CONSOLE->println(WARNING, "Reboot in progress.....");
    WATCHDOG->reboot();
  } else {
    CONSOLE->println(WARNING, "Unable to Upgrade System due to missing files.");
  }
}

void Files::deleteCommand(Terminal* terminal) {
  char* value;
  terminal->println();
  value = terminal->readParameter();
  if (value != NULL) {
    FILES->deleteFile(value, terminal);
  } else {
    terminal->invalidParameter();
  }
  terminal->prompt();
}

void Files::printDir(Terminal* terminal) {
  terminal->println();
  FILES->printInfo(terminal);
  terminal->prompt();
}

#define BUFFER_SIZE 2
static char fileBuffer[BUFFER_SIZE];

bool Files::catFile(String path, Terminal* terminal) {
  bool success = false;
  if (verifyFile(path)) {
    File file = FILES->getFile(path);
    if (file && file.isFile()) {
      success = true;
      memset(fileBuffer, 0, BUFFER_SIZE);
      unsigned long loops = file.size();
      unsigned long count = 0;
      String line = "";
      for (unsigned long i = 0; i < loops; i++) {
        file.readBytes(fileBuffer, 1);
        count++;
        if (fileBuffer[0] == '\n') line += String("\r");
        line += String(fileBuffer);
        memset(fileBuffer, 0, BUFFER_SIZE);
        if (count % 200 == 0) {
          terminal->print(INFO, line);
          line = "";
          count = 0;
        }
      }

      terminal->println(INFO, line);
      file.close();
    }
  }
  return success;
}

void Files::catCommand(Terminal* terminal) {
  char* value;
  terminal->println();
  value = terminal->readParameter();
  if (value != NULL) {
    if (FILES->verifyFile(value)) {
      FILES->catFile("/" + String(value), terminal);
    } else {
      terminal->println(ERROR, "\"" + String(value) + "\" File does not exist!!!");
    }
  } else {
    terminal->invalidParameter();
  }
  terminal->prompt();
}

void Files::formatCommand(Terminal* terminal) {
  terminal->println();
  bool success = FILES->format();
  terminal->println((success) ? PASSED : FAILED, "Format of Little File System Complete");
  terminal->prompt();
}
