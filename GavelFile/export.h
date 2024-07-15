#ifndef __GAVEL_EXPORT
#define __GAVEL_EXPORT

#include "files.h"
#include "terminal.h"

#include <Arduino.h>

class Export {
public:
  Export(String __filename);
  bool verifyFile();
  bool close();
  bool exportData(String parameter, String value);
  bool exportData(String parameter, byte value);
  bool exportData(String parameter, int value);
  bool exportData(String parameter, unsigned long value);
  bool exportData(String parameter, byte* value, int length);
  bool exportData(String parameter, bool value);

private:
  String filename;
  File exportFile;
};

class Import {
public:
  Import(String __filename);
  bool verifyFile();
  bool importParameter(String* parameter);
  bool importData(String* value);
  bool importData(int* value);
  bool importData(unsigned long* value);
  bool importData(byte* value);
  bool importData(byte* value, int length);
  bool importData(bool* value);

private:
  bool exists;
  String filename;
  File importFile;
};

#endif
