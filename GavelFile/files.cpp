#include "files.h"

#include "serialport.h"
#include "watchdog.h"

#include <PicoOTA.h>

Files* Files::files = nullptr;

Files* Files::get() {
  if (files == nullptr) files = new Files();
  return files;
}

void Files::setup() {
  PORT->addCmd("del", "[name|*]", "Deletes a file", Files::deleteCommand);
  PORT->addCmd("dir", "", "Contents of File System", Files::printDir);
  PORT->addCmd("cat", "[name]", "Displays the contents of a file", Files::catCommand);
  if (LittleFS.begin())
    PORT->println(PASSED, "File System Complete");
  else
    PORT->println(FAILED, "File System FAILED");
  if (LittleFS.exists(UPGRADE_COMMAND_FILE_NAME)) deleteFile(UPGRADE_COMMAND_FILE_NAME);
  if (LittleFS.exists(UPGRADE_FILE_NAME)) deleteFile(UPGRADE_FILE_NAME);
}

bool Files::verifyFile(String path) {
  bool success = false;
  if (LittleFS.exists(path)) {
    // File file = FILES->getFile(path);
    // if (file && file.isFile()) {
    success = true;
    //  file.close();
    //}
  }
  return success;
}

File Files::getFile(String path) {
  File file;
  if (verifyFile(path)) {
    file = LittleFS.open(path, "r");
  } else
    PORT->println(ERROR, "FS: " + path + " file does not exist!!!!");

  return file;
}

void Files::deleteFile(String path) {
  File file;
  if (path == String("*")) {
    Dir dir = LittleFS.openDir("/");
    PORT->println(INFO, "Deleting ALL Files");
    while (dir.next()) {
      LittleFS.remove(dir.fileName());
      PORT->println(INFO, dir.fileName() + String(" file deleted."));
    }
    PORT->println(PASSED, "All Files Deleted");
  } else if (verifyFile(path)) {
    LittleFS.remove(path);
    PORT->println(INFO, path + String(" file deleted."));
  } else
    PORT->println(WARNING, "FS: " + path + " file does not exist!!!!");
}

File Files::writeFile(String path) {
  File file;
  file = LittleFS.open(path, "w");
  if (!file) PORT->println(WARNING, "FS: " + path + " opening file for writing!!!!");

  return file;
}

void Files::printInfo() {
  int size = 0;
  int count = 0;
  FSInfo info;
  LittleFS.info(info);

  PORT->println(INFO, "File system info:");

  PORT->print(INFO, "Total space:      ");
  PORT->print(INFO, String(info.totalBytes));
  PORT->println(INFO, "byte");

  PORT->print(INFO, "Total space used: ");
  PORT->print(INFO, String(info.usedBytes));
  PORT->println(INFO, "byte");

  PORT->print(INFO, "Total space free: ");
  PORT->print(INFO, String(info.totalBytes - info.usedBytes));
  PORT->println(INFO, "byte");
  PORT->println();

  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    if (dir.isFile()) {
      PORT->print(INFO, dir.fileName());
      if (dir.fileSize()) {
        File file = dir.openFile("r");
        count++;
        size += file.size();
        for (int i = dir.fileName().length(); i < 17; i++) PORT->print(INFO, " ");
        PORT->println(INFO, " " + String(file.size()));
        file.close();
      }
    }
  }
  String countString = "   " + String(count) + " File(s)";
  PORT->print(INFO, countString);
  for (int i = countString.length(); i < 17; i++) PORT->print(INFO, " ");
  PORT->println(INFO, " " + String(size));
}

unsigned long Files::availableSpace() {
  FSInfo info;
  LittleFS.info(info);
  return (info.totalBytes - info.usedBytes);
}

void Files::UPGRADE_SYSTEM() {
  if (LittleFS.exists(UPGRADE_FILE_NAME)) {
    PORT->println();
    PORT->println(WARNING, "Starting Upgrade Process.....");
    picoOTA.begin();
    picoOTA.addFile(UPGRADE_FILE_NAME);
    picoOTA.commit();
    LittleFS.end();
    PORT->println(WARNING, "Reboot in progress.....");
    WATCHDOG->reboot();
  } else {
    PORT->println(WARNING, "Unable to Upgrade System due to missing files.");
  }
}

void Files::deleteCommand() {
  char* value;
  PORT->println();
  value = PORT->readParameter();
  if (value != NULL) {
    FILES->deleteFile(value);
  } else {
    PORT->invalidParameter();
  }
  PORT->prompt();
}

void Files::printDir() {
  PORT->println();
  FILES->printInfo();
  PORT->prompt();
}

#define BUFFER_SIZE 2
static char fileBuffer[BUFFER_SIZE];

bool Files::catFile(String path) {
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
          PORT->print(INFO, line);
          line = "";
          count = 0;
        }
      }

      PORT->println(INFO, line);
      file.close();
    }
  }
  return success;
}

void Files::catCommand() {
  char* value;
  PORT->println();
  value = PORT->readParameter();
  if (value != NULL) {
    if (FILES->verifyFile(value)) {
      FILES->catFile("/" + String(value));
    } else {
      PORT->println(ERROR, "\"" + String(value) + "\" File does not exist!!!");
    }
  } else {
    PORT->invalidParameter();
  }
  PORT->prompt();
}
