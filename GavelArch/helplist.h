#include <Arduino.h>

#define MAX_HELP_CMD 50

class HelpItem {
public:
  String command;
  String description;
  String parameter;
  void* function;
};

class HelpList {
public:
  HelpList();
  int getHelpCount() { return countHelp; };
  int setHelp(String command, String parameterDesc, String description, void (*function)());
  int findHelp(String command);
  String getCmd(int index);
  String getParameter(int index);
  String getDescription(int index);
  void callFunction(int index);

private:
  HelpItem list[MAX_HELP_CMD];
  int countHelp;
};
