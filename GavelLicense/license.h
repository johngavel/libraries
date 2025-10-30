#ifndef __GAVEL_LICENSE
#define __GAVEL_LICENSE

#include "datastructure.h"
#include "libraries.h"

#include <Arduino.h>
#include <Terminal.h>

#define MAX_LICENSES 20

#define LICENSE LicenseManager::get()
#define LICENSE_ADD                                                                                                                                            \
  if (LICENSE_AVAILABLE) LicenseManager::addLicense
#define LICENSE_AVAILABLE LicenseManager::initialized()

#define ARDUINO_IDE_LICENSE LICENSE_ADD("Arduino IDE", "2.3.6", "")

#define LIBRARY_INFO_INDEX_MISSING -1
struct LicenseFile {
  char libraryName[40];
  char version[20];
  char link[80];
  int libraryInfoIndex;
};

class LicenseManager {
public:
  static LicenseManager* get();
  static bool initialized() { return licenseManager != nullptr; };
  static void addLicense(String libraryName, String version, String link, int index = LIBRARY_INFO_INDEX_MISSING);
  static void addLicense(LibraryInfo libraryInfo, int index);

  void setup();
  unsigned long count() { return licenseList.count(); };
  bool getFile(unsigned long index, LicenseFile* element) { return licenseList.get(index, element); };

private:
  static LicenseManager* licenseManager;
  static void printTable(OutputInterface* terminal);
  static void printLicense(OutputInterface* terminal);
  LicenseManager(){};
  void addLicenseToDatabase(String libraryName, String version, String link, int index);
  ClassicQueue licenseList = ClassicQueue(MAX_LICENSES, sizeof(LicenseFile));
};

#endif
