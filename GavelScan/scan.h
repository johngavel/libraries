#ifndef __GAVEL_SCANNER
#define __GAVEL_SCANNER

#include <Terminal.h>

#define SCAN Scan::get()
#define SCAN_AVAILABLE Scan::initialized()

class Scan {
public:
  static Scan* get();
  static bool initialized() { return scanner != nullptr; };
  void setup();

private:
  Scan(){};
  static Scan* scanner;
  static void scani2c(Terminal* terminal);
};

#endif
