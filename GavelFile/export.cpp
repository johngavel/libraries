#include "export.h"

#include "serialport.h"

#define PARAMETER_COUNT String("ParameterCount")

Export::Export(String __filename) {
  filename = "/" + __filename;
  if (verifyFile()) FILES->deleteFile(filename, CONSOLE);
  exportFile = FILES->writeFile(filename);
}

bool Export::verifyFile() {
  bool success = false;
  success = FILES->verifyFile(filename);
  return success;
}

bool Export::close() {
  exportFile.close();
  return true;
}

bool Export::exportData(String parameter, String value) {
  bool success = true;

  exportFile.println(parameter + "=" + value);
  return success;
}

bool Export::exportData(String parameter, byte value) {
  return exportData(parameter, String(value));
}

bool Export::exportData(String parameter, int value) {
  return exportData(parameter, String(value));
}

bool Export::exportData(String parameter, unsigned long value) {
  return exportData(parameter, String(value));
}

bool Export::exportData(String parameter, byte* value, int length) {
  String line = "";
  for (int i = 0; i < length; i++) line += String(value[i]) + ".";
  return exportData(parameter, line);
}

bool Export::exportData(String parameter, bool value) {
  return exportData(parameter, String(value));
}

Import::Import(String __filename) {
  filename = "/" + __filename;
  exists = verifyFile();
  if (exists) importFile = FILES->getFile(filename);
}

bool Import::verifyFile() {
  bool success = false;
  success = FILES->verifyFile(filename);
  return success;
}

bool Import::importParameter(String* parameter) {
  bool success = false;
  if (exists) {
    *parameter = importFile.readStringUntil('=');
    if (*parameter != NULL) {
      parameter->trim();
      success = true;
    }
  }
  return success;
}

bool Import::importData(String* value) {
  bool success = false;
  if (exists) {
    *value = importFile.readStringUntil('\n');
    value->trim();
    success = true;
  }
  return success;
}

bool Import::importData(int* value) {
  String string;
  bool success = importData(&string);
  if (success) { *value = string.toInt(); }
  return success;
}

bool Import::importData(unsigned long* value) {
  String string;
  bool success = importData(&string);
  if (success) { *value = string.toInt(); }
  return success;
}

bool Import::importData(byte* value) {
  String string;
  bool success = importData(&string);
  if (success) { *value = string.toInt(); }
  return success;
}

#define MAX_C_STRING 64
bool Import::importData(byte* value, int length) {
  String string;
  char c_string[MAX_C_STRING];
  char* token;
  memset(c_string, 0, MAX_C_STRING);
  bool success = importData(&string);
  if (success) {
    strncpy(c_string, string.c_str(), MAX_C_STRING - 1);
    token = strtok(c_string, ".");
    int index;
    for (index = 0; ((index < length) && (token != NULL)); index++) {
      value[index] = atoi(token);
      token = strtok(NULL, ".");
    }
    success = (index == length);
  }
  return success;
}

bool Import::importData(bool* value) {
  String string;
  bool success = importData(&string);
  if (success) { *value = (string.toInt() == 1); }
  return success;
}
