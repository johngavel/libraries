#ifndef __GAVEL_SCANNER
#define __GAVEL_SCANNER

#define SCAN Scan::get()

#include <Terminal.h>

class Scan {
public:
  static Scan* get();
  void setup();

private:
  Scan(){};
  static Scan* scanner;
  static void scani2c(Terminal* terminal);
};

#endif
