#include "license.h"

#include "asciitable.h"
#include "files.h"
#include "serialport.h"
#include "servermodule.h"
#include "stringutils.h"

LicenseManager* LicenseManager::licenseManager = nullptr;

LicenseManager* LicenseManager::get() {
  if (licenseManager == nullptr) licenseManager = new LicenseManager();
  return licenseManager;
}

void LicenseManager::setup() {
  LicenseFile oldLicense;
  LibraryInfo* libraryInfo;
  TERM_CMD->addCmd("license", "[n]", "Prints the License File indicated by n.", LicenseManager::printLicense);
  TERM_CMD->addCmd("library", "", "Prints the libraries used in this build.", LicenseManager::printTable);
  if (SERVER_AVAILABLE) {
    for (unsigned long i = 0; i < licenseList.count(); i++) {
      if (licenseList.get(i, &oldLicense)) {
        if (oldLicense.libraryInfoIndex != LIBRARY_INFO_INDEX_MISSING) {
          libraryInfo = (LibraryInfo*) &libraries[oldLicense.libraryInfoIndex];
          String location = "license/" + String(libraryInfo->license_name);
          SERVER->setDigitalFile(location.c_str(), libraryInfo->license_buffer, libraryInfo->license_size);
        }
      }
    }
  }
  CONSOLE->println(PASSED, "License Manager Complete");
}

void LicenseManager::addLicense(String libraryName, String version, String link, int index) {
  if (initialized()) { get()->addLicenseToDatabase(libraryName, version, link, index); }
}

void LicenseManager::addLicense(LibraryInfo libraryInfo, int index) {
  if (libraryInfo.license_buffer == nullptr) {
    CONSOLE->println(WARNING, "Invalid Digital License File Added.");
    addLicense(String(libraryInfo.name), String(libraryInfo.version), String(libraryInfo.license_file), LIBRARY_INFO_INDEX_MISSING);
  } else {
    addLicense(String(libraryInfo.name), String(libraryInfo.version), String(libraryInfo.license_file), index);
  }
}

void LicenseManager::addLicenseToDatabase(String libraryName, String version, String link, int index) {
  bool found = false;
  LicenseFile newLicense;
  LicenseFile oldLicense;
  memset(&newLicense, 0, sizeof(LicenseFile));
  strncpy(newLicense.libraryName, libraryName.c_str(), 39);
  strncpy(newLicense.version, version.c_str(), 19);
  strncpy(newLicense.link, link.c_str(), 79);
  newLicense.libraryInfoIndex = index;
  for (unsigned long i = 0; i < licenseList.count(); i++) {
    if (licenseList.get(i, &oldLicense)) {
      if (strncmp(newLicense.libraryName, oldLicense.libraryName, 40) == 0) { found = true; }
    }
  }
  if (!found) { licenseList.push(&newLicense); }
}

void LicenseManager::printTable(Terminal* terminal) {
  AsciiTable table(terminal);
  LicenseFile oldLicense;
  table.addColumn(Cyan, "#", 4);
  table.addColumn(Normal, "Name", 28);
  table.addColumn(Yellow, "Version", 15);
  table.addColumn(Green, "File Name", 28);
  table.addColumn(Magenta, "Size", 8);
  table.addColumn(Blue, "Type", 6);
  table.printHeader();
  for (unsigned long i = 0; i < LICENSE->count(); i++) {
    if (LICENSE->getFile(i, &oldLicense)) {
      String size = "0";
      String type = "";
      if (oldLicense.libraryInfoIndex != LIBRARY_INFO_INDEX_MISSING) {
        size = String(libraries[oldLicense.libraryInfoIndex].license_size);
        type = "d";
      } else if (FILES_AVAILABLE) {
        if (FILES->verifyFile(String("license/") + String(oldLicense.link))) {
          size = String(FILES->sizeFile(String("license/") + String(oldLicense.link)));
          type = "f";
        } else {
          size = "-";
          type = "m";
        }
      }
      table.printData(String(i + 1), oldLicense.libraryName, oldLicense.version, oldLicense.link, size, type);
    }
  }
  table.printDone("All Libraries");
  terminal->prompt();
}

#define PRINT_BUFFER_SIZE 200
static char fileBuffer[PRINT_BUFFER_SIZE + 1];

void LicenseManager::printLicense(Terminal* terminal) {
  bool success = false;
  LicenseFile oldLicense;
  char* value;
  value = terminal->readParameter();
  if (value != NULL) {
    unsigned int index = atoi(value) - 1;
    if (index < (LICENSE->count())) {
      LICENSE->getFile(index, &oldLicense);
      terminal->println(INFO, String(oldLicense.libraryName) + " " + String(oldLicense.version) + "  - License File");
      terminal->println();
      if (oldLicense.libraryInfoIndex != LIBRARY_INFO_INDEX_MISSING) {
        unsigned long size = libraries[oldLicense.libraryInfoIndex].license_size;
        unsigned long loops = size / PRINT_BUFFER_SIZE;
        unsigned long remainder = size % PRINT_BUFFER_SIZE;

        for (unsigned long i = 0; i < loops; i++) {
          memset(fileBuffer, 0, PRINT_BUFFER_SIZE + 1);
          memcpy(fileBuffer, &libraries[oldLicense.libraryInfoIndex].license_buffer[i * PRINT_BUFFER_SIZE], PRINT_BUFFER_SIZE);
          terminal->print(INFO, String(fileBuffer));
        }
        memset(fileBuffer, 0, PRINT_BUFFER_SIZE + 1);
        memcpy(fileBuffer, &libraries[oldLicense.libraryInfoIndex].license_buffer[loops * PRINT_BUFFER_SIZE], remainder);
        terminal->print(INFO, String(fileBuffer));
        success = true;
      } else if ((FILES_AVAILABLE) && (FILES->catFile("/license/" + String(oldLicense.link), terminal))) {
        success = true;
      } else
        terminal->println(ERROR, "Missing License File.");
    } else
      terminal->println(ERROR, "Invalid Index to Library.");
  } else
    terminal->println(ERROR, "Missing Index to Library.");
  terminal->println((success) ? PASSED : FAILED, "License File Complete");
  terminal->prompt();
}
