#ifndef __GAVEL_DEBUG_SCREEN
#define __GAVEL_DEBUG_SCREEN

#include "datastructure.h"
#include "screen.h"

#define DEBUG_SCREEN DebugScreen::get()
#define DEBUG_SCREEN_AVAILABLE DebugScreen::initialized()

class DebugScreen : public RefreshScreen {
public:
  static DebugScreen* get() {
    if (debugScreen == nullptr) debugScreen = new DebugScreen();
    return debugScreen;
  };
  static bool initialized() { return debugScreen != nullptr; };
  void add(String __line) {
    char line[21];
    memset(line, 0, sizeof(line));
    strncpy(line, __line.c_str(), 20);
    if (queue.full()) queue.pop();
    queue.push(line);
  };

  void screen() {
    char* line[8];
    for (unsigned int i = 0; i < 8; i++) { line[i] = (char*) queue.get(i); }
    SCREEN->setScreen(String(line[0]), String(line[1]), String(line[2]), String(line[3]), String(line[4]), String(line[5]), String(line[6]), String(line[7]));
  };

private:
  static DebugScreen* debugScreen;
  DebugScreen() {
    SCREEN->setRefreshScreen(this, 500);
    SCREEN->setup();
  }
  ClassicQueue queue = ClassicQueue(8, 21);
};

#endif
