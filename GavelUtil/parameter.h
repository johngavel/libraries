#ifndef __GAVEL_PARAMETER
#define __GAVEL_PARAMETER

#include <Arduino.h>

#define MAX_PARAMETERS 150
#define LIST ParameterList::get()

class ParameterList {
public:
  class Parameter {
  public:
    Parameter() : parameter(""), value(""){};
    String parameter;
    String value;
  };
  static ParameterList* get();
  void clear();
  void addParameter(String parameter, String value);
  int getCount();
  String getParameter(int index);
  String getValue(int index);

private:
  ParameterList() : parameterCount(0){};
  static ParameterList* list;
  Parameter parameters[MAX_PARAMETERS];
  int parameterCount;
};

#endif
