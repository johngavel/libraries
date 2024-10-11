#include "files.h"

#include "batch.h"
#include "serialport.h"
#include "stringutils.h"
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
  TERM_CMD->addCmd("copycon", "[name]", "Writes a File (CTRL^z to exit)", Files::copyconCommand);
  addBatchTerminalCommands();
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

void Files::printDirectory(Terminal* terminal, String path) {
  int size = 0;
  int count = 0;
  ClassicQueue queue(10, 32);
  terminal->println(INFO, "Directory: " + path);
  Dir dir = LittleFS.openDir(path);
  while (dir.next()) {
    if (dir.isFile()) {
      terminal->print(INFO, dir.fileName());
      if (dir.fileSize()) {
        File file = dir.openFile("r");
        count++;
        size += file.size();
        terminal->print(INFO, tab(dir.fileName().length(), 22));
        terminal->println(INFO, String(file.size()));
        file.close();
      }
    }
    if (dir.isDirectory()) {
      String dirPath = path + dir.fileName() + "/";
      terminal->println(INFO, "<DIR>  " + dir.fileName());

      queue.push((void*) dirPath.c_str());
    }
  }
  String countString = "   " + String(count) + " File(s)";
  terminal->print(INFO, countString);
  terminal->print(INFO, tab(countString.length(), 22));
  terminal->println(INFO, String(size));
  terminal->println();
  for (unsigned long i = 0; i < queue.count(); i++) printDirectory(terminal, String((char*) queue.get(i)));
}

void Files::printInfo(Terminal* terminal) {
  FSInfo info;
  LittleFS.info(info);

  terminal->println(INFO, "File system info:");

  terminal->print(INFO, "Total space:      ");
  terminal->print(INFO, String((unsigned long) info.totalBytes));
  terminal->println(INFO, "byte");

  terminal->print(INFO, "Total space used: ");
  terminal->print(INFO, String((unsigned long) info.usedBytes));
  terminal->println(INFO, "byte");

  terminal->print(INFO, "Total space free: ");
  terminal->print(INFO, String((unsigned long) (info.totalBytes - info.usedBytes)));
  terminal->println(INFO, "byte");
  terminal->println();
  printDirectory(terminal, "/");
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
  value = terminal->readParameter();
  if (value != NULL) {
    FILES->deleteFile(value, terminal);
  } else {
    terminal->invalidParameter();
  }
  terminal->prompt();
}

void Files::printDir(Terminal* terminal) {
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
  bool success = FILES->format();
  terminal->println((success) ? PASSED : FAILED, "Format of Little File System Complete");
  terminal->prompt();
}

void Files::copyconCommand(Terminal* terminal) {
  char* value;
  bool exit = false;
  char readChar[3];
  int available = 0;
  File writeFile;

  value = terminal->readParameter();
  if (value != NULL) {
    if (FILES->verifyFile(value)) {
      terminal->println(ERROR, "\"" + String(value) + "\" File already exists!!!");
    } else {
      writeFile = FILES->writeFile(value);
      if (writeFile) {
        terminal->println(INFO, "\"" + String(value) + "\" File open for writing.");
        terminal->println(INFO, "All keystrokes will be written to the file.");
        terminal->println(INFO, "Enter CTRL^z to exit.");
        while (!exit) {
          available = terminal->getInput()->available();
          if (available > 0) {
            terminal->getInput()->readBytes(&readChar[0], 1);
            if (isPrintable(readChar[0])) {
              if (terminal->getEcho()) terminal->getOutput()->print(readChar[0]);
              writeFile.write(readChar, 1);
            } else if (readChar[0] == 0x1A)
              exit = true;
            else if (readChar[0] == 0x0D) {
              if (terminal->getEcho()) terminal->getOutput()->print("\r\n");
              writeFile.write("\r\n", 2);
            }
          }
          delay(20);
          WATCHDOG->petWatchdog();
        }
        writeFile.close();
        terminal->println(PASSED, "File Written.");
      } else {
        terminal->println(ERROR, "Unknown Error");
      }
    }
  } else {
    terminal->invalidParameter();
  }
  terminal->prompt();
}
