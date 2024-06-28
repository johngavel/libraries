#include "helplist.h"

HelpList::HelpList() {
  countHelp = 0;
  for (int i = 0; i < MAX_HELP_CMD; i++) {
    list[i].command = "";
    list[i].parameter = "";
    list[i].description = "";
    list[i].function = NULL;
  }
}

int HelpList::setHelp(String command, String parameterDesc, String description, void (*function)()) {
  int returnInt = -1;
  if (countHelp < MAX_HELP_CMD) {
    list[countHelp].command = command;
    list[countHelp].parameter = parameterDesc;
    list[countHelp].description = description;
    list[countHelp].function = (void*) function;
    returnInt = countHelp;
    countHelp++;
  }
  return returnInt;
}

int HelpList::findHelp(String command) {
  int helpIndex = -1;
  for (int i = 0; i < countHelp; i++) {
    if (command == list[i].command) helpIndex = i;
  }
  return helpIndex;
}

String HelpList::getCmd(int index) {
  return list[index].command;
}

String HelpList::getParameter(int index) {
  return list[index].parameter;
}

String HelpList::getDescription(int index) {
  return list[index].description;
}

void HelpList::callFunction(int index) {
  void (*cmd)() = (void (*)()) list[index].function;
  (*cmd)();
}
