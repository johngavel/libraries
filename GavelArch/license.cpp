#include "license.h"

#include "files.h"
#include "serialport.h"
#include "stringutils.h"

#define MAX_LICENSES 20

LicenseManager* LicenseManager::licenseManager = nullptr;

LicenseManager* LicenseManager::get() {
  if (licenseManager == nullptr) licenseManager = new LicenseManager();
  return licenseManager;
}

LicenseManager::LicenseManager() {
  licenseList = new ClassicQueue(MAX_LICENSES, sizeof(LicenseFile));
}

void LicenseManager::setup() {
  PORT->addCmd("license", "[n]", "Prints the License File of the library indicated by n.", LicenseManager::printLicense);
  PORT->addCmd("library", "", "Prints the libraries used in this build.", LicenseManager::printTable);
}

void LicenseManager::addLicense(String libraryName, String version, String link) {
  if (initialized()) { get()->addLicenseToDatabase(libraryName, version, link); }
}

void LicenseManager::addLicenseToDatabase(String libraryName, String version, String link) {
  bool found = false;
  LicenseFile newLicense;
  LicenseFile oldLicense;
  memset(&newLicense, 0, sizeof(LicenseFile));
  strncpy(newLicense.libraryName, libraryName.c_str(), 39);
  strncpy(newLicense.version, version.c_str(), 19);
  strncpy(newLicense.link, link.c_str(), 79);
  for (unsigned long i = 0; i < licenseList->count(); i++) {
    if (licenseList->get(i, &oldLicense)) {
      if (strncmp(newLicense.libraryName, oldLicense.libraryName, 40) == 0) { found = true; }
    }
  }
  if (!found) { licenseList->push(&newLicense); }
}

void LicenseManager::printTable() {
  String name;
  String version;
  String number;
  LicenseFile oldLicense;
  PORT->println();
  PORT->println(HELP, " # | Name                                    ", "| Version   ");
  PORT->println(HELP, "---|-----------------------------------------", "|-----------");
  for (unsigned long i = 0; i < LICENSE->count(); i++) {
    if (LICENSE->getFile(i, &oldLicense)) {
      number = String(i + 1);
      number += tab(number.length(), 3) + "| ";
      name = oldLicense.libraryName;
      version = oldLicense.version;
      name += tab(name.length(), 40);
      version = "| " + version;
      PORT->println(HELP, number + name, version);
    }
  }
  PORT->println();
  PORT->println(PASSED, "All Libraries");
  PORT->prompt();
}

#define BUFFER_SIZE 2
static char fileBuffer[BUFFER_SIZE];

void LicenseManager::printLicense() {
  bool success = false;
  LicenseFile oldLicense;
  char* value;
  PORT->println();
  value = PORT->readParameter();
  if (value != NULL) {
    unsigned int index = atoi(value) - 1;
    if (index < (LICENSE->count())) {
      LICENSE->getFile(index, &oldLicense);
      PORT->println(INFO, String(oldLicense.libraryName) + " " + String(oldLicense.version) + "  - License File");
      PORT->println();
      File file = FILES->getFile("/license/" + String(oldLicense.link));
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
      } else
        PORT->println(ERROR, "Missing License File.");
    } else
      PORT->println(ERROR, "Invalid Index to Library.");
  } else
    PORT->println(ERROR, "Missing Index to Library.");
  PORT->println((success) ? PASSED : FAILED, "License File Complete");
  PORT->prompt();
}
