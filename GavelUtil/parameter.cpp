#include "parameter.h"

ParameterList* ParameterList::list = nullptr;

ParameterList* ParameterList::get() {
  if (list == nullptr) list = new ParameterList();
  return list;
}

int ParameterList::getCount() {
  return parameterCount;
}

void ParameterList::clear() {
  parameterCount = 0;
}

void ParameterList::addParameter(String parameter, String value) {
  if (parameterCount < MAX_PARAMETERS) {
    parameters[parameterCount].parameter = parameter;
    parameters[parameterCount].value = value;
    parameterCount++;
  }
}

String ParameterList::getParameter(int index) {
  return parameters[index].parameter;
}

String ParameterList::getValue(int index) {
  return parameters[index].value;
}
